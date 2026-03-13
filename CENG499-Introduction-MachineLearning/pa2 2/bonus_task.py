import numpy as np
import pandas as pd
from task1 import load_weather_dataset, load_air_quality_dataset
from task3 import SVM
from task4 import create_svm_with_kernel
from sklearn.metrics import accuracy_score, precision_score, recall_score, f1_score, confusion_matrix
from imblearn.over_sampling import SMOTE
import matplotlib.pyplot as plt
from scipy.optimize import minimize

def calculate_imbalance_ratio(y):
    unique, counts = np.unique(y, return_counts=True)
    minority_count = min(counts)
    majority_count = max(counts)
    imbalance_ratio = minority_count / majority_count
    return imbalance_ratio, minority_count, majority_count

def create_weighted_svm(C, kernel, y_train):
    _, minority_count, majority_count = calculate_imbalance_ratio(y_train)
    C_minority = C * (majority_count / minority_count)
    
    svm = SVM(C=C, kernel=kernel)
    svm.C_minority = C_minority
    
    def weighted_objective(alphas, K):
        weights = np.where(y_train == 1, C_minority, C)
        return 0.5 * np.sum(np.outer(y_train, y_train) * np.outer(alphas, alphas) * K) - np.sum(weights * alphas)
    
    svm.weighted_objective = weighted_objective
    return svm

def oversample_with_smote(X_train, y_train, random_state=42):
    smote = SMOTE(random_state=random_state)
    
    X_resampled, y_resampled = smote.fit_resample(X_train, y_train)
    
    return X_resampled, y_resampled

def evaluate_model(y_true, y_pred):
    accuracy = accuracy_score(y_true, y_pred)
    precision = precision_score(y_true, y_pred)
    recall = recall_score(y_true, y_pred)
    f1 = f1_score(y_true, y_pred)
    
    return {
        'accuracy': accuracy,
        'precision': precision,
        'recall': recall,
        'f1_score': f1
    }

def plot_imbalance_handling_results(original_results, weighted_results, smote_results, dataset_name):
    metrics = ['accuracy', 'precision', 'recall', 'f1_score']
    methods = ['Original', 'Weighted', 'SMOTE']
    
    plt.figure(figsize=(12, 6))
    
    for i, metric in enumerate(metrics):
        plt.subplot(2, 2, i+1)
        values = [
            original_results[metric],
            weighted_results[metric],
            smote_results[metric]
        ]
        plt.bar(methods, values)
        plt.title(f'{metric.capitalize()}')
        plt.ylim(0, 1)
        plt.grid(axis='y', linestyle='--', alpha=0.7)
    
    plt.suptitle(f'Imbalance Handling Results - {dataset_name} Dataset')
    plt.tight_layout()
    plt.savefig(f'imbalance_handling_{dataset_name.lower().replace(" ", "_")}.png')
    plt.close()

def main():
    weather_data = load_weather_dataset()
    air_quality_data = load_air_quality_dataset()
    
    X_weather = weather_data[[f'Temp_{i}' for i in range(1, 8)]].values
    y_weather = weather_data['Target'].values
    
    X_air = air_quality_data[[f'PM2.5_{i}' for i in range(1, 8)]].values
    y_air = air_quality_data['Target'].values
    
    train_size = int(0.8 * len(X_weather))
    
    X_weather_train, X_weather_test = X_weather[:train_size], X_weather[train_size:]
    y_weather_train, y_weather_test = y_weather[:train_size], y_weather[train_size:]
    
    X_air_train, X_air_test = X_air[:train_size], X_air[train_size:]
    y_air_train, y_air_test = y_air[:train_size], y_air[train_size:]
    
    weather_ratio, weather_minority, weather_majority = calculate_imbalance_ratio(y_weather)
    air_ratio, air_minority, air_majority = calculate_imbalance_ratio(y_air)
    
    print("\nimbalance analysis:")
    print(f"weather Dataset - imbalance ratio: {weather_ratio:.4f}")
    print(f"  minority class: {weather_minority}, majority class: {weather_majority}")
    print(f"air quality Dataset - imbalance ratio: {air_ratio:.4f}")
    print(f"  minority class: {air_minority}, majority class: {air_majority}")
    
    datasets = {
        'Weather': (X_weather_train, y_weather_train, X_weather_test, y_weather_test),
        'Air Quality': (X_air_train, y_air_train, X_air_test, y_air_test)
    }
    
    for dataset_name, (X_train, y_train, X_test, y_test) in datasets.items():
        print(f"\nProcessing {dataset_name} dataset...")
        
        kernel = create_svm_with_kernel('time_series', C=1.0, period=7 if dataset_name == 'Weather' else 30)
        
        # no imbalance handling
        print("\nTraining original SVM...")
        svm_original = SVM(C=1.0, kernel=kernel.kernel)
        svm_original.fit(X_train, y_train)
        y_pred_original = svm_original.predict(X_test)
        original_results = evaluate_model(y_test, y_pred_original)
        
        print("\nTraining weighted SVM...")
        svm_weighted = create_weighted_svm(1.0, kernel.kernel, y_train)
        
        n_samples = len(X_train)
        K = np.zeros((n_samples, n_samples))
        for i in range(n_samples):
            for j in range(n_samples):
                K[i, j] = kernel.kernel(X_train[i], X_train[j])
        
        def weighted_fit(self, X, y):
            y_binary = np.where(y <= 0, -1, 1)
            alphas_init = np.zeros(len(y))
            bounds = [(0, self.C) for _ in range(len(y))]
            constraints = [{'type': 'eq', 'fun': lambda alphas: np.sum(alphas * y_binary), 'jac': lambda alphas: y_binary}]
            
            result = minimize(
                lambda alphas: self.weighted_objective(alphas, K),
                alphas_init,
                method='SLSQP',
                bounds=bounds,
                constraints=constraints,
                options={'maxiter': 1000, 'disp': False, 'ftol': 1e-6}
            )
            
            self.alphas = result.x
            sv_threshold = 1e-5
            self.support_vector_indices = np.where(self.alphas > sv_threshold)[0]
            self.support_vectors = X[self.support_vector_indices]
            self.support_vector_labels = y_binary[self.support_vector_indices]
            self.support_vector_alphas = self.alphas[self.support_vector_indices]
            
            # Calculate bias
            self.b = 0
            for i in range(len(self.support_vector_indices)):
                self.b += self.support_vector_labels[i]
                self.b -= np.sum(self.support_vector_alphas * self.support_vector_labels * 
                                K[self.support_vector_indices, self.support_vector_indices[i]])
            
            if len(self.support_vector_indices) > 0:
                self.b /= len(self.support_vector_indices)
            
            print(f"training completed, {len(self.support_vector_indices)} support vecs.")
            return self
        
        svm_weighted.fit = weighted_fit.__get__(svm_weighted, SVM)
        svm_weighted.fit(X_train, y_train)
        y_pred_weighted = svm_weighted.predict(X_test)
        weighted_results = evaluate_model(y_test, y_pred_weighted)
        
        print("\nSVM with SMOTE training")
        X_resampled, y_resampled = oversample_with_smote(X_train, y_train)
        svm_smote = SVM(C=1.0, kernel=kernel.kernel)
        svm_smote.fit(X_resampled, y_resampled)
        y_pred_smote = svm_smote.predict(X_test)
        smote_results = evaluate_model(y_test, y_pred_smote)

        print("\nResults Comparison:")
        print("Method\t\tAccuracy\tPrecision\tRecall\t\tF1-Score")
        print("-" * 80)
        print(f"Original\t{original_results['accuracy']:.4f}\t\t{original_results['precision']:.4f}\t\t{original_results['recall']:.4f}\t\t{original_results['f1_score']:.4f}")
        print(f"Weighted\t{weighted_results['accuracy']:.4f}\t\t{weighted_results['precision']:.4f}\t\t{weighted_results['recall']:.4f}\t\t{weighted_results['f1_score']:.4f}")
        print(f"SMOTE\t\t{smote_results['accuracy']:.4f}\t\t{smote_results['precision']:.4f}\t\t{smote_results['recall']:.4f}\t\t{smote_results['f1_score']:.4f}")
        
        plot_imbalance_handling_results(original_results, weighted_results, smote_results, dataset_name)

if __name__ == "__main__":
    main()
