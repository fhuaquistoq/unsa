import matplotlib
from sklearn import datasets

matplotlib.use("Agg")
import matplotlib.pyplot as plt


N = 100
LEARNING_RATE = 0.0001
ITERATIONS = 1000
INITIAL_INTERCEPT = 0.0
INITIAL_SLOPE = 0.0


def format_number(value):
    return f"{value:.6f}".rstrip("0").rstrip(".")


def generate_data():
    X, Y = datasets.make_regression(n_samples=N, n_features=1, noise=20)
    return [(float(x[0]), float(y)) for x, y in zip(X, Y)]


def calculate_derivatives(data, intercept, slope):
    derivative_intercept = 0.0
    derivative_slope = 0.0

    for x, y in data:
        prediction = intercept + slope * x
        residual = y - prediction
        derivative_intercept += -2 * residual
        derivative_slope += -2 * x * residual

    return derivative_intercept, derivative_slope


def print_header(data):
    print("Nombres:")
    print("Fabricio Jesús Huaquisto Quispe")
    print()
    print("Gradiente Descendiente")
    print()
    print("X={" + ",".join(format_number(x) for x, _ in data) + "}")
    print("Y={" + ",".join(format_number(y) for _, y in data) + "}")
    print()


def print_initial_values(intercept, slope, learning_rate):
    print(f"Intercepto Inicial = {format_number(intercept)}")
    print(f"Pendiente Inicial = {format_number(slope)}")
    print(f"Tasa de Aprendizaje = {format_number(learning_rate)}")
    print()


def print_iteration(
    iteration,
    previous_intercept,
    previous_slope,
    learning_rate,
    derivative_intercept,
    derivative_slope,
    new_intercept,
    new_slope,
):
    print(f"Iteración {iteration}")
    print(f"Anterior Intercepto = {format_number(previous_intercept)}")
    print(f"Anterior Pendiente = {format_number(previous_slope)}")
    print(f"Derivada Intercepto = {format_number(derivative_intercept)}")
    print(f"Derivada Pendiente = {format_number(derivative_slope)}")
    print(f"Tasa de Aprendizaje = {format_number(learning_rate)}")
    print(f"Nuevo Intercepto = {format_number(new_intercept)}")
    print(f"Nuevo Pendiente = {format_number(new_slope)}")
    print()


def run_gradient_descent(data):
    intercept = INITIAL_INTERCEPT
    slope = INITIAL_SLOPE

    print_initial_values(intercept, slope, LEARNING_RATE)

    for iteration in range(1, ITERATIONS + 1):
        previous_intercept = intercept
        previous_slope = slope
        derivative_intercept, derivative_slope = calculate_derivatives(
            data,
            previous_intercept,
            previous_slope,
        )

        intercept = previous_intercept - LEARNING_RATE * derivative_intercept
        slope = previous_slope - LEARNING_RATE * derivative_slope

        print_iteration(
            iteration,
            previous_intercept,
            previous_slope,
            LEARNING_RATE,
            derivative_intercept,
            derivative_slope,
            intercept,
            slope,
        )

    return intercept, slope


def save_graph(data, intercept, slope):
    ordered_data = sorted(data)
    x_values = [x for x, _ in ordered_data]
    y_values = [y for _, y in ordered_data]
    regression_values = [intercept + slope * x for x in x_values]

    plt.figure(figsize=(8, 5))
    plt.scatter(x_values, y_values, label="Datos")
    plt.plot(x_values, regression_values, color="red", label="Recta de regresión")
    plt.xlabel("X")
    plt.ylabel("Y")
    plt.title("Gradiente Descendiente")
    plt.legend()
    plt.tight_layout()
    plt.savefig("gradiente_descendiente.png")
    plt.close()


def main():
    data = generate_data()
    print_header(data)
    intercept, slope = run_gradient_descent(data)
    save_graph(data, intercept, slope)


if __name__ == "__main__":
    main()
