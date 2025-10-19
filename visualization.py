import pandas as pd
import matplotlib.pyplot as plt
scheduler_data = pd.read_csv("scheduler_mode.csv")
non_scheduler_data = pd.read_csv("non_scheduler_mode.csv")
plt.plot(pd.to_datetime(scheduler_data["Timestamp"], unit="s"), scheduler_data["BatteryPercent"], label="Scheduler Mode")
plt.plot(pd.to_datetime(non_scheduler_data["Timestamp"], unit="s"), non_scheduler_data["BatteryPercent"], label="Non-Scheduler Mode")
plt.title("Battery Drain Comparison")
plt.xlabel("Time")
plt.ylabel("Battery (%)")
plt.legend()
plt.grid(True)
scheduler_drop = scheduler_data["BatteryPercent"].iloc[0] - scheduler_data["BatteryPercent"].iloc[-1]
non_scheduler_drop = non_scheduler_data["BatteryPercent"].iloc[0] - non_scheduler_data["BatteryPercent"].iloc[-1]
if non_scheduler_drop > 0:
    battery_saved = ((non_scheduler_drop - scheduler_drop) / non_scheduler_drop) * 100
    plt.text(0.5, 0.95, f"Battery Saved: {battery_saved:.2f}%", transform=plt.gca().transAxes, ha="center")
plt.show()