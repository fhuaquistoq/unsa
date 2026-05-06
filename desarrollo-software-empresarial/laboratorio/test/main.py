import pandas as pd
import re

catalog_orders = pd.read_csv('Catalog_Orders.txt', sep=',')
web_orders = pd.read_csv('Web_orders.txt', sep=';')
products = pd.read_csv('products.txt', sep=',')

print("--- PUNTO 3: ANÁLISIS EXPLORATORIO ---")

dataframes = {'Catalog_Orders': catalog_orders, 'Web_orders': web_orders, 'Products': products}

for name, df in dataframes.items():
    print(f"\n[{name}] - Información General:")
    print(df.info())
    
    print(f"\n[{name}] - Resumen Estadístico (Intervalos, Varianza):")
    print(df.describe(include='all'))
    
    print(f"\n[{name}] - Singularidad (Valores únicos por columna):")
    print(df.nunique())

print("\nDistribución de categorías en Catalog_Orders (CATALOG):")
print(catalog_orders['CATALOG'].value_counts())

print("\nDistribución de proveedores en Products (supplier):")
print(products['supplier'].value_counts())

patron_pcode = r'^[A-Za-z]{2}\d{4}$'
catalog_pcode_ok = catalog_orders['PCODE'].astype(str).str.match(patron_pcode).mean() * 100
web_pcode_ok = web_orders['PCODE'].astype(str).str.match(patron_pcode).mean() * 100
print(f"\nPorcentaje de PCODE con formato estándar (LLNNNN) en Catalog_Orders: {catalog_pcode_ok:.2f}%")
print(f"Porcentaje de PCODE con formato estándar (LLNNNN) en Web_orders: {web_pcode_ok:.2f}%")

print("\nTop 5 Clientes recurrentes en Web_orders (custnum):")
print(web_orders['custnum'].value_counts().head(5))

print("\n--- PUNTO 4: CALIDAD DE LOS DATOS ---")

for name, df in dataframes.items():
    nulos = df.isnull().sum()
    if nulos.sum() > 0:
        print(f"\nValores nulos encontrados en {name}:")
        print(nulos[nulos > 0])

print("\nFrecuencia y errores ortográficos en el atributo CATALOG (Combinado):")
all_catalogs = pd.concat([catalog_orders['CATALOG'], web_orders['CATALOG']])
print(all_catalogs.value_counts(dropna=False))

print("\nValores corruptos identificados en PCODE (Contienen caracteres no numéricos al final):")
all_pcodes = pd.concat([catalog_orders['PCODE'], web_orders['PCODE']]).astype(str)
pcodes_corruptos = all_pcodes[~all_pcodes.str.match(patron_pcode)]
print(pcodes_corruptos.value_counts())

espacios_blanco_catalog = catalog_orders[catalog_orders['CATALOG'].str.strip() == ""]
if not espacios_blanco_catalog.empty:
    print(f"\nSe encontraron {len(espacios_blanco_catalog)} registros con espacios en blanco en CATALOG.")

print("\nInconsistencias en el nombrado de 'supplier' (Products):")
print(products['supplier'].value_counts())

