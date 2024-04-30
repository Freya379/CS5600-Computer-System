# PA2: Scheduler Simulation
- Team Name: Shuiming Chen, Yaohui Wang, Yu Ma, Zhixuan Shen
  FCFS: Zhixuan Shen
  SJF PSJF: YU MA
  PRI, PPRI: Martin
  RR: Yaohui Wang
  
- Date: 03/01/2024 

## Design Overview

This programming assignment implements a library for process scheduling in a simulated operating system environment. The library supports different scheduling algorithms: First-Come, First-Served (FCFS), Shortest Job First (SJF), Preemptive Shortest Job First (PSJF), Priority (PRI), Preemptive Priority (PPRI), and Round-Robin (RR). It uses Unix system calls for process management, simulating the behavior of an operating system scheduler. The scheduler decides which process to execute next based on the algorithm selected by the user.


## Complete Specification

- **Scheduling Algorithms**: Implements First-Come, First-Served (FCFS), Shortest Job First (SJF), Preemptive SJF (PSJF), Priority (PRI), Preemptive Priority (PPRI), and Round-Robin (RR) scheduling algorithms.

- **Job Management**: Facilitates adding new jobs, completing jobs, and preemptively switching jobs based on the scheduling policy.

- **Performance Metrics**: Calculates average waiting time, turnaround time, and response time to evaluate the effectiveness of the scheduling algorithm.

- **Efficiency**: Ensures efficient job scheduling by prioritizing tasks according to the chosen algorithm, with special handling for preemptive scheduling and time quantums in RR.

- **Clean-up**: Frees all allocated resources at the end of the simulation to prevent memory leaks.


## Known Bugs or Problems
- Currently, there are no known bugs. 


