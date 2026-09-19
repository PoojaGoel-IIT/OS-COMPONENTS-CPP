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

int main() {
    // Classic textbook example: arrival times are already sorted, so the
    // FCFS order matches insertion order, and expected numbers are easy
    // to hand-verify against a Gantt chart.
    std::vector<Process> processes = {
        {1, 0, 5},
        {2, 1, 3},
        {3, 2, 8},
        {4, 3, 6},
    };

    auto results = scheduler::fcfs(processes);
    print_results(results);

    assert(results[0].completion_time == 5  && results[0].waiting_time == 0);
    assert(results[1].completion_time == 8  && results[1].waiting_time == 4);
    assert(results[2].completion_time == 16 && results[2].waiting_time == 6);
    assert(results[3].completion_time == 22 && results[3].waiting_time == 13);

    std::printf("\nAll tests passed.\n");
}
