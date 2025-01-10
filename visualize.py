import pandas as pd
import matplotlib.pyplot as plt
import sys

# Define the column names
column_names = ['Type', 'Tree Construction Time', 'Tree Construction Memory', 'Query Time']

# Read the CSV file without headers, and assign column names manually
df = pd.read_csv(sys.argv[1], header=None, names=column_names)

# Group by 'Type' and calculate the mean for each metric
df_avg = df.groupby('Type').mean()

# Reshape the data to match the desired format
df_avg = df_avg.T  # Transpose the dataframe so that metrics are on the x-axis

# Plotting the data
fig, ax = plt.subplots(figsize=(10, 6))

# Define the width of each bar and the positions for each group
bar_width = 0.2  # Width of each bar
positions = range(len(df_avg))  # Position of bars on x-axis

# Create bars for each type in a grouped manner
types = df_avg.columns  # Get types (Fixed, Variable, HashMap, VariableSIMD)

# Create bars for each type, each type's bars will be placed next to each other
for i, type_name in enumerate(types):
    ax.bar(
        [pos + i * bar_width for pos in positions],  # Adjust position for each type
        df_avg[type_name],  # Values for each type
        width=bar_width,  # Set the width of bars
        label=type_name  # Label for the legend
    )

# Adjust the x-axis labels to represent the metrics
ax.set_xticks([pos + bar_width * (len(types) - 1) / 2 for pos in positions])
ax.set_xticklabels(df_avg.index)  # Set the labels as the metrics names

# Adding titles and labels
plt.title('Average Performance Metrics for Different Tree Types', fontsize=16)
plt.xlabel('Metrics', fontsize=12)
plt.ylabel('Average Values', fontsize=12)
plt.xticks(rotation=0)  # Keep x-axis labels horizontal
plt.legend(title="Type", bbox_to_anchor=(1.05, 1), loc='upper left')

# Display the plot
plt.tight_layout()
plt.show()

# Display the averaged data in the console
print(df_avg)
