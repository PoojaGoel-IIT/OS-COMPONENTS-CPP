#include <cassert>
#include <cstdio>
#include <string>
#include <vector>
#include "thread.h"

using threading::Thread;
using threading::ThreadState;

static ucontext_t main_ctx;
static std::vector<std::string> trace; // records what actually ran, in order

static void log(const std::string& msg) {
    std::printf("%s\n", msg.c_str());
    trace.push_back(msg);
}

int main() {
    Thread a([]{
        for (int i = 1; i <= 3; ++i) {
            log("[Thread A] step " + std::to_string(i));
            threading::yield();
        }
        log("[Thread A] done");
    }, &main_ctx);

    Thread b([]{
        for (int i = 1; i <= 3; ++i) {
            log("[Thread B] step " + std::to_string(i));
            threading::yield();
        }
        log("[Thread B] done");
    }, &main_ctx);

    // No scheduler yet -- main manually alternates between the two
    // threads. The point of this milestone is proving that resume()
    // and yield() correctly suspend and resume execution mid-function,
    // not building a real dispatch policy.
    while (a.state != ThreadState::Terminated || b.state != ThreadState::Terminated) {
        if (a.state != ThreadState::Terminated) threading::resume(a, &main_ctx);
        if (b.state != ThreadState::Terminated) threading::resume(b, &main_ctx);
    }

    log("[main] both threads finished");

    // If context switching were broken -- e.g. threads sharing a stack,
    // or resuming from the wrong saved point -- this exact interleaving
    // could not happen: each thread's loop counter `i` only survives
    // across yield() calls because it lives on that thread's own,
    // independent stack.
    std::vector<std::string> expected = {
        "[Thread A] step 1", "[Thread B] step 1",
        "[Thread A] step 2", "[Thread B] step 2",
        "[Thread A] step 3", "[Thread B] step 3",
        "[Thread A] done",   "[Thread B] done",
        "[main] both threads finished",
    };
    assert(trace == expected);

    std::printf("\nAll tests passed.\n");
}
