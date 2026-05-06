import pandas as pd
import numpy as np


def print_section(title: str) -> None:
    print("\n" + "=" * 80)
    print(title)
    print("=" * 80)


def summarize_basic(df: pd.DataFrame) -> pd.DataFrame:
    summary = pd.DataFrame(
        {
            "dtype": df.dtypes.astype(str),
            "non_null": df.notnull().sum(),
            "nulls": df.isnull().sum(),
            "nunique": df.nunique(dropna=True),
        }
    )

    numeric_cols = df.select_dtypes(include="number").columns
    if len(numeric_cols) > 0:
        summary.loc[numeric_cols, "min"] = df[numeric_cols].min()
        summary.loc[numeric_cols, "max"] = df[numeric_cols].max()
        summary.loc[numeric_cols, "mean"] = df[numeric_cols].mean()
        summary.loc[numeric_cols, "variance"] = df[numeric_cols].var()

    object_cols = df.select_dtypes(include=["object", "string"]).columns
    for col in object_cols:
        lengths = df[col].dropna().astype(str).str.len()
        if not lengths.empty:
            summary.loc[col, "min_len"] = lengths.min()
            summary.loc[col, "max_len"] = lengths.max()

    return summary


def detect_key_attributes(df: pd.DataFrame) -> tuple[list[str], list[str]]:
    numeric_cols = df.select_dtypes(include="number").columns.tolist()
    categorical_cols = df.select_dtypes(
        include=["object", "string", "category"]
    ).columns.tolist()

    for col in numeric_cols:
        if df[col].nunique(dropna=True) <= 10:
            categorical_cols.append(col)

    categorical_cols = list(dict.fromkeys(categorical_cols))

    key_numeric: list[str] = []
    if numeric_cols:
        variances = df[numeric_cols].var().sort_values(ascending=False)
        key_numeric = variances.head(3).index.tolist()

    key_categorical: list[str] = []
    if categorical_cols:
        cardinality = (
            df[categorical_cols].nunique(dropna=True).sort_values(ascending=False)
        )
        key_categorical = cardinality.head(3).index.tolist()

    return key_numeric, key_categorical


def print_distributions_and_relationships(df: pd.DataFrame) -> None:
    numeric_cols = df.select_dtypes(include="number").columns.tolist()
    key_numeric, key_categorical = detect_key_attributes(df)

    if key_numeric:
        print("\nDistribución de atributos numéricos clave:")
        for col in key_numeric:
            print(f"\n- {col}")
            print(df[col].describe())
    else:
        print("\nNo se detectaron atributos numéricos clave para distribución.")

    if key_categorical:
        print("\nDistribución de atributos categóricos clave (top 5):")
        for col in key_categorical:
            print(f"\n- {col}")
            value_counts = df[col].value_counts(dropna=False)
            proportions = (value_counts / len(df)).round(3)
            summary = pd.DataFrame({"count": value_counts, "ratio": proportions})
            print(summary.head(5))
    else:
        print("\nNo se detectaron atributos categóricos clave para distribución.")

    if len(numeric_cols) >= 2:
        print("\nRelaciones entre pares de atributos numéricos (correlaciones):")
        corr = df[numeric_cols].corr()
        upper_triangle = corr.where(np.triu(np.ones(corr.shape), k=1).astype(bool))
        corr_pairs = upper_triangle.stack()
        if not corr_pairs.empty:
            corr_pairs = corr_pairs.reindex(
                corr_pairs.abs().sort_values(ascending=False).index
            )
            print(corr_pairs.head(5))
        else:
            print("No hay pares de correlación disponibles.")
    else:
        print("\nNo hay suficientes atributos numéricos para relaciones por pares.")


def print_string_patterns(df: pd.DataFrame) -> None:
    patterns = {
        "fecha_iso": r"^\d{4}-\d{2}-\d{2}$",
        "fecha_dmy": r"^\d{2}/\d{2}/\d{4}$",
        "fecha_mdy": r"^\d{2}-\d{2}-\d{4}$",
        "telefono": r"^\+?\d[\d\s\-().]{6,}$",
        "email": r"^[^@\s]+@[^@\s]+\.[^@\s]+$",
        "solo_digitos": r"^\d+$",
    }

    object_cols = df.select_dtypes(include=["object", "string"]).columns
    if len(object_cols) == 0:
        print("No hay columnas de texto para analizar patrones.")
        return

    for col in object_cols:
        series = df[col].dropna().astype(str)
        if series.empty:
            continue

        sample = series
        if len(series) > 1000:
            sample = series.sample(n=1000, random_state=42)

        detected = []
        for name, pattern in patterns.items():
            ratio = sample.str.match(pattern).mean()
            if ratio >= 0.6:
                detected.append((name, ratio))

        date_ratio = pd.to_datetime(sample, errors="coerce").notna().mean()
        if date_ratio >= 0.6:
            detected.append(("parseable_date", date_ratio))

        if detected:
            print(f"\n- {col}")
            for name, ratio in detected:
                print(f"  patrón '{name}' ~ {ratio:.2f}")


def print_subpopulation_properties(df: pd.DataFrame) -> None:
    numeric_cols = df.select_dtypes(include="number").columns.tolist()
    categorical_cols = df.select_dtypes(
        include=["object", "string", "category"]
    ).columns.tolist()

    for col in numeric_cols:
        if df[col].nunique(dropna=True) <= 10:
            categorical_cols.append(col)

    candidates = []
    for col in categorical_cols:
        nunique = df[col].nunique(dropna=True)
        if 2 <= nunique <= 10:
            candidates.append(col)

    if not candidates:
        print("No se detectaron sub-poblaciones claras (categorías 2-10).")
        return

    candidates = candidates[:2]
    for col in candidates:
        print(f"\nSub-población por '{col}':")
        counts = df[col].value_counts(dropna=False)
        print("Conteo por categoría:")
        print(counts)
        if numeric_cols:
            print("Promedios por categoría (numéricos):")
            group_means = df.groupby(col)[numeric_cols].mean(numeric_only=True)
            print(group_means)


def analyze_dataset(name: str, df: pd.DataFrame) -> None:
    print_section(f"DATASET: {name} ({df.shape[0]} filas, {df.shape[1]} columnas)")

    print("Subtarea 1: Tipos de datos, longitudes, intervalos, varianza, singularidad")
    basic_summary = summarize_basic(df)
    print(basic_summary)

    print("\nSubtarea 2: Distribución de atributos clave y relaciones")
    print_distributions_and_relationships(df)

    print("\nSubtarea 3: Patrones típicos de string")
    print_string_patterns(df)

    print("\nSubtarea 4: Propiedades de sub-poblaciones")
    print_subpopulation_properties(df)


def main() -> None:
    pd.set_option("display.max_columns", None)
    pd.set_option("display.width", 160)

    datasets = {
        "Web_orders": pd.read_csv("data/Web_orders.txt", sep=";"),
        "Catalog_orders": pd.read_csv("data/Catalog_orders.txt", sep=","),
        "Products": pd.read_csv("data/products.txt", sep=","),
    }

    for name, df in datasets.items():
        analyze_dataset(name, df)


if __name__ == "__main__":
    main()
