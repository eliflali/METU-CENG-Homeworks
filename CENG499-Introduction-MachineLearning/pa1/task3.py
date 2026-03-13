import numpy as np
from sklearn.model_selection import train_test_split
from task2 import DecisionTree
from sklearn.datasets import load_wine

wine = load_wine()
X = wine.data
y = wine.target

# training, validation, and test (60%, 20%, 20%)
# train_test_split(*arrays, test_size=None, train_size=None, random_state=None, shuffle=True, stratify=None)
# *arrays: sequence of indexables with same length / shape[0]
X_train_val, X_test, y_train_val, y_test = train_test_split(X, y, test_size=0.2, random_state=42)
X_train, X_val, y_train, y_val = train_test_split(X_train_val, y_train_val, test_size=0.25, random_state=42)

print(f"Training set: {X_train.shape[0]} samples")
print(f"Validation set: {X_val.shape[0]} samples")
print(f"Test set: {X_test.shape[0]} samples")

# no depth limit
dt = DecisionTree()
dt.fit(X_train, y_train)

# evaluation on test set
y_pred = dt.predict(X_test)

# Accuracy = Number of correct predictions / Total test samples
accuracy = np.mean(y_pred == y_test)
print(f"Unpruned tree accuracy on test set: {accuracy:.4f}")

# print tree
print("\nTree structure:")
dt.print_tree()