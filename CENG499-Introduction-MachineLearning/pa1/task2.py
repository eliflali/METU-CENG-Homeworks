import numpy as np

class Node:
    def __init__(self, feature_idx=None, threshold=None, left=None, right=None, value=None):
        # left and right are nodes
        self.feature_idx = feature_idx
        self.threshold = threshold
        self.left = left
        self.right = right
        self.value = value
        
    def is_leaf(self):
        return self.value is not None


class DecisionTree:
    def __init__(self, max_depth=None, min_samples_split=2):
        self.max_depth = max_depth
        self.min_samples_split = min_samples_split
        # to traverse
        self.root = None
        
    def fit(self, X, y):
        X = np.array(X)
        y = np.array(y)
        
        self.root = self.tree_creator(X, y, depth=0)
        
    def tree_creator(self, X, y, depth):
        # n_samples = # of samples in the current node
        # n_features = # of features in the current node
        # print(X.shape)
        n_samples, n_features = X.shape
        # n_classes = # of classes in the current node
        n_classes = len(np.unique(y))
        
        # to stop recursion:
        # max_depth !none depth >= max_depth
        # n_samples < min_samples_split
        # n_classes == 1 (all samples belong to the same class)
        if (self.max_depth is not None and depth >= self.max_depth or
            n_samples < self.min_samples_split or
            n_classes == 1):
            leaf_value = np.argmax(np.bincount(y))
            return Node(value=leaf_value)
        
        # find the best split
        best_feature_idx, best_threshold = self.find_best_split(X, y)
        
        # no split for better Gini index -> create a leaf node
        if best_feature_idx is None:
            leaf_value = np.argmax(np.bincount(y))
            return Node(value=leaf_value)
        
        # splitting samples 
        left_indices = X[:, best_feature_idx] <= best_threshold
        right_indices = ~left_indices
        
        # Grow the left and right subtrees
        left_subtree = self.tree_creator(X[left_indices], y[left_indices], depth + 1)
        right_subtree = self.tree_creator(X[right_indices], y[right_indices], depth + 1)
        
        return Node(feature_idx=best_feature_idx, threshold=best_threshold,
                   left=left_subtree, right=right_subtree)
    
    def find_best_split(self, X, y):
        n_samples, n_features = X.shape
        
        if n_samples <= 1:
            return None, None
        
        # Gini = 1 − sum(pi^2)
        # pi = probability of class i
        # sum(pi^2) = sum((count(class i) / n_samples)^2) for all classes
        
        best_gini = 1.0  # worst possible gini
        best_feature_idx, best_threshold = None, None
        
        # loop through all features
        for feature_idx in range(n_features):
            # unique values of the feature
            thresholds = np.unique(X[:, feature_idx])
            
            # only one unique value -> skip this feature
            if len(thresholds) <= 1:
                continue
            
            #midpoint
            thresholds = (thresholds[:-1] + thresholds[1:]) / 2

            for threshold in thresholds:
                # split the data
                left_indices = X[:, feature_idx] <= threshold
                right_indices = ~left_indices
                
                # all samples belong to the same class
                if np.sum(left_indices) == 0 or np.sum(right_indices) == 0:
                    continue
                
                # weighted Gini index:
                # Weighted Gini = nleft/n * gini_left + nright/n * gini_right
                left_gini = self.calculate_gini(y[left_indices])
                right_gini = self.calculate_gini(y[right_indices])
                
                n_left = np.sum(left_indices)
                n_right = np.sum(right_indices)
                
                weighted_gini = (n_left * left_gini + n_right * right_gini) / n_samples
                
                # update the best split if curr is better
                if weighted_gini < best_gini:
                    best_gini = weighted_gini
                    best_feature_idx = feature_idx
                    best_threshold = threshold
        
        return best_feature_idx, best_threshold
    
    def calculate_gini(self, y):
        # n_samples = # of samples in the current node
        n_samples = len(y)
        
        # no samples -> no gini index
        if n_samples == 0:
            return 0.0
        
        _, counts = np.unique(y, return_counts=True)
      
        probabilities = counts / n_samples
        
        # Gini index:
        # Gini = 1 − sum(pi^2)
        gini = 1.0 - np.sum(probabilities**2)
        
        return gini
    
    def predict(self, X):
        # X: (n_samples, n_features)
        X = np.array(X)

        # traverse for each sample
        return np.array([self.sample_predict(sample) for sample in X])
    
    def sample_predict(self, sample):  
        # start from root
        node = self.root

        # traverse until leaf node
        while node.is_leaf() == False:
            # threshold != None  - should be before comparison
            if node.threshold is None:
                # check node value else: 0
                return node.value if hasattr(node, 'value') else 0
            
            if sample[node.feature_idx] <= node.threshold:
                node = node.left
            else:
                node = node.right
        return node.value
    
    def print_tree(self, node=None, depth=0):
        if node is None:
            node = self.root
            
        indent = "  " * depth
        
        if node.is_leaf():
            print(f"{indent}Leaf: Class {node.value}")
        else:
            # threshold check bcs will be printed
            if node.threshold is None:
                print(f"{indent}Feature {node.feature_idx} <= None")
            else:
                print(f"{indent}Feature {node.feature_idx} <= {node.threshold:.2f}")
            
            # left child check 
            if node.left is not None:
                self.print_tree(node.left, depth + 1)
            else:
                print(f"{indent}  Leaf: None")
                
            if node.threshold is None:
                print(f"{indent}Feature {node.feature_idx} > None")
            else:
                print(f"{indent}Feature {node.feature_idx} > {node.threshold:.2f}")
            if node.right is not None:
                self.print_tree(node.right, depth + 1)
            else:
                print(f"{indent}  Leaf: None")