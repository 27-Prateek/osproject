// /home/nishit/Desktop/OS/nishit/osproject/include/scheduler.h
#ifndef TASK_MANAGER_H
#define TASK_MANAGER_H

#include "utils.h"

// TASK STRUCTURES
// Task structure
typedef struct {
    int task_id;                    // Unique task identifier
    char task_name[MAX_TASK_NAME];  // Task name/description
    int priority;                   // Task priority (1=high, 2=med, 3=low)
    int energy_cost;                // Energy consumption level (1-3)
    int burst_time;                 // CPU burst time in ms
    int remaining_time;             // Remaining execution time in ms
    int arrival_time;               // Time when task arrived
    int start_time;                 // Time when task started execution
    int completion_time;            // Time when task completed
    int waiting_time;               // Time spent waiting
    int turnaround_time;            // Total time from arrival to completion
    TaskState state;                // Current task state
    bool is_critical;               // Is this a critical/urgent task?
    int deadline;                   // Deadline for task completion (ms)
} Task;

// Task queue structure
typedef struct {
    Task tasks[MAX_TASKS];          // Array of tasks
    int count;                      // Number of tasks in queue
    int front;                      // Front of queue index
    int rear;                       // Rear of queue index
} TaskQueue;

// Task statistics structure
typedef struct {
    int total_tasks;                // Total number of tasks processed
    int completed_tasks;            // Number of completed tasks
    int suspended_tasks;            // Number of suspended tasks
    double avg_waiting_time;        // Average waiting time
    double avg_turnaround_time;     // Average turnaround time
    int missed_deadlines;           // Number of missed deadlines
} TaskStats;

// ============================================
// TASK MANAGER FUNCTIONS
// ============================================

// Initialization and cleanup
int task_manager_init(void);
void task_manager_cleanup(void);

// Task creation and management
Task* create_task(const char *name, int priority, int energy_cost, 
                  int burst_time, bool is_critical, int deadline);
int add_task(Task *task);
int remove_task(int task_id);
Task* get_task(int task_id);

// Task queue operations
TaskQueue* create_task_queue(void);
void destroy_task_queue(TaskQueue *queue);
int enqueue_task(TaskQueue *queue, Task *task);
Task* dequeue_task(TaskQueue *queue);
bool is_queue_empty(TaskQueue *queue);
bool is_queue_full(TaskQueue *queue);
int get_queue_size(TaskQueue *queue);

// Task state management
int set_task_state(Task *task, TaskState state);
TaskState get_task_state(Task *task);
int update_task_times(Task *task);

// Task filtering and sorting
Task** get_tasks_by_priority(int priority, int *count);
Task** get_tasks_by_state(TaskState state, int *count);
Task** get_critical_tasks(int *count);
void sort_tasks_by_priority(Task *tasks[], int count);
void sort_tasks_by_energy(Task *tasks[], int count);

// Task statistics
TaskStats* get_task_statistics(void);
void update_task_statistics(Task *task);
void print_task_statistics(void);

// Task display
void print_task(Task *task);
void print_all_tasks(void);
void print_task_queue(TaskQueue *queue);

#endif // TASK_MANAGER_H
