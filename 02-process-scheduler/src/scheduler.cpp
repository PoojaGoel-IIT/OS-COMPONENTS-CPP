#include "scheduler.h"
#include <algorithm>

namespace scheduler {

std::vector<Metrics> fcfs(std::vector<Process> processes) {
    // Stable sort: processes that arrive at the same tick keep the
    // order they were given in, rather than an arbitrary tie-break.
    std::stable_sort(processes.begin(), processes.end(),
        [](const Process& a, const Process& b) {
            return a.arrival_time < b.arrival_time;
        });

    std::vector<Metrics> results;
    results.reserve(processes.size());

    int clock = 0;
    for (auto& p : processes) {
        // CPU sits idle if the next process hasn't arrived yet.
        if (clock < p.arrival_time) clock = p.arrival_time;

        p.state = ProcessState::Running;
        clock += p.burst_time;
        p.remaining_time = 0;
        p.state = ProcessState::Terminated;

        int completion = clock;
        int turnaround = completion - p.arrival_time;
        int waiting    = turnaround - p.burst_time;
        results.push_back({p.pid, completion, turnaround, waiting});
    }

    return results;
}

} // namespace scheduler
