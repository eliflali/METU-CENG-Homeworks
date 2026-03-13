import pandas as pd
import numpy as np
import seaborn as sns
import matplotlib.pyplot as plt
from scipy import stats
import os

def perform_eda(csv_file_path: str):
    if not os.path.exists(csv_file_path):
        print(f"csv not found error task3 line 13")
        return
    df = pd.read_csv(csv_file_path)
    
    print("\nDATASET INFO")
    print("first 5 rows:")
    print(df.head())
    print("\nDataFrame info:")
    df.info()
    
    print("\nmissing values")
    missing_values = df.isnull().sum()
    if missing_values.sum() == 0:
        print("no missing values line 23.")
    else:
        print("missing values:")
        print(missing_values[missing_values > 0])

    numerical_features = ['Age', 'Annual Income (k$)', 'Spending Score (1-100)']
    for feature in numerical_features:
        if feature not in df.columns:
            print(f"ERROR: feature '{feature}' not exist cols: {df.columns.tolist()}")
            return

    print("\n--- SUMMARY STATISTICS FOR NUMERICAL FEATURES ---")
    summary_stats = df[numerical_features].describe()
    print(summary_stats)

    print("\n--- VISUALIZATIONS ---")
    
    print("pairwise scatter plots")
    if 'Genre' in df.columns:
        sns.pairplot(df, vars=numerical_features, hue='Genre', diag_kind='kde')
        plt.suptitle('Pairwise Scatter Plots - Colored by Genre', y=1.02)
        plt.show()
    else:
        print("'Genre' colmn not found. coloring by gender")
        sns.pairplot(df, vars=numerical_features, diag_kind='kde')
        plt.suptitle('Pairwise Scatter Plots - Colored by Gender', y=1.02)
        plt.show()

    print("\nbox plots")
    for feature in numerical_features:
        plt.figure(figsize=(8, 5))
        sns.boxplot(x=df[feature])
        plt.title(f'Box Plot of {feature}')
        plt.show()

    print("\n--- outlier detection using z-scores (|z| > 3) ---")
    df_for_outliers = df[numerical_features].copy()
    
    any_outliers_found_overall = False
    for feature in numerical_features:
        z_scores_feature = np.abs(stats.zscore(df[feature]))
        outliers_for_feature_df = df[z_scores_feature > 3]
        
        print(f"\potential outliers in '{feature}':")
        if not outliers_for_feature_df.empty:
            print(outliers_for_feature_df[numerical_features]) 
            any_outliers_found_overall = True
        else:
            print(f"no outliers in '{feature}' line71")

    if not any_outliers_found_overall:
        print("\nno outliers in any feature line 74")

    print("\nEDA finished.")

# 0.05 & 0.95
def cap_outliers(df, features):
    df_capped = df.copy()
    for feature in features:
        lower_cap = df[feature].quantile(0.05)
        upper_cap = df[feature].quantile(0.95)
        df_capped[feature] = df_capped[feature].clip(lower_cap, upper_cap)
    return df_capped

if __name__ == '__main__':
    csv_file_name = 'Mall_Customers.csv'
    perform_eda(csv_file_name)