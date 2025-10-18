#include "../include/task_manager.h"
#include "../include/utils.h"
#include <stdio.h>
#include <assert.h>
#include <string.h>


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


// Test task manager initialization
void test_task_manager_init(void) {
    int result = task_manager_init();
    TEST_ASSERT(result == SUCCESS, "Task manager initialization");
    
    TaskStats *stats = get_task_statistics();
    TEST_ASSERT(stats != NULL, "Get task statistics after init");
    TEST_ASSERT(stats->total_tasks == 0, "Initial task count is 0");
    
    task_manager_cleanup();
}

// Test double initialization
void test_double_initialization(void) {
    task_manager_init();
    
    int result = task_manager_init();
    TEST_ASSERT(result == ERROR, "Prevent double initialization");
    
    task_manager_cleanup();
}

// Test task creation
void test_create_task(void) {
    task_manager_init();
    
    Task *task = create_task("TestTask", PRIORITY_HIGH, ENERGY_LOW, 
                             500, false, 5000);
    
    TEST_ASSERT(task != NULL, "Create task");
    TEST_ASSERT(strcmp(task->task_name, "TestTask") == 0, "Task name set correctly");
    TEST_ASSERT(task->priority == PRIORITY_HIGH, "Task priority set correctly");
    TEST_ASSERT(task->energy_cost == ENERGY_LOW, "Task energy cost set correctly");
    TEST_ASSERT(task->burst_time == 500, "Task burst time set correctly");
    TEST_ASSERT(task->remaining_time == 500, "Task remaining time equals burst time");
    TEST_ASSERT(task->is_critical == false, "Task critical flag set correctly");
    TEST_ASSERT(task->deadline == 5000, "Task deadline set correctly");
    TEST_ASSERT(task->state == TASK_STATE_READY, "Task initial state is READY");
    
    task_manager_cleanup();
}

// Test critical task creation
void test_create_critical_task(void) {
    task_manager_init();
    
    Task *task = create_task("CriticalTask", PRIORITY_HIGH, ENERGY_MEDIUM, 
                             800, true, 3000);
    
    TEST_ASSERT(task != NULL, "Create critical task");
    TEST_ASSERT(task->is_critical == true, "Critical flag set correctly");
    
    task_manager_cleanup();
}

// Test task ID generation
void test_task_id_generation(void) {
    task_manager_init();
    
    Task *task1 = create_task("Task1", PRIORITY_MEDIUM, ENERGY_LOW, 300, false, 5000);
    Task *task2 = create_task("Task2", PRIORITY_MEDIUM, ENERGY_LOW, 300, false, 5000);
    Task *task3 = create_task("Task3", PRIORITY_MEDIUM, ENERGY_LOW, 300, false, 5000);
    
    TEST_ASSERT(task1->task_id != task2->task_id, "Task IDs are unique");
    TEST_ASSERT(task2->task_id != task3->task_id, "Task IDs are unique");
    TEST_ASSERT(task1->task_id < task2->task_id, "Task IDs are sequential");
    TEST_ASSERT(task2->task_id < task3->task_id, "Task IDs are sequential");
    
    task_manager_cleanup();
}

// Test get task by ID
void test_get_task(void) {
    task_manager_init();
    
    Task *task = create_task("TestTask", PRIORITY_HIGH, ENERGY_LOW, 500, false, 5000);
    int task_id = task->task_id;
    
    Task *retrieved = get_task(task_id);
    TEST_ASSERT(retrieved != NULL, "Get task by ID");
    TEST_ASSERT(retrieved->task_id == task_id, "Retrieved correct task");
    
    Task *not_found = get_task(9999);
    TEST_ASSERT(not_found == NULL, "Return NULL for non-existent task");
    
    task_manager_cleanup();
}

// Test remove task
void test_remove_task(void) {
    task_manager_init();
    
    Task *task = create_task("TestTask", PRIORITY_HIGH, ENERGY_LOW, 500, false, 5000);
    int task_id = task->task_id;
    
    int result = remove_task(task_id);
    TEST_ASSERT(result == SUCCESS, "Remove task");
    
    Task *retrieved = get_task(task_id);
    TEST_ASSERT(retrieved == NULL, "Task removed successfully");
    
    result = remove_task(9999);
    TEST_ASSERT(result == ERROR, "Error when removing non-existent task");
    
    task_manager_cleanup();
}

// Test task queue creation
void test_create_task_queue(void) {
    TaskQueue *queue = create_task_queue();
    
    TEST_ASSERT(queue != NULL, "Create task queue");
    TEST_ASSERT(queue->count == 0, "Queue initially empty");
    TEST_ASSERT(is_queue_empty(queue), "Queue is empty");
    TEST_ASSERT(!is_queue_full(queue), "Queue is not full");
    
    destroy_task_queue(queue);
}

// Test enqueue and dequeue
void test_enqueue_dequeue(void) {
    task_manager_init();
    TaskQueue *queue = create_task_queue();
    
    Task *task1 = create_task("Task1", PRIORITY_HIGH, ENERGY_LOW, 300, false, 5000);
    Task *task2 = create_task("Task2", PRIORITY_MEDIUM, ENERGY_MEDIUM, 400, false, 6000);
    
    // Enqueue tasks
    int result1 = enqueue_task(queue, task1);
    int result2 = enqueue_task(queue, task2);
    
    TEST_ASSERT(result1 == SUCCESS, "Enqueue task 1");
    TEST_ASSERT(result2 == SUCCESS, "Enqueue task 2");
    TEST_ASSERT(get_queue_size(queue) == 2, "Queue size is 2");
    TEST_ASSERT(!is_queue_empty(queue), "Queue is not empty");
    
    // Dequeue tasks
    Task *dequeued1 = dequeue_task(queue);
    TEST_ASSERT(dequeued1 != NULL, "Dequeue task 1");
    TEST_ASSERT(dequeued1->task_id == task1->task_id, "FIFO order maintained");
    
    Task *dequeued2 = dequeue_task(queue);
    TEST_ASSERT(dequeued2 != NULL, "Dequeue task 2");
    TEST_ASSERT(dequeued2->task_id == task2->task_id, "FIFO order maintained");
    
    TEST_ASSERT(is_queue_empty(queue), "Queue is empty after dequeuing all");
    
    destroy_task_queue(queue);
    task_manager_cleanup();
}

// Test queue empty and full
void test_queue_empty_full(void) {
    task_manager_init();
    TaskQueue *queue = create_task_queue();
    
    TEST_ASSERT(is_queue_empty(queue), "Queue initially empty");
    
    // Fill queue to maximum
    for (int i = 0; i < MAX_TASKS; i++) {
        char name[MAX_TASK_NAME];
        snprintf(name, MAX_TASK_NAME, "Task%d", i);
        Task *task = create_task(name, PRIORITY_MEDIUM, ENERGY_LOW, 300, false, 5000);
        enqueue_task(queue, task);
    }
    
    TEST_ASSERT(is_queue_full(queue), "Queue is full");
    TEST_ASSERT(get_queue_size(queue) == MAX_TASKS, "Queue size at maximum");
    
    // Try to enqueue when full
    Task *extra = create_task("Extra", PRIORITY_LOW, ENERGY_LOW, 200, false, 4000);
    int result = enqueue_task(queue, extra);
    TEST_ASSERT(result == ERROR, "Cannot enqueue when full");
    
    destroy_task_queue(queue);
    task_manager_cleanup();
}

// Test task state management
void test_task_state_management(void) {
    task_manager_init();
    
    Task *task = create_task("TestTask", PRIORITY_HIGH, ENERGY_LOW, 500, false, 5000);
    
    TEST_ASSERT(task->state == TASK_STATE_READY, "Initial state is READY");
    
    set_task_state(task, TASK_STATE_RUNNING);
    TEST_ASSERT(task->state == TASK_STATE_RUNNING, "State changed to RUNNING");
    
    set_task_state(task, TASK_STATE_WAITING);
    TEST_ASSERT(task->state == TASK_STATE_WAITING, "State changed to WAITING");
    
    set_task_state(task, TASK_STATE_COMPLETED);
    TEST_ASSERT(task->state == TASK_STATE_COMPLETED, "State changed to COMPLETED");
    
    task_manager_cleanup();
}

// Test task timing updates
void test_update_task_times(void) {
    task_manager_init();
    
    Task *task = create_task("TestTask", PRIORITY_HIGH, ENERGY_LOW, 500, false, 5000);
    
    // Set start time
    task->start_time = get_current_time_ms();
    sleep_ms(50);
    
    int result = update_task_times(task);
    TEST_ASSERT(result == SUCCESS, "Update task times");
    TEST_ASSERT(task->waiting_time >= 0, "Waiting time calculated");
    
    // Set completion time
    sleep_ms(50);
    task->completion_time = get_current_time_ms();
    update_task_times(task);
    
    TEST_ASSERT(task->turnaround_time > 0, "Turnaround time calculated");
    
    task_manager_cleanup();
}

// Test task statistics
void test_task_statistics(void) {
    task_manager_init();
    
    Task *task = create_task("TestTask", PRIORITY_HIGH, ENERGY_LOW, 100, false, 5000);
    
    TaskStats *stats = get_task_statistics();
    int initial_total = stats->total_tasks;
    
    TEST_ASSERT(stats->total_tasks > 0, "Total tasks count updated");
    
    // Complete the task
    task->start_time = get_current_time_ms();
    task->completion_time = task->start_time + 100;
    task->turnaround_time = 100;
    task->waiting_time = 0;
    set_task_state(task, TASK_STATE_COMPLETED);
    
    TEST_ASSERT(stats->completed_tasks > 0, "Completed tasks count updated");
    
    task_manager_cleanup();
}

// Test task priorities
void test_task_priorities(void) {
    task_manager_init();
    
    Task *high = create_task("High", PRIORITY_HIGH, ENERGY_LOW, 300, false, 5000);
    Task *medium = create_task("Medium", PRIORITY_MEDIUM, ENERGY_LOW, 300, false, 5000);
    Task *low = create_task("Low", PRIORITY_LOW, ENERGY_LOW, 300, false, 5000);
    
    TEST_ASSERT(high->priority == PRIORITY_HIGH, "High priority task");
    TEST_ASSERT(medium->priority == PRIORITY_MEDIUM, "Medium priority task");
    TEST_ASSERT(low->priority == PRIORITY_LOW, "Low priority task");
    TEST_ASSERT(high->priority < medium->priority, "High priority < Medium priority");
    TEST_ASSERT(medium->priority < low->priority, "Medium priority < Low priority");
    
    task_manager_cleanup();
}

// Test task energy costs
void test_task_energy_costs(void) {
    task_manager_init();
    
    Task *low = create_task("Low", PRIORITY_MEDIUM, ENERGY_LOW, 300, false, 5000);
    Task *medium = create_task("Medium", PRIORITY_MEDIUM, ENERGY_MEDIUM, 300, false, 5000);
    Task *high = create_task("High", PRIORITY_MEDIUM, ENERGY_HIGH, 300, false, 5000);
    
    TEST_ASSERT(low->energy_cost == ENERGY_LOW, "Low energy cost task");
    TEST_ASSERT(medium->energy_cost == ENERGY_MEDIUM, "Medium energy cost task");
    TEST_ASSERT(high->energy_cost == ENERGY_HIGH, "High energy cost task");
    TEST_ASSERT(low->energy_cost < medium->energy_cost, "Low < Medium energy");
    TEST_ASSERT(medium->energy_cost < high->energy_cost, "Medium < High energy");
    
    task_manager_cleanup();
}

// Test task name length
void test_task_name_length(void) {
    task_manager_init();
    
    // Very long name
    char long_name[MAX_TASK_NAME + 50];
    for (int i = 0; i < MAX_TASK_NAME + 40; i++) {
        long_name[i] = 'A';
    }
    long_name[MAX_TASK_NAME + 40] = '\0';
    
    Task *task = create_task(long_name, PRIORITY_MEDIUM, ENERGY_LOW, 300, false, 5000);
    
    TEST_ASSERT(task != NULL, "Create task with long name");
    TEST_ASSERT(strlen(task->task_name) < MAX_TASK_NAME, "Task name truncated to max length");
    
    task_manager_cleanup();
}

// Test maximum tasks limit
void test_maximum_tasks_limit(void) {
    task_manager_init();
    
    // Create maximum tasks
    int created = 0;
    for (int i = 0; i < MAX_TASKS + 5; i++) {
        char name[MAX_TASK_NAME];
        snprintf(name, MAX_TASK_NAME, "Task%d", i);
        Task *task = create_task(name, PRIORITY_MEDIUM, ENERGY_LOW, 300, false, 5000);
        if (task != NULL) {
            created++;
        }
    }
    
    TEST_ASSERT(created <= MAX_TASKS, "Cannot create more than MAX_TASKS");
    
    task_manager_cleanup();
}

// Test cleanup without initialization
void test_cleanup_without_init(void) {
    // This should not crash
    task_manager_cleanup();
    TEST_ASSERT(true, "Cleanup without init does not crash");
}


// MAIN TEST RUNNER


int main(void) {
    printf("\n");
    printf("========================================\n");
    printf("   TASK MANAGER UNIT TESTS\n");
    printf("========================================\n");
    
    // Run all tests
    RUN_TEST(test_task_manager_init);
    RUN_TEST(test_double_initialization);
    RUN_TEST(test_create_task);
    RUN_TEST(test_create_critical_task);
    RUN_TEST(test_task_id_generation);
    RUN_TEST(test_get_task);
    RUN_TEST(test_remove_task);
    RUN_TEST(test_create_task_queue);
    RUN_TEST(test_enqueue_dequeue);
    RUN_TEST(test_queue_empty_full);
    RUN_TEST(test_task_state_management);
    RUN_TEST(test_update_task_times);
    RUN_TEST(test_task_statistics);
    RUN_TEST(test_task_priorities);
    RUN_TEST(test_task_energy_costs);
    RUN_TEST(test_task_name_length);
    RUN_TEST(test_maximum_tasks_limit);
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
