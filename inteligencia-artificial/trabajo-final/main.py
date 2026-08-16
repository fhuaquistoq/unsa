"""Regresion lineal multiple para curvas IDF, siguiendo la tesis analizada.

La ecuacion de Sherman usada en la tesis es:

    I = k * T**m / (d + c)**n

Al aplicar logaritmos naturales se obtiene un modelo lineal:

    ln(I) = ln(k) + m*ln(T) + n*(-ln(d + c))

Por tanto, el descenso de gradiente se ejecuta sobre ln(I), no sobre I
directamente. Las intensidades corresponden a la tabla 30 de la tesis.
"""

import math


# Tabla 30 de la tesis: intensidades (mm/h) para d=5, 10, ..., 60 min.
RETURN_PERIODS = (2, 5, 10, 20, 25, 50, 100, 500)
DURATIONS_MIN = tuple(range(5, 65, 5))
INTENSITIES = (
    (46.05, 30.04, 23.39, 19.59, 17.08, 15.26, 13.88, 12.78, 11.89, 11.14, 10.50, 9.95),
    (50.52, 32.96, 25.67, 21.50, 18.74, 16.74, 15.23, 14.02, 13.04, 12.22, 11.52, 10.92),
    (54.20, 35.35, 27.54, 23.06, 20.10, 17.96, 16.33, 15.04, 13.99, 13.11, 12.36, 11.72),
    (58.14, 37.93, 29.54, 24.74, 21.56, 19.27, 17.52, 16.14, 15.01, 14.06, 13.26, 12.57),
    (59.47, 38.79, 30.21, 25.30, 22.05, 19.71, 17.92, 16.51, 15.35, 14.39, 13.56, 12.86),
    (63.80, 41.61, 32.41, 27.15, 23.66, 21.14, 19.23, 17.71, 16.47, 15.43, 14.55, 13.79),
    (68.44, 44.64, 34.77, 29.12, 25.38, 22.68, 20.62, 18.99, 17.66, 16.55, 15.61, 14.79),
    (80.55, 52.55, 40.93, 34.28, 29.87, 26.70, 24.28, 22.36, 20.79, 19.48, 18.37, 17.41),
)


def build_dataset(c=0.0):
    """Construye X e y con la transformacion logaritmica de la tesis."""
    if any(duration + c <= 0 for duration in DURATIONS_MIN):
        raise ValueError("c debe cumplir d + c > 0 para todas las duraciones")

    X = []
    y = []
    for row, return_period in zip(INTENSITIES, RETURN_PERIODS):
        for duration, intensity in zip(DURATIONS_MIN, row):
            X.append([
                1.0,
                math.log(return_period),
                -math.log(duration + c),
            ])
            y.append(math.log(intensity))
    return X, y


def predict_log(X, weights):
    return [sum(x * weight for x, weight in zip(row, weights)) for row in X]


def compute_cost(X, y, weights):
    """J(theta) = 1/(2N) * sum((y_hat - y)^2), sobre log(I)."""
    errors = [prediction - target for prediction, target in zip(predict_log(X, weights), y)]
    return sum(error * error for error in errors) / (2 * len(y))


def gradient_descent(X, y, learning_rate=0.05, epochs=10000):
    sample_count = len(y)
    weights = [0.0] * len(X[0])

    for epoch in range(epochs):
        errors = [prediction - target for prediction, target in zip(predict_log(X, weights), y)]
        gradients = [
            sum(error * row[index] for error, row in zip(errors, X)) / sample_count
            for index in range(len(weights))
        ]
        weights = [weight - learning_rate * gradient for weight, gradient in zip(weights, gradients)]

        # if epoch % 1000 == 0 or epoch == epochs - 1:
        print(f"Iteracion {epoch:05d} | costo(log) = {compute_cost(X, y, weights):.10f}")

    return weights


def coefficient_of_determination(actual, predicted):
    mean = sum(actual) / len(actual)
    total = sum((value - mean) ** 2 for value in actual)
    residual = sum((value - estimate) ** 2 for value, estimate in zip(actual, predicted))
    return 1.0 - residual / total if total else 1.0


def intensity(return_period, duration_min, k, m, n, c=0.0):
    """Evalua I = k*T^m/(d+c)^n en mm/h."""
    return k * return_period**m / (duration_min + c) ** n


def main():
    # La tesis usa d directamente en su ecuacion final, por eso c=0.
    c = 0.0
    X, y_log = build_dataset(c)

    print("Regresion lineal multiple para curvas IDF")
    print(f"Datos: {len(y_log)} | duraciones: {DURATIONS_MIN} min")
    print(f"Periodos de retorno: {RETURN_PERIODS} anos | c = {c}")
    print("Modelo lineal: ln(I) = theta0 + theta1*ln(T) + theta2*(-ln(d+c))")
    print("-" * 70)

    weights = gradient_descent(X, y_log)
    log_k, m, n = weights
    k = math.exp(log_k)

    predicted_log = predict_log(X, weights)
    predicted = [math.exp(value) for value in predicted_log]
    actual = [math.exp(value) for value in y_log]

    print("-" * 70)
    print("Parametros estimados")
    print(f"ln(k) = {log_k:.8f}")
    print(f"k     = {k:.8f}   (tesis: 115.7558)")
    print(f"m     = {m:.8f}   (tesis: 0.101290)")
    print(f"n     = {n:.8f}   (tesis: 0.61639)")
    print(f"R^2   = {coefficient_of_determination(actual, predicted):.8f}")
    print(f"I(T=20, d=60 min) = {intensity(20, 60, k, m, n, c):.4f} mm/h")
    print("Ecuacion: I = {:.6f} * T^{:.6f} / (d + {:.1f})^{:.6f}".format(k, m, c, n))


if __name__ == "__main__":
    main()
