import numpy as np
from scipy.optimize import minimize
import matplotlib.pyplot as plt

class SVM:
    def __init__(self, C=1.0, kernel=None):
        self.C = C
        self.kernel = kernel if kernel else self._linear_kernel
        self.alphas = None
        self.support_vectors = None
        self.support_vector_labels = None 
        self.support_vector_indices = None
        self.b = 0.0
        self.w = None  # linear kernel only
        
    def _linear_kernel(self, x1, x2):
        # K(x1, x2) = x1 · x2
        return np.dot(x1, x2)
    
    def fit(self, X, y):
        n_samples, n_features = X.shape
        
        # sign func
        y_binary = np.where(y <= 0, -1, 1)
        
        # dot product kernel matrix
        K = np.zeros((n_samples, n_samples))
        for i in range(n_samples):
            for j in range(n_samples):
                K[i, j] = self.kernel(X[i], X[j])
                
        def objective(alphas):
            return 0.5 * np.sum(np.outer(y_binary, y_binary) * np.outer(alphas, alphas) * K) - np.sum(alphas)
        
        def gradient(alphas):
            return np.sum(np.outer(y_binary, y_binary) * np.outer(alphas, alphas) * K, axis=1) - 1
        
        #a_i.y_i = 0
        constraints = [{'type': 'eq', 'fun': lambda alphas: np.sum(alphas * y_binary), 'jac': lambda alphas: y_binary}]
        
        # 0 <= a_i <= C
        bounds = [(0, self.C) for _ in range(n_samples)]
        
        # initial guess
        alphas_init = np.zeros(n_samples)
        
        # solve the dual optimization problem
        #minimize(fun, x0, args=(), 
        # method=None, jac=None, 
        # hess=None, hessp=None, 
        # bounds=None, constraints=(), 
        # tol=None, callback=None, options=None)
        result = minimize(
            objective,
            alphas_init,
            method='L-BFGS-B',
            jac=gradient,
            bounds=bounds,
            constraints=constraints,
            options={'maxiter': 1000, 'disp': False}
        )
        
        # alphas: Lagrange multipliers
        self.alphas = result.x
        
        # support vectors (samples w/alpha > 0)
        sv_threshold = 1e-5  # threshold for sup. vecs
        self.support_vector_indices = np.where(self.alphas > sv_threshold)[0]
        self.support_vectors = X[self.support_vector_indices]
        self.support_vector_labels = y_binary[self.support_vector_indices]
        self.support_vector_alphas = self.alphas[self.support_vector_indices]
        
        # bias
        # b = y_i - sum(a_j * y_j * K(x_i, x_j))
        self.b = 0
        for i in range(len(self.support_vector_indices)):
            # yi
            self.b += self.support_vector_labels[i]
            # alpha_i * y_i * K(x_i, x_i)
            self.b -= np.sum(self.support_vector_alphas * self.support_vector_labels * 
                            K[self.support_vector_indices, self.support_vector_indices[i]])
        
        # 1/ns
        if len(self.support_vector_indices) > 0:
            self.b /= len(self.support_vector_indices)
        
        # linear kernel
        if self.kernel == self._linear_kernel:
            
            self.w = np.zeros(n_features)
            for i in range(len(self.support_vector_indices)):
                self.w += self.support_vector_alphas[i] * self.support_vector_labels[i] * self.support_vectors[i]
        
        print(f"training completed, {len(self.support_vector_indices)} support vecs.")
        return self
    
    # αiyiK(xi,x)+b
    def predict(self, X):
        if self.alphas is None:
            raise ValueError("no alpha values found")
        
        # linear kernel, weight vector
        if self.kernel == self._linear_kernel and self.w is not None:
            decision = np.dot(X, self.w) + self.b
        else:
            # non-linear kernel, αiyiK(xi,x)+b
            decision = np.zeros(X.shape[0])
            for i in range(X.shape[0]):
                for j in range(len(self.support_vector_indices)):
                    decision[i] += self.support_vector_alphas[j] * self.support_vector_labels[j] * \
                                  self.kernel(X[i], self.support_vectors[j])
                decision[i] += self.b
        
        # decision vals to binary predictions
        return np.where(decision <= 0, 0, 1)
    
    # decision vals delete after test
    def decision_function(self, X):
        if self.alphas is None:
            raise ValueError("no alpha values found")
        
        # linear kernel -> weight vector
        # is it true check it
        if self.kernel == self._linear_kernel and self.w is not None:
            return np.dot(X, self.w) + self.b
        else:
            # non-linear kernel -> dual form
            decision = np.zeros(X.shape[0])
            for i in range(X.shape[0]):
                for j in range(len(self.support_vector_indices)):
                    decision[i] += self.support_vector_alphas[j] * self.support_vector_labels[j] * \
                                  self.kernel(X[i], self.support_vectors[j])
                decision[i] += self.b
            return decision
    
    def score(self, X, y):
        y_pred = self.predict(X)
        return np.mean(y_pred == y)
