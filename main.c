#include "header.h"
#include <windows.h>
#include <stdio.h>
#include <stdbool.h>

#define ARENA_SIZE 40960 // 10 pages of memory (40,960 bytes)

Header* arena = NULL;

Header* CreateArena() {
    Header* newArena = (Header*)VirtualAlloc(
        NULL,
        ARENA_SIZE,
        MEM_RESERVE | MEM_COMMIT,
        PAGE_READWRITE
    );

    if (newArena == NULL) return NULL;

    newArena->size = ARENA_SIZE - sizeof(Header);
    newArena->isFree = true;
    newArena->next = NULL;

    return newArena;
}

void* nalloc(size_t bytes) {
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
    while (currentBlock != NULL) {
        if (currentBlock->isFree && currentBlock->size >= bytes) {
            break;
        }
        currentBlock = currentBlock->next;
    }

    if (currentBlock == NULL) {
        printf("Out of memory in arena.\n");
        return NULL;
    }

    if (currentBlock->size >= bytes + sizeof(Header) + 16) {
        size_t remainingSize = currentBlock->size - bytes - sizeof(Header);

        Header* nextFreeBlock = (Header*)((char*)(currentBlock + 1) + bytes);

        nextFreeBlock->size = remainingSize;
        nextFreeBlock->isFree = true;
        nextFreeBlock->next = currentBlock->next;

        currentBlock->size = bytes;
        currentBlock->isFree = false;
        currentBlock->next = nextFreeBlock;
    } else {
        currentBlock->isFree = false;
    }

    void* payloadStartAddr = (void*)(currentBlock + 1);

    printf("Successfully allocated requested memory (%zu bytes) at payload address %p\n", 
           bytes, payloadStartAddr);

    if (currentBlock->next) {
        printf("Next free block is at address %p and contains %zu bytes\n", 
               (void*)currentBlock->next, currentBlock->next->size);
    }

    return payloadStartAddr;
}

/*
1.) Create another arena, and concatenate them if current arena runs out of memory.
2.) Free memory using nfree so that block of memory can be reused.
*/

int nfree(void* payloadStartAddr) {
    Header* payloadHeader = ((Header*)payloadStartAddr) - 1;
    printf("Payload header sits at address %p\n", (void*)payloadHeader);
    payloadHeader->isFree = true;
    return 1;
}

int main() {
    int* number = nalloc(sizeof(int));
    int* number2 = nalloc(sizeof(int) *2);
    char* letter = nalloc(sizeof(char));

    *number = 5;

    printf("Number: %d, Address: %p\n", *number, number);

    int isFree = nfree(number);

    return 0;
}