import numpy as np
from sklearn.model_selection import train_test_split
from scipy.stats import ks_2samp
from task1 import load_and_preprocess_data

def split_data(df_normalized, seed):
    np.random.seed(seed)
    
    # training 0.6  - 0.4 validation + test
    train_df, temp_df = train_test_split(df_normalized, test_size=0.4, random_state=seed, shuffle=True)
    
    # validation 0.2 test 0.2
    validation_df, test_df = train_test_split(temp_df, test_size=0.5, random_state=seed, shuffle=True)
    
    return train_df, validation_df, test_df

def check_covariate_shift(train_df, validation_df, test_df, features):
    p_values_results = {}
    all_p_values = []

    for feature in features:
        # train vs val
        ks_stat_tv, p_val_tv = ks_2samp(train_df[feature], validation_df[feature])
        p_values_results[f'{feature}_train_vs_validation'] = p_val_tv
        all_p_values.append(p_val_tv)

        # train vs test
        ks_stat_tt, p_val_tt = ks_2samp(train_df[feature], test_df[feature])
        p_values_results[f'{feature}_train_vs_test'] = p_val_tt
        all_p_values.append(p_val_tt)

        # val vs test
        ks_stat_vt, p_val_vt = ks_2samp(validation_df[feature], test_df[feature])
        p_values_results[f'{feature}_validation_vs_test'] = p_val_vt
        all_p_values.append(p_val_vt)
        
    min_p_value = np.min(all_p_values) if all_p_values else 0
    return p_values_results, min_p_value

def find_optimal_split_seed(df_normalized, features, max_iterations=1000, p_value_threshold=0.05):
    best_seed = -1
    max_min_p_value = -1.0
    best_p_values_report = {}
    found_ideal_seed = False

    for seed_val in range(max_iterations):
        current_seed = seed_val
        train_df, validation_df, test_df = split_data(df_normalized, current_seed)
        
        p_values_report, current_min_p = check_covariate_shift(train_df, validation_df, test_df, features)
        
        print(f"\ncurrent_seed {current_seed}:")
        print(f"  current_min_p: {current_min_p}")
        for test_name, p_value in p_values_report.items():
            print(f"  {test_name}: {p_value}")
        
        all_above_threshold = all(p > p_value_threshold for p in p_values_report.values())

        if all_above_threshold:
            print(f"  FOUND IDEAL SEED {current_seed} - all p-values > {p_value_threshold}")
            best_seed = current_seed
            best_p_values_report = p_values_report
            found_ideal_seed = True
            break 
            
        if current_min_p > max_min_p_value:
            max_min_p_value = current_min_p
            best_seed = current_seed
            best_p_values_report = p_values_report
            print(f"  NEW BEST SEED {current_seed} max_min_p_value: {max_min_p_value}")

    if not found_ideal_seed:
        print(f"\nnot find a seed -> all p-values > {p_value_threshold}. iterations till now: {max_iterations}")
        print(f"best seed {best_seed} max_min_p_value w/best seed: {max_min_p_value}.")
    
    return best_seed, best_p_values_report

if __name__ == '__main__':
    csv_file = 'Mall_Customers.csv'
    normalized_data = load_and_preprocess_data(csv_file)

    if normalized_data is not None:
        features_for_ks_test = normalized_data.columns.tolist()
        
        print(f"\noptimal split seed search starts")
        chosen_seed, final_p_values = find_optimal_split_seed(normalized_data, features_for_ks_test)

        print(f"\n--- covariate shift check results ---")
        print(f"seed: {chosen_seed}")
        print("p-values for ks tests:")
        for test_name, p_value in final_p_values.items():
            print(f"  {test_name}: {p_value:.4f}")

        final_train_df, final_validation_df, final_test_df = split_data(normalized_data, chosen_seed)
        
    else:
        print("\nFAIL line 100 task2.py")
