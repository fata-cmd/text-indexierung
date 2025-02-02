import pandas as pd
import matplotlib.pyplot as plt
import sys
import numpy as np

# Define column names
column_names = ['File', 'Type', 'Tree Construction Time', 'Tree Construction Memory', 'Query Time']

# Read the CSV file
csv_file = sys.argv[1]  # Take file path as command-line argument
df = pd.read_csv(csv_file, header=None, names=column_names)

# Group by 'File' and 'Type', then calculate the mean for each metric
df_avg = df.groupby(['File', 'Type']).mean().reset_index()

# Get unique files and attributes
files = df_avg['File'].unique()
attributes = ['Tree Construction Time', 'Tree Construction Memory', 'Query Time']

# Generate a plot for each file and each metric
for file in files:
    df_file = df_avg[df_avg['File'] == file]  # Filter data for the specific file

    for attribute in attributes:
        fig, ax = plt.subplots(figsize=(8, 5))

        # Extract data for plotting
        types = df_file['Type'].unique()  # Different types (e.g., Fixed, Variable)
        values = [df_file[df_file['Type'] == type_name][attribute].values[0] for type_name in types]

        # Create a bar plot
        ax.bar(types, values, color=['blue', 'green', 'red', 'orange', 'grey'])

        # Set labels and title
        plt.title(f'{file}', fontsize=14)
        plt.xlabel('Type', fontsize=12)
        plt.ylabel(attribute, fontsize=12)
        plt.xticks(rotation=20, ha='right')

        # Save and display the plot
        plt.tight_layout()
        filename = f"./resources/visualizations/{file}_{attribute.replace(' ', '_')}.png"
        plt.savefig(filename, dpi=300)
