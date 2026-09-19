#pragma once
#include <vector>
#include "process.h"

namespace scheduler {

// Derived, not stored on Process: keeping these separate avoids
// duplicating state that can drift out of sync with the simulation.
struct Metrics {
    int pid;
    int completion_time;
    int turnaround_time;  // completion_time - arrival_time
    int waiting_time;      // turnaround_time - burst_time
};

// Runs processes strictly in arrival order, one at a time, to completion.
// Non-preemptive: once dispatched, a process holds the CPU until it finishes.
// `processes` is taken by value because we sort it and mutate `state`
// as a local simulation log, without affecting the caller's list.
std::vector<Metrics> fcfs(std::vector<Process> processes);

// At every decision point, dispatches whichever arrived process has the
// smallest burst_time. Non-preemptive: once a process starts, it runs
// to completion even if a shorter job arrives while it's running.
// (The preemptive variant, "Shortest Remaining Time First", would
// interrupt it instead — that's a different algorithm, not covered here.)
std::vector<Metrics> sjf(std::vector<Process> processes);

// Preemptive: each dispatch runs a process for at most `quantum` time
// units, then (if it isn't finished) sends it to the back of a FIFO
// ready queue instead of letting it run to completion. This is what
// actually creates the "many programs at once" illusion on one CPU.
//
// Quantum size is a real tradeoff: too large and it degenerates toward
// FCFS; too small and (in a real OS) context-switch overhead would
// dominate -- we don't model switch cost here, but it's why real
// kernels don't use a quantum of 1.
std::vector<Metrics> round_robin(std::vector<Process> processes, int quantum);

} // namespace scheduler
