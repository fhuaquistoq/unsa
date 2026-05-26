import matplotlib.pyplot as plt
import numpy as np

from sklearn.datasets import make_blobs

N = 5000
K = 10
CLUSTER_STD = 1.2
BOX_SIZE = 10


def k_means(points):
    centers = np.random.uniform(-BOX_SIZE, BOX_SIZE, size=(K, 2))

    iter_count = 0

    while True:
        groups = []

        for point in points:
            px, py = float(point[0]), float(point[1])
            best_idx = min(
                range(len(centers)),
                key=lambda i: (px - centers[i][0]) ** 2 + (py - centers[i][1]) ** 2,
            )
            groups.append(best_idx + 1)

        new_centers = []
        for i in range(1, K + 1):
            cluster_points = [points[idx] for idx, g in enumerate(groups) if g == i]
            if cluster_points:
                sum_x = sum(float(p[0]) for p in cluster_points)
                sum_y = sum(float(p[1]) for p in cluster_points)
                count = len(cluster_points)
                new_center = [sum_x / count, sum_y / count]
            else:
                new_center = np.random.uniform(-BOX_SIZE, BOX_SIZE, size=2)
            new_centers.append(new_center)

        new_centers = np.array(new_centers)

        iter_count += 1
        print(f"Iteracion {iter_count}")
        print("Centroides:")
        for i, c in enumerate(centers):
            print(f"\t{i + 1}: {float(c[0]):.6f}\t{float(c[1]):.6f}")
        print("Puntos (id - x - y - grupo):")
        for idx, p in enumerate(points):
            gx = float(p[0])
            gy = float(p[1])
            g = groups[idx]
            print(f"\t{idx + 1}\t{gx:.6f}\t{gy:.6f}\t{g}")
        print("\n---\n")

        if np.all(centers == new_centers):
            print(f"Convegio en {iter_count} iteraciones.")
            break

        centers = new_centers

    return centers, groups


def main():
    points, _ = make_blobs(
        n_samples=N,
        centers=K,
        cluster_std=CLUSTER_STD,
        center_box=(-BOX_SIZE, BOX_SIZE),
    )

    centers, groups_result = k_means(points)

    cmap = plt.get_cmap("viridis", K)
    color_indices = [g - 1 for g in groups_result]

    plt.scatter(points[:, 0], points[:, 1], c=color_indices, s=1, cmap=cmap)
    plt.scatter(
        centers[:, 0],
        centers[:, 1],
        c="red",
        marker="X",
        s=50,
    )

    img_path = "kmeans_grafico.png"
    plt.savefig(img_path, dpi=300)

    plt.show()


if __name__ == "__main__":
    main()
