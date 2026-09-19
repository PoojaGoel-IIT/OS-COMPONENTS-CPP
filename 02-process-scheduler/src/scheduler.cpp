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

std::vector<Metrics> sjf(std::vector<Process> processes) {
    const std::size_t n = processes.size();
    std::vector<bool> done(n, false);
    std::vector<Metrics> results;
    results.reserve(n);

    int clock = 0;
    std::size_t completed = 0;
    while (completed < n) {
        // Pick the shortest job among processes that have arrived by `clock`.
        // Tie-break by earlier arrival, then by original position, so the
        // result is deterministic instead of depending on vector order.
        int best = -1;
        for (std::size_t i = 0; i < n; ++i) {
            if (done[i] || processes[i].arrival_time > clock) continue;
            if (best == -1 ||
                processes[i].burst_time < processes[best].burst_time ||
                (processes[i].burst_time == processes[best].burst_time &&
                 processes[i].arrival_time < processes[best].arrival_time)) {
                best = static_cast<int>(i);
            }
        }

        if (best == -1) {
            // Nothing has arrived yet: CPU is idle, fast-forward to the
            // next arrival instead of scanning tick by tick.
            int next_arrival = -1;
            for (std::size_t i = 0; i < n; ++i) {
                if (!done[i] && (next_arrival == -1 || processes[i].arrival_time < next_arrival))
                    next_arrival = processes[i].arrival_time;
            }
            clock = next_arrival;
            continue;
        }

        Process& p = processes[static_cast<std::size_t>(best)];
        p.state = ProcessState::Running;
        clock += p.burst_time;
        p.remaining_time = 0;
        p.state = ProcessState::Terminated;
        done[static_cast<std::size_t>(best)] = true;
        ++completed;

        int completion = clock;
        int turnaround = completion - p.arrival_time;
        int waiting    = turnaround - p.burst_time;
        results.push_back({p.pid, completion, turnaround, waiting});
    }

    return results;
}

} // namespace scheduler
