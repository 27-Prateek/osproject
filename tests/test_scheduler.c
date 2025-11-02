// /home/nishit/Desktop/OS/nishit/osproject/src/task_manager.c
#include "../include/scheduler.h"
#include "../include/battery_monitor.h"
#include "../include/task_manager.h"
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


// Test scheduler initialization
void test_scheduler_initialization(void) {
    int result = scheduler_init(SCHEDULER_BATTERY_AWARE);
    TEST_ASSERT(result == SUCCESS, "Scheduler initialization");
    
    SchedulerConfig *config = get_scheduler_config();
    TEST_ASSERT(config != NULL, "Get scheduler config");
    TEST_ASSERT(config->algorithm == SCHEDULER_BATTERY_AWARE, 
                "Scheduler algorithm set correctly");
    
    scheduler_cleanup();
}

// Test scheduler start/stop
void test_scheduler_control(void) {
    scheduler_init(SCHEDULER_BATTERY_AWARE);
    
    int result = scheduler_start();
    TEST_ASSERT(result == SUCCESS, "Scheduler start");
    
    result = scheduler_stop();
    TEST_ASSERT(result == SUCCESS, "Scheduler stop");
    
    result = scheduler_pause();
    TEST_ASSERT(result == ERROR, "Scheduler pause when not running");
    
    scheduler_cleanup();
}

// Test algorithm switching
void test_algorithm_switching(void) {
    scheduler_init(SCHEDULER_BATTERY_AWARE);
    
    int result = set_scheduler_algorithm(SCHEDULER_FCFS);
    TEST_ASSERT(result == SUCCESS, "Switch to FCFS");
    
    SchedulerConfig *config = get_scheduler_config();
    TEST_ASSERT(config->algorithm == SCHEDULER_FCFS, "FCFS algorithm set");
    
    result = set_scheduler_algorithm(SCHEDULER_PRIORITY);
    TEST_ASSERT(result == SUCCESS, "Switch to Priority");
    TEST_ASSERT(config->algorithm == SCHEDULER_PRIORITY, "Priority algorithm set");
    
    scheduler_cleanup();
}

// Test mode determination based on battery
void test_mode_determination(void) {
    SchedulerMode mode;
    
    mode = determine_scheduler_mode(100);
    TEST_ASSERT(mode == MODE_PERFORMANCE, "Performance mode at 100%");
    
    mode = determine_scheduler_mode(60);
    TEST_ASSERT(mode == MODE_PERFORMANCE, "Performance mode at 60%");
    
    mode = determine_scheduler_mode(40);
    TEST_ASSERT(mode == MODE_BALANCED, "Balanced mode at 40%");
    
    mode = determine_scheduler_mode(20);
    TEST_ASSERT(mode == MODE_POWER_SAVE, "Power save mode at 20%");
    
    mode = determine_scheduler_mode(5);
    TEST_ASSERT(mode == MODE_CRITICAL, "Critical mode at 5%");
}

// Test task admission control
void test_task_admission(void) {
    scheduler_init(SCHEDULER_BATTERY_AWARE);
    
    // Create tasks with different characteristics
    Task *critical_task = create_task("Critical", PRIORITY_HIGH, ENERGY_LOW, 
                                      500, true, 5000);
    TEST_ASSERT(critical_task != NULL, "Create critical task");
    
    Task *normal_task = create_task("Normal", PRIORITY_MEDIUM, ENERGY_MEDIUM, 
                                    600, false, 8000);
    TEST_ASSERT(normal_task != NULL, "Create normal task");
    
    // Test admission in performance mode
    set_scheduler_mode(MODE_PERFORMANCE);
    TEST_ASSERT(can_admit_task(critical_task), "Admit critical task in performance mode");
    TEST_ASSERT(can_admit_task(normal_task), "Admit normal task in performance mode");
    
    // Test admission in critical mode
    set_scheduler_mode(MODE_CRITICAL);
    TEST_ASSERT(can_admit_task(critical_task), "Admit critical task in critical mode");
    TEST_ASSERT(!can_admit_task(normal_task), "Reject normal task in critical mode");
    
    scheduler_cleanup();
}

// Test task scheduling
void test_task_scheduling(void) {
    scheduler_init(SCHEDULER_BATTERY_AWARE);
    scheduler_start();
    
    // Create and admit tasks
    Task *task1 = create_task("Task1", PRIORITY_HIGH, ENERGY_LOW, 200, false, 5000);
    Task *task2 = create_task("Task2", PRIORITY_MEDIUM, ENERGY_MEDIUM, 300, false, 8000);
    
    int result1 = admit_task_to_scheduler(task1);
    int result2 = admit_task_to_scheduler(task2);
    
    TEST_ASSERT(result1 == SUCCESS, "Admit task 1");
    TEST_ASSERT(result2 == SUCCESS, "Admit task 2");
    
    // Select next task
    Task *next = select_next_task();
    TEST_ASSERT(next != NULL, "Select next task");
    
    scheduler_cleanup();
}

// Test battery-aware scheduling
void test_battery_aware_scheduling(void) {
    scheduler_init(SCHEDULER_BATTERY_AWARE);
    scheduler_start();
    
    // Create tasks with different energy costs
    Task *low_energy = create_task("LowEnergy", PRIORITY_MEDIUM, ENERGY_LOW, 
                                   300, false, 8000);
    Task *high_energy = create_task("HighEnergy", PRIORITY_MEDIUM, ENERGY_HIGH, 
                                    300, false, 8000);
    
    admit_task_to_scheduler(high_energy);
    admit_task_to_scheduler(low_energy);
    
    // Set low battery mode
    set_scheduler_mode(MODE_POWER_SAVE);
    
    // Battery-aware scheduler should prefer low-energy tasks
    Task *selected = select_next_task();
    TEST_ASSERT(selected != NULL, "Select task in power save mode");
    
    scheduler_cleanup();
}

// Test preemption
void test_preemption(void) {
    scheduler_init(SCHEDULER_BATTERY_AWARE);
    
    Task *task = create_task("TestTask", PRIORITY_MEDIUM, ENERGY_MEDIUM, 
                            1000, false, 10000);
    
    schedule_task(task);
    TEST_ASSERT(task->state == TASK_STATE_RUNNING, "Task scheduled and running");
    
    int result = preempt_task(task);
    TEST_ASSERT(result == SUCCESS, "Task preempted");
    TEST_ASSERT(task->state == TASK_STATE_READY, "Task state changed to ready");
    
    scheduler_cleanup();
}

// Test task suspension and resume
void test_suspend_resume(void) {
    scheduler_init(SCHEDULER_BATTERY_AWARE);
    
    Task *task = create_task("TestTask", PRIORITY_MEDIUM, ENERGY_MEDIUM, 
                            500, false, 8000);
    
    int result = suspend_task(task);
    TEST_ASSERT(result == SUCCESS, "Task suspended");
    TEST_ASSERT(task->state == TASK_STATE_SUSPENDED, "Task state is suspended");
    
    result = resume_task(task);
    TEST_ASSERT(result == SUCCESS, "Task resumed");
    TEST_ASSERT(task->state == TASK_STATE_READY, "Task state is ready");
    
    scheduler_cleanup();
}

// Test scheduler statistics
void test_scheduler_statistics(void) {
    scheduler_init(SCHEDULER_BATTERY_AWARE);
    scheduler_start();
    
    // Create and execute a task
    Task *task = create_task("TestTask", PRIORITY_HIGH, ENERGY_LOW, 
                            100, false, 5000);
    admit_task_to_scheduler(task);
    
    SchedulerStats *stats = get_scheduler_statistics();
    TEST_ASSERT(stats != NULL, "Get scheduler statistics");
    TEST_ASSERT(stats->total_tasks_scheduled >= 1, "Task count updated");
    
    scheduler_cleanup();
}

// Test time quantum configuration
void test_time_quantum(void) {
    scheduler_init(SCHEDULER_ROUND_ROBIN);
    
    int result = set_time_quantum(200);
    TEST_ASSERT(result == SUCCESS, "Set time quantum");
    
    SchedulerConfig *config = get_scheduler_config();
    TEST_ASSERT(config->time_quantum == 200, "Time quantum updated");
    
    scheduler_cleanup();
}

// Test context switching
void test_context_switching(void) {
    scheduler_init(SCHEDULER_BATTERY_AWARE);
    
    Task *task1 = create_task("Task1", PRIORITY_HIGH, ENERGY_LOW, 200, false, 5000);
    Task *task2 = create_task("Task2", PRIORITY_MEDIUM, ENERGY_MEDIUM, 300, false, 8000);
    
    int result = perform_context_switch(task1, task2);
    TEST_ASSERT(result == SUCCESS, "Context switch performed");
    
    SchedulerStats *stats = get_scheduler_statistics();
    TEST_ASSERT(stats->context_switches > 0, "Context switch count updated");
    
    scheduler_cleanup();
}


// MAIN TEST RUNNER


int main(void) {
    printf("\n");
    printf("========================================\n");
    printf("   SCHEDULER UNIT TESTS\n");
    printf("========================================\n");
    
    // Run all tests
    RUN_TEST(test_scheduler_initialization);
    RUN_TEST(test_scheduler_control);
    RUN_TEST(test_algorithm_switching);
    RUN_TEST(test_mode_determination);
    RUN_TEST(test_task_admission);
    RUN_TEST(test_task_scheduling);
    RUN_TEST(test_battery_aware_scheduling);
    RUN_TEST(test_preemption);
    RUN_TEST(test_suspend_resume);
    RUN_TEST(test_scheduler_statistics);
    RUN_TEST(test_time_quantum);
    RUN_TEST(test_context_switching);
    
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
