#pragma once

namespace scheduler {

enum class ProcessState {
    New,
    Ready,
    Running,
    Terminated
};

// Simplified PCB: just enough to drive scheduling decisions and
// compute waiting/turnaround/response time metrics. A real PCB also
// holds saved registers, a page table pointer, open file handles, etc.
struct Process {
    int pid;
    int arrival_time;
    int burst_time;      // total CPU time this process needs
    int remaining_time;   // decremented as the scheduler runs it
    int priority;          // lower value = higher priority (unused by FCFS/SJF/RR)
    ProcessState state;

    Process(int pid, int arrival_time, int burst_time, int priority = 0)
        : pid(pid),
          arrival_time(arrival_time),
          burst_time(burst_time),
          remaining_time(burst_time),
          priority(priority),
          state(ProcessState::New) {}
};

} // namespace scheduler
