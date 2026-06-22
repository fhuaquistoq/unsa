from __future__ import annotations

import csv
import math
import random
from collections import Counter, defaultdict
from dataclasses import dataclass
from pathlib import Path


K = math.floor(math.sqrt(50))
CSV_FILE = "fruit.csv"

FEATURES = ("mass", "width", "height", "color_score")


@dataclass(frozen=True)
class Fruit:
    index: int
    fruit_name: str
    mass: float
    width: float
    height: float
    color_score: float

    def feature_vector(self) -> tuple[float, float, float, float]:
        return (self.mass, self.width, self.height, self.color_score)


def load_fruits(csv_path: Path) -> list[Fruit]:
    fruits: list[Fruit] = []
    with csv_path.open("r", encoding="utf-8", newline="") as file:
        reader = csv.DictReader(file)
        for index, row in enumerate(reader, start=1):
            fruits.append(
                Fruit(
                    index=index,
                    fruit_name=row["fruit_name"],
                    mass=float(row["mass"]),
                    width=float(row["width"]),
                    height=float(row["height"]),
                    color_score=float(row["color_score"]),
                )
            )
    return fruits


def proportional_split(
    fruits: list[Fruit], rng: random.Random, train_ratio: float = 0.8
) -> tuple[list[Fruit], list[Fruit]]:
    grouped: dict[str, list[Fruit]] = defaultdict(list)
    for fruit in fruits:
        grouped[fruit.fruit_name].append(fruit)

    total_count = len(fruits)
    target_test_total = total_count - round(total_count * train_ratio)
    if target_test_total <= 0:
        target_test_total = 1

    class_names = sorted(grouped)
    quotas: dict[str, float] = {
        class_name: len(grouped[class_name]) * (1 - train_ratio)
        for class_name in class_names
    }
    test_counts = {
        class_name: math.floor(quota) for class_name, quota in quotas.items()
    }

    remaining = target_test_total - sum(test_counts.values())
    tie_breakers = {class_name: rng.random() for class_name in class_names}
    ordered_remainders = sorted(
        class_names,
        key=lambda class_name: (
            quotas[class_name] - test_counts[class_name],
            tie_breakers[class_name],
        ),
        reverse=True,
    )

    for class_name in ordered_remainders[:remaining]:
        test_counts[class_name] += 1

    training_set: list[Fruit] = []
    test_set: list[Fruit] = []

    for class_name in class_names:
        class_fruits = grouped[class_name][:]
        rng.shuffle(class_fruits)
        test_count = test_counts[class_name]
        test_set.extend(class_fruits[:test_count])
        training_set.extend(class_fruits[test_count:])

    training_set.sort(key=lambda fruit: fruit.index)
    test_set.sort(key=lambda fruit: fruit.index)
    return training_set, test_set


def build_normalization(training_set: list[Fruit]) -> dict[str, tuple[float, float]]:
    normalization: dict[str, tuple[float, float]] = {}
    for feature in FEATURES:
        values = [getattr(fruit, feature) for fruit in training_set]
        normalization[feature] = (min(values), max(values))
    return normalization


def normalize_value(value: float, minimum: float, maximum: float) -> float:
    if math.isclose(minimum, maximum):
        return 0.0
    return (value - minimum) / (maximum - minimum)


def euclidean_distance(
    unknown: Fruit, known: Fruit, normalization: dict[str, tuple[float, float]]
) -> float:
    squared_sum = 0.0
    for feature in FEATURES:
        min_value, max_value = normalization[feature]
        unknown_value = normalize_value(getattr(unknown, feature), min_value, max_value)
        known_value = normalize_value(getattr(known, feature), min_value, max_value)
        squared_sum += (unknown_value - known_value) ** 2
    return math.sqrt(squared_sum)


def classify(
    unknown: Fruit,
    training_set: list[Fruit],
    k: int,
    normalization: dict[str, tuple[float, float]],
) -> tuple[list[dict[str, object]], list[dict[str, object]], str]:
    comparisons: list[dict[str, object]] = []
    for known in training_set:
        comparisons.append(
            {
                "fruit": known,
                "distance": euclidean_distance(unknown, known, normalization),
            }
        )

    comparisons.sort(key=lambda item: (item["distance"], item["fruit"].index))
    neighbors = comparisons[:k]

    votes = Counter(item["fruit"].fruit_name for item in neighbors)
    prediction = sorted(
        votes.items(),
        key=lambda item: (
            item[1],
            -min(
                neighbor["distance"]
                for neighbor in neighbors
                if neighbor["fruit"].fruit_name == item[0]
            ),
            item[0],
        ),
        reverse=True,
    )[0][0]

    return comparisons, neighbors, prediction


def format_number(value: float) -> str:
    if float(value).is_integer():
        return str(int(value))
    return f"{value:.4f}".rstrip("0").rstrip(".")


def print_separator(char: str = "=", width: int = 110) -> None:
    print(char * width)


def print_table(title: str, headers: list[str], rows: list[list[object]]) -> None:
    print(title)
    string_rows = [[str(cell) for cell in row] for row in rows]
    widths = [len(header) for header in headers]
    for row in string_rows:
        for index, cell in enumerate(row):
            widths[index] = max(widths[index], len(cell))

    header_line = " | ".join(
        header.ljust(widths[index]) for index, header in enumerate(headers)
    )
    separator_line = "-+-".join("-" * widths[index] for index in range(len(headers)))

    print(header_line)
    print(separator_line)
    for row in string_rows:
        print(" | ".join(cell.ljust(widths[index]) for index, cell in enumerate(row)))
    print()


def fruit_row(fruit: Fruit) -> list[object]:
    return [
        fruit.index,
        fruit.fruit_name,
        format_number(fruit.mass),
        format_number(fruit.width),
        format_number(fruit.height),
        format_number(fruit.color_score),
    ]


def comparison_row(comparison: dict[str, object]) -> list[object]:
    fruit = comparison["fruit"]
    return fruit_row(fruit) + [format_number(comparison["distance"])]


def print_header(csv_path: Path) -> None:
    print_separator()
    print("LABORATORIO 08 - K-NN")
    print_separator()
    print("Alumno: Fabricio Jesús Huaquisto Quispe")
    print("Algoritmo: K-Nearest Neighbors (K-NN)")
    print("Métrica de distancia: Euclidiana")
    print("Normalización: Min-Max usando solo el conjunto de entrenamiento")
    print(f"Archivo fuente: {csv_path}")
    print(f"Valor de K: {K}")
    print()


def print_dataset_summary(training_set: list[Fruit], test_set: list[Fruit]) -> None:
    training_counts = Counter(fruit.fruit_name for fruit in training_set)
    test_counts = Counter(fruit.fruit_name for fruit in test_set)

    print_table(
        "Resumen de partición",
        ["Conjunto", "Total", "apple", "orange", "lemon"],
        [
            [
                "Entrenamiento",
                len(training_set),
                training_counts.get("apple", 0),
                training_counts.get("orange", 0),
                training_counts.get("lemon", 0),
            ],
            [
                "Test",
                len(test_set),
                test_counts.get("apple", 0),
                test_counts.get("orange", 0),
                test_counts.get("lemon", 0),
            ],
        ],
    )

    headers = ["ID", "Clase", "Mass", "Width", "Height", "Color Score"]
    print_table(
        "Conjunto de entrenamiento",
        headers,
        [fruit_row(fruit) for fruit in training_set],
    )
    print_table("Conjunto de test", headers, [fruit_row(fruit) for fruit in test_set])


def print_test_case(
    test_number: int,
    unknown: Fruit,
    comparisons: list[dict[str, object]],
    neighbors: list[dict[str, object]],
    prediction: str,
) -> bool:
    print_separator()
    print(f"TEST #{test_number}")
    print_separator()

    print_table(
        "Fruta desconocida",
        ["ID", "Clase Real", "Mass", "Width", "Height", "Color Score"],
        [
            [
                unknown.index,
                unknown.fruit_name,
                format_number(unknown.mass),
                format_number(unknown.width),
                format_number(unknown.height),
                format_number(unknown.color_score),
            ]
        ],
    )

    print_table(
        "Comparación contra todo el conjunto de entrenamiento",
        ["ID", "Clase", "Mass", "Width", "Height", "Color Score", "Distancia"],
        [comparison_row(comparison) for comparison in comparisons],
    )

    print_table(
        "K vecinos más cercanos",
        ["ID", "Clase", "Mass", "Width", "Height", "Color Score", "Distancia"],
        [comparison_row(comparison) for comparison in neighbors],
    )

    is_correct = prediction == unknown.fruit_name
    print(f"Clase real: {unknown.fruit_name}")
    print(f"Clase estimada: {prediction}")
    print(f"¿Predicción correcta?: {'Sí' if is_correct else 'No'}")
    print()
    return is_correct


def main() -> None:
    csv_path = Path(CSV_FILE)
    fruits = load_fruits(csv_path)
    if K > len(fruits):
        raise ValueError("K no puede ser mayor que la cantidad total de elementos.")

    rng = random.Random()
    training_set, test_set = proportional_split(fruits, rng, train_ratio=0.8)
    if K > len(training_set):
        raise ValueError(
            "K no puede ser mayor que el tamaño del conjunto de entrenamiento."
        )

    normalization = build_normalization(training_set)

    print_header(csv_path)
    print_dataset_summary(training_set, test_set)

    hits = 0
    for test_number, unknown in enumerate(test_set, start=1):
        comparisons, neighbors, prediction = classify(
            unknown, training_set, K, normalization
        )
        if print_test_case(test_number, unknown, comparisons, neighbors, prediction):
            hits += 1

    accuracy = (hits / len(test_set)) * 100 if test_set else 0.0

    print_separator()
    print("RESULTADO FINAL")
    print_separator()
    print(f"Total de elementos evaluados: {len(test_set)}")
    print(f"Cantidad de aciertos: {hits}")
    print(f"Cantidad de errores: {len(test_set) - hits}")
    print(f"Porcentaje de acierto: {accuracy:.2f}%")


if __name__ == "__main__":
    main()
