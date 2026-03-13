import pandas as pd
import numpy as np
import matplotlib.pyplot as plt
import seaborn as sns
from task1 import load_weather_dataset, load_air_quality_dataset

def analyze_weather_dataset(weather_df):
    print("\n=== Weather Dataset Analysis ===")
    
    original_df = pd.read_csv('daily-minimum-temperatures-in-me.csv', 
                             header=None, 
                             names=['Date', 'Temperature'])
    original_df['Date'] = pd.to_datetime(original_df['Date'].str.strip('"'))
    
    # SUMMARY STATISTICS
    feature_cols = [f'Temp_{i}' for i in range(1, 8)]
    print("\nSummary Statistics for Weather Features:")
    print(weather_df[feature_cols].describe())
    
    # TIME SERIES VISUALIZATION
    plt.figure(figsize=(12, 6))
    plt.plot(original_df['Date'], original_df['Temperature'])
    plt.title('Daily Minimum Temperatures in Melbourne')
    plt.xlabel('Date')
    plt.ylabel('Temperature (°C)')
    plt.grid(True)
    plt.tight_layout()
    plt.savefig('weather_time_series.png')
    
    # SEASONAL PATTERNS
    plt.figure(figsize=(12, 6))
    original_df['Month'] = original_df['Date'].dt.month
    monthly_avg = original_df.groupby('Month')['Temperature'].mean()
    plt.plot(monthly_avg.index, monthly_avg.values, marker='o')
    plt.title('Average Monthly Minimum Temperature in Melbourne')
    plt.xlabel('Month')
    plt.ylabel('Average Temperature (°C)')
    plt.xticks(range(1, 13))
    plt.grid(True)
    plt.tight_layout()
    plt.savefig('weather_monthly_pattern.png')
    
    # CLASS DISTRIBUTION
    class_counts = weather_df['Target'].value_counts()
    # IMBALANCE RATIO
    # minority/majority
    imbalance_ratio = class_counts.min() / class_counts.max()
    
    plt.figure(figsize=(10, 6))
    plt.pie(class_counts, labels=['Below Mean', 'Above Mean'], autopct='%1.1f%%', 
            colors=['skyblue', 'salmon'], startangle=90)
    plt.title(f'Weather Dataset Class Distribution\nImbalance Ratio: {imbalance_ratio:.2f}')
    plt.savefig('weather_class_distribution.png')
    
    print(f"\nClass distribution: {class_counts}")
    print(f"Class imbalance ratio: {imbalance_ratio:.2f}")
    

def analyze_air_quality_dataset(aqi_df):
    print("\n=== Air Quality Dataset Analysis ===")
    
    # SUMMARY STATISTICS
    feature_cols = [f'PM2.5_{i}' for i in range(1, 8)]
    print("\nSummary Statistics for Air Quality Features:")
    print(aqi_df[feature_cols].describe())
    
    # TIME SERIES VISUALIZATION
    plt.figure(figsize=(12, 6))
    plt.plot(aqi_df['Date'], aqi_df['PM2.5'])
    plt.axhline(y=100, color='r', linestyle='--', label='Unhealthy Threshold (PM2.5 = 100)')
    plt.title('Daily Average PM2.5 Levels in India')
    plt.xlabel('Date')
    plt.ylabel('PM2.5')
    plt.legend()
    plt.grid(True)
    plt.tight_layout()
    plt.savefig('aqi_time_series.png')
    
    # SEASONAL PATTERNS
    aqi_df['Month'] = pd.DatetimeIndex(aqi_df['Date']).month
    monthly_avg = aqi_df.groupby('Month')['PM2.5'].mean()
    
    plt.figure(figsize=(12, 6))
    plt.plot(monthly_avg.index, monthly_avg.values, marker='o')
    plt.title('Average Monthly PM2.5 Levels')
    plt.xlabel('Month')
    plt.ylabel('Average PM2.5')
    plt.xticks(range(1, 13))
    plt.axhline(y=100, color='r', linestyle='--', label='Unhealthy Threshold (PM2.5 = 100)')
    plt.legend()
    plt.grid(True)
    plt.tight_layout()
    plt.savefig('aqi_monthly_pattern.png')
    
    # CLASS DISTRIBUTION
    class_counts = aqi_df['Target'].value_counts()
    # IMBALANCE RATIO
    imbalance_ratio = class_counts.min() / class_counts.max()
    
    plt.figure(figsize=(10, 6))
    plt.pie(class_counts, labels=['Under 100', 'Over 100'], autopct='%1.1f%%', 
            colors=['lightgreen', 'tomato'], startangle=90)
    plt.title(f'Air Quality Dataset Class Distribution\nImbalance Ratio: {imbalance_ratio:.2f}')
    plt.savefig('aqi_class_distribution.png')
    
    print(f"\nClass distribution: {class_counts}")
    print(f"Class imbalance ratio: {imbalance_ratio:.2f}")

if __name__ == "__main__":
    try:
        weather_data = pd.read_csv('preprocessed_weather_data.csv', parse_dates=['Date'])
        air_quality_data = pd.read_csv('preprocessed_air_quality_data.csv', parse_dates=['Date'])
    except FileNotFoundError:
        print("Preprocessed files not found. Loading and preprocessing raw data...")
        weather_data = load_weather_dataset()
        air_quality_data = load_air_quality_dataset()
    
    analyze_weather_dataset(weather_data)
    analyze_air_quality_dataset(air_quality_data)
    
    print("\nexploratory data analysis completed.")
