#include <cassert>
#include <cstdio>
#include "allocator.h"

static char heap[4096];
static void section(const char* label) {
    std::printf("\n=== %s ===\n", label);
    mm::dump();
}

int main() {
    mm::heap_init(heap, sizeof(heap));
    section("Initial heap");

    // --- basic allocations ---
    auto* a = static_cast<int*>(mm::malloc(sizeof(int) * 4));
    assert(a);
    for (int i = 0; i < 4; i++) a[i] = i * 10;

    auto* s = static_cast<char*>(mm::malloc(64));
    assert(s);
    s[0] = 'H'; s[1] = 'i'; s[2] = '\0';

    auto* d = static_cast<double*>(mm::malloc(sizeof(double)));
    assert(d);
    *d = 3.14;

    section("After 3 allocations");

    // --- free the middle block (s), creating a hole ---
    mm::free(s);
    section("After freeing middle block (hole in list)");

    // --- free the first block; should coalesce with s's hole ---
    mm::free(a);
    section("After freeing first block (coalesced with hole)");

    // --- free last block; should give back one big free region ---
    mm::free(d);
    section("After freeing all blocks");

    // --- realloc: grow an array ---
    auto* arr = static_cast<int*>(mm::malloc(sizeof(int) * 2));
    assert(arr);
    arr[0] = 1; arr[1] = 2;
    arr = static_cast<int*>(mm::realloc(arr, sizeof(int) * 8));
    assert(arr && arr[0] == 1 && arr[1] == 2);
    section("After realloc (2 -> 8 ints, data preserved)");
    mm::free(arr);

    // --- null / zero-size edge cases ---
    assert(mm::malloc(0) == nullptr);
    mm::free(nullptr);
    auto* p = static_cast<int*>(mm::malloc(sizeof(int)));
    assert(mm::realloc(p, 0) == nullptr);

    section("Final heap (should be fully coalesced)");
    std::printf("\nAll tests passed.\n");
}
