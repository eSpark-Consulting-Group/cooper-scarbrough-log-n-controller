#! python

import pandas as pd
import os

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
    
    # Load your CSV file
    df = pd.read_csv(csv_file)

    # Delete the 'time' column if it exists
    if 'time' in df.columns:
        df = df.drop(columns=['time'])

    # Remove any inverted commas (quotes) from all string entries in the DataFrame
    df = df.applymap(lambda x: x.replace('"', ' ') if isinstance(x, str) else x)

    # Split the 'value' column into separate columns
    df[['TimeDuration', 'pH(actual)', 'pH(target)', 'Oxygen(actual)', 'Oxygen(target)', 'P1(Stimulant)', 'P2(acid)', 'P3(Base)']] = df['value'].str.replace('"', '').str.split(',', expand=True)

    # Drop the original 'value' column
    df = df.drop(columns=['value'])

    # Clean up whitespace from columns (optional, if there are spaces around values)
    df = df.apply(lambda x: x.str.strip() if x.dtype == "object" else x)

    # Format the 'TimeDuration' column as mm:ss
    def format_time(duration):
        try:
            
        
            total_seconds = int(duration)  # Assuming the value is in seconds

        # for sec
            # hours = total_seconds // 3600
            # minutes = (total_seconds % 3600) // 60
            # seconds = total_seconds % 60
            # return f"{hours:02}:{minutes:02}:{seconds:02}"

        # for mints
            minutes = (total_seconds ) // 60
            seconds = total_seconds % 60
            return f"{minutes:02}:{seconds:02}"
        
        except ValueError:
            return duration  # return original if not convertible

    df['TimeDuration'] = df['TimeDuration'].apply(format_time)

    df = df[df['TimeDuration'] != '00:00:00']

    df = df.dropna(how='all')

    # Display the updated DataFrame
    print(df)

    # Save the updated DataFrame to a new CSV file without quotes around values
    updated_file_name = 'updated_file.csv'
    df.to_csv(updated_file_name, index=False, quoting=0)  # quoting=0 means no quotes in output
    print(f"Updated CSV saved as '{updated_file_name}'")
