import numpy as np
import matplotlib.pyplot as plt


N = 123
LEARNING_RATE = 0.01
ITERATIONS = 100000

PENDIENTE_INICIAL = -0.9874
INTERCEPTO_INICIAL = -2.1789

GRAFICO_SALIDA = "regresion_logistica.png"


def sigmoid(z):
    z = np.clip(z, -500, 500)
    return 1.0 / (1.0 + np.exp(-z))


def binary_cross_entropy(y, y_hat):
    eps = 1e-15
    y_hat = np.clip(y_hat, eps, 1.0 - eps)
    return -np.mean(y * np.log(y_hat) + (1 - y) * np.log(1 - y_hat))


def generar_datos():
    x = np.random.uniform(0, 20, N)

    beta0 = -5
    beta1 = 0.5

    p = sigmoid(beta0 + beta1 * x)
    y = np.random.binomial(1, p)

    return x, y


def imprimir_datos(x, y):
    print("DATOS UTILIZADOS")
    print("i\tX\tY")
    for i in range(len(x)):
        print(f"{i + 1}\t{x[i]:.6f}\t{y[i]:.6f}")
    print()


def entrenar_regresion_logistica(x, y):
    m = PENDIENTE_INICIAL
    b = INTERCEPTO_INICIAL

    print("ITERACIONES")
    print(
        "Iteracion\t"
        "Pendiente anterior\t"
        "Intercepto anterior\t"
        "Error (Entropia Cruzada Binaria)\t"
        "Tasa de aprendizaje\t"
        "Derivada pendiente\t"
        "Derivada intercepto\t"
        "Pendiente nueva\t"
        "Intercepto nuevo"
    )

    for iteration in range(1, ITERATIONS + 1):
        m_anterior = m
        b_anterior = b

        z = m_anterior * x + b_anterior
        y_hat = sigmoid(z)

        error = binary_cross_entropy(y, y_hat)

        derivada_pendiente = np.mean((y_hat - y) * x)
        derivada_intercepto = np.mean(y_hat - y)

        m = m_anterior - LEARNING_RATE * derivada_pendiente
        b = b_anterior - LEARNING_RATE * derivada_intercepto

        print(f"Iteración {iteration}")
        print(f"Pendiente anterior = {m_anterior:.6f}")
        print(f"Intercepto anterior = {b_anterior:.6f}")
        print(f"Error = {error:.6f}")
        print(f"Tasa de aprendizaje = {LEARNING_RATE:.6f}")
        print(f"Derivada pendiente = {derivada_pendiente:.6f}")
        print(f"Derivada intercepto = {derivada_intercepto:.6f}")
        print(f"Pendiente nueva = {m:.6f}")
        print(f"Intercepto nuevo = {b:.6f}")
        print()

    return m, b


def generar_grafico(x, y, pendiente, intercepto):
    x_line = np.linspace(min(x), max(x), 300)
    y_line = sigmoid(pendiente * x_line + intercepto)

    plt.figure(figsize=(8, 5))
    plt.scatter(x, y, label="Datos generados")
    plt.plot(x_line, y_line, label="Regresion logistica obtenida")
    plt.xlabel("Tiempo en el sitio web (min)")
    plt.ylabel("Compra (0=No, 1=Si)")
    plt.title("Datos para Regresion Logistica")
    plt.grid(True)
    plt.legend()
    plt.savefig(GRAFICO_SALIDA, dpi=300, bbox_inches="tight")
    plt.close()


def main():
    x, y = generar_datos()

    print("Alumno\tFabricio Jesus Huaquisto Quispe")
    print("Metodo utilizado\tGradiente Descendiente")
    print(f"Cantidad de datos\t{N}")
    print(f"Tasa de aprendizaje\t{LEARNING_RATE:.6f}")
    print(f"Iteraciones\t{ITERATIONS}")
    print(f"Pendiente inicial\t{PENDIENTE_INICIAL:.6f}")
    print(f"Intercepto inicial\t{INTERCEPTO_INICIAL:.6f}")
    print()

    imprimir_datos(x, y)

    pendiente_final, intercepto_final = entrenar_regresion_logistica(x, y)

    print()
    print("RESULTADO FINAL")
    print("Pendiente final\tIntercepto final")
    print(f"{pendiente_final:.6f}\t{intercepto_final:.6f}")

    generar_grafico(x, y, pendiente_final, intercepto_final)


if __name__ == "__main__":
    main()
