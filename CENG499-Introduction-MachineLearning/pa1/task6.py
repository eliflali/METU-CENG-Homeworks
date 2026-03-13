import numpy as np
from task2 import DecisionTree
from task3 import dt
from task4 import reduced_error_pruning
from task5 import custom_pruning
from sklearn.datasets import load_wine
from sklearn.model_selection import train_test_split

wine = load_wine()
X = wine.data
y = wine.target

n_runs = 5
results = {
    'unpruned': [],
    'reduced_error': [],
    'custom_0.05': [],
    'custom_0.1': [],
    'custom_0.2': []
}

alphas = [0.05, 0.1, 0.2]

for i in range(n_runs):
    print(f"\nRun {i+1}/{n_runs}")
    
    #random seed
    seed = 42 + i
    X_train_val, X_test, y_train_val, y_test = train_test_split(X, y, test_size=0.2, random_state=seed)
    X_train, X_val, y_train, y_val = train_test_split(X_train_val, y_train_val, test_size=0.25, random_state=seed)
    
    # unpruned tree
    dt = DecisionTree()
    dt.fit(X_train, y_train)
    y_pred = dt.predict(X_test)
    accuracy = np.mean(y_pred == y_test)
    results['unpruned'].append(accuracy)
    
    # reduced error pruning
    dt_pruned = reduced_error_pruning(dt, X_val, y_val)
    y_pred_pruned = dt_pruned.predict(X_test)
    accuracy_pruned = np.mean(y_pred_pruned == y_test)
    results['reduced_error'].append(accuracy_pruned)
    
    # custom pruning 
    for alpha in alphas:
        dt_fresh = DecisionTree()
        dt_fresh.fit(X_train, y_train)
        dt_custom = custom_pruning(dt_fresh, X_train, y_train, X_val, y_val, alpha)
        y_pred_custom = dt_custom.predict(X_test)
        accuracy_custom = np.mean(y_pred_custom == y_test)
        results[f'custom_{alpha}'].append(accuracy_custom)
    
    print(f"Unpruned: {accuracy:.4f}, Reduced Error: {accuracy_pruned:.4f}, "
          f"Custom (0.05): {results['custom_0.05'][-1]:.4f}, "
          f"Custom (0.1): {results['custom_0.1'][-1]:.4f}, "
          f"Custom (0.2): {results['custom_0.2'][-1]:.4f}")

# average results
print("\nAverage results over", n_runs, "runs:")
print(f"Unpruned tree accuracy: {np.mean(results['unpruned']):.4f} ± {np.std(results['unpruned']):.4f}")
print(f"Reduced error pruned tree accuracy: {np.mean(results['reduced_error']):.4f} ± {np.std(results['reduced_error']):.4f}")
print(f"Custom pruned tree (alpha=0.05) accuracy: {np.mean(results['custom_0.05']):.4f} ± {np.std(results['custom_0.05']):.4f}")
print(f"Custom pruned tree (alpha=0.1) accuracy: {np.mean(results['custom_0.1']):.4f} ± {np.std(results['custom_0.1']):.4f}")
print(f"Custom pruned tree (alpha=0.2) accuracy: {np.mean(results['custom_0.2']):.4f} ± {np.std(results['custom_0.2']):.4f}")
