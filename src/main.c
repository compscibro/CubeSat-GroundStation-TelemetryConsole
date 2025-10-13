#include <stdio.h>
#include <stdlib.h>

// Function Prototypes
void initialState(unsigned int *timestamp, int *temp, unsigned int *battery, int *orient_err);
void generatePacket(unsigned int *timestamp, int *temp, unsigned int *battery, int *orient_err);

int main() 
{
    /*
    Startup:
    - Greet the operator and prompt for an integer seed. 
    - Call srand(seed) exactly once.
    - Initialize the satellite state.
    */
    char buffer[11]; // stores user input for seed, 10 digits + a null terminator
    unsigned int seed; // starting point for generating random numbers
    int tempSeed; // use to validate user input

    printf("\nWelcome to CubeSat Ground-Station Telemetry Console!\n");

    while (1) 
    {
        // Keep prompting until valid
        printf("Enter an integer seed: ");
        fgets(buffer, sizeof(buffer), stdin);

        // If the user input is positive then explicitly typecast
        // tempSeed to unsgined int and theen assign it to seed.
        if (sscanf(buffer, "%d", &tempSeed) == 1 && tempSeed > 0) {
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

    // Providing memory addresses for the pointer variables to get initialized
    initialState(&timestamp, &temp, &battery, &orient_err);

    printf("\n--- Telemetry Packet Before ---\n");
    printf("Timestamp: %u s\n", timestamp);
    printf("Temperature: %d °C\n", temp);
    printf("Battery: %u %%\n", battery);
    printf("Orientation Error: %d °\n", orient_err);
    printf("-------------------------------\n");

    generatePacket(&timestamp, &temp, &battery, &orient_err);

    printf("\n--- Telemetry Packet After ---\n");
    printf("Timestamp: %u s\n", timestamp);
    printf("Temperature: %d °C\n", temp);
    printf("Battery: %u %%\n", battery);
    printf("Orientation Error: %d °\n", orient_err);
    printf("------------------------------\n");

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

*/
void generatePacket(unsigned int *timestamp, int *temp, unsigned int *battery, int *orient_err) 
{
    int rad_hits;

    (*timestamp)++; // increment the timestamp of how long our CubeSat has been active for

    // Apply status changes using the ranges below:
    *temp += (rand() % 10) - 5;
    *battery -= rand() % 3;
    *orient_err += (rand() % 7) - 3;
    rad_hits = rand() % 6;

    // If rad_hits = 5 the radiation has caused bit-level errors in our CubeSat: add 8◦ to orient_err.
    if (rad_hits == 5) {
        *orient_err += 8;
    }

    // Ensure battery is never below 0%.
    if ((int) (*battery) < 0) {
        *battery = 0;
    }

}