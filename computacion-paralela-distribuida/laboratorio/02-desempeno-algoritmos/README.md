# Laboratorio 2: Desempeño de algoritmos

Comparación de algoritmos secuenciales de multiplicación de matrices y de orden de bucles,
perfilados con Valgrind (Cachegrind) y KCachegrind.

## Requisitos

- `g++` con soporte de C++17
- `make`
- `valgrind` (incluye la herramienta Cachegrind)
- `kcachegrind` (visualización gráfica de los perfiles)
- `node` / `npx` (solo para descargar el código con degit)

En Ubuntu/Debian:

```bash
sudo apt install build-essential valgrind kcachegrind
```

## Descargar el código

Con [degit](https://github.com/Rich-Harris/degit), que descarga la carpeta del repositorio sin
traer el historial de git:

```bash
npx degit fhuaquistoq/unsa/computacion-paralela-distribuida/laboratorio/02-desempeno-algoritmos 02-desempeno-algoritmos
cd 02-desempeno-algoritmos
```

El primer argumento es la carpeta dentro del repositorio
(`https://github.com/fhuaquistoq/unsa/tree/main/computacion-paralela-distribuida/laboratorio/02-desempeno-algoritmos`)
y el segundo es el nombre de la carpeta local donde se descarga.

## Compilar y ejecutar

```bash
make        # compila todos los src/**/*.cpp y deja los binarios en bin/
make list   # lista los binarios disponibles
make clean  # borra bin/
```

Para ejecutar cualquiera de los binarios:

```bash
make run <binario> [argumentos...]
```

Por ejemplo, `make run row-major 1024`. También se puede pasar todo por variables:
`make run BIN=row-major ARGS="1024"`.

### Binarios y sus parámetros

| Binario | Parámetros | Descripción |
|---|---|---|
| `row-major` | `<n>` | Producto matriz-vector `y = A * x` con bucles `i` (externo) y `j` (interno), recorriendo `A` por filas. |
| `column-major` | `<n>` | El mismo cálculo, con bucles `j` (externo) e `i` (interno), recorriendo `A` por columnas. |
| `multiply-matrix` | `<n>` | Multiplicación clásica `C = A * B` con tres bucles anidados. |
| `multiply-matrix-blocked` | `<n> <bloque>` | Multiplicación por bloques `C = A * B` con seis bucles anidados. |

Donde:

- `n`: tamaño de la matriz cuadrada `n x n`.
- `bloque`: tamaño del bloque (por ejemplo `64`). Si `bloque` no divide a `n`, el último bloque
  se recorta con `std::min`.

Todas las matrices son cuadradas y se llenan con los valores `1..n²`; la creación y el llenado
no entran en la medición. Cada programa imprime una sola línea:

```text
elapsed_seconds=0.039888845
```

Ejemplos:

```bash
make run row-major 1024
make run column-major 1024
make run multiply-matrix 512
make run multiply-matrix-blocked 512 64
```

## Perfilado con Valgrind (Cachegrind) y KCachegrind

Primero se compila y luego se genera el perfil de cada binario:

```bash
make cachegrind                      # perfila todos los binarios
make cachegrind-multiply-matrix      # perfila solo uno
make cachegrind SIZE=512 BLOCK=64    # cambia el tamaño de las matrices
```

Los perfiles quedan en `profiles/cachegrind.out.<binario>`. Por defecto usa `SIZE=256` y
`BLOCK=64`. La simulación de caché se activa con `--cache-sim=yes`, que en Valgrind 3.24 viene
desactivada por defecto; se puede ampliar con:

```bash
make cachegrind CACHEGRIND_FLAGS="--cache-sim=yes --branch-sim=yes"
```

Al terminar, Valgrind imprime el resumen de cada corrida: referencias de instrucciones (`I refs`)
y accesos y fallos de datos (`D refs`, `D1 misses`, `LLd misses`). Esos son los valores para
comparar los algoritmos.

Para ver los resultados gráficamente:

```bash
make kcachegrind                      # abre todos los perfiles en una ventana
make kcachegrind-multiply-matrix      # abre el perfil de un binario
```

`make clean-profiles` borra la carpeta `profiles/`.
