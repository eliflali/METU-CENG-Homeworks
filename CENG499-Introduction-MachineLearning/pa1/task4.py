import numpy as np
from task2 import DecisionTree
from task3 import dt
from sklearn.datasets import load_wine
from sklearn.model_selection import train_test_split

wine = load_wine()
X = wine.data
y = wine.target

# training, validation, and test sets (60%, 20%, 20%)
# 20% for test set - %80 for train & val
X_train_val, X_test, y_train_val, y_test = train_test_split(X, y, test_size=0.2, random_state=42)

# 25% * %80 -> val: 20%
# 75% * %80 -> train: 60%
X_train, X_val, y_train, y_val = train_test_split(X_train_val, y_train_val, test_size=0.25, random_state=42)

def reduced_error_pruning(tree, X_val, y_val):
    pruned_tree = DecisionTree()
    pruned_tree.root = tree.root
    
    init_predictions = pruned_tree.predict(X_val)
    init_accuracy = np.mean(init_predictions == y_val)
    
    pruned_tree.root = prune_node(pruned_tree.root, X_val, y_val, pruned_tree)
    
    final_pred = pruned_tree.predict(X_val)
    final_accuracy = np.mean(final_pred == y_val)
    
    print(f"Reduced error pruning: Initial accuracy: {init_accuracy:.4f}, Final accuracy: {final_accuracy:.4f}")
    
    return pruned_tree

def prune_node(node, X_val, y_val, tree, indices=None):
    if indices is None:
        indices = list(range(len(X_val)))
    
    if len(indices) == 0 or node.is_leaf():
        return node
    
    original_feature_idx = node.feature_idx
    original_threshold = node.threshold
    
    # child's X values
    left_indices = [i for i in indices if X_val[i][original_feature_idx] <= original_threshold]
    right_indices = [i for i in indices if X_val[i][original_feature_idx] > original_threshold]
    
    # recursive call to get to leaves
    node.left = prune_node(node.left, X_val, y_val, tree, left_indices)
    node.right = prune_node(node.right, X_val, y_val, tree, right_indices)
    
    
    if node.left.is_leaf() and node.right.is_leaf():
        # accuracy before
        pred_before = tree.predict(X_val)
        accuracy_before = np.mean(pred_before == y_val)
        
        left_child, right_child = node.left, node.right
        
        left_samples = len(left_indices)
        right_samples = len(right_indices)
        
        node.left = None
        node.right = None
        node.feature_idx = None
        node.threshold = None
        
        if left_samples > right_samples:
            node.value = left_child.value
        else:
            node.value = right_child.value
        
        pred_after = tree.predict(X_val)
        accuracy_after = np.mean(pred_after == y_val)
        
        # reverting
        if accuracy_after < accuracy_before:
            node.left = left_child
            node.right = right_child
            node.feature_idx = original_feature_idx
            node.threshold = original_threshold
            node.value = None
    
    return node

dt_pruned = reduced_error_pruning(dt, X_val, y_val)


y_pred_pruned = dt_pruned.predict(X_test)
accuracy_pruned = np.mean(y_pred_pruned == y_test)
print(f"Pruned tree accuracy on test set: {accuracy_pruned:.4f}")

#pruned tree:
print("\nPruned tree structure:")
dt_pruned.print_tree()
