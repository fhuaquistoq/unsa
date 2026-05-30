from collections import deque

import matplotlib
import matplotlib.pyplot as plt
import numpy as np


matplotlib.use("Agg")

EPSILON = 0.2
MIN_POINTS = 5
POINTS_PER_SHAPE = 333
NOISE = -1
UNCLASSIFIED = 0


def generate_points():
    np.random.seed()

    # Forma 1: luna creciente
    t1 = np.linspace(0, np.pi, POINTS_PER_SHAPE)
    x1 = np.cos(t1) + np.random.normal(0, 0.1, len(t1))
    y1 = np.sin(t1) + np.random.normal(0, 0.1, len(t1))

    # Forma 2: espiral
    t2 = np.linspace(0, 4 * np.pi, POINTS_PER_SHAPE)
    r = 0.1 * t2
    x2 = r * np.cos(t2) + 3 + np.random.normal(0, 0.05, len(t2))
    y2 = r * np.sin(t2) + np.random.normal(0, 0.05, len(t2))

    # Forma 3: curva seno, incluye coordenadas negativas
    x3 = np.linspace(-3, 3, POINTS_PER_SHAPE)
    y3 = np.sin(2 * x3) - 3 + np.random.normal(0, 0.1, len(x3))

    x = np.concatenate([x1, x2, x3])
    y = np.concatenate([y1, y2, y3])

    plt.figure(figsize=(10, 8))
    plt.scatter(x, y, s=12, color="blue", label="Puntos generados")
    plt.title("Puntos generados para DBSCAN")
    plt.xlabel("X")
    plt.ylabel("Y")
    plt.grid(True)
    plt.legend(loc="best")
    plt.tight_layout()
    plt.savefig("puntos_generados.png", dpi=300)

    return np.column_stack((x, y))


def squared_distance(point_a, point_b):
    dx = float(point_a[0]) - float(point_b[0])
    dy = float(point_a[1]) - float(point_b[1])
    return dx * dx + dy * dy


def get_neighbors(points, point_index, epsilon):
    eps_squared = epsilon * epsilon
    return [
        idx
        for idx, point in enumerate(points)
        if squared_distance(points[point_index], point) <= eps_squared
    ]


def print_assignments(iteration, action, labels, points):
    print(f"Iteracion {iteration}: {action}")
    print("Puntos (id - x - y - grupo):")
    for idx, point in enumerate(points):
        label = labels[idx]
        group = (
            "SIN_ASIGNAR"
            if label == UNCLASSIFIED
            else "RUIDO"
            if label == NOISE
            else label
        )
        print(f"\t{idx + 1}\t{float(point[0]):.6f}\t{float(point[1]):.6f}\t{group}")
    print("\n---\n")


def expand_cluster(
    points, labels, visited, seed_index, neighbors, cluster_id, epsilon, min_points
):
    labels[seed_index] = cluster_id
    seeds = deque(neighbors)

    while seeds:
        current_index = seeds.popleft()

        if not visited[current_index]:
            visited[current_index] = True
            current_neighbors = get_neighbors(points, current_index, epsilon)

            if len(current_neighbors) >= min_points:
                for neighbor_index in current_neighbors:
                    if neighbor_index not in seeds:
                        seeds.append(neighbor_index)

        if labels[current_index] in (UNCLASSIFIED, NOISE):
            labels[current_index] = cluster_id


def dbscan(points, epsilon, min_points):
    labels = [UNCLASSIFIED] * len(points)
    visited = [False] * len(points)
    cluster_id = 0
    iteration = 0

    print("DBSCAN")
    print(f"Total de puntos: {len(points)}")
    print(f"Epsilon: {epsilon}")
    print(f"minPoints: {min_points}")
    print("\n=== Puntos generados ===")
    print("id\tx\ty")
    for idx, point in enumerate(points):
        print(f"{idx + 1}\t{float(point[0]):.6f}\t{float(point[1]):.6f}")
    print("\n=== Asignacion de grupos por iteracion ===\n")

    for point_index in range(len(points)):
        if visited[point_index]:
            continue

        visited[point_index] = True
        neighbors = get_neighbors(points, point_index, epsilon)

        if len(neighbors) < min_points:
            labels[point_index] = NOISE
            action = (
                f"Punto {point_index + 1} marcado como RUIDO "
                f"({len(neighbors)} vecinos < minPoints)"
            )
        else:
            cluster_id += 1
            expand_cluster(
                points,
                labels,
                visited,
                point_index,
                neighbors,
                cluster_id,
                epsilon,
                min_points,
            )
            action = (
                f"Cluster {cluster_id} expandido desde punto {point_index + 1} "
                f"({len(neighbors)} vecinos iniciales)"
            )

        iteration += 1
        print_assignments(iteration, action, labels, points)

    print("=== Resumen final ===")
    print(f"Clusters encontrados: {cluster_id}")
    print(f"Outliers encontrados: {sum(1 for label in labels if label == NOISE)}")
    for current_cluster in range(1, cluster_id + 1):
        amount = sum(1 for label in labels if label == current_cluster)
        print(f"Cluster {current_cluster}: {amount} puntos")

    return labels, cluster_id


def save_plot(points, labels, cluster_count, output_path="dbscan_grafico.png"):
    plt.figure(figsize=(10, 8))
    cmap = plt.get_cmap("tab10", max(cluster_count, 1))

    for cluster_id in range(1, cluster_count + 1):
        cluster_points = np.array(
            [points[idx] for idx, label in enumerate(labels) if label == cluster_id]
        )
        if len(cluster_points) > 0:
            plt.scatter(
                cluster_points[:, 0],
                cluster_points[:, 1],
                s=12,
                color=cmap(cluster_id - 1),
                label=f"Cluster {cluster_id}",
            )

    noise_points = np.array(
        [points[idx] for idx, label in enumerate(labels) if label == NOISE]
    )
    if len(noise_points) > 0:
        plt.scatter(
            noise_points[:, 0],
            noise_points[:, 1],
            s=18,
            c="black",
            marker="x",
            label="Outliers",
        )

    plt.title(f"DBSCAN (eps={EPSILON}, minPoints={MIN_POINTS})")
    plt.xlabel("X")
    plt.ylabel("Y")
    plt.grid(True)
    plt.legend(loc="best")
    plt.tight_layout()
    plt.savefig(output_path, dpi=300)
    plt.close()


def main():
    points = generate_points()
    labels, cluster_count = dbscan(points, EPSILON, MIN_POINTS)
    save_plot(points, labels, cluster_count)


if __name__ == "__main__":
    main()
