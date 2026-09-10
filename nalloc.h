#include "header.h"
#include <windows.h>
#include <stdio.h>
#include <stdbool.h>

#define RESERVE_SIZE 40960 // 10 pages of memory (40,960 bytes)

Header* reserve = NULL; // Used to track if a reserve exists yet before attempting to carve memory out of reserve

Header* CreateReserve() {
    Header* newReserve = (Header*)VirtualAlloc(
        NULL,                           // Let the system determine addressing. I found that when I try 
                                        // to it will silently crash if memory space isn't free
        RESERVE_SIZE,
        MEM_RESERVE | MEM_COMMIT,
        PAGE_READWRITE
    );

    if (newReserve == NULL) {
        printf("Failed to create reserve.\n");
        return NULL;
    }

    newReserve->size = RESERVE_SIZE;
    newReserve->isFree = true;          // Reserve is initially free, but reserve header will turn into first
                                        // carved out piece of memory's header and will not be free until freed
    newReserve->next = NULL; // Points to next free block of memory. When block isn't free, it will no longer be null

    return newReserve;
}

void* nalloc(size_t bytes) {
    if (bytes == 0) return NULL;
    
    if (reserve == NULL) {
        printf("No free block, creating reserve.\n");
        reserve = CreateReserve();

        if (reserve == NULL) {
            printf("Failed to create reserve.\n");
            return NULL;
        }

        printf("Successfully created reserve of %d bytes at address %p.\n", reserve->size-sizeof(Header), reserve+1);
    }

    // This is the loop to find the next free block big enough for requested allocation
    Header* currentBlock = reserve;
    while (currentBlock != NULL) {
        if (currentBlock->isFree && currentBlock->size >= bytes) {
            break;
        }
        currentBlock = currentBlock->next;
    }

    // This conditional is responsible for creating a new reserve if current reserve does not contain a large enough free block
    if (currentBlock == NULL) {
        printf("No memory left in reserve.\n");
        printf("Old reserve start addr: %p, size: %d, end addr: %p\n", (void*)reserve, RESERVE_SIZE, (void*)reserve + RESERVE_SIZE);
        printf("Creating new reserve\n");
        Header* newReserve = CreateReserve();
        printf("New reserve address %p\n", newReserve);

        Header* currentBlock = reserve;
        size_t oldReserveSize = 0;
        while (currentBlock != NULL) {
            if (currentBlock->isFree == true) {
                oldReserveSize += currentBlock->size;
                printf("Free block of %d bytes\n", currentBlock->size);
                if (currentBlock->next == NULL) {
                    currentBlock->next = newReserve;
                    printf("Current block points to new reserve\n");
                    break;
                }
            } else {
                printf("Block isn't free, next.\n");
            }
            currentBlock = currentBlock->next;
        }

        size_t totalReserveSize = oldReserveSize + newReserve->size;

        printf("Old reserve size: %d, New reserve size: %d, Total reserve size: %d\n", oldReserveSize, newReserve->size, totalReserveSize);

        return NULL;
    }

    if (currentBlock->size >= bytes + sizeof(Header) + 16) {
        size_t remainingSize = currentBlock->size - bytes - sizeof(Header);

        printf("Remaining size of block is %d bytes\n", remainingSize);

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