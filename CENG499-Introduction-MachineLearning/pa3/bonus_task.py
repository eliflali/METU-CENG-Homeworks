import numpy as np
import pandas as pd
import matplotlib.pyplot as plt
from sklearn.decomposition import PCA

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

try:
    from task4 import KMeans 
except ImportError:
    print("task4.py import error")
    exit()

try:
    from task5 import calculate_inertia, calculate_silhouette_score_scratch
except ImportError:
    print("task5.py import error")
    exit()

if __name__ == '__main__':
    OPTIMAL_K = 3 
    print(f"BONUS TASK: OPTIMAL_K = {OPTIMAL_K}")

    csv_file = 'Mall_Customers.csv'
    normalized_data = load_and_preprocess_data(csv_file)

    if normalized_data is None:
        print("data loading/preprocessing failed")
        exit()

    print("\nSPLITTING DATA")
    features = normalized_data.columns.tolist()
    chosen_seed, _ = find_optimal_split_seed(normalized_data, features, max_iterations=100)
    print(f"using seed {chosen_seed}")
    
    train_df, _, test_df = split_data(normalized_data, chosen_seed)  

    X_train_np = train_df.values
    X_test_np = test_df.values

    if X_test_np.shape[0] == 0:
        print("test set is empty")
        exit()

    distance_metrics_to_test = [
        {'name': 'Euclidean', 'param': 'euclidean'},
        {'name': 'Manhattan', 'param': 'manhattan'},
        {'name': 'Chebyshev', 'param': 'chebyshev'},
        {'name': 'Minkowski (p=3)', 'param': 'minkowski', 'minkowski_p': 3}
    ]

    results = []

    for metric_info in distance_metrics_to_test:
        metric_name = metric_info['name']
        metric_param = metric_info['param']
        minkowski_p_val = metric_info.get('minkowski_p', 3)
        
        print(f"\nEVALUATION WITH {metric_name}")
        
        kmeans_model = KMeans(k=OPTIMAL_K, 
                                max_iters=100, 
                                random_state=chosen_seed, 
                                metric=metric_param,
                                minkowski_p=minkowski_p_val)
        
        print(f"TRAINING with {metric_name}")
        kmeans_model.fit(X_train_np)
        print("training complete")

        if kmeans_model.centroids is None:
            print(f"centroids None {metric_name}")
            results.append({'metric': metric_name, 'inertia': np.nan, 'silhouette': np.nan})
            continue

        print(f"EVALUATION ON TEST SET {metric_name}")
        test_labels = kmeans_model.predict(X_test_np)
        
        current_inertia = calculate_inertia(X_test_np, test_labels, kmeans_model.centroids)
        print(f"  Inertia for {metric_name}: {current_inertia:.4f}")

        unique_labels_in_test = np.unique(test_labels)
        if len(unique_labels_in_test) < 2 or len(unique_labels_in_test) >= X_test_np.shape[0]:
            current_silhouette = -1
            print(f"  Silhouette Score (Scratch) for {metric_name}: not well-defined labels: {len(unique_labels_in_test)}")
        else:
            current_silhouette = calculate_silhouette_score_scratch(X_test_np, test_labels, 
                                                                    len(unique_labels_in_test), 
                                                                    kmeans_model.distance_func)
            print(f"  Silhouette Score (Scratch) for {metric_name}: {current_silhouette:.4f}")
        
        results.append({'metric': metric_name, 'inertia': current_inertia, 'silhouette': current_silhouette})

        print(f"VISUALIZING CLUSTERS FOR {metric_name}")
        pca = PCA(n_components=2, random_state=chosen_seed)
        X_test_pca = pca.fit_transform(X_test_np)
        centroids_pca = pca.transform(kmeans_model.centroids)

        plt.figure(figsize=(10, 7))
        scatter = plt.scatter(X_test_pca[:, 0], X_test_pca[:, 1], c=test_labels, cmap='viridis', alpha=0.7, edgecolors='k')
        plt.scatter(centroids_pca[:, 0], centroids_pca[:, 1], marker='X', s=200, color='red', edgecolors='black', label='Centroids')
        plt.title(f'K-Means Clusters (K={OPTIMAL_K}) on Test Set - {metric_name} Distance (PCA-reduced)')
        plt.xlabel('Principal Component 1')
        plt.ylabel('Principal Component 2')
        plt.legend(handles=scatter.legend_elements()[0] + [plt.Line2D([0], [0], marker='X', color='w', label='Centroids', markerfacecolor='red', markersize=10)], title="Clusters")
        plt.grid(True)
        plt.show()

    print("\n--- SUMMARY OF BONUS TASK RESULTS (K={OPTIMAL_K}) ---")
    results_df = pd.DataFrame(results)
    print(results_df)

    print("\nBONUS TASK COMPLETE")
