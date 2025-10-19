import pandas as pd
import matplotlib.pyplot as plt

# Read both CSVs
df1 = pd.read_csv('non_scheduler_mode.csv')
df2 = pd.read_csv('scheduler_mode.csv')

# Since both start together, just use index as time
df1['Time (s)'] = range(len(df1))
df2['Time (s)'] = range(len(df2))

# If data has repeated battery percent entries, average them by time if needed
df1 = df1.groupby('Time (s)', as_index=False)['BatteryPercent'].mean()
df2 = df2.groupby('Time (s)', as_index=False)['BatteryPercent'].mean()

# Plot both discharge curves
plt.figure(figsize=(10,6))
plt.plot(df1['Time (s)'], df1['BatteryPercent'], label='Non-Scheduler Mode', marker='o')
plt.plot(df2['Time (s)'], df2['BatteryPercent'], label='Scheduler Mode', marker='x')

plt.xlabel('Time (seconds)')
plt.ylabel('Battery Percentage')
plt.title('Battery Discharge Comparison (Scheduler vs Non-Scheduler Mode)')
plt.legend()
plt.grid(True)
plt.tight_layout()
plt.show()
