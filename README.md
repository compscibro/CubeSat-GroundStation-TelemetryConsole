# 🛰️ CubeSat Ground-Station Telemetry Console

## Overview  
**CubeSat Ground-Station Telemetry Console** is a command-line simulation written in C that emulates live satellite telemetry. The system simulates temperature drift, battery drain, and orientation error over time. It supports commands to process live packets, run multiple ticks, fire thrusters, toggle **SAFE-MODE**, and safely terminate the mission.

The project demonstrates the use of structured programming, pointer parameter passing, random number generation (`rand()`/`srand()`), modular function design, and formatted output in tabular form.

---

## 🎯 Objectives
This project was developed as part of a systems programming assignment to:
- Practice **structured procedural design** in C.
- Use **pointer parameters** to simulate state updates.
- Handle **user input safely** with `fgets()` and `sscanf()` (no `scanf()`).
- Generate and interpret **telemetry packets** through random drift and system constraints.
- Implement command processing and mode toggling using `switch` logic.
- Maintain strict output formatting to match test scripts run on the **Zeus** server.

---

## ⚙️ Features

| Command | Description |
|----------|--------------|
| `L` | Process exactly **1 live packet** and display telemetry. |
| `R` | Run **N simulated ticks** silently, then display the final telemetry packet. |
| `T` | **Fire thruster** to reduce orientation error by up to 5°, costs 3% battery. Ignored in SAFE-MODE or when battery < 5%. |
| `S` | **Toggle SAFE-MODE** on/off. Restricts commands and clamps drift. |
| `Q` | Quit mission and print final telemetry state. |

---

## 🧩 Core Functions
```bash
void initialState(unsigned int *timestamp, int *temp, unsigned int *battery, int *orient_err)
```
Initializes the CubeSat’s telemetry parameters to default safe values.

```bash
void generatePacket(unsigned int *timestamp, int *temp, unsigned int *battery, int *orient_err, int safe_mode)
```
Simulates one telemetry tick by updating temperature, orientation error, and battery using pseudo-random changes. Handles **SAFE-MODE** behavior and radiation events.

```bash
void printPacket(unsigned int timestamp, int temp, unsigned int battery, int orient_err, int safe_mode)
```
Prints a formatted telemetry table matching the grader’s expected output. Adjusts singular/plural units for orientation error (“degree” vs “degrees”).

```bash
void handleCommand(char command, unsigned int *timestamp, int *temp, unsigned int *battery, int *orient_err, int *safe_mode, int *abort_flag)
```
Interprets and executes user commands. Validates inputs, enforces SAFE-MODE rules, and updates the system state.

```bash
int checkAbort(int temp, unsigned int battery, int orient_err)
```
Checks for mission failure due to:
- Temperature out of range ( < −20 °C or > 90 °C )  
- Battery depletion ( 0% )  
- Orientation error exceeding 45° for more than 3 consecutive ticks  

---

## 🧠 Design Notes

- **Pointer parameters** are used in state-modifying functions (`initialState`, `generatePacket`, `handleCommand`) so changes persist across function calls.  
- **Value parameters** are used in `printPacket` and `checkAbort` since these functions only read the state.  
- **SAFE-MODE** clamps temperature/orientation drift to ±1 per tick and disables radiation upsets.  
- **Random sequences** are consistent on Zeus when the same seed is entered, ensuring deterministic grading.  
- **All user inputs** are read using `fgets()` + `sscanf()` to prevent buffer overflows.

---

## 🖥️ Compilation & Execution

Compile using:
```bash
gcc -Wall -pedantic-errors -std=c99 main.c -o main
```

Run the program:
```bash
./main
```

Sample interaction:
```text
Welcome to CubeSat Ground-Station Telemetry Console!
Enter an integer seed: 1

[L] Live-tick (process exactly 1 packet)
[R] Run N ticks (simulate N packets)
[T] Fire thruster (reduce |orient_err| by 5 degrees, costs 3% battery)
[S] Toggle SAFE-MODE (restricts commands and clamps drift)
[Q] Quit mission

> L
+---------+----------+----------+--------------------+------------+
|   t(s)  |  temp C  | battery% | orient err         |   mode     |
+---------+----------+----------+--------------------+------------+
|       1 |       26 |       99 |       +1 degree    | NORMAL     |
+---------+----------+----------+--------------------+------------+
```

---

## 🧪 Testing Instructions

- **Compile and run on Zeus** (`gcc -std=c99`) to ensure grading consistency
- Enter the **same integer seed** as used in the grading sample (e.g., 1) for deterministic results
- Verify that each telemetry packet, SAFE-MODE toggle, and abort message matches the provided reference output
- Do **not** modify formatting or spacing in the telemetry table

---

## 📁 Structure

```
CUBESAT-GROUNDSTATION-TELEMETRYCONSOLE/
│
├── src/
│   └── main.c                 # All source code goes here (clean separation)
│
├── .gitignore                 # Ignore executables, temp, and TypeScript logs
├── Makefile                   # Builds and runs the project on Zeus or locally
├── typescript_main            # Reference output (for verification)
└── README.md                  # Full documentation (project overview, build steps)

```

---

## 🚀 Key Learning Outcomes
- Practical application of **C pointers and memory safety**.
- Controlled random simulation of physical telemetry systems.
- Modular decomposition and strict **I/O specification adherence**.
- Proper **testing discipline on shared UNIX servers (Zeus)**.
