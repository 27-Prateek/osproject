# Battery-Aware Laptop Scheduler

## Overview

A battery-aware scheduler implemented in C for Linux, designed to optimize power consumption by monitoring battery status, executing real tasks, and adjusting screen brightness. The scheduler can operate in two modes: **scheduler mode** (with task skipping and brightness control) and **non-scheduler mode** (without optimization). Logs are saved to `scheduler.log`, and battery data is recorded in `scheduler_mode.csv` or `non_scheduler_mode.csv` for analysis.

## Features

- Monitors battery charge and discharging status via `/sys/class/power_supply`
- Executes real tasks (e.g., `uptime`, `stress --cpu 1 --timeout 5`, `date`), skipping high-energy tasks when battery ≤ 20% in scheduler mode
- Adjusts brightness (50% when battery ≤ 20%, 100% otherwise) in scheduler mode
- Logs all actions to `scheduler.log` and battery data to CSV files
- Supports visualization of battery drain with Python/Matplotlib, showing percentage drops and differences

## Requirements

- Linux with `/sys/class/power_supply/BAT0` and `/sys/class/backlight`
- Root privileges for brightness control
- GCC, Make, `stress` for high-energy tasks
- Python 3, pandas, and Matplotlib for visualization

## Setup

### 1. Install Dependencies

```bash
sudo apt update
sudo apt install gcc make stress python3 python3-pip
pip3 install pandas matplotlib
```

### 2. Clone or Navigate to Project Directory

```bash
cd ~/Desktop/battery_aware_scheduler
```

### 3. Build the Project

```bash
make
```

This generates the executable `build/bin/scheduler`.

## How to Run

### Scheduler Mode

**Description:** Runs the scheduler with task skipping (skips stress when battery ≤ 20%) and brightness control (50% when low, 100% when high).

**Steps:**

1. Ensure the laptop is unplugged to observe discharging (optional but recommended)

2. Run the scheduler:
   ```bash
   sudo ./build/bin/scheduler
   ```

3. The program runs for 10 minutes (RUN_DURATION = 600 seconds) and logs data to `scheduler_mode.csv`

4. Check logs after execution:
   ```bash
   cat scheduler.log
   cat scheduler_mode.csv
   ```

**Expected Behavior:** Skips high-energy tasks and adjusts brightness when battery is low.

### Non-Scheduler Mode

**Description:** Runs the scheduler without task skipping or brightness control, executing all tasks regardless of battery level.

**Steps:**

1. Modify `src/scheduler.c` to disable optimizations:
   - Change `#define LOW_BATTERY_THRESHOLD 20` to `#define LOW_BATTERY_THRESHOLD 0`
   - In the `schedule_tasks` function, comment out the line:
     ```c
     // set_brightness(battery_level, is_discharging, csv_file);
     ```

2. Rebuild the project:
   ```bash
   make
   ```

3. Ensure the laptop is unplugged to observe discharging

4. Run the scheduler:
   ```bash
   sudo ./build/bin/scheduler --non-scheduler
   ```

5. The program runs for 10 minutes and logs data to 'non_scheduler_mode.csv/'sudo non_scheduler_mode.csv'

6. Check logs after execution:
   ```bash
   cat scheduler.log
   cat non_scheduler_mode.csv
   ```

**Expected Behavior:** Executes all tasks (including stress) and maintains default brightness.

## Visualization

Analyze battery drain by comparing `scheduler_mode.csv` and `non_scheduler_mode.csv`.

### Create Visualization Script



### Run the Visualization

```bash
python3 visulization.py
```

### Run the Visualization

```bash
python3 visualization1.py
```




**Expected Output:** A plot showing battery levels over time with annotations for the percentage drop in each mode and the percentage difference.

## Notes

- Modify `BRIGHTNESS_PATH` in `src/scheduler.c` if your backlight differs (e.g., `acpi_video0`)
- Run with `sudo` if brightness control fails
- Ensure `stress` is installed for high-energy task simulation
- Check `scheduler.log` for detailed action logs

### Permission Denied Errors

Ensure you run the scheduler with `sudo` for system-level operations:

```bash
sudo ./build/bin/scheduler
```
## 👥 Contributors

| Name | Email | GitHub |
|------|--------|--------|
| Prateek | [202351109@iiitvadodara.ac.in](mailto:202351109@iiitvadodara.ac.in) | [@27-Prateek](https://github.com/27-Prateek) |
| Parth Jindal | [202351097@iiitvadodara.ac.in](mailto:202351097@iiitvadodara.ac.in) | [@ParthJindal23k](https://github.com/ParthJindal23k) |
| Nishit Prajapati | [202351106@iiitvadodara.ac.in](mailto:202351106@iiitvadodara.ac.in) | [@NishitVSP](https://github.com/NishitVSP) |

