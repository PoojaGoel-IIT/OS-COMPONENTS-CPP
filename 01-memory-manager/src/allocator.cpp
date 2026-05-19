#include "allocator.h"
#include <cassert>
#include <cstdint>
#include <cstdio>
#include <cstring>

namespace memory_manager {

struct BlockHeader {
    std::size_t  size;  // usable bytes, NOT counting this header
    bool         free;
    BlockHeader* next;
};

static constexpr std::size_t ALIGN       = alignof(std::max_align_t);
static constexpr std::size_t HEADER_SIZE = sizeof(BlockHeader);

static BlockHeader* g_head = nullptr;

static std::size_t align_up(std::size_t n) {
    return (n + ALIGN - 1) & ~(ALIGN - 1);
}

void heap_init(void* memory, std::size_t size) {
    assert(size > HEADER_SIZE);
    g_head        = static_cast<BlockHeader*>(memory);
    g_head->size  = size - HEADER_SIZE;
    g_head->free  = true;
    g_head->next  = nullptr;
}

void* malloc(std::size_t size) {
    if (size == 0) return nullptr;
    size = align_up(size);

    for (BlockHeader* blk = g_head; blk; blk = blk->next) {
        if (!blk->free || blk->size < size) continue;

        // Split only when the leftover can hold a header + at least one aligned word.
        // Splitting a block that's barely large enough just wastes a header.
        if (blk->size >= size + HEADER_SIZE + ALIGN) {
            auto* tail  = reinterpret_cast<BlockHeader*>(
                              reinterpret_cast<char*>(blk) + HEADER_SIZE + size);
            tail->size  = blk->size - size - HEADER_SIZE;
            tail->free  = true;
            tail->next  = blk->next;
            blk->size   = size;
            blk->next   = tail;
        }

        blk->free = false;
        return reinterpret_cast<char*>(blk) + HEADER_SIZE;
    }

    return nullptr; // OOM
}

void free(void* ptr) {
    if (!ptr) return;

    auto* blk = reinterpret_cast<BlockHeader*>(
                    reinterpret_cast<char*>(ptr) - HEADER_SIZE);
    assert(!blk->free && "double-free");
    blk->free = true;

    // Single forward pass: merge any two adjacent free blocks.
    // Running this after every free keeps the list compact without a separate compaction step.
    for (BlockHeader* cur = g_head; cur && cur->next; ) {
        if (cur->free && cur->next->free) {
            cur->size += HEADER_SIZE + cur->next->size;
            cur->next  = cur->next->next;
        } else {
            cur = cur->next;
        }
    }
}

void* realloc(void* ptr, std::size_t new_size) {
    if (!ptr)      return malloc(new_size);
    if (!new_size) { free(ptr); return nullptr; }

    auto* blk = reinterpret_cast<BlockHeader*>(
                    reinterpret_cast<char*>(ptr) - HEADER_SIZE);
    if (blk->size >= align_up(new_size)) return ptr;

    void* dst = malloc(new_size);
    if (!dst) return nullptr;
    std::memcpy(dst, ptr, blk->size);
    free(ptr);
    return dst;
}

void dump() {
    std::size_t i = 0;
    for (BlockHeader* blk = g_head; blk; blk = blk->next, ++i)
        std::printf("  [%2zu]  addr=%p  size=%-6zu  %s\n",
                    i, static_cast<void*>(blk), blk->size,
                    blk->free ? "FREE" : "USED");
}

} // namespace mm
