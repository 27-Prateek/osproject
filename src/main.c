#include "../include/scheduler.h"
#include "../include/battery_monitor.h"
#include "../include/task_manager.h"
#include "../include/utils.h"
#include <stdio.h>
#include <stdlib.h>


// FUNCTION DECLARATIONS


void print_banner(void);
void print_menu(void);
void create_sample_tasks(void);
void run_simulation(void);
void interactive_mode(void);


// MAIN FUNCTION


int main(int argc, char *argv[]) {
    print_banner();
    
    // Initialize scheduler with battery-aware algorithm
    if (scheduler_init(SCHEDULER_BATTERY_AWARE) != SUCCESS) {
        log_error("Failed to initialize scheduler");
        return EXIT_FAILURE;
    }
    
    log_info("Battery-Aware Scheduler System Started");
    
    // Check command line arguments
    if (argc > 1 && strcmp(argv[1], "--simulate") == 0) {
        // Run automatic simulation
        run_simulation();
    } else {
        // Run interactive mode
        interactive_mode();
    }
    
    // Cleanup
    scheduler_cleanup();
    log_info("Battery-Aware Scheduler System Shutdown");
    
    return EXIT_SUCCESS;
}


// HELPER FUNCTIONS


// Print banner
void print_banner(void) {
    printf("\n");
    printf("========================================\n");
    printf("   BATTERY-AWARE SCHEDULER SYSTEM\n");
    printf("========================================\n");
    printf("  A Power-Efficient Task Scheduler\n");
    printf("========================================\n\n");
}

// Print menu
void print_menu(void) {
    printf("\n=== MENU ===\n");
    printf("1. Create Sample Tasks\n");
    printf("2. Add Custom Task\n");
    printf("3. Start Scheduler\n");
    printf("4. Stop Scheduler\n");
    printf("5. Show Battery Status\n");
    printf("6. Show Scheduler Status\n");
    printf("7. Show Task Statistics\n");
    printf("8. Show Scheduler Statistics\n");
    printf("9. Change Scheduler Algorithm\n");
    printf("10. Simulate Battery Drain\n");
    printf("11. Run Full Simulation\n");
    printf("0. Exit\n");
    printf("============\n");
    printf("Enter choice: ");
}

// Create sample tasks for testing
void create_sample_tasks(void) {
    log_info("Creating sample tasks...");
    
    // Critical low-energy task
    Task *task1 = create_task("System Monitor", PRIORITY_HIGH, ENERGY_LOW, 
                              500, true, 5000);
    admit_task_to_scheduler(task1);
    
    // High priority medium energy task
    Task *task2 = create_task("File Sync", PRIORITY_HIGH, ENERGY_MEDIUM, 
                              800, false, 10000);
    admit_task_to_scheduler(task2);
    
    // Medium priority low energy task
    Task *task3 = create_task("Log Writer", PRIORITY_MEDIUM, ENERGY_LOW, 
                              300, false, 8000);
    admit_task_to_scheduler(task3);
    
    // Low priority high energy task
    Task *task4 = create_task("Video Processing", PRIORITY_LOW, ENERGY_HIGH, 
                              1200, false, 20000);
    admit_task_to_scheduler(task4);
    
    // Medium priority medium energy task
    Task *task5 = create_task("Network Sync", PRIORITY_MEDIUM, ENERGY_MEDIUM, 
                              600, false, 12000);
    admit_task_to_scheduler(task5);
    
    // Critical high energy task
    Task *task6 = create_task("Emergency Backup", PRIORITY_HIGH, ENERGY_HIGH, 
                              1000, true, 15000);
    admit_task_to_scheduler(task6);
    
    // Low priority low energy task
    Task *task7 = create_task("Cache Cleanup", PRIORITY_LOW, ENERGY_LOW, 
                              400, false, 30000);
    admit_task_to_scheduler(task7);
    
    // Medium priority task
    Task *task8 = create_task("Data Analysis", PRIORITY_MEDIUM, ENERGY_MEDIUM, 
                              700, false, 18000);
    admit_task_to_scheduler(task8);
    
    printf("Created 8 sample tasks with varying priorities and energy costs\n");
    log_info("Sample tasks created successfully");
}

// Run automatic simulation
void run_simulation(void) {
    log_info("Starting automatic simulation mode");
    
    printf("\n=== AUTOMATIC SIMULATION MODE ===\n\n");
    
    // Create sample tasks
    create_sample_tasks();
    
    // Show initial status
    printf("\n--- Initial Status ---\n");
    print_battery_status();
    print_scheduler_status();
    print_ready_queue();
    
    // Start scheduler
    printf("\n--- Starting Scheduler ---\n");
    scheduler_start();
    
    // Run scheduler loop
    scheduler_run_loop();
    
    // Show final status
    printf("\n--- Final Status ---\n");
    print_battery_status();
    print_scheduler_status();
    print_task_statistics();
    print_scheduler_statistics();
    
    log_info("Simulation completed");
}

// Interactive mode
void interactive_mode(void) {
    int choice;
    bool running = true;
    
    log_info("Starting interactive mode");
    
    while (running) {
        print_menu();
        
        if (scanf("%d", &choice) != 1) {
            // Clear invalid input
            while (getchar() != '\n');
            printf("Invalid input. Please enter a number.\n");
            continue;
        }
        
        switch (choice) {
            case 1: {
                // Create sample tasks
                create_sample_tasks();
                printf("Sample tasks created!\n");
                break;
            }
            
            case 2: {
                // Add custom task
                char name[MAX_TASK_NAME];
                int priority, energy, burst_time, deadline;
                bool is_critical;
                int critical_input;
                
                printf("Enter task name: ");
                scanf(" %[^\n]", name);
                printf("Enter priority (1=High, 2=Medium, 3=Low): ");
                scanf("%d", &priority);
                printf("Enter energy cost (1=Low, 2=Medium, 3=High): ");
                scanf("%d", &energy);
                printf("Enter burst time (ms): ");
                scanf("%d", &burst_time);
                printf("Is critical? (1=Yes, 0=No): ");
                scanf("%d", &critical_input);
                is_critical = (critical_input == 1);
                printf("Enter deadline (ms): ");
                scanf("%d", &deadline);
                
                Task *task = create_task(name, priority, energy, burst_time, 
                                        is_critical, deadline);
                if (admit_task_to_scheduler(task) == SUCCESS) {
                    printf("Task added successfully!\n");
                } else {
                    printf("Failed to add task (battery constraints)\n");
                }
                break;
            }
            
            case 3: {
                // Start scheduler
                if (scheduler_start() == SUCCESS) {
                    printf("Scheduler started! Running tasks...\n");
                    scheduler_run_loop();
                    printf("Scheduler finished executing tasks.\n");
                } else {
                    printf("Failed to start scheduler\n");
                }
                break;
            }
            
            case 4: {
                // Stop scheduler
                if (scheduler_stop() == SUCCESS) {
                    printf("Scheduler stopped\n");
                } else {
                    printf("Failed to stop scheduler\n");
                }
                break;
            }
            
            case 5: {
                // Show battery status
                update_battery_status();
                print_battery_status();
                break;
            }
            
            case 6: {
                // Show scheduler status
                print_scheduler_status();
                break;
            }
            
            case 7: {
                // Show task statistics
                print_task_statistics();
                break;
            }
            
            case 8: {
                // Show scheduler statistics
                update_scheduler_statistics();
                print_scheduler_statistics();
                break;
            }
            
            case 9: {
                // Change scheduler algorithm
                int algo;
                printf("\nSelect Algorithm:\n");
                printf("0. FCFS\n");
                printf("1. SJF\n");
                printf("2. Priority\n");
                printf("3. Round Robin\n");
                printf("4. Battery Aware\n");
                printf("Enter choice: ");
                scanf("%d", &algo);
                
                if (algo >= 0 && algo <= 4) {
                    set_scheduler_algorithm((SchedulerAlgorithm)algo);
                    printf("Algorithm changed successfully\n");
                } else {
                    printf("Invalid algorithm choice\n");
                }
                break;
            }
            
            case 10: {
                // Simulate battery drain
                int drain_amount;
                printf("Enter drain amount (%%): ");
                scanf("%d", &drain_amount);
                
                for (int i = 0; i < drain_amount; i++) {
                    simulate_battery_drain(ENERGY_MEDIUM);
                }
                printf("Battery drained by %d%%\n", drain_amount);
                print_battery_status();
                break;
            }
            
            case 11: {
                // Run full simulation
                run_simulation();
                break;
            }
            
            case 0: {
                // Exit
                printf("Exiting Battery-Aware Scheduler System...\n");
                running = false;
                break;
            }
            
            default: {
                printf("Invalid choice. Please try again.\n");
                break;
            }
        }
    }
    
    log_info("Interactive mode ended");
}
