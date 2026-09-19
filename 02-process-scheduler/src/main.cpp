#include <cassert>
#include <cstdio>
#include <vector>
#include "process.h"
#include "scheduler.h"

using scheduler::Process;
using scheduler::Metrics;

static void print_results(const std::vector<Metrics>& results) {
    std::printf("PID  Completion  Turnaround  Waiting\n");
    int total_turnaround = 0, total_waiting = 0;
    for (const auto& m : results) {
        std::printf(" %-3d  %-10d  %-10d  %-7d\n",
                     m.pid, m.completion_time, m.turnaround_time, m.waiting_time);
        total_turnaround += m.turnaround_time;
        total_waiting    += m.waiting_time;
    }
    std::printf("Avg turnaround: %.2f\n", double(total_turnaround) / results.size());
    std::printf("Avg waiting:    %.2f\n", double(total_waiting) / results.size());
}

static void section(const char* label) {
    std::printf("\n=== %s ===\n", label);
}

int main() {
    // Classic textbook example: arrival times are already sorted, so the
    // FCFS order matches insertion order, and expected numbers are easy
    // to hand-verify against a Gantt chart.
    std::vector<Process> fcfs_processes = {
        {1, 0, 5},
        {2, 1, 3},
        {3, 2, 8},
        {4, 3, 6},
    };

    section("FCFS");
    auto fcfs_results = scheduler::fcfs(fcfs_processes);
    print_results(fcfs_results);

    assert(fcfs_results[0].completion_time == 5  && fcfs_results[0].waiting_time == 0);
    assert(fcfs_results[1].completion_time == 8  && fcfs_results[1].waiting_time == 4);
    assert(fcfs_results[2].completion_time == 16 && fcfs_results[2].waiting_time == 6);
    assert(fcfs_results[3].completion_time == 22 && fcfs_results[3].waiting_time == 13);

    // Another classic textbook example, chosen because arrivals are spread
    // out enough that SJF picks a different execution order than FCFS
    // would (P1, P2, P4, P3) -- a good check that eligibility-by-arrival
    // is actually being respected, not just sorting by burst_time.
    std::vector<Process> sjf_processes = {
        {1, 0, 8},
        {2, 1, 4},
        {3, 2, 9},
        {4, 3, 5},
    };

    section("SJF (non-preemptive)");
    auto sjf_results = scheduler::sjf(sjf_processes);
    print_results(sjf_results);

    assert(sjf_results[0].pid == 1 && sjf_results[0].completion_time == 8  && sjf_results[0].waiting_time == 0);
    assert(sjf_results[1].pid == 2 && sjf_results[1].completion_time == 12 && sjf_results[1].waiting_time == 7);
    assert(sjf_results[2].pid == 4 && sjf_results[2].completion_time == 17 && sjf_results[2].waiting_time == 9);
    assert(sjf_results[3].pid == 3 && sjf_results[3].completion_time == 26 && sjf_results[3].waiting_time == 15);

    // Standard Round Robin textbook example, quantum = 2. Execution order
    // (P1 slice, P2 slice, P3 slice, P1 slice, P4 slice, P2 slice, P1 slice)
    // finishes processes in the order P3, P4, P2, P1 -- none of which
    // matches FCFS or SJF order, which is exactly the point of preemption.
    std::vector<Process> rr_processes = {
        {1, 0, 5},
        {2, 1, 4},
        {3, 2, 2},
        {4, 3, 1},
    };

    section("Round Robin (quantum = 2)");
    auto rr_results = scheduler::round_robin(rr_processes, 2);
    print_results(rr_results);

    assert(rr_results[0].pid == 3 && rr_results[0].completion_time == 6  && rr_results[0].waiting_time == 2);
    assert(rr_results[1].pid == 4 && rr_results[1].completion_time == 9  && rr_results[1].waiting_time == 5);
    assert(rr_results[2].pid == 2 && rr_results[2].completion_time == 11 && rr_results[2].waiting_time == 6);
    assert(rr_results[3].pid == 1 && rr_results[3].completion_time == 12 && rr_results[3].waiting_time == 7);

    std::printf("\nAll tests passed.\n");
}
