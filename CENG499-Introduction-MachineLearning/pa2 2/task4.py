import numpy as np
from task3 import SVM

#K(x, y) = x · y
class LinearKernel:
    def __init__(self):
        self.name = "Linear"
    
    def __call__(self, x, y):
        return np.dot(x, y)

#K(x, y) = (x · y + c)^d
class PolynomialKernel:
    def __init__(self, degree=3, c=1.0):
        self.degree = degree
        self.c = c
        self.name = f"Polynomial (degree={degree}, c={c})"
    
    def __call__(self, x, y):
        return (np.dot(x, y) + self.c) ** self.degree

#K(x, y) = exp(-||x - y||^2 / (2 * sigma^2))
class RBFKernel:
    def __init__(self, sigma=1.0):
        self.sigma = sigma
        self.name = f"RBF (sigma={sigma})"
    
    def __call__(self, x, y):
        # euclidean dist:
        squared_dist = np.sum((x - y) ** 2)
        return np.exp(-squared_dist / (2 * self.sigma ** 2))


class TimeSeriesKernel:
    def __init__(self, max_lag=3, sigma_tau=1.0, period=7, sigma=1.0, 
                 w_lin=0.3, w_per=0.3, w_rbf=0.4):
        self.max_lag = max_lag
        self.sigma_tau = sigma_tau
        self.period = period
        self.sigma = sigma
        
        # "Weights summing to 1"
        # LATEST CHANGE
        total_weight = w_lin + w_per + w_rbf
        self.w_lin = w_lin / total_weight
        self.w_per = w_per / total_weight
        self.w_rbf = w_rbf / total_weight
        
        # lag weights precompute
        self.lag_weights = np.exp(-(np.arange(-max_lag, max_lag + 1) ** 2) / (2 * sigma_tau ** 2))
        
        self.name = f"Time Series (max_lag={max_lag}, period={period})"
    
    def __call__(self, x, y):
        T = len(x)
        kernel_sum = 0.0
        
        # lag iterate
        for tau in range(-self.max_lag, self.max_lag + 1):
            valid_indices = np.arange(max(0, -tau), min(T, T - tau))
            
            if len(valid_indices) == 0:
                continue
                
            # shifted vecs
            x_shifted = x[valid_indices]
            y_shifted = y[valid_indices + tau]
            
            # Compute all components at once using vectorized operations
            # LINEAR
            k_lin = x_shifted * y_shifted
            
            # PERIODIC
            phase_diff = np.sin(np.pi * (x_shifted - y_shifted) / self.period) ** 2
            k_per = np.exp(-2 * phase_diff / (self.sigma ** 2))
            
            # RBF
            squared_diff = (x_shifted - y_shifted) ** 2
            k_rbf = np.exp(-squared_diff / (2 * self.sigma ** 2))
            
            # weighted sum
            component_sum = np.sum(
                self.w_lin * k_lin + 
                self.w_per * k_per + 
                self.w_rbf * k_rbf
            )
            #addition here CHECK
            kernel_sum += self.lag_weights[tau + self.max_lag] * (component_sum / len(valid_indices))
        
        return kernel_sum

#wil called from next task
def create_svm_with_kernel(kernel_type, **kwargs):
    # params
    C = kwargs.pop('C', 1.0)
    
    if kernel_type.lower() == 'linear':
        kernel = LinearKernel()
    elif kernel_type.lower() == 'polynomial':
        degree = kwargs.pop('degree', 3)
        c = kwargs.pop('c', 1.0)
        kernel = PolynomialKernel(degree=degree, c=c)
    elif kernel_type.lower() == 'rbf':
        sigma = kwargs.pop('sigma', 1.0)
        kernel = RBFKernel(sigma=sigma)
    elif kernel_type.lower() == 'time_series':
        max_lag = kwargs.pop('max_lag', 3)
        sigma_tau = kwargs.pop('sigma_tau', 1.0)
        period = kwargs.pop('period', 7)
        sigma = kwargs.pop('sigma', 1.0)
        w_lin = kwargs.pop('w_lin', 0.3)
        w_per = kwargs.pop('w_per', 0.3)
        w_rbf = kwargs.pop('w_rbf', 0.4)
        kernel = TimeSeriesKernel(max_lag=max_lag, sigma_tau=sigma_tau, period=period,
                                 sigma=sigma, w_lin=w_lin, w_per=w_per, w_rbf=w_rbf)
    else:
        raise ValueError(f"error wrong kernel: {kernel_type}")
    
    return SVM(C=C, kernel=kernel)
