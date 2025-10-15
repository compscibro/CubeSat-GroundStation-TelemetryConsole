# 🛰️ CubeSat Ground-Station Telemetry Console

## Overview  
**CubeSat Ground-Station Telemetry Console** is a command-line simulation written in C that emulates live satellite telemetry. The system simulates temperature drift, battery drain, and orientation error over time. It supports commands to process live packets, run multiple ticks, fire thrusters, toggle **SAFE-MODE**, and safely terminate the mission.

The project demonstrates the use of structured programming, pointer parameter passing, random number generation (`rand()`/`srand()`), modular function design, and formatted output in tabular form.



## ⚙️ Features

| Command | Description |
|----------|--------------|
| `L` | Process exactly **1 live packet** and display telemetry. |
| `R` | Run **N simulated ticks** silently, then display the final telemetry packet. |
| `T` | **Fire thruster** to reduce orientation error by up to 5°, costs 3% battery. Ignored in SAFE-MODE or when battery < 5%. |
| `S` | **Toggle SAFE-MODE** on/off. Restricts commands and clamps drift. |
| `Q` | Quit mission and print final telemetry state. |



## 🎥 Demo
[![Demo Video](https://img.youtube.com/vi/jMWRyZtGA0c/0.jpg)](https://youtu.be/jMWRyZtGA0c)



## 📁 Project Structure

```
CUBESAT-GROUNDSTATION-TELEMETRYCONSOLE/
│
├── src/
│   └── main.c                 # All source code goes here (clean separation)
├── .gitignore                 # Ignore executables, temp, and TypeScript logs
├── Makefile                   # Builds and runs the project on Zeus or locally
└── README.md                  # Full documentation (project overview, build steps)

```



## 🧱 Project Architecture

The CubeSat Ground-Station Telemetry Console follows a **modular procedural architecture** emphasizing clear function responsibility and deterministic simulation.

### **Core Components**
- **Main Control Loop (`main.c`)** – Handles command input, user prompts, and mission flow.
- **Initialization (`initialState`)** – Establishes safe baseline state before simulation begins.
- **Telemetry Module (`generatePacket`)** – Simulates CubeSat behavior (temperature, battery, and orientation error).  
- **Display Module (`printPacket`)** – Outputs formatted telemetry tables.  
- **Command Processor (`handleCommand`)** – Parses commands (`L`, `R`, `T`, `S`, `Q`) and updates state.  
- **Safety & Abort Logic (`checkAbort`)** – Monitors for mission-critical parameter violations.  

### **Data Flow**
1. **User Input → Command Handler**  
2. **Command Handler → Telemetry Generator**  
3. **Telemetry Generator → Output Display + Abort Checker**  
4. **Loop repeats** until mission abort or manual quit.

This architecture maintains **separation of concerns**, improving readability, maintainability, and portability between UNIX systems such as **Zeus**.



## 🖥️ Compilation & Execution

Compile using:
```bash
make all
```
OR
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
|   t(s)  |  temp C  | battery% |     orient err     |    mode    |
+---------+----------+----------+--------------------+------------+
|       1 |       26 |       99 |       +1 degree    | NORMAL     |
+---------+----------+----------+--------------------+------------+
```
