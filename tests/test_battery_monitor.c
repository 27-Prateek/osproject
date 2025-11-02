// /home/nishit/Desktop/OS/nishit/osproject/src/task_manager.c
#include "../include/battery_monitor.h"
#include "../include/utils.h"
#include <stdio.h>
#include <assert.h>


// TEST COUNTER


static int tests_passed = 0;
static int tests_failed = 0;


// TEST HELPER MACROS


#define TEST_ASSERT(condition, message) \
    do { \
        if (condition) { \
            printf("[PASS] %s\n", message); \
            tests_passed++; \
        } else { \
            printf("[FAIL] %s\n", message); \
            tests_failed++; \
        } \
    } while(0)

#define RUN_TEST(test_func) \
    do { \
        printf("\n--- Running %s ---\n", #test_func); \
        test_func(); \
    } while(0)


// TEST FUNCTIONS


// Test battery monitor initialization
void test_battery_monitor_init(void) {
    int result = battery_monitor_init();
    TEST_ASSERT(result == SUCCESS, "Battery monitor initialization");
    
    BatteryInfo *info = get_battery_info();
    TEST_ASSERT(info != NULL, "Get battery info after init");
    TEST_ASSERT(info->current_level == 100, "Initial battery level is 100%");
    TEST_ASSERT(info->state == BATTERY_STATE_DISCHARGING, "Initial state is discharging");
    
    battery_monitor_cleanup();
}

// Test double initialization
void test_double_initialization(void) {
    battery_monitor_init();
    
    int result = battery_monitor_init();
    TEST_ASSERT(result == ERROR, "Prevent double initialization");
    
    battery_monitor_cleanup();
}

// Test battery level retrieval
void test_get_battery_level(void) {
    battery_monitor_init();
    
    int level = get_battery_level();
    TEST_ASSERT(level >= 0 && level <= 100, "Battery level in valid range");
    TEST_ASSERT(level == 100, "Initial battery level is 100%");
    
    battery_monitor_cleanup();
}

// Test battery state retrieval
void test_get_battery_state(void) {
    battery_monitor_init();
    
    BatteryState state = get_battery_state();
    TEST_ASSERT(state == BATTERY_STATE_DISCHARGING, "Get battery state");
    TEST_ASSERT(state != BATTERY_STATE_UNKNOWN, "State is not unknown");
    
    battery_monitor_cleanup();
}

// Test discharge rate
void test_discharge_rate(void) {
    battery_monitor_init();
    
    int rate = get_discharge_rate();
    TEST_ASSERT(rate > 0, "Discharge rate is positive");
    TEST_ASSERT(rate == 5, "Default discharge rate is 5%/hour");
    
    battery_monitor_cleanup();
}

// Test battery drain simulation
void test_simulate_battery_drain(void) {
    battery_monitor_init();
    
    int initial_level = get_battery_level();
    
    // Drain with low energy
    int result = simulate_battery_drain(ENERGY_LOW);
    TEST_ASSERT(result == SUCCESS, "Simulate low energy drain");
    int level_after_low = get_battery_level();
    TEST_ASSERT(level_after_low == initial_level - 1, "Battery drained by 1% (low energy)");
    
    // Drain with medium energy
    result = simulate_battery_drain(ENERGY_MEDIUM);
    TEST_ASSERT(result == SUCCESS, "Simulate medium energy drain");
    int level_after_medium = get_battery_level();
    TEST_ASSERT(level_after_medium == level_after_low - 2, "Battery drained by 2% (medium energy)");
    
    // Drain with high energy
    result = simulate_battery_drain(ENERGY_HIGH);
    TEST_ASSERT(result == SUCCESS, "Simulate high energy drain");
    int level_after_high = get_battery_level();
    TEST_ASSERT(level_after_high == level_after_medium - 3, "Battery drained by 3% (high energy)");
    
    battery_monitor_cleanup();
}

// Test battery threshold management
void test_battery_thresholds(void) {
    battery_monitor_init();
    
    BatteryThresholds custom_thresholds;
    custom_thresholds.critical_threshold = 5;
    custom_thresholds.low_threshold = 20;
    custom_thresholds.medium_threshold = 40;
    custom_thresholds.high_threshold = 70;
    
    set_battery_thresholds(&custom_thresholds);
    
    BatteryThresholds *thresholds = get_battery_thresholds();
    TEST_ASSERT(thresholds != NULL, "Get battery thresholds");
    TEST_ASSERT(thresholds->critical_threshold == 5, "Critical threshold set correctly");
    TEST_ASSERT(thresholds->low_threshold == 20, "Low threshold set correctly");
    TEST_ASSERT(thresholds->medium_threshold == 40, "Medium threshold set correctly");
    TEST_ASSERT(thresholds->high_threshold == 70, "High threshold set correctly");
    
    battery_monitor_cleanup();
}

// Test critical battery detection
void test_is_battery_critical(void) {
    battery_monitor_init();
    
    // At 100%, battery should not be critical
    TEST_ASSERT(!is_battery_critical(), "Battery not critical at 100%");
    
    // Drain to critical level
    for (int i = 0; i < 30; i++) {
        simulate_battery_drain(ENERGY_HIGH);
    }
    
    int level = get_battery_level();
    if (level <= BATTERY_CRITICAL) {
        TEST_ASSERT(is_battery_critical(), "Battery is critical at low level");
    } else {
        TEST_ASSERT(!is_battery_critical(), "Battery not critical above threshold");
    }
    
    battery_monitor_cleanup();
}

// Test low battery detection
void test_is_battery_low(void) {
    battery_monitor_init();
    
    // At 100%, battery should not be low
    TEST_ASSERT(!is_battery_low(), "Battery not low at 100%");
    
    // Drain to low level
    for (int i = 0; i < 26; i++) {
        simulate_battery_drain(ENERGY_HIGH);
    }
    
    int level = get_battery_level();
    if (level <= BATTERY_LOW) {
        TEST_ASSERT(is_battery_low(), "Battery is low at low level");
    } else {
        TEST_ASSERT(!is_battery_low(), "Battery not low above threshold");
    }
    
    battery_monitor_cleanup();
}

// Test charging state detection
void test_is_battery_charging(void) {
    battery_monitor_init();
    
    // Initially discharging
    TEST_ASSERT(!is_battery_charging(), "Battery not charging initially");
    
    // Manually set to charging state
    BatteryInfo *info = get_battery_info();
    info->state = BATTERY_STATE_CHARGING;
    
    TEST_ASSERT(is_battery_charging(), "Battery is charging");
    
    battery_monitor_cleanup();
}

// Test battery status update
void test_update_battery_status(void) {
    battery_monitor_init();
    
    int result = update_battery_status();
    TEST_ASSERT(result == SUCCESS, "Update battery status");
    
    BatteryInfo *info = get_battery_info();
    TEST_ASSERT(info->last_update_time > 0, "Last update time set");
    
    battery_monitor_cleanup();
}

// Test remaining time estimation
void test_estimate_remaining_time(void) {
    battery_monitor_init();
    
    // With full battery
    int remaining = estimate_remaining_time(5);
    TEST_ASSERT(remaining > 0, "Remaining time estimated");
    
    // When charging, should return -1 (unlimited)
    BatteryInfo *info = get_battery_info();
    info->state = BATTERY_STATE_CHARGING;
    remaining = estimate_remaining_time(5);
    TEST_ASSERT(remaining == -1, "Unlimited time when charging");
    
    battery_monitor_cleanup();
}

// Test voltage correlation with battery level
void test_voltage_correlation(void) {
    battery_monitor_init();
    
    BatteryInfo *info = get_battery_info();
    int initial_voltage = info->voltage;
    
    // Drain battery
    for (int i = 0; i < 10; i++) {
        simulate_battery_drain(ENERGY_HIGH);
    }
    
    int final_voltage = info->voltage;
    TEST_ASSERT(final_voltage < initial_voltage, "Voltage decreases with battery drain");
    
    battery_monitor_cleanup();
}

// Test battery level bounds
void test_battery_level_bounds(void) {
    battery_monitor_init();
    
    // Drain battery to 0%
    for (int i = 0; i < 40; i++) {
        simulate_battery_drain(ENERGY_HIGH);
    }
    
    int level = get_battery_level();
    TEST_ASSERT(level >= 0, "Battery level not below 0%");
    TEST_ASSERT(level <= 100, "Battery level not above 100%");
    
    battery_monitor_cleanup();
}

// Test battery info structure
void test_battery_info_structure(void) {
    battery_monitor_init();
    
    BatteryInfo *info = get_battery_info();
    TEST_ASSERT(info != NULL, "Battery info structure not NULL");
    TEST_ASSERT(info->current_level >= 0, "Valid current level");
    TEST_ASSERT(info->voltage > 0, "Valid voltage");
    TEST_ASSERT(info->temperature > -50 && info->temperature < 100, "Valid temperature range");
    TEST_ASSERT(info->discharge_rate > 0, "Valid discharge rate");
    
    battery_monitor_cleanup();
}

// Test cleanup without initialization
void test_cleanup_without_init(void) {
    // This should not crash
    battery_monitor_cleanup();
    TEST_ASSERT(true, "Cleanup without init does not crash");
}


// MAIN TEST RUNNER


int main(void) {
    printf("\n");
    printf("========================================\n");
    printf("   BATTERY MONITOR UNIT TESTS\n");
    printf("========================================\n");
    
    // Run all tests
    RUN_TEST(test_battery_monitor_init);
    RUN_TEST(test_double_initialization);
    RUN_TEST(test_get_battery_level);
    RUN_TEST(test_get_battery_state);
    RUN_TEST(test_discharge_rate);
    RUN_TEST(test_simulate_battery_drain);
    RUN_TEST(test_battery_thresholds);
    RUN_TEST(test_is_battery_critical);
    RUN_TEST(test_is_battery_low);
    RUN_TEST(test_is_battery_charging);
    RUN_TEST(test_update_battery_status);
    RUN_TEST(test_estimate_remaining_time);
    RUN_TEST(test_voltage_correlation);
    RUN_TEST(test_battery_level_bounds);
    RUN_TEST(test_battery_info_structure);
    RUN_TEST(test_cleanup_without_init);
    
    // Print summary
    printf("\n");
    printf("========================================\n");
    printf("   TEST SUMMARY\n");
    printf("========================================\n");
    printf("Tests Passed: %d\n", tests_passed);
    printf("Tests Failed: %d\n", tests_failed);
    printf("Total Tests: %d\n", tests_passed + tests_failed);
    printf("Success Rate: %.2f%%\n", 
           (tests_passed * 100.0) / (tests_passed + tests_failed));
    printf("========================================\n\n");
    
    return (tests_failed == 0) ? EXIT_SUCCESS : EXIT_FAILURE;
}
