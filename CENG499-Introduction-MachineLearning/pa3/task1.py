import pandas as pd
from sklearn.preprocessing import MinMaxScaler
import os
def load_and_preprocess_data(csv_file_path: str):
    if not os.path.exists(csv_file_path):
        print(f"error: '{os.path.basename(csv_file_path)}' not found")
        return None

    df = pd.read_csv(csv_file_path)
    #print("raw dataset")
    #print(df.head())

    print("\nmissing values")
    missing_values = df.isnull().sum()
    print(missing_values[missing_values > 0])
    if df.isnull().any().any():
        print("missing values-imputing with median...")
        for col in df.select_dtypes(include=['number']).columns:
            if df[col].isnull().any():
                df[col].fillna(df[col].median(), inplace=True)
        print("missing values fixed")
    else:
        print("no missing values")
    #Age, Annual Income, and Spending Score
    features_to_select = ['Age', 'Annual Income (k$)', 'Spending Score (1-100)']
    missing_cols = [col for col in features_to_select if col not in df.columns]
    if missing_cols:
        print(f"missing cols: {missing_cols}")
        return None
        
    df_selected_features = df[features_to_select].copy()
    #print(df_selected_features.head())

    print("\nnormalization")
    scaler = MinMaxScaler()
    df_normalized = pd.DataFrame(scaler.fit_transform(df_selected_features), columns=df_selected_features.columns)
    #print(df_normalized.head())

    return df_normalized

if __name__ == '__main__':
    csv_file_name = 'Mall_Customers.csv'

    normalized_df = load_and_preprocess_data(csv_file_name)
    
    if normalized_df is not None:
        print("\npreprocessing finished")
    else:
        print("\npreprocessing failed")