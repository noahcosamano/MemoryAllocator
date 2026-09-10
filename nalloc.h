#include "header.h"
#include <windows.h>
#include <stdio.h>
#include <stdbool.h>

#define RESERVE_SIZE 40960 // 10 pages of memory (40,960 bytes)

Header* reserve = NULL;

Header* CreateReserve(void* memAddr) {
    Header* newReserve = (Header*)VirtualAlloc(
        memAddr,
        RESERVE_SIZE,
        MEM_RESERVE | MEM_COMMIT,
        PAGE_READWRITE
    );

    if (newReserve == NULL) return NULL;

    newReserve->size = RESERVE_SIZE - sizeof(Header);
    newReserve->isFree = true;
    newReserve->next = NULL;

    return newReserve;
}

void* nalloc(size_t bytes) {
    if (bytes == 0) return NULL;
    
    if (reserve == NULL) {
        printf("No free block, creating reserve.\n");
        reserve = CreateReserve(NULL);

        if (reserve == NULL) {
            printf("Failed to create reserve.\n");
            return NULL;
        }

        printf("Successfully created reserve of %d bytes at address %p\n", RESERVE_SIZE, (void*)reserve);
    }

    Header* currentBlock = reserve;
    while (currentBlock != NULL) {
        if (currentBlock->isFree && currentBlock->size >= bytes) {
            break;
        }
        currentBlock = currentBlock->next;
    }

    if (currentBlock == NULL) {
        printf("No memory left in reserve.\n");
        Header* newReserve = CreateReserve((void*)reserve + RESERVE_SIZE);
        printf("Old reserve start addr: %p, size: %d, end addr: %p\n", (void*)reserve, RESERVE_SIZE, (void*)reserve + RESERVE_SIZE);
        printf("Creating new reserve at address %p\n", (void*)reserve + RESERVE_SIZE);
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
1.) Create another reserve, and concatenate them if current reserve runs out of memory.
2.) Free memory using nfree so that block of memory can be reused.
3.) Verify byte-by-byte that memory is being freed/overwrite when using nfree.
4.) When nfree is called, concatenate any free neighbor blocks together.
5.) Create nrealloc function to reallocate more or less memory to pre-existing memory chunk.
*/

int nfree(void* payloadStartAddr) {
    Header* payloadHeader = ((Header*)payloadStartAddr) - 1;
    printf("Payload header sits at address %p\n", (void*)payloadHeader);
    payloadHeader->isFree = true;
    return 1;
}