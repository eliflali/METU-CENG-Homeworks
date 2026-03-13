import numpy as np
import pandas as pd
import matplotlib.pyplot as plt
from sklearn.metrics import accuracy_score, precision_score, recall_score, f1_score, confusion_matrix
from task1 import load_weather_dataset, load_air_quality_dataset
from task3 import SVM
from task4 import create_svm_with_kernel
import time

def time_series_cv_split(X, y, n_splits=5):
    n_samples = len(y)
    if n_samples < n_splits * 2:
        raise ValueError(f"too few samples ({n_samples}) for {n_splits} splits")
    
    fold_size = n_samples // n_splits
    splits = []
    
    for i in range(n_splits - 1):
        train_end = (i + 1) * fold_size
        val_start = train_end   
        val_end = val_start + fold_size
        
        train_idx = np.arange(0, train_end)
        val_idx = np.arange(val_start, val_end)
        
        splits.append((train_idx, val_idx))
    
    train_idx = np.arange(0, (n_splits - 1) * fold_size)
    val_idx = np.arange((n_splits - 1) * fold_size, n_samples)
    splits.append((train_idx, val_idx))
    
    return splits

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

def tune_hyperparameters(X_train, y_train, kernel_type, param_grid):
    print(f"\nTuning hyperparameters for {kernel_type} kernel...")
    
    from itertools import product
    keys = list(param_grid.keys())
    values = list(param_grid.values())
    
    cv_splits = time_series_cv_split(X_train, y_train, n_splits=5)
    
    best_score = -1
    best_params = None
    
    for params in product(*values):
        param_dict = dict(zip(keys, params))
        param_str = ', '.join([f"{k}={v}" for k, v in param_dict.items()])
        print(f"  Testing {param_str}...")
        
        cv_scores = []
        for train_idx, val_idx in cv_splits:
            X_cv_train, y_cv_train = X_train[train_idx], y_train[train_idx]
            X_cv_val, y_cv_val = X_train[val_idx], y_train[val_idx]
            
            svm = create_svm_with_kernel(kernel_type, **param_dict)
            svm.fit(X_cv_train, y_cv_train)
            
            y_cv_pred = svm.predict(X_cv_val)
            cv_score = accuracy_score(y_cv_val, y_cv_pred)
            cv_scores.append(cv_score)
        
        avg_score = np.mean(cv_scores)
        print(f"    Average CV accuracy: {avg_score:.4f}")
        
        if avg_score > best_score:
            best_score = avg_score
            best_params = param_dict
    
    print(f"Best parameters: {best_params}")
    print(f"Best CV accuracy: {best_score:.4f}")
    
    return best_params, best_score

def train_and_evaluate_models(dataset_name, X, y):
    print(f"\n=== Training and evaluating models on {dataset_name} dataset ===")
    
    train_size = int(0.8 * len(X))
    X_train, X_test = X[:train_size], X[train_size:]
    y_train, y_test = y[:train_size], y[train_size:]
    
    print(f"Training set size: {len(X_train)}")
    print(f"Test set size: {len(X_test)}")
    
    kernel_configs = {
        'linear': {
            'C': [0.1, 1.0, 10.0]
        },
        'polynomial': {
            'C': [0.1, 1.0, 10.0],
            'degree': [2, 3],
            'c': [0.0, 1.0]
        },
        'rbf': {
            'C': [0.1, 1.0, 10.0],
            'sigma': [0.1, 1.0, 10.0]
        },
        'time_series': {
            'C': [0.1, 1.0],
            'sigma': [0.1, 1.0],
            'period': [7] if dataset_name == 'Weather' else [30],
            'w_lin': [0.33, 0.5],
            'w_per': [0.33, 0.5],
            'w_rbf': [0.34, 0.2]
        }
    }
    
    results = {}
    
    for kernel_type, param_grid in kernel_configs.items():
        print(f"\n--- {kernel_type.capitalize()} Kernel ---")
        
        start_time = time.time()
        best_params, _ = tune_hyperparameters(X_train, y_train, kernel_type, param_grid)
        
        print("Training final model with best hyperparameters...")
        svm = create_svm_with_kernel(kernel_type, **best_params)
        svm.fit(X_train, y_train)
        
        y_pred = svm.predict(X_test)
        metrics = evaluate_model(y_test, y_pred)
        
        cm = confusion_matrix(y_test, y_pred)
        
        results[kernel_type] = {
            'params': best_params,
            'metrics': metrics,
            'confusion_matrix': cm,
            'training_time': time.time() - start_time
        }
        
        print("\nTest set evaluation:")
        for metric, value in metrics.items():
            print(f"  {metric}: {value:.4f}")
        
        print("\nConfusion Matrix:")
        print(cm)
        print(f"Training time: {results[kernel_type]['training_time']:.2f} seconds")
    
    return results

def plot_results(weather_results, air_quality_results):
    kernels = list(weather_results.keys())
    metrics = ['accuracy', 'precision', 'recall', 'f1_score']
    
    fig, axes = plt.subplots(2, 2, figsize=(15, 12))
    axes = axes.flatten()
    
    for i, metric in enumerate(metrics):
        weather_values = [weather_results[k]['metrics'][metric] for k in kernels]
        air_quality_values = [air_quality_results[k]['metrics'][metric] for k in kernels]
        
        x = np.arange(len(kernels))
        width = 0.35
        
        axes[i].bar(x - width/2, weather_values, width, label='Weather Dataset')
        axes[i].bar(x + width/2, air_quality_values, width, label='Air Quality Dataset')
        
        axes[i].set_title(f'{metric.capitalize()} Comparison')
        axes[i].set_xticks(x)
        axes[i].set_xticklabels([k.capitalize() for k in kernels])
        axes[i].set_ylim(0, 1)
        axes[i].legend()
        axes[i].grid(axis='y', linestyle='--', alpha=0.7)
    
    plt.tight_layout()
    plt.savefig('kernel_performance_comparison.png')
    
    plt.figure(figsize=(10, 6))
    weather_times = [weather_results[k]['training_time'] for k in kernels]
    air_quality_times = [air_quality_results[k]['training_time'] for k in kernels]
    
    x = np.arange(len(kernels))
    width = 0.35
    
    plt.bar(x - width/2, weather_times, width, label='Weather Dataset')
    plt.bar(x + width/2, air_quality_times, width, label='Air Quality Dataset')
    
    plt.title('Training Time Comparison')
    plt.xlabel('Kernel')
    plt.ylabel('Time (seconds)')
    plt.xticks(x, [k.capitalize() for k in kernels])
    plt.legend()
    plt.grid(axis='y', linestyle='--', alpha=0.7)
    plt.tight_layout()
    plt.savefig('training_time_comparison.png')

def save_results_to_csv(weather_results, air_quality_results):
    weather_data = []
    for kernel, result in weather_results.items():
        row = {
            'Kernel': kernel,
            'Accuracy': result['metrics']['accuracy'],
            'Precision': result['metrics']['precision'],
            'Recall': result['metrics']['recall'],
            'F1_Score': result['metrics']['f1_score'],
            'Training_Time': result['training_time']
        }
        for param, value in result['params'].items():
            row[f'Param_{param}'] = value
        
        weather_data.append(row)
    
    air_quality_data = []
    for kernel, result in air_quality_results.items():
        row = {
            'Kernel': kernel,
            'Accuracy': result['metrics']['accuracy'],
            'Precision': result['metrics']['precision'],
            'Recall': result['metrics']['recall'],
            'F1_Score': result['metrics']['f1_score'],
            'Training_Time': result['training_time']
        }
        for param, value in result['params'].items():
            row[f'Param_{param}'] = value
        
        air_quality_data.append(row)
    
    pd.DataFrame(weather_data).to_csv('weather_results.csv', index=False)
    pd.DataFrame(air_quality_data).to_csv('air_quality_results.csv', index=False)

if __name__ == "__main__":
    try:
        weather_data = pd.read_csv('preprocessed_weather_data.csv', parse_dates=['Date'])
        air_quality_data = pd.read_csv('preprocessed_air_quality_data.csv', parse_dates=['Date'])
        print("Loaded preprocessed datasets from files.")
    except FileNotFoundError:
        print("no preprocessed files found.")
        weather_data = load_weather_dataset()
        air_quality_data = load_air_quality_dataset()
    
    X_weather = weather_data[[f'Temp_{i}' for i in range(1, 8)]].values
    y_weather = weather_data['Target'].values
    
    X_air = air_quality_data[[f'PM2.5_{i}' for i in range(1, 8)]].values
    y_air = air_quality_data['Target'].values
    
    weather_results = train_and_evaluate_models('Weather', X_weather, y_weather)
    air_quality_results = train_and_evaluate_models('Air Quality', X_air, y_air)
    
    
    print("\nmodel training and evaluation completed.")
