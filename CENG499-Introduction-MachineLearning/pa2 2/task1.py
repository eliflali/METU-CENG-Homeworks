import pandas as pd
import numpy as np
from sklearn.preprocessing import StandardScaler

def load_weather_dataset(filepath='daily-minimum-temperatures-in-me.csv'):
    try:
        with open(filepath, 'r') as f:
            lines = f.readlines()
        
        dates = []
        temps = []
        for line in lines:
            # skip the first line if exists
            # deleted column names
            if ',' in line:
                parts = line.strip().split(',')
                if len(parts) == 2:
                    date_str = parts[0].strip('"')
                    temp_str = parts[1].strip('"')
                    if date_str and temp_str:
                        try:
                            dates.append(pd.to_datetime(date_str))
                            temps.append(float(temp_str))
                        except Exception as e:
                            print(f"invalid line: {line}, error: {e}")
        
        weather_df = pd.DataFrame({'Date': dates, 'Temperature': temps})
        
        # sort by date
        weather_df = weather_df.sort_values('Date').reset_index(drop=True)
        
    except Exception as e:
        print(f"Error parsing file: {e}")
        raise
    
    # MISSING VALUES
    # isna(): check if there is any missing value
    # sum(): count the number of missing values
    missing_values = weather_df.isna().sum()
    if missing_values.sum() > 0:
        print(f"Missing values in weather dataset: {missing_values}")
        # interpolate(method='linear', *, axis=0, limit=None, 
        # inplace=False, limit_direction=None, limit_area=None, 
        # downcast=<no_default>, **kwargs)
        weather_df['Temperature'] = weather_df['Temperature'].interpolate(method='linear')
    
    # MEAN TEMP
    mean_temp = weather_df['Temperature'].mean()
    print(f"Mean temperature: {mean_temp:.2f}")
    
    # TARGET LABEL
    # (1 if next day's temp > mean, 0 otherwise)
    # in this line thought:
    # we should check i+1=next day's temp and compare it with the mean temp
    # write the target to day i's column
    # if Ti+1 > μ, yi = 1, else yi = 0
    # shift(-1): down by 1
    weather_df['Target'] = (weather_df['Temperature'].shift(-1) > mean_temp).astype(int)
    
    ## FEATURE ENGINEERING
    # 7-day sliding window
    # shift(7-i): shift by 7-i
    for i in range(1, 8):
        weather_df[f'Temp_{i}'] = weather_df['Temperature'].shift(7-i)
    
    # MISSING VALUES
    # first 7 days dont have all 7 past temps
    # last day dont have target label
    # here check:
    # is it true creating target labels like that?
    weather_features = weather_df.dropna().reset_index(drop=True)
    
    # NORMALIZE
    # StandardScaler(*, copy=True, with_mean=True, with_std=True)
    scaler = StandardScaler()
    feature_cols = [f'Temp_{i}' for i in range(1, 8)]
    weather_features[feature_cols] = scaler.fit_transform(weather_features[feature_cols])
    
    print(f"weather dataset shape after: {weather_features.shape}")
    return weather_features

def load_air_quality_dataset(filepath='air-quality-india.csv'):
    air_quality_df = pd.read_csv(filepath)
    
    # DATE COLUMN
    air_quality_df['Date'] = pd.to_datetime(air_quality_df[['Year', 'Month', 'Day']])
    
    # AVERAGING DAILY VALS
    daily_aqi = air_quality_df.groupby('Date')['PM2.5'].mean().reset_index()
    
    # MISSING VALUES
    missing_values = daily_aqi.isna().sum()
    if missing_values.sum() > 0:
        print(f"Missing values in air quality dataset: {missing_values}")
        daily_aqi['PM2.5'] = daily_aqi['PM2.5'].interpolate(method='linear')
    
    daily_aqi['Target'] = (daily_aqi['PM2.5'].shift(-1) > 100).astype(int)
    
    # FEATURE ENGINEERING
    # 7-day sliding window
    for i in range(1, 8):
        daily_aqi[f'PM2.5_{i}'] = daily_aqi['PM2.5'].shift(7-i)
    
    # MISSING VALUES
    aqi_features = daily_aqi.dropna().reset_index(drop=True)
    
    # NORMALIZE
    feature_cols = [f'PM2.5_{i}' for i in range(1, 8)]
    scaler = StandardScaler()
    aqi_features[feature_cols] = scaler.fit_transform(aqi_features[feature_cols])
    
    print(f"air quality dataset shape after: {aqi_features.shape}")
    return aqi_features

if __name__ == "__main__":
    weather_data = load_weather_dataset()
    air_quality_data = load_air_quality_dataset()

    weather_data.to_csv('preprocessed_weather_data.csv', index=False)
    air_quality_data.to_csv('preprocessed_air_quality_data.csv', index=False)
    
    print("task1 completed datasets saved")
