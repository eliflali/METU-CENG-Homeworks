import numpy as np
import pandas as pd
import matplotlib.pyplot as plt
from sklearn.decomposition import PCA
from sklearn.metrics import silhouette_score as sklearn_silhouette_score

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
    print(f" TASK6 STARTING: OPTIMAL_K = {OPTIMAL_K}")

    csv_file = 'Mall_Customers.csv'
    normalized_data = load_and_preprocess_data(csv_file)

    if normalized_data is None:
        print("data loading fail")
        exit()

    print("\nSPLITTING DATA")
    features = normalized_data.columns.tolist()
    
    print("OPTIMAL SPLIT SEED")
    chosen_seed, _ = find_optimal_split_seed(normalized_data, features, max_iterations=100) 
    print(f"using seed:{chosen_seed}")
    
    train_df, validation_df, test_df = split_data(normalized_data, chosen_seed)

    X_train_np = train_df.values
    X_test_np = test_df.values

    if X_test_np.shape[0] == 0:
        print("test set is empty")
        exit()

    print(f"\TRAINING with K={OPTIMAL_K}")
    kmeans_final_model = KMeans(k=OPTIMAL_K, max_iters=100, random_state=chosen_seed) 
    kmeans_final_model.fit(X_train_np)
    print("training finished")

    print("\nEVALUATION")
    test_labels = kmeans_final_model.predict(X_test_np)
    final_centroids = kmeans_final_model.centroids

    if final_centroids is None:
        print("no final centroids")
        exit()

    test_inertia = calculate_inertia(X_test_np, test_labels, final_centroids)
    print(f"  Inertia on Test Set: {test_inertia}")

    temp_kmeans_for_dist = KMeans(k=2)
    
    unique_labels_in_test = np.unique(test_labels)
    if len(unique_labels_in_test) < 2 or len(unique_labels_in_test) >= X_test_np.shape[0]:
        scratch_silhouette_test = -1
        print(f"  Silhouette Score (Scratch) on Test Set: not well-defined unique labels: {len(unique_labels_in_test)}")
    else:
        scratch_silhouette_test = calculate_silhouette_score_scratch(X_test_np, test_labels, 
                                                                len(unique_labels_in_test),
                                                                temp_kmeans_for_dist._euclidean_distance)
        print(f"  Silhouette Score (Scratch) on Test Set: {scratch_silhouette_test}")

    if len(unique_labels_in_test) < 2 or len(unique_labels_in_test) >= X_test_np.shape[0]:
        sklearn_silhouette_test = -1
        print(f"  Silhouette Score (Scikit-learn) on Test Set: not well-defined unique labels: {len(unique_labels_in_test)}")
    else:
        sklearn_silhouette_test = sklearn_silhouette_score(X_test_np, test_labels)
        print(f"  Silhouette Score (Scikit-learn) on Test Set: {sklearn_silhouette_test}")

    print("\nVISUALIZATION")
    pca = PCA(n_components=2, random_state=chosen_seed)
    X_test_pca = pca.fit_transform(X_test_np)
    
    centroids_pca = pca.transform(final_centroids)

    plt.figure(figsize=(10, 7))
    scatter = plt.scatter(X_test_pca[:, 0], X_test_pca[:, 1], c=test_labels, cmap='viridis', alpha=0.7, edgecolors='k')
    
    plt.scatter(centroids_pca[:, 0], centroids_pca[:, 1], marker='X', s=200, color='red', edgecolors='black', label='Centroids')
    
    plt.title(f'K-Means Clusters on Test Set (K={OPTIMAL_K}, PCA-reduced)')
    plt.xlabel('Principal Component 1')
    plt.ylabel('Principal Component 2')
    plt.legend(handles=scatter.legend_elements()[0] + [plt.Line2D([0], [0], marker='X', color='w', label='Centroids', markerfacecolor='red', markersize=10)], title="Clusters")
    plt.grid(True)
    plt.show()

    print("\n TASK 6 COMPLETE")
