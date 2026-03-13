import numpy as np
import pandas as pd
import matplotlib.pyplot as plt

try:
    from task1 import load_and_preprocess_data
except ImportError:
    print("task1 import error")
    exit()

try:
    from task2 import split_data, find_optimal_split_seed 
except ImportError:
    print("task2 import error")
    exit()

try:
    from task4 import KMeans
except ImportError:
    print("task4 import error")
    exit()

def calculate_inertia(X_np, labels, centroids):
    inertia = 0.0
    for i, point in enumerate(X_np):
        centroid_of_point = centroids[labels[i]]
        inertia += np.sum((point - centroid_of_point) ** 2)
    return inertia

def calculate_silhouette_score_scratch(X_np, labels, k, distance_metric_func):
    n_samples = X_np.shape[0]
    if k <= 1 or k >= n_samples:
        return -1

    silhouette_coeffs = []
    for i in range(n_samples):
        point_i = X_np[i]
        label_i = labels[i]

        points_in_same_cluster = X_np[labels == label_i]
        if len(points_in_same_cluster) <= 1:
            a_i = 0
        else:
            a_i = np.mean([distance_metric_func(point_i, point_j) 
                           for idx_j, point_j in enumerate(points_in_same_cluster) 
                           if i != (np.where(labels == label_i)[0])[idx_j]])
            if len(points_in_same_cluster) == 1:
                 a_i = 0

        b_i_values = []
        for cluster_idx in range(k):
            if cluster_idx == label_i:
                continue
            
            points_in_other_cluster = X_np[labels == cluster_idx]
            if len(points_in_other_cluster) == 0:
                continue
            
            avg_dist_to_other_cluster = np.mean([distance_metric_func(point_i, point_j) 
                                                 for point_j in points_in_other_cluster])
            b_i_values.append(avg_dist_to_other_cluster)
        
        if not b_i_values:
            b_i = 0
        else:
            b_i = np.min(b_i_values)

        if max(a_i, b_i) == 0:
            s_i = 0
        else:
            s_i = (b_i - a_i) / max(a_i, b_i)
        silhouette_coeffs.append(s_i)
    
    return np.mean(silhouette_coeffs)


if __name__ == '__main__':
    csv_file = 'Mall_Customers.csv'
    print(f"loading data:'{csv_file}'")
    normalized_data = load_and_preprocess_data(csv_file)

    if normalized_data is None:
        print("task5 failed")
        exit()

    print("\nDATA SPLITTING")
    features = normalized_data.columns.tolist()
    
    print("OPTIMAL SPLIT SEED")
    chosen_seed, _ = find_optimal_split_seed(normalized_data, features, max_iterations=100)
    print(f"best seed: {chosen_seed}")
    
    train_df, validation_df, _ = split_data(normalized_data, chosen_seed)
    print(f"DATA SPLIT COMPLETE")

    X_train_np = train_df.values
    X_validation_np = validation_df.values

    if X_validation_np.shape[0] == 0:
        print("task5 failed")
        exit()

    k_range = range(2, 11)
    inertia_values = []
    silhouette_scores = []

    temp_kmeans_for_dist = KMeans(k=2) 

    print(f"\n--- Hyperparameter Tuning for K (K from {k_range.start} to {k_range.stop -1}) ---")
    for k_val in k_range:
        print(f"\n training k-means K={k_val}")
        kmeans_model = KMeans(k=k_val, max_iters=100, random_state=chosen_seed)
        kmeans_model.fit(X_train_np)
        
        train_labels = kmeans_model.predict(X_train_np)
        current_inertia = calculate_inertia(X_train_np, train_labels, kmeans_model.centroids)
        inertia_values.append(current_inertia)
        print(f"\n inertia for K={k_val}: {current_inertia:.4f}")

        if X_validation_np.shape[0] > 0:
            validation_labels = kmeans_model.predict(X_validation_np)
            unique_labels_in_validation = np.unique(validation_labels)
            if len(unique_labels_in_validation) < 2 or len(unique_labels_in_validation) >= X_validation_np.shape[0]:
                 current_silhouette = -1
                 print(f"\n  Silhouette Score for K={k_val} on validation set: not well-defined unique labels: {len(unique_labels_in_validation)}")
            else:
                current_silhouette = calculate_silhouette_score_scratch(X_validation_np, validation_labels, 
                                                                  len(unique_labels_in_validation),
                                                                  temp_kmeans_for_dist._euclidean_distance)
                print(f"\n  Silhouette Score for K={k_val} on validation set: {current_silhouette:.4f}")         
            silhouette_scores.append(current_silhouette)
        else:
            silhouette_scores.append(np.nan)
            print(f"\n  Silhouette Score for K={k_val} on validation set: EMPTY VALIDATION SET!!!")

    print("\nPLOTTING METRICS")
    plt.figure(figsize=(12, 5))

    plt.subplot(1, 2, 1)
    plt.plot(list(k_range), inertia_values, marker='o')
    plt.title('Inertia vs. Number of Clusters (K)')
    plt.xlabel('Number of Clusters (K)')
    plt.ylabel('Inertia (Sum of Squared Distances)')
    plt.xticks(list(k_range))
    plt.grid(True)

    plt.subplot(1, 2, 2)
    plt.plot(list(k_range), silhouette_scores, marker='o')
    plt.title('Silhouette Score vs. Number of Clusters (K)')
    plt.xlabel('Number of Clusters (K)')
    plt.ylabel('Average Silhouette Score')
    plt.xticks(list(k_range))
    plt.grid(True)

    plt.tight_layout()
    plt.show()

    print("\nTASK 5 COMPLETE")
    print("K values tested:", list(k_range))
    print("Inertia values:", [round(v, 2) for v in inertia_values])
    print("Silhouette scores:", [round(v, 4) for v in silhouette_scores])
