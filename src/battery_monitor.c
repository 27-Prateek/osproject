#include "../include/battery_monitor.h"
#include <stdio.h>
#include <stdlib.h>


// GLOBAL VARIABLES


static BatteryInfo battery_info;
static BatteryThresholds battery_thresholds;
static bool is_initialized = false;


// INITIALIZATION AND CLEANUP


// Initialize battery monitor
int battery_monitor_init(void) {
    if (is_initialized) {
        log_error("Battery monitor already initialized");
        return ERROR;
    }
    
    // Initialize battery info with default values
    battery_info.current_level = 100;  // Start at full battery
    battery_info.state = BATTERY_STATE_DISCHARGING;
    battery_info.voltage = 4200;  // 4.2V in mV (typical Li-ion full charge)
    battery_info.current = 0;
    battery_info.temperature = 25;  // 25°C room temperature
    battery_info.last_update_time = get_current_time_ms();
    battery_info.discharge_rate = 5;  // 5% per hour default
    
    // Initialize thresholds with default values
    battery_thresholds.critical_threshold = BATTERY_CRITICAL;
    battery_thresholds.low_threshold = BATTERY_LOW;
    battery_thresholds.medium_threshold = BATTERY_MEDIUM;
    battery_thresholds.high_threshold = BATTERY_HIGH;
    
    is_initialized = true;
    log_info("Battery monitor initialized successfully");
    
    return SUCCESS;
}

// Cleanup battery monitor
void battery_monitor_cleanup(void) {
    if (!is_initialized) {
        return;
    }
    
    is_initialized = false;
    log_info("Battery monitor cleaned up");
}


// BATTERY INFORMATION RETRIEVAL


// Get battery information structure
BatteryInfo* get_battery_info(void) {
    if (!is_initialized) {
        log_error("Battery monitor not initialized");
        return NULL;
    }
    return &battery_info;
}

// Get current battery level
int get_battery_level(void) {
    if (!is_initialized) {
        log_error("Battery monitor not initialized");
        return ERROR;
    }
    return battery_info.current_level;
}

// Get battery state
BatteryState get_battery_state(void) {
    if (!is_initialized) {
        log_error("Battery monitor not initialized");
        return BATTERY_STATE_UNKNOWN;
    }
    return battery_info.state;
}

// Get discharge rate
int get_discharge_rate(void) {
    if (!is_initialized) {
        log_error("Battery monitor not initialized");
        return ERROR;
    }
    return battery_info.discharge_rate;
}


// UPDATE BATTERY STATUS


// Update battery status (simulated for now)
int update_battery_status(void) {
    if (!is_initialized) {
        log_error("Battery monitor not initialized");
        return ERROR;
    }
    
    long current_time = get_current_time_ms();
    long time_elapsed = current_time - battery_info.last_update_time;
    
    // Only update if charging or discharging
    if (battery_info.state == BATTERY_STATE_DISCHARGING) {
        // Simulate natural discharge (small amount over time)
        double hours_elapsed = time_elapsed / (1000.0 * 60.0 * 60.0);
        int drain = (int)(battery_info.discharge_rate * hours_elapsed);
        
        battery_info.current_level = max(0, battery_info.current_level - drain);
        
        // Update voltage based on level (linear approximation)
        battery_info.voltage = 3300 + (battery_info.current_level * 9);  // 3.3V to 4.2V
        
    } else if (battery_info.state == BATTERY_STATE_CHARGING) {
        // Simulate charging
        double hours_elapsed = time_elapsed / (1000.0 * 60.0 * 60.0);
        int charge = (int)(20 * hours_elapsed);  // 20% per hour charge rate
        
        battery_info.current_level = min(100, battery_info.current_level + charge);
        battery_info.voltage = 3300 + (battery_info.current_level * 9);
        
        if (battery_info.current_level >= 100) {
            battery_info.state = BATTERY_STATE_FULL;
        }
    }
    
    battery_info.last_update_time = current_time;
    
    return SUCCESS;
}

// Simulate battery drain from task execution
int simulate_battery_drain(int task_energy_cost) {
    if (!is_initialized) {
        log_error("Battery monitor not initialized");
        return ERROR;
    }
    
    // Drain battery based on energy cost
    int drain_amount = 0;
    
    switch(task_energy_cost) {
        case ENERGY_LOW:
            drain_amount = 1;
            break;
        case ENERGY_MEDIUM:
            drain_amount = 2;
            break;
        case ENERGY_HIGH:
            drain_amount = 3;
            break;
        default:
            drain_amount = 1;
            break;
    }
    
    battery_info.current_level = max(0, battery_info.current_level - drain_amount);
    battery_info.voltage = 3300 + (battery_info.current_level * 9);
    battery_info.last_update_time = get_current_time_ms();
    
    char log_msg[MAX_LOG_MSG];
    snprintf(log_msg, MAX_LOG_MSG, "Battery drained by %d%%. Current level: %d%%", 
             drain_amount, battery_info.current_level);
    log_debug(log_msg);
    
    return SUCCESS;
}


// BATTERY THRESHOLD MANAGEMENT


// Set battery thresholds
void set_battery_thresholds(BatteryThresholds *thresholds) {
    if (thresholds == NULL) {
        log_error("Invalid thresholds pointer");
        return;
    }
    
    battery_thresholds = *thresholds;
    log_info("Battery thresholds updated");
}

// Get battery thresholds
BatteryThresholds* get_battery_thresholds(void) {
    return &battery_thresholds;
}


// BATTERY LEVEL CHECKING


// Check if battery is critical
bool is_battery_critical(void) {
    if (!is_initialized) return false;
    return (battery_info.current_level <= battery_thresholds.critical_threshold);
}

// Check if battery is low
bool is_battery_low(void) {
    if (!is_initialized) return false;
    return (battery_info.current_level <= battery_thresholds.low_threshold);
}

// Check if battery is charging
bool is_battery_charging(void) {
    if (!is_initialized) return false;
    return (battery_info.state == BATTERY_STATE_CHARGING);
}


// BATTERY STATISTICS


// Print battery status
void print_battery_status(void) {
    if (!is_initialized) {
        printf("Battery monitor not initialized\n");
        return;
    }
    
    printf("\n=== Battery Status ===\n");
    printf("Level: %d%%\n", battery_info.current_level);
    printf("State: ");
    switch(battery_info.state) {
        case BATTERY_STATE_CHARGING:
            printf("Charging\n");
            break;
        case BATTERY_STATE_DISCHARGING:
            printf("Discharging\n");
            break;
        case BATTERY_STATE_FULL:
            printf("Full\n");
            break;
        default:
            printf("Unknown\n");
            break;
    }
    printf("Voltage: %d mV\n", battery_info.voltage);
    printf("Current: %d mA\n", battery_info.current);
    printf("Temperature: %d°C\n", battery_info.temperature);
    printf("Discharge Rate: %d%%/hour\n", battery_info.discharge_rate);
    printf("=====================\n\n");
}

// Estimate remaining battery time
int estimate_remaining_time(int current_load) {
    if (!is_initialized) {
        log_error("Battery monitor not initialized");
        return ERROR;
    }
    
    if (battery_info.state == BATTERY_STATE_CHARGING || 
        battery_info.state == BATTERY_STATE_FULL) {
        return -1;  // Unlimited when charging
    }
    
    // Calculate based on current level and discharge rate
    int effective_discharge_rate = battery_info.discharge_rate + current_load;
    
    if (effective_discharge_rate <= 0) {
        return -1;  // Unlimited
    }
    
    // Time in minutes
    int remaining_minutes = (battery_info.current_level * 60) / effective_discharge_rate;
    
    return remaining_minutes;
}
