#include "header.h"
#include <windows.h>
#include <stdio.h>

#define ARENA_SIZE 40960 // 10 pages of memory (10 * 4096B = 40,960B = 40.96Kb)

Header* arena = NULL;

Header* CreateArena() {
    Header* arena = (Header*)VirtualAlloc(
        NULL,
        ARENA_SIZE,
        MEM_RESERVE | MEM_COMMIT,
        PAGE_READWRITE
    );

    if (arena == NULL) return NULL;

    arena->size = ARENA_SIZE - sizeof(Header);
    arena->isFree = true;
    arena->next = NULL;

    return arena;
}

void* AllocateMem(size_t bytes) {
    if (bytes == 0) return NULL;
    
    if (arena == NULL) {
        printf("No free block, allocating arena.\n");
        arena = CreateArena();

        if (arena == NULL) {
            printf("Failed to allocate arena.\n");
            return NULL;
        }

        printf("Successfully allocated arena of %d bytes at address %p\n", ARENA_SIZE, (void*)arena);
    }

    Header* currentBlock = arena;

    if (currentBlock->isFree && currentBlock->size >= bytes) {
        currentBlock->isFree = false;

        printf("Successfully allocated requested memory (%zu bytes) at payload address %p\n", 
               bytes, (void*)(currentBlock + 1));

        return (void*)(currentBlock + 1);
    }
}

int main() {
    AllocateMem(100);
    return 1;
}