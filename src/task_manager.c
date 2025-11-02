// /home/nishit/Desktop/OS/nishit/osproject/src/task_manager.c
#include "../include/task_manager.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


// GLOBAL VARIABLES


static Task tasks[MAX_TASKS];
static int task_count = 0;
static int next_task_id = 1;
static TaskStats task_stats;
static bool is_initialized = false;


// INITIALIZATION AND CLEANUP


// Initialize task manager
int task_manager_init(void) {
    if (is_initialized) {
        log_error("Task manager already initialized");
        return ERROR;
    }
    
    task_count = 0;
    next_task_id = 1;
    
    // Initialize statistics
    task_stats.total_tasks = 0;
    task_stats.completed_tasks = 0;
    task_stats.suspended_tasks = 0;
    task_stats.avg_waiting_time = 0.0;
    task_stats.avg_turnaround_time = 0.0;
    task_stats.missed_deadlines = 0;
    
    is_initialized = true;
    log_info("Task manager initialized successfully");
    
    return SUCCESS;
}

// Cleanup task manager
void task_manager_cleanup(void) {
    if (!is_initialized) {
        return;
    }
    
    task_count = 0;
    next_task_id = 1;
    is_initialized = false;
    log_info("Task manager cleaned up");
}


// TASK CREATION AND MANAGEMENT


// Create a new task
Task* create_task(const char *name, int priority, int energy_cost, 
                  int burst_time, bool is_critical, int deadline) {
    if (!is_initialized) {
        log_error("Task manager not initialized");
        return NULL;
    }
    
    if (task_count >= MAX_TASKS) {
        log_error("Maximum task limit reached");
        return NULL;
    }
    
    Task *task = &tasks[task_count];
    
    task->task_id = next_task_id++;
    strncpy(task->task_name, name, MAX_TASK_NAME - 1);
    task->task_name[MAX_TASK_NAME - 1] = '\0';
    task->priority = priority;
    task->energy_cost = energy_cost;
    task->burst_time = burst_time;
    task->remaining_time = burst_time;
    task->arrival_time = get_current_time_ms();
    task->start_time = 0;
    task->completion_time = 0;
    task->waiting_time = 0;
    task->turnaround_time = 0;
    task->state = TASK_STATE_READY;
    task->is_critical = is_critical;
    task->deadline = deadline;
    
    task_count++;
    task_stats.total_tasks++;
    
    char log_msg[MAX_LOG_MSG];
    snprintf(log_msg, MAX_LOG_MSG, "Task created: ID=%d, Name=%s, Priority=%d, Energy=%d", 
             task->task_id, task->task_name, task->priority, task->energy_cost);
    log_info(log_msg);
    
    return task;
}

// Add task to the task list
int add_task(Task *task) {
    if (!is_initialized || task == NULL) {
        log_error("Invalid task or task manager not initialized");
        return ERROR;
    }
    
    if (task_count >= MAX_TASKS) {
        log_error("Cannot add task: maximum limit reached");
        return ERROR;
    }
    
    return SUCCESS;
}

// Remove task from the list
int remove_task(int task_id) {
    if (!is_initialized) {
        log_error("Task manager not initialized");
        return ERROR;
    }
    
    for (int i = 0; i < task_count; i++) {
        if (tasks[i].task_id == task_id) {
            // Shift remaining tasks
            for (int j = i; j < task_count - 1; j++) {
                tasks[j] = tasks[j + 1];
            }
            task_count--;
            
            char log_msg[MAX_LOG_MSG];
            snprintf(log_msg, MAX_LOG_MSG, "Task removed: ID=%d", task_id);
            log_info(log_msg);
            
            return SUCCESS;
        }
    }
    
    log_error("Task not found");
    return ERROR;
}

// Get task by ID
Task* get_task(int task_id) {
    if (!is_initialized) {
        log_error("Task manager not initialized");
        return NULL;
    }
    
    for (int i = 0; i < task_count; i++) {
        if (tasks[i].task_id == task_id) {
            return &tasks[i];
        }
    }
    
    return NULL;
}


// TASK QUEUE OPERATIONS


// Create a new task queue
TaskQueue* create_task_queue(void) {
    TaskQueue *queue = (TaskQueue*)safe_malloc(sizeof(TaskQueue));
    queue->count = 0;
    queue->front = 0;
    queue->rear = -1;
    
    return queue;
}

// Destroy task queue
void destroy_task_queue(TaskQueue *queue) {
    if (queue != NULL) {
        free(queue);
    }
}

// Enqueue a task
int enqueue_task(TaskQueue *queue, Task *task) {
    if (queue == NULL || task == NULL) {
        log_error("Invalid queue or task");
        return ERROR;
    }
    
    if (is_queue_full(queue)) {
        log_error("Queue is full");
        return ERROR;
    }
    
    queue->rear = (queue->rear + 1) % MAX_TASKS;
    queue->tasks[queue->rear] = *task;
    queue->count++;
    
    return SUCCESS;
}

// Dequeue a task
Task* dequeue_task(TaskQueue *queue) {
    if (queue == NULL || is_queue_empty(queue)) {
        return NULL;
    }
    
    Task *task = &queue->tasks[queue->front];
    queue->front = (queue->front + 1) % MAX_TASKS;
    queue->count--;
    
    return task;
}

// Check if queue is empty
bool is_queue_empty(TaskQueue *queue) {
    if (queue == NULL) return true;
    return (queue->count == 0);
}

// Check if queue is full
bool is_queue_full(TaskQueue *queue) {
    if (queue == NULL) return true;
    return (queue->count >= MAX_TASKS);
}

// Get queue size
int get_queue_size(TaskQueue *queue) {
    if (queue == NULL) return 0;
    return queue->count;
}


// TASK STATE MANAGEMENT


// Set task state
int set_task_state(Task *task, TaskState state) {
    if (task == NULL) {
        log_error("Invalid task");
        return ERROR;
    }
    
    task->state = state;
    
    if (state == TASK_STATE_RUNNING && task->start_time == 0) {
        task->start_time = get_current_time_ms();
    } else if (state == TASK_STATE_COMPLETED) {
        task->completion_time = get_current_time_ms();
        update_task_times(task);
        update_task_statistics(task);
    } else if (state == TASK_STATE_SUSPENDED) {
        task_stats.suspended_tasks++;
    }
    
    return SUCCESS;
}

// Get task state
TaskState get_task_state(Task *task) {
    if (task == NULL) {
        return TASK_STATE_READY;
    }
    return task->state;
}

// Update task timing information
int update_task_times(Task *task) {
    if (task == NULL) {
        return ERROR;
    }
    
    // long current_time = get_current_time_ms();
    
    if (task->completion_time > 0) {
        task->turnaround_time = task->completion_time - task->arrival_time;
        task->waiting_time = task->turnaround_time - task->burst_time;
    } else if (task->start_time > 0) {
        task->waiting_time = task->start_time - task->arrival_time;
    }
    
    return SUCCESS;
}


// TASK FILTERING AND SORTING


// Get tasks by priority (simplified - returns count)
Task** get_tasks_by_priority(int priority, int *count) {
    *count = 0;
    for (int i = 0; i < task_count; i++) {
        if (tasks[i].priority == priority) {
            (*count)++;
        }
    }
    return NULL;  // Simplified for now
}

// Get tasks by state
Task** get_tasks_by_state(TaskState state, int *count) {
    *count = 0;
    for (int i = 0; i < task_count; i++) {
        if (tasks[i].state == state) {
            (*count)++;
        }
    }
    return NULL;  // Simplified for now
}

// Get critical tasks
Task** get_critical_tasks(int *count) {
    *count = 0;
    for (int i = 0; i < task_count; i++) {
        if (tasks[i].is_critical) {
            (*count)++;
        }
    }
    return NULL;  // Simplified for now
}

// Sort tasks by priority (bubble sort for simplicity)
void sort_tasks_by_priority(Task *task_array[], int count) {
    for (int i = 0; i < count - 1; i++) {
        for (int j = 0; j < count - i - 1; j++) {
            if (task_array[j]->priority > task_array[j + 1]->priority) {
                Task *temp = task_array[j];
                task_array[j] = task_array[j + 1];
                task_array[j + 1] = temp;
            }
        }
    }
}

// Sort tasks by energy cost
void sort_tasks_by_energy(Task *task_array[], int count) {
    for (int i = 0; i < count - 1; i++) {
        for (int j = 0; j < count - i - 1; j++) {
            if (task_array[j]->energy_cost > task_array[j + 1]->energy_cost) {
                Task *temp = task_array[j];
                task_array[j] = task_array[j + 1];
                task_array[j + 1] = temp;
            }
        }
    }
}


// TASK STATISTICS


// Get task statistics
TaskStats* get_task_statistics(void) {
    return &task_stats;
}

// Update task statistics
void update_task_statistics(Task *task) {
    if (task == NULL || task->state != TASK_STATE_COMPLETED) {
        return;
    }
    
    task_stats.completed_tasks++;
    
    // Update average waiting time
    double total_waiting = task_stats.avg_waiting_time * (task_stats.completed_tasks - 1);
    total_waiting += task->waiting_time;
    task_stats.avg_waiting_time = total_waiting / task_stats.completed_tasks;
    
    // Update average turnaround time
    double total_turnaround = task_stats.avg_turnaround_time * (task_stats.completed_tasks - 1);
    total_turnaround += task->turnaround_time;
    task_stats.avg_turnaround_time = total_turnaround / task_stats.completed_tasks;
    
    // Check for missed deadline
    if (task->deadline > 0 && task->completion_time > (task->arrival_time + task->deadline)) {
        task_stats.missed_deadlines++;
    }
}

// Print task statistics
void print_task_statistics(void) {
    printf("\n=== Task Statistics ===\n");
    printf("Total Tasks: %d\n", task_stats.total_tasks);
    printf("Completed Tasks: %d\n", task_stats.completed_tasks);
    printf("Suspended Tasks: %d\n", task_stats.suspended_tasks);
    printf("Average Waiting Time: %.2f ms\n", task_stats.avg_waiting_time);
    printf("Average Turnaround Time: %.2f ms\n", task_stats.avg_turnaround_time);
    printf("Missed Deadlines: %d\n", task_stats.missed_deadlines);
    printf("=====================\n\n");
}


// TASK DISPLAY


// Print single task
void print_task(Task *task) {
    if (task == NULL) {
        printf("Invalid task\n");
        return;
    }
    
    printf("Task ID: %d | Name: %s | Priority: %d | Energy: %d | Burst: %d ms | State: ", 
           task->task_id, task->task_name, task->priority, task->energy_cost, task->burst_time);
    
    switch(task->state) {
        case TASK_STATE_READY: printf("READY"); break;
        case TASK_STATE_RUNNING: printf("RUNNING"); break;
        case TASK_STATE_WAITING: printf("WAITING"); break;
        case TASK_STATE_COMPLETED: printf("COMPLETED"); break;
        case TASK_STATE_SUSPENDED: printf("SUSPENDED"); break;
    }
    
    printf(" | Critical: %s\n", task->is_critical ? "YES" : "NO");
}

// Print all tasks
void print_all_tasks(void) {
    printf("\n=== All Tasks ===\n");
    for (int i = 0; i < task_count; i++) {
        print_task(&tasks[i]);
    }
    printf("=================\n\n");
}

// Print task queue
void print_task_queue(TaskQueue *queue) {
    if (queue == NULL || is_queue_empty(queue)) {
        printf("Queue is empty\n");
        return;
    }
    
    printf("\n=== Task Queue (Size: %d) ===\n", queue->count);
    int index = queue->front;
    for (int i = 0; i < queue->count; i++) {
        print_task(&queue->tasks[index]);
        index = (index + 1) % MAX_TASKS;
    }
    printf("============================\n\n");
}
