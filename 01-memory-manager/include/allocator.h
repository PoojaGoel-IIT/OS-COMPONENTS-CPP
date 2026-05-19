#pragma once
#include <cstddef>

namespace mm {

void  heap_init(void* memory, std::size_t size);
void* malloc(std::size_t size);
void  free(void* ptr);
void* realloc(void* ptr, std::size_t new_size);
void  dump();

} // namespace mm
