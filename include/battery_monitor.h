#ifndef BATTERY_MONITOR_H
#define BATTERY_MONITOR_H

#include "utils.h"

// BATTERY MONITOR STRUCTURES

// Battery information structure
typedef struct {
    int current_level;              // Current battery percentage (0-100)
    BatteryState state;             // Charging, discharging, full, etc.
    int voltage;                    // Battery voltage in mV
    int current;                    // Current draw in mA
    int temperature;                // Battery temperature in °C
    long last_update_time;          // Timestamp of last update
    int discharge_rate;             // Rate of discharge in %/hour
} BatteryInfo;

// Battery threshold configuration
typedef struct {
    int critical_threshold;         // Critical battery level
    int low_threshold;              // Low battery level
    int medium_threshold;           // Medium battery level
    int high_threshold;             // High battery level
} BatteryThresholds;


// BATTERY MONITOR FUNCTIONS
// Initialization and cleanup
int battery_monitor_init(void);
void battery_monitor_cleanup(void);

// Battery information retrieval
BatteryInfo* get_battery_info(void);
int get_battery_level(void);
BatteryState get_battery_state(void);
int get_discharge_rate(void);

// Update battery status
int update_battery_status(void);
int simulate_battery_drain(int task_energy_cost);

// Battery threshold management
void set_battery_thresholds(BatteryThresholds *thresholds);
BatteryThresholds* get_battery_thresholds(void);

// Battery level checking
bool is_battery_critical(void);
bool is_battery_low(void);
bool is_battery_charging(void);

// Battery statistics
void print_battery_status(void);
int estimate_remaining_time(int current_load);

#endif // BATTERY_MONITOR_H
