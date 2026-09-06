#include "header.h"
#include <windows.h>
#include <stdio.h>

#define ARENA_SIZE 10; // 10 pages of memory (10 * 4096B = 40,960B = 40.96Kb)

void* AllocateMem(size_t bytes) {
    if (bytes == 0) return NULL;

    size_t totalSize = bytes + sizeof(Header);
    
    Header* block = (Header*)VirtualAlloc(
        NULL,
        totalSize,
        MEM_RESERVE | MEM_COMMIT,
        PAGE_READWRITE
    );
 
    if (block == NULL) return NULL;

    printf("Allocated %d bytes\n", totalSize);

    block->size = bytes;
    block->isFree = false;
    block->next = NULL;

    return (void*)(block + 1); // Returns pointer to memory address after header.
}

void* CreateArena() {

}

int main() {
    void* address = AllocateMem(100);
    printf("Memory Address: %p\n", address);
    return 1;
}