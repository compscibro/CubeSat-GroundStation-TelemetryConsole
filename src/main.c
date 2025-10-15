#include <stdio.h>
#include <stdlib.h>

// Function Prototypes
void initialState(unsigned int *timestamp, int *temp, unsigned int *battery, int *orient_err);
void generatePacket(unsigned int *timestamp, int *temp, unsigned int *battery, int *orient_err, int safe_mode);
void printPacket(unsigned int timestamp, int temp, unsigned int battery, int orient_err, int safe_mode);
void handleCommand(char command, unsigned int *timestamp, int *temp, unsigned int *battery, int *orient_err, int *safe_mode, int *abort_flag);
int checkAbort(int temp, unsigned int battery, int orient_err);

int main(void) 
{
    /*
    Startup:
    - Greet the operator and prompt for an integer seed. 
    - Call srand(seed) exactly once.
    - Initialize the satellite state.
    */
    char seed_buffer[20]; // stores user input for seed
    unsigned int seed; // starting point for generating random numbers
    int tempSeed; // use to validate user input

    printf("\nWelcome to CubeSat Ground-Station Telemetry Console!\n");

    while (1) 
    {
        // Keep prompting until valid
        printf("Enter an integer seed: ");
        if (!fgets(seed_buffer, sizeof(seed_buffer), stdin)) {
            printf("Warning: Invalid Input!\n");
            continue;
        }

        // If the user input is positive then explicitly typecast
        // tempSeed to unsgined int and theen assign it to seed.
        if (sscanf(seed_buffer, "%d", &tempSeed) == 1 && tempSeed > 0) {
            seed = (unsigned int) tempSeed;
            break;
        }
        printf("Warning: Invalid Input!\n"); // Keep giving warnings until valid
    }
    srand(seed);

    unsigned int timestamp;
    int temp;
    unsigned int battery;
    int orient_err;
    int safe_mode = 0;
    int abort_flag = 0;

    // Providing memory addresses for the pointer variables to get initialized
    initialState(&timestamp, &temp, &battery, &orient_err);

    while (!abort_flag) 
    {
        char command_menu_buffer[20];
        char command;

        printf("\n[L] Live-tick (process exactly 1 packet)\n");
        printf("[R] Run N ticks (simulate N packets)\n");
        printf("[T] Fire thruster (reduce |orient_err| by 5 degrees, costs 3%% battery)\n");
        printf("[S] Toggle SAFE-MODE (restricts commands and clamps drift)\n");
        printf("[Q] Quit mission\n\n");
        printf("> ");


        // Take and validate user input.
        if (!fgets(command_menu_buffer, sizeof(command_menu_buffer), stdin)) {
            printf("Warning: Invalid Input!\n");
            continue;
        }
        
        if (sscanf(command_menu_buffer, " %c", &command) != 1) {
            printf("Warning: Invalid Input!\n");
            continue;
        }
        // Keep prompting for command
        handleCommand(command, &timestamp, &temp, &battery, &orient_err, &safe_mode, &abort_flag);
    }
    return 0;
}

/*
This function sets up the initial satellite state using pointer parameters.
*/
void initialState(unsigned int *timestamp, int *temp, unsigned int *battery, int *orient_err) 
{
    *timestamp = 0; // seconds, increments every tick
    *temp = 28; // celcius, safe range (0-85)
    *battery = 100; // percentage, warning below 20%
    *orient_err = 0; // degrees, deviation from target attitude
}

/*
Each simulation tick executes the following steps:
- timestamp++ (increment the timestamp of how long our CubeSat has been active for)
- Apply status changes using the ranges below (all calls use the rand() sequence seeded at the start)
*/
void generatePacket(unsigned int *timestamp, int *temp, unsigned int *battery, int *orient_err, int safe_mode) 
{
    // Increment the mission time
    (*timestamp)++;

    // Random changes each tick
    int temp_change = (rand() % 10) - 5;     // Temperature drift: -5 to +4
    int orient_change = (rand() % 7) - 3;    // Orientation drift: -3 to +3
    int radiation = rand() % 6;              // Radiation event trigger (0–5)

    // Adjust for SAFEMODE
    if (safe_mode) {
        // Clamp small changes and ignore radiation upsets
        if (temp_change > 1) temp_change = 1;
        if (temp_change < -1) temp_change = -1;
        if (orient_change > 1) orient_change = 1;
        if (orient_change < -1) orient_change = -1;
    } 
    else if (radiation == 5) {
        // Add +8° orientation error on radiation event
        orient_change += 8;
    }

    // Apply changes
    *temp += temp_change;
    *orient_err += orient_change;

    // Battery drain 
    unsigned drain = rand() % 3;
    if (*battery > drain)
        *battery -= drain;
    else
        *battery = 0;
}

/*
This function displays the current state.
*/
void printPacket(unsigned int t, int temp, unsigned int battery, int orient_err, int safe_mode)
{
    const char *mode = safe_mode ? "SAFE-MODE" : "NORMAL";
    int mag = (orient_err < 0) ? -orient_err : orient_err;
    char sign = (orient_err < 0) ? '-' : '+';
    const char *unit = (mag == 1) ? "degree" : "degrees";

    printf("+---------+----------+----------+--------------------+------------+\n");
    printf("|   t(s)  |  temp C  | battery%% | orient err         |   mode     |\n");
    printf("+---------+----------+----------+--------------------+------------+\n");
    // exactly one space between number and unit, columns stay aligned
    printf("|%8u |%9d |%9u |%8c%2d %-8s | %-10s |\n", t, temp, battery, sign, mag, unit, mode);
    printf("+---------+----------+----------+--------------------+------------+\n");
}

/*
This function displays the command menu, keeps reprompting until a valid command is entered.
*/
void handleCommand(char command, unsigned int *timestamp, int *temp, unsigned int *battery, int *orient_err, int *safe_mode, int *abort_flag) 
{
    char buffer[20];
    int N;

    switch (command) 
    {
        // runs exactly one tick and prints the decoded packet immediately.
        case 'L': case 'l':
            generatePacket(timestamp, temp, battery, orient_err, *safe_mode);
            printPacket(*timestamp, *temp, *battery, *orient_err, *safe_mode);
            *abort_flag = checkAbort(*temp, *battery, *orient_err);
            break;
        // asks the user for N, simulates that many ticks silently, and then prints the final packet
        case 'R': case 'r':
            printf("Enter N (positive integer): ");
            fgets(buffer, sizeof(buffer), stdin);
            if (sscanf(buffer, "%d", &N) != 1 || N <= 0) {
                printf("Warning: Invalid Input!\n");
                break;
            }
            for (int i = 0; i < N; i++) {
                generatePacket(timestamp, temp, battery, orient_err, *safe_mode);
                if (checkAbort(*temp, *battery, *orient_err)) {
                    *abort_flag = 1;
                    return;
                }
            }
            printPacket(*timestamp, *temp, *battery, *orient_err, *safe_mode);
            break;
        // reduces the absolute orientation error by 5◦(never overshooting zero) and costs 3% battery
        // Ignored if battery < 5%. If orient_err is positive, subtract 5◦ unless it is between +1 and +4,
        // in which case subtract just enough to bring it to 0.
        case 'T': case 't':
            if (*safe_mode) {
                printf("Thrusters cannot be operated in SAFE-MODE.\n");
            } 
            else if (*battery < 5) {
                printf("Thrusters not fired: battery below 5%%.\n");
            } 
            else {
                // consume 3% battery
                *battery = (*battery >= 3) ? *battery - 3 : 0;

                // reduce orientation error by up to 5° without overshooting zero
                if (*orient_err > 0)
                    *orient_err -= (*orient_err < 5) ? *orient_err : 5;
                else if (*orient_err < 0)
                    *orient_err += (-*orient_err < 5) ? -*orient_err : 5;

                printf("Thrusters fired.\n");
            }
            break;

        /*
        [S] Toggle SAFE-MODE: SAFE-MODE can be entered and exited by pressing S again. 
        While in SAFE-MODE:
        - Commands to fire thrusters are ignored. If the command to fire thrusters is executed, 
        a message should be printed out stating that thrusters cannot be operated in SAFE-MODE.
        - Printed packets resulting from commands L or R while in SAFE-MODE shouldindicate clearly that SAFE-MODE is active.
        - Temperature and attitude drift are clamped and the radiation upsets which add +8 degrees to orient_err do not apply in SAFEMODE.
        */
        case 'S': case 's':
            *safe_mode = !(*safe_mode);
            printf(*safe_mode ? "SAFE-MODE ENABLED.\n" : "SAFE-MODE DISABLED.\n");
            break;
        // ends the program normally and prints a final health report (same as a packet)
        case 'Q': case 'q':
            printPacket(*timestamp, *temp, *battery, *orient_err, *safe_mode);
            *abort_flag = 1;
            break;

        default:
            printf("Unrecognized command '%c'. Please choose L, R, T, S, or Q.\n", command);
            break;
    }
}

/*
Print Mission failed and exit, with a message stating the reason for mission failure.
*/
int checkAbort(int temp, unsigned int battery, int orient_err) 
{
    static int high_err_count = 0;

    if (temp < -20 || temp > 90) {
        printf("Mission failed: Temperature out of range.\n");
        return 1;
    }
    if (battery == 0) {
        printf("Mission failed: battery depleted (0%%).\n");
        return 1;
    }

    int abs_err = (orient_err < 0) ? -orient_err : orient_err;
    if (abs_err > 45) {
        high_err_count++;
    } else {
        high_err_count = 0;
    }

    if (high_err_count > 3) {
        printf("Mission failed: orientation error exceeded 45 degrees for >3 consecutive ticks (current=%+d degrees).\n", orient_err);
        return 1;
    }
    return 0;
}

