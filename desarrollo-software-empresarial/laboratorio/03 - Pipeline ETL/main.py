import os
import re
from difflib import get_close_matches
from pathlib import Path

import pandas as pd
from pandas.api.types import is_bool_dtype, is_datetime64_any_dtype, is_object_dtype, is_string_dtype


BASE_DIR = Path(__file__).resolve().parent
DATA_DIR = BASE_DIR / "data"
OUTPUT_DIR = BASE_DIR / "output"
SQL_DIR = BASE_DIR / "sql"

RAW_FILES = {
    "catalog_orders": DATA_DIR / "Catalog_orders.txt",
    "web_orders": DATA_DIR / "Web_orders.txt",
    "products": DATA_DIR / "products.txt",
}

CATALOG_BY_PREFIX = {
    "GD": "Gardening",
    "SP": "Sports",
    "PT": "Pets",
    "TY": "Toys",
    "SW": "Software",
    "CC": "Collectibles",
}

CANONICAL_CATALOGS = sorted(set(CATALOG_BY_PREFIX.values()))


def ensure_directories() -> None:
    OUTPUT_DIR.mkdir(exist_ok=True)
    SQL_DIR.mkdir(exist_ok=True)


def load_env_file(path: Path = BASE_DIR / ".env") -> None:
    """Carga variables simples KEY=VALUE desde .env sin depender de paquetes extra."""
    if not path.exists():
        return

    for raw_line in path.read_text(encoding="utf-8").splitlines():
        line = raw_line.strip()
        if not line or line.startswith("#") or "=" not in line:
            continue

        key, value = line.split("=", 1)
        os.environ.setdefault(key.strip(), value.strip().strip('"').strip("'"))


def read_source_data() -> dict[str, pd.DataFrame]:
    """Extract: lee los tres archivos solicitados con Pandas."""
    catalog_orders = pd.read_csv(RAW_FILES["catalog_orders"], sep=",", dtype=str)
    web_orders = pd.read_csv(RAW_FILES["web_orders"], sep=";", dtype=str)
    products = pd.read_csv(RAW_FILES["products"], sep=",", dtype=str)
    return {
        "catalog_orders_raw": catalog_orders,
        "web_orders_raw": web_orders,
        "products_raw": products,
    }


def normalize_columns(df: pd.DataFrame) -> pd.DataFrame:
    renamed = {column: column.strip().lower() for column in df.columns}
    return df.rename(columns=renamed)


def fix_web_orders_header_shift(df: pd.DataFrame) -> pd.DataFrame:
    """
    Web_orders tiene encabezados desplazados: DATE contiene PCODE,
    CATALOG contiene fecha y PCODE contiene catálogo. Se corrige antes de limpiar.
    """
    fixed = normalize_columns(df).copy()
    fixed = fixed.rename(
        columns={
            "date": "product_code_raw",
            "catalog": "order_date_raw",
            "pcode": "catalog_raw",
            "custnum": "customer_raw",
        }
    )
    fixed["source_system"] = "web"
    return fixed


def normalize_catalog_orders(df: pd.DataFrame) -> pd.DataFrame:
    normalized = normalize_columns(df).copy()
    normalized = normalized.rename(
        columns={
            "date": "order_date_raw",
            "catalog": "catalog_raw",
            "pcode": "product_code_raw",
            "custnum": "customer_raw",
        }
    )
    normalized["source_system"] = "catalog"
    return normalized


def clean_text(series: pd.Series) -> pd.Series:
    return series.fillna("").astype(str).str.strip()


def standardize_code(value: object, valid_codes: set[str]) -> str | pd.NA:
    if pd.isna(value):
        return pd.NA

    raw = str(value).strip().upper()
    if not raw:
        return pd.NA

    translated = raw.translate(str.maketrans({"O": "0", "I": "1", "!": "1", ")": "0", "(": "0"}))
    translated = re.sub(r"[^A-Z0-9]", "", translated)

    if translated in valid_codes:
        return translated

    prefix_match = re.match(r"^[A-Z]{2}", translated)
    prefix = prefix_match.group(0) if prefix_match else ""
    candidates = [code for code in valid_codes if code.startswith(prefix)] or list(valid_codes)
    close = get_close_matches(translated, candidates, n=1, cutoff=0.74)
    if close:
        return close[0]

    return translated


def canonical_catalog_from_code(product_code: object) -> str | pd.NA:
    if pd.isna(product_code):
        return pd.NA
    prefix = str(product_code).strip().upper()[:2]
    return CATALOG_BY_PREFIX.get(prefix, pd.NA)


def parse_web_dates(series: pd.Series) -> pd.Series:
    primary = pd.to_datetime(series, format="%d/%m/%Y %H:%M:%S", errors="coerce")
    fallback = pd.to_datetime(series, dayfirst=True, errors="coerce")
    return primary.fillna(fallback)


def parse_catalog_dates(series: pd.Series) -> pd.Series:
    text = clean_text(series)
    parts = text.str.extract(r"^(?P<month>\d{1,2})/(?P<year>\d{2,4})/(?P<day>\d{1,2})")
    month = pd.to_numeric(parts["month"], errors="coerce")
    year = pd.to_numeric(parts["year"], errors="coerce")
    day = pd.to_numeric(parts["day"], errors="coerce")

    year = year.where(year >= 100, year.where(year < 70, year + 1900))
    year = year.where(year >= 100, year + 2000)

    parsed = pd.to_datetime(
        {"year": year, "month": month, "day": day},
        errors="coerce",
    )
    return parsed


def clean_products(products_raw: pd.DataFrame) -> pd.DataFrame:
    products = normalize_columns(products_raw).copy()
    products["product_key"] = pd.to_numeric(products["id"], errors="coerce").astype("Int64")
    products["product_code"] = clean_text(products["pcode"]).str.upper()
    products["product_type"] = clean_text(products["type"])
    products["description"] = clean_text(products["descrip"])
    products["supplier"] = clean_text(products["supplier"])
    products["price"] = pd.to_numeric(products["price"], errors="coerce")
    products["cost"] = pd.to_numeric(products["cost"], errors="coerce")
    products["product_family"] = products["product_code"].str[:2].map(CATALOG_BY_PREFIX)
    products["gross_margin"] = products["price"] - products["cost"]

    return products[
        [
            "product_key",
            "product_code",
            "product_family",
            "product_type",
            "description",
            "supplier",
            "price",
            "cost",
            "gross_margin",
        ]
    ].drop_duplicates(subset=["product_code"])


def clean_orders(orders_raw: pd.DataFrame, valid_codes: set[str], source: str) -> pd.DataFrame:
    orders = orders_raw.copy()
    orders["source_transaction_id"] = pd.to_numeric(orders["id"], errors="coerce").astype("Int64")
    orders["invoice_number"] = pd.to_numeric(orders["inv"], errors="coerce").astype("Int64")
    orders["customer_identifier"] = clean_text(orders["customer_raw"])
    orders["catalog_original"] = clean_text(orders["catalog_raw"])
    orders["product_code_original"] = clean_text(orders["product_code_raw"])
    orders["product_code"] = orders["product_code_original"].map(lambda value: standardize_code(value, valid_codes))
    orders["catalog"] = orders["product_code"].map(canonical_catalog_from_code)
    orders["quantity_original_missing"] = orders["qty"].isna() | clean_text(orders["qty"]).eq("")
    orders["quantity"] = pd.to_numeric(orders["qty"], errors="coerce")
    orders["quantity"] = orders["quantity"].fillna(1).astype("Int64")
    orders["order_date"] = parse_web_dates(orders["order_date_raw"]) if source == "web" else parse_catalog_dates(orders["order_date_raw"])
    orders["order_year"] = orders["order_date"].dt.year.astype("Int64")
    orders["order_month"] = orders["order_date"].dt.month.astype("Int64")

    return orders[
        [
            "source_system",
            "source_transaction_id",
            "invoice_number",
            "order_date_raw",
            "order_date",
            "order_year",
            "order_month",
            "catalog_original",
            "catalog",
            "product_code_original",
            "product_code",
            "quantity_original_missing",
            "quantity",
            "customer_identifier",
        ]
    ]


def build_dimensions_and_fact(clean_orders_df: pd.DataFrame, dim_products: pd.DataFrame) -> dict[str, pd.DataFrame]:
    dim_customers = (
        clean_orders_df[["source_system", "customer_identifier"]]
        .drop_duplicates()
        .sort_values(["source_system", "customer_identifier"])
        .reset_index(drop=True)
    )
    dim_customers.insert(0, "customer_key", range(1, len(dim_customers) + 1))

    dim_dates = (
        clean_orders_df[["order_date"]]
        .dropna()
        .drop_duplicates()
        .sort_values("order_date")
        .reset_index(drop=True)
    )
    dim_dates["date_key"] = dim_dates["order_date"].dt.strftime("%Y%m%d").astype(int)
    dim_dates["year"] = dim_dates["order_date"].dt.year
    dim_dates["quarter"] = dim_dates["order_date"].dt.quarter
    dim_dates["month"] = dim_dates["order_date"].dt.month
    dim_dates["day"] = dim_dates["order_date"].dt.day
    dim_dates["weekday"] = dim_dates["order_date"].dt.day_name()
    dim_dates = dim_dates[["date_key", "order_date", "year", "quarter", "month", "day", "weekday"]]

    fact_orders = clean_orders_df.merge(dim_products, on="product_code", how="left")
    fact_orders = fact_orders.merge(dim_customers, on=["source_system", "customer_identifier"], how="left")
    fact_orders = fact_orders.merge(dim_dates[["date_key", "order_date"]], on="order_date", how="left")
    fact_orders["gross_sales"] = fact_orders["quantity"].astype(float) * fact_orders["price"]
    fact_orders["gross_cost"] = fact_orders["quantity"].astype(float) * fact_orders["cost"]
    fact_orders["gross_profit"] = fact_orders["gross_sales"] - fact_orders["gross_cost"]
    fact_orders.insert(0, "order_fact_key", range(1, len(fact_orders) + 1))

    fact_orders = fact_orders[
        [
            "order_fact_key",
            "source_system",
            "source_transaction_id",
            "invoice_number",
            "date_key",
            "product_key",
            "customer_key",
            "catalog",
            "quantity",
            "price",
            "cost",
            "gross_sales",
            "gross_cost",
            "gross_profit",
            "catalog_original",
            "product_code_original",
            "product_code",
            "quantity_original_missing",
        ]
    ]

    return {
        "dim_products": dim_products,
        "dim_customers": dim_customers,
        "dim_dates": dim_dates,
        "fact_orders": fact_orders,
    }


def summarize_dataframe(df: pd.DataFrame) -> pd.DataFrame:
    summary = pd.DataFrame(
        {
            "dtype": df.dtypes.astype(str),
            "non_null": df.notna().sum(),
            "nulls": df.isna().sum(),
            "unique_values": df.nunique(dropna=True),
        }
    )
    for metric_column in [
        "min",
        "max",
        "mean",
        "variance",
        "min_length",
        "max_length",
        "true_values",
        "false_values",
    ]:
        summary[metric_column] = pd.Series(pd.NA, index=summary.index, dtype="object")

    for column in df.columns:
        if is_bool_dtype(df[column].dtype):
            summary.loc[column, "true_values"] = int(df[column].fillna(False).sum())
            summary.loc[column, "false_values"] = int((~df[column].fillna(False)).sum())
            continue

        if is_datetime64_any_dtype(df[column].dtype):
            dates = df[column].dropna()
            if not dates.empty:
                summary.loc[column, "min"] = str(dates.min())
                summary.loc[column, "max"] = str(dates.max())
            continue

        numeric = pd.to_numeric(df[column], errors="coerce")
        if numeric.notna().any() and not is_bool_dtype(numeric.dtype):
            summary.loc[column, "min"] = float(numeric.min())
            summary.loc[column, "max"] = float(numeric.max())
            summary.loc[column, "mean"] = float(numeric.mean())
            summary.loc[column, "variance"] = float(numeric.var())

        text_lengths = df[column].dropna().astype(str).str.len()
        if not text_lengths.empty:
            summary.loc[column, "min_length"] = text_lengths.min()
            summary.loc[column, "max_length"] = text_lengths.max()

    return summary


def get_text_columns(df: pd.DataFrame) -> list[str]:
    return [
        column
        for column in df.columns
        if is_object_dtype(df[column].dtype) or is_string_dtype(df[column].dtype)
    ]


def dataframe_block(df: pd.DataFrame, max_rows: int = 30) -> str:
    shown = df.head(max_rows)
    return "```text\n" + shown.to_string(index=True) + "\n```"


def profile_dataset(name: str, df: pd.DataFrame) -> str:
    lines = [f"## {name}", f"Filas: {len(df):,} | Columnas: {len(df.columns):,}", ""]
    lines.append("### Tipos, longitudes, intervalos, varianza y singularidad")
    lines.append(dataframe_block(summarize_dataframe(df)))

    numeric_source = df.drop(
        columns=[column for column in df.columns if is_bool_dtype(df[column].dtype) or is_datetime64_any_dtype(df[column].dtype)],
        errors="ignore",
    )
    numeric_df = numeric_source.apply(pd.to_numeric, errors="coerce")
    numeric_cols = [column for column in numeric_df.columns if numeric_df[column].notna().any()]
    if numeric_cols:
        variances = numeric_df[numeric_cols].var().sort_values(ascending=False).head(3)
        lines.append("\n### Distribución de atributos numéricos clave")
        lines.append(dataframe_block(numeric_df[variances.index].describe().T))

    object_cols = get_text_columns(df)
    if object_cols:
        lines.append("\n### Distribución de atributos categóricos clave")
        for column in object_cols[:5]:
            counts = df[column].fillna("<NULL>").astype(str).value_counts().head(8).to_frame("count")
            counts["ratio"] = counts["count"] / len(df)
            lines.append(f"\n#### {column}")
            lines.append(dataframe_block(counts))

    if len(numeric_cols) >= 2:
        corr = numeric_df[numeric_cols].corr(numeric_only=True)
        corr_rows = []
        for left_index, left_column in enumerate(numeric_cols):
            for right_column in numeric_cols[left_index + 1 :]:
                value = corr.loc[left_column, right_column]
                if pd.notna(value):
                    corr_rows.append(
                        {
                            "left": left_column,
                            "right": right_column,
                            "correlation": value,
                            "abs_correlation": abs(value),
                        }
                    )
        if corr_rows:
            corr_pairs = (
                pd.DataFrame(corr_rows)
                .sort_values("abs_correlation", ascending=False)
                .drop(columns="abs_correlation")
                .head(5)
            )
            lines.append("\n### Relaciones entre pares numéricos")
            lines.append(dataframe_block(corr_pairs))

    lines.append("\n### Patrones típicos de string detectados")
    pattern_rows = []
    patterns = {
        "fecha_dmy_hms": r"^\d{1,2}/\d{1,2}/\d{4} \d{2}:\d{2}:\d{2}$",
        "fecha_m_yy_d_hms": r"^\d{1,2}/\d{2}/\d{1,2} \d{2}:\d{2}:\d{2}$",
        "codigo_producto": r"^[A-Za-z]{2}[A-Za-z0-9)!]{3,5}$",
        "solo_digitos": r"^\d+$",
    }
    for column in object_cols:
        sample = df[column].dropna().astype(str)
        if sample.empty:
            continue
        for pattern_name, pattern in patterns.items():
            ratio = sample.str.match(pattern).mean()
            if ratio >= 0.5:
                pattern_rows.append({"column": column, "pattern": pattern_name, "ratio": round(float(ratio), 3)})
    lines.append(dataframe_block(pd.DataFrame(pattern_rows) if pattern_rows else pd.DataFrame({"message": ["Sin patrones dominantes"]})))

    lines.append("\n### Propiedades de sub-poblaciones")
    subpopulation_columns = [column for column in df.columns if 2 <= df[column].nunique(dropna=True) <= 10]
    if subpopulation_columns:
        for column in subpopulation_columns[:3]:
            counts = df[column].fillna("<NULL>").astype(str).value_counts().to_frame("count")
            lines.append(f"\n#### {column}")
            lines.append(dataframe_block(counts))
    else:
        lines.append("No se detectaron sub-poblaciones claras con cardinalidad entre 2 y 10.")

    return "\n".join(lines)


def write_exploration_report(path: Path, datasets: dict[str, pd.DataFrame], title: str) -> None:
    sections = [f"# {title}", ""]
    for name, df in datasets.items():
        sections.append(profile_dataset(name, df))
        sections.append("\n---\n")
    path.write_text("\n".join(sections), encoding="utf-8")


def build_quality_report(
    raw_orders: pd.DataFrame,
    clean_orders_df: pd.DataFrame,
    valid_codes: set[str],
) -> pd.DataFrame:
    catalog_raw = clean_text(raw_orders["catalog_raw"])
    pcode_raw = clean_text(raw_orders["product_code_raw"]).str.upper()
    date_missing = clean_orders_df["order_date"].isna()
    product_unmatched = ~clean_orders_df["product_code"].isin(valid_codes)

    return pd.DataFrame(
        [
            {
                "issue": "CATALOG no canónico antes de limpiar",
                "count": int((~catalog_raw.isin(CANONICAL_CATALOGS)).sum()),
                "resolution": "Se reemplaza usando el prefijo del PCODE validado.",
            },
            {
                "issue": "PCODE no coincide exactamente con Products.txt antes de reparar",
                "count": int((~pcode_raw.isin(valid_codes)).sum()),
                "resolution": "Se normaliza mayúscula/minúscula, O→0, símbolos y coincidencia cercana por prefijo.",
            },
            {
                "issue": "PCODE sin producto luego de reparar",
                "count": int(product_unmatched.sum()),
                "resolution": "Queda trazado para revisión manual; no se elimina la transacción.",
            },
            {
                "issue": "QTY faltante o corrupto",
                "count": int(clean_orders_df["quantity_original_missing"].sum()),
                "resolution": "Se imputa QTY=1 y se conserva quantity_original_missing=True.",
            },
            {
                "issue": "DATE inválida o no parseable",
                "count": int(date_missing.sum()),
                "resolution": "Se aplican parsers específicos por fuente; fechas inválidas quedan para revisión.",
            },
        ]
    )


def write_quality_markdown(path: Path, quality: pd.DataFrame, clean_orders_df: pd.DataFrame) -> None:
    invalid_catalog_examples = (
        clean_orders_df.loc[clean_orders_df["catalog_original"] != clean_orders_df["catalog"], ["source_system", "source_transaction_id", "catalog_original", "catalog", "product_code_original", "product_code"]]
        .head(25)
    )
    missing_qty_examples = clean_orders_df.loc[
        clean_orders_df["quantity_original_missing"],
        ["source_system", "source_transaction_id", "product_code", "quantity"],
    ]

    content = [
        "# Informe de calidad de datos",
        "",
        "Este informe responde los puntos de calidad solicitados y documenta las estrategias aplicadas en el pipeline ETL.",
        "",
        "## Resumen de problemas",
        dataframe_block(quality, max_rows=20),
        "",
        "## Errores en CATALOG: representación y ubicación",
        "Los errores aparecen como singular/plural incorrecto, typos, mayúsculas inconsistentes, espacios vacíos o categorías derivadas de un PCODE correcto.",
        dataframe_block(invalid_catalog_examples),
        "",
        "## Valores faltantes",
        "La cantidad (`QTY`) tiene valores faltantes/corruptos puntuales. No se eliminaron filas; se imputó `1` y se conservó una bandera de auditoría.",
        dataframe_block(missing_qty_examples if not missing_qty_examples.empty else pd.DataFrame({"message": ["Sin QTY faltantes"]})),
    ]
    path.write_text("\n".join(content), encoding="utf-8")


def write_schema_sql(path: Path) -> None:
    path.write_text(
        """CREATE SCHEMA IF NOT EXISTS dw_lab3;

CREATE TABLE IF NOT EXISTS dw_lab3.dim_products (
    product_key INTEGER PRIMARY KEY,
    product_code VARCHAR(20) UNIQUE NOT NULL,
    product_family VARCHAR(50),
    product_type VARCHAR(100),
    description VARCHAR(255),
    supplier VARCHAR(255),
    price NUMERIC(12, 2),
    cost NUMERIC(12, 2),
    gross_margin NUMERIC(12, 2)
);

CREATE TABLE IF NOT EXISTS dw_lab3.dim_customers (
    customer_key INTEGER PRIMARY KEY,
    source_system VARCHAR(20) NOT NULL,
    customer_identifier VARCHAR(255) NOT NULL
);

CREATE TABLE IF NOT EXISTS dw_lab3.dim_dates (
    date_key INTEGER PRIMARY KEY,
    order_date DATE NOT NULL,
    year INTEGER,
    quarter INTEGER,
    month INTEGER,
    day INTEGER,
    weekday VARCHAR(20)
);

CREATE TABLE IF NOT EXISTS dw_lab3.fact_orders (
    order_fact_key INTEGER PRIMARY KEY,
    source_system VARCHAR(20) NOT NULL,
    source_transaction_id INTEGER,
    invoice_number BIGINT,
    date_key INTEGER REFERENCES dw_lab3.dim_dates(date_key),
    product_key INTEGER REFERENCES dw_lab3.dim_products(product_key),
    customer_key INTEGER REFERENCES dw_lab3.dim_customers(customer_key),
    catalog VARCHAR(50),
    quantity INTEGER,
    price NUMERIC(12, 2),
    cost NUMERIC(12, 2),
    gross_sales NUMERIC(14, 2),
    gross_cost NUMERIC(14, 2),
    gross_profit NUMERIC(14, 2),
    catalog_original VARCHAR(100),
    product_code_original VARCHAR(30),
    product_code VARCHAR(20),
    quantity_original_missing BOOLEAN
);
""",
        encoding="utf-8",
    )


def write_pipeline_graph(path: Path) -> None:
    path.write_text(
        """# Diseño del pipeline ETL

```mermaid
flowchart LR
    A[Catalog_orders.txt] --> E[Extract: pandas.read_csv]
    B[Web_orders.txt] --> E
    C[products.txt] --> E
    E --> P[Profiling inicial]
    P --> T[Transform]
    T --> T1[Normalizar fechas por fuente]
    T --> T2[Reparar PCODE y CATALOG]
    T --> T3[Gestionar QTY faltante]
    T --> I[Integrar pedidos + productos]
    I --> D1[dim_products]
    I --> D2[dim_customers]
    I --> D3[dim_dates]
    I --> F[fact_orders]
    D1 --> L[Load: CSV + PostgreSQL opcional]
    D2 --> L
    D3 --> L
    F --> L
    L --> R[Reportes de calidad y exploración final]
```
""",
        encoding="utf-8",
    )


def try_load_postgresql(tables: dict[str, pd.DataFrame]) -> str:
    database_url = os.getenv("DATABASE_URL") or os.getenv("POSTGRES_URL")
    if not database_url:
        return "No se cargó a PostgreSQL porque DATABASE_URL/POSTGRES_URL no está configurado. Se generaron CSV finales y sql/schema.sql."

    try:
        from sqlalchemy import create_engine, text
        from sqlalchemy.exc import SQLAlchemyError
    except ImportError:
        return "No se cargó a PostgreSQL porque falta SQLAlchemy o el driver PostgreSQL. Ejecuta: uv sync"

    try:
        engine = create_engine(database_url)
        schema_sql = (SQL_DIR / "schema.sql").read_text(encoding="utf-8")

        with engine.begin() as connection:
            connection.execute(text("DROP SCHEMA IF EXISTS dw_lab3 CASCADE"))
            for statement in schema_sql.split(";"):
                statement = statement.strip()
                if statement:
                    connection.execute(text(statement))

            load_order = ["dim_products", "dim_customers", "dim_dates", "fact_orders"]
            for table_name in load_order:
                tables[table_name].to_sql(
                    table_name,
                    connection,
                    schema="dw_lab3",
                    if_exists="append",
                    index=False,
                    method="multi",
                    chunksize=1000,
                )
    except SQLAlchemyError as exc:
        return f"No se pudo cargar a PostgreSQL. Verifica que Docker esté levantado y la URL sea correcta. Error: {exc}"

    return "Carga PostgreSQL completada en el esquema dw_lab3."


def main() -> None:
    load_env_file()
    ensure_directories()
    pd.set_option("display.max_columns", None)
    pd.set_option("display.width", 180)

    raw = read_source_data()
    write_exploration_report(OUTPUT_DIR / "01_exploracion_inicial.md", raw, "Exploración inicial de datasets crudos")

    dim_products = clean_products(raw["products_raw"])
    valid_codes = set(dim_products["product_code"])

    catalog_orders = normalize_catalog_orders(raw["catalog_orders_raw"])
    web_orders = fix_web_orders_header_shift(raw["web_orders_raw"])
    raw_orders = pd.concat([catalog_orders, web_orders], ignore_index=True)

    clean_catalog = clean_orders(catalog_orders, valid_codes, source="catalog")
    clean_web = clean_orders(web_orders, valid_codes, source="web")
    clean_orders_df = pd.concat([clean_catalog, clean_web], ignore_index=True)

    warehouse_tables = build_dimensions_and_fact(clean_orders_df, dim_products)
    for table_name, df in warehouse_tables.items():
        df.to_csv(OUTPUT_DIR / f"{table_name}.csv", index=False)

    quality = build_quality_report(raw_orders, clean_orders_df, valid_codes)
    write_quality_markdown(OUTPUT_DIR / "02_informe_calidad.md", quality, clean_orders_df)
    write_exploration_report(
        OUTPUT_DIR / "03_exploracion_final.md",
        {
            "clean_orders": clean_orders_df,
            "dim_products": warehouse_tables["dim_products"],
            "dim_customers": warehouse_tables["dim_customers"],
            "dim_dates": warehouse_tables["dim_dates"],
            "fact_orders": warehouse_tables["fact_orders"],
        },
        "Exploración posterior a limpieza e integración",
    )

    write_schema_sql(SQL_DIR / "schema.sql")
    write_pipeline_graph(OUTPUT_DIR / "04_pipeline_etl.md")
    load_status = try_load_postgresql(warehouse_tables)

    print("Pipeline ETL ejecutado correctamente.")
    print(f"Filas integradas en fact_orders: {len(warehouse_tables['fact_orders']):,}")
    print(f"Productos dimensionados: {len(warehouse_tables['dim_products']):,}")
    print(f"Clientes dimensionados: {len(warehouse_tables['dim_customers']):,}")


if __name__ == "__main__":
    main()
