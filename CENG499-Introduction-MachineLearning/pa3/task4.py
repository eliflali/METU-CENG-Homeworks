import numpy as np
import pandas as pd

class KMeans:
    def __init__(self, k, max_iters=100, random_state=None, metric='euclidean', minkowski_p=3):
        self.k = k
        self.max_iters = max_iters
        self.random_state = random_state
        self.centroids = None
        self.metric_name = metric
        self.minkowski_p = minkowski_p

        if metric == 'euclidean':
            self.distance_func = self._euclidean_distance
        elif metric == 'manhattan':
            self.distance_func = self._manhattan_distance
        elif metric == 'chebyshev':
            self.distance_func = self._chebyshev_distance
        elif metric == 'minkowski':
            # p selectable!!
            self.distance_func = lambda p1, p2: self._minkowski_distance(p1, p2, p=self.minkowski_p)
        else:
            raise ValueError(f"distance metric {metric} false")

    def _euclidean_distance(self, point1, point2):
        return np.sqrt(np.sum((point1 - point2) ** 2))

    def _manhattan_distance(self, point1, point2):
        return np.sum(np.abs(point1 - point2))

    def _chebyshev_distance(self, point1, point2):
        return np.max(np.abs(point1 - point2))

    def _minkowski_distance(self, point1, point2, p):
        return np.sum(np.abs(point1 - point2) ** p) ** (1/p)

    def fit(self, X):
        if self.random_state is not None:
            np.random.seed(self.random_state)

        if isinstance(X, pd.DataFrame):
            X_np = X.values
        elif isinstance(X, np.ndarray):
            X_np = X
        else:
            raise ValueError("x must be DataFrame or np.ndarray --wrong")

        n_samples, _ = X_np.shape

        random_indices = np.random.choice(n_samples, self.k, replace=False)
        self.centroids = X_np[random_indices]

        for iteration in range(self.max_iters):
            clusters = [[] for _ in range(self.k)]
            
            for point in X_np:
                distances = [self.distance_func(point, centroid) for centroid in self.centroids]
                closest_centroid_idx = np.argmin(distances)
                clusters[closest_centroid_idx].append(point)
            
            old_centroids = np.copy(self.centroids)

            for i in range(self.k):
                if clusters[i]:
                    self.centroids[i] = np.mean(clusters[i], axis=0)

            if np.allclose(old_centroids, self.centroids):
                print(f"converged at iteration {iteration + 1} distance metric: {self.metric_name}")
                break
        else:
            print(f"max iteration reached: ({self.max_iters}) distance metric: {self.metric_name}")

    def predict(self, X):
        if self.centroids is None:
            raise ValueError("centroids not found")

        if isinstance(X, pd.DataFrame):
            X_np = X.values
        elif isinstance(X, np.ndarray):
            X_np = X
        else:
            raise ValueError("x must be DataFrame or np.ndarray --wrong")
            
        labels = np.zeros(X_np.shape[0], dtype=int)
        for i, point in enumerate(X_np):
            distances = [self.distance_func(point, centroid) for centroid in self.centroids]
            labels[i] = np.argmin(distances)
        
        return labels

if __name__ == '__main__': 
    try:
        from task1 import load_and_preprocess_data
    except ImportError:
        print("task1.py import error")
        exit()
    
    try:
        from task2 import split_data, find_optimal_split_seed 
    except ImportError:
        print("task2.py import error")
        exit()

    csv_file = 'Mall_Customers.csv'
    print(f"loading data:'{csv_file}'")
    normalized_data = load_and_preprocess_data(csv_file)

    if normalized_data is None:
        print("task4 failed")
        exit()

    if not isinstance(normalized_data, pd.DataFrame):
        print(f"task4 failed")
        exit()
        
    print("\nDATA SPLITTING")
    features = normalized_data.columns.tolist()
    
    print("OPTIMAL SPLIT SEED")
    chosen_seed, _ = find_optimal_split_seed(normalized_data, features)
    print(f"best seed: {chosen_seed}")
    
    train_df, validation_df, test_df = split_data(normalized_data, chosen_seed)
    print(f"DATA SPLIT COMPLETE")

    X_train_np = train_df.values

    k_for_demonstration = 5 
    print(f"\n--- K-Means from Scratch (K={k_for_demonstration}) with 4 metrics ---")
    
    metrics_to_test = ['euclidean', 'manhattan', 'chebyshev', 'minkowski']
    for metric_name in metrics_to_test:
        print(f"\ntesting with {metric_name} distance...")
        kmeans_scratch = KMeans(k=k_for_demonstration, max_iters=100, random_state=42, metric=metric_name)
        kmeans_scratch.fit(X_train_np)
        if kmeans_scratch.centroids is not None:
            print(f"final centroids (2) for {metric_name}:")
            print(kmeans_scratch.centroids[:2])
        train_labels_scratch = kmeans_scratch.predict(X_train_np)
        print(f"first 5 training labels for {metric_name}: {train_labels_scratch[:5]}")
    print("\ntask 4 finished")
