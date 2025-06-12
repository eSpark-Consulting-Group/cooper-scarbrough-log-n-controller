import pandas as pd
import os
from datetime import datetime

# Get the current directory
current_directory = os.getcwd()
print(f"Current Directory: {current_directory}")

# List all files in the current directory
files = os.listdir(current_directory)
print(f"Files in Directory: {files}")

# Filter for CSV files
csv_files = [file for file in files if file.endswith('.csv')]

# Check if any CSV files were found
if not csv_files:
    print("No CSV files found in the current directory.")
else:
    # Load the first CSV file found
    csv_file = csv_files[0]
    print(f"Processing file: {csv_file}")
    
    # Load CSV
    df = pd.read_csv(csv_file)

    # Drop 'time' column if it exists
    if 'time' in df.columns:
        df = df.drop(columns=['time'])

    # Remove quote characters from all string entries
    df = df.map(lambda x: x.replace('"', ' ') if isinstance(x, str) else x)

    # Define known invalid header string to ignore
    invalid_header = "Time , pH(actual), pH(Traget) ,Oxygen(actual), Oxygen(target),P1(Stimulant),P2(Acid),P3(Base)"

    # Filter out rows with invalid or missing 'value' field
    def is_valid_value(val):
        if not isinstance(val, str):
            return False
        if val.strip() == "":
            return False
        if val.strip() == invalid_header:
            return False
        parts = val.split(',')
        return len(parts) == 8

    df = df[df['value'].apply(is_valid_value)]

    # Now split the value column into individual columns
    df[['TimeDuration', 'pH(actual)', 'pH(target)', 'Oxygen(actual)', 'Oxygen(target)', 'P1(Stimulant)', 'P2(base)', 'P3(acid)']] = df['value'].str.strip().str.split(',', expand=True)

    # Drop the original 'value' column
    df = df.drop(columns=['value'])

    # Strip whitespace from string columns
    df = df.apply(lambda x: x.str.strip() if x.dtype == "object" else x)

    # Format TimeDuration as mm:ss
    def format_time(duration):
        try:
            total_seconds = int(duration)
            minutes = total_seconds // 60
            seconds = total_seconds % 60
            return f"{minutes:02}:{seconds:02}"
        except ValueError:
            return duration

    df['TimeDuration'] = df['TimeDuration'].apply(format_time)

    # # Remove entries where TimeDuration is "00:00" or all values are NaN
    # df = df[df['TimeDuration'] != '00:00']
    # df = df.dropna(how='all')

    # Show the cleaned dataframe
    print(df)

    # Save to timestamped CSV
    timestamp = datetime.now().strftime("%Y-%m-%d_%H-%M-%S")
    updated_file_name = f'Timestamp_{timestamp}.csv'
    df.to_csv(updated_file_name, index=False, quoting=0)
    print(f"Updated CSV saved as '{updated_file_name}'")
