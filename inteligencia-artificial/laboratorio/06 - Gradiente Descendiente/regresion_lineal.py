import matplotlib
from sklearn import datasets

matplotlib.use("Agg")
import matplotlib.pyplot as plt


N = 100


def format_number(value):
    formatted = f"{value:.6f}".rstrip(".")
    return formatted if formatted else "0"


def generate_data():
    X, Y = datasets.make_regression(n_samples=N, n_features=1, noise=20)
    return [(float(x[0]), float(y)) for x, y in zip(X, Y)]


def calculate_sums(data):
    sum_x = sum(x for x, _ in data)
    sum_y = sum(y for _, y in data)
    sum_xy = sum(x * y for x, y in data)
    sum_x2 = sum(x**2 for x, _ in data)

    return sum_x, sum_y, sum_xy, sum_x2


def least_squares(data):
    sum_x, sum_y, sum_xy, sum_x2 = calculate_sums(data)
    count = len(data)
    denominator = count * sum_x2 - sum_x**2
    w1 = (count * sum_xy - sum_x * sum_y) / denominator
    w0 = (sum_y - w1 * sum_x) / count

    return sum_x, sum_y, sum_xy, sum_x2, w1, w0


def print_results(data, sum_x, sum_y, sum_xy, sum_x2, w1, w0):
    print("Nombres:")
    print("Fabricio Jesús Huaquisto Quispe")
    print()
    print("Regresión Lineal")
    print()
    print("Datos utilizados:")

    for x, y in data:
        print(f"{format_number(x)}\t{format_number(y)}")

    print()
    print()
    print(f"Pendiente: {format_number(w1)}")
    print(f"Intercepto: {format_number(w0)}")


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
    plt.title("Regresión Lineal")
    plt.legend()
    plt.tight_layout()
    plt.savefig("regresion_lineal.png")
    plt.close()


def main():
    data = generate_data()
    sum_x, sum_y, sum_xy, sum_x2, w1, w0 = least_squares(data)
    print_results(data, sum_x, sum_y, sum_xy, sum_x2, w1, w0)
    save_graph(data, w0, w1)


if __name__ == "__main__":
    main()
