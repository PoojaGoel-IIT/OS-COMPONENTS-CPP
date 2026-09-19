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
    ProcessState state;

    Process(int pid, int arrival_time, int burst_time)
        : pid(pid),
          arrival_time(arrival_time),
          burst_time(burst_time),
          remaining_time(burst_time),
          state(ProcessState::New) {}
};

} // namespace scheduler
