import numpy as np
from scipy.stats import norm
from task2 import DecisionTree
from sklearn.datasets import load_wine
from sklearn.model_selection import train_test_split

wine = load_wine()
X = wine.data
y = wine.target

X_train_val, X_test, y_train_val, y_test = train_test_split(X, y, test_size=0.2, random_state=42)
X_train, X_val, y_train, y_val = train_test_split(X_train_val, y_train_val, test_size=0.25, random_state=42)



def custom_pruning(tree, X_train, y_train, X_val, y_val, alpha=0.1):   
    pruned_tree = DecisionTree()
    pruned_tree.root = tree.root
    
    initial_pred = pruned_tree.predict(X_val)
    initial_accuracy = np.mean(initial_pred == y_val)
    
    pruned_tree.root = custom_prune_node(pruned_tree.root, X_train, y_train, alpha)
    
    final_pred = pruned_tree.predict(X_val)
    final_accuracy = np.mean(final_pred == y_val)
    
    print(f"Custom pruning (alpha={alpha}): Initial accuracy: {initial_accuracy:.4f}, Final accuracy: {final_accuracy:.4f}")
    
    return pruned_tree

def custom_prune_node(node, X, y, alpha, indices=None):
    # indices none -> it is the root
    if indices is None:
        indices = list(range(len(X)))
    
    # no samples in node
    if len(indices) == 0 or node.is_leaf():
        return node
    
    feature_idx = node.feature_idx
    threshold = node.threshold
    
    # child's samples for the node
    left_indices = [i for i in indices if X[i][feature_idx] <= threshold]
    right_indices = [i for i in indices if X[i][feature_idx] > threshold]

    node.left = custom_prune_node(node.left, X, y, alpha, left_indices)
    node.right = custom_prune_node(node.right, X, y, alpha, right_indices)
    
    if node.left.is_leaf() and node.right.is_leaf():
        feature_values = X[indices, node.feature_idx]
        
        # mean&variance
        mean = np.mean(feature_values)
        std = np.std(feature_values)
        
        # div by 0 - bug
        if std == 0:  
            return node
        
        # cdf((threshold - mean) / std)
        p_left = norm.cdf((node.threshold - mean) / std)
        p_right = 1 - p_left
    
        # min(pleft, pright) < alpha -> prune
        if min(p_left, p_right) < alpha:
            # print(f"Pruning node with feature {node.feature_idx}")
            
            left_child, right_child = node.left, node.right
            
            left_samples = len(left_indices)
            right_samples = len(right_indices)
            
            node.left = None
            node.right = None
            node.feature_idx = None
            node.threshold = None
            
            # majority voting:
            if left_samples > right_samples:
                node.value = left_child.value
            else:
                node.value = right_child.value
    
    return node

alphas = [0.05, 0.1, 0.2]
dt_custom_pruned = {}

for alpha in alphas:
    dt_fresh = DecisionTree()
    dt_fresh.fit(X_train, y_train)
    
    dt_custom_pruned[alpha] = custom_pruning(dt_fresh, X_train, y_train, X_val, y_val, alpha)
    
    y_pred_custom = dt_custom_pruned[alpha].predict(X_test)
    accuracy_custom = np.mean(y_pred_custom == y_test)
    print(f"Custom pruned tree (alpha={alpha}) accuracy on test set: {accuracy_custom:.4f}")