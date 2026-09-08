#include <windows.h>
#include <stddef.h>
#include <stdbool.h>

typedef struct Header Header;

struct Header {
    size_t size;            // 8 bytes: size of payload exculding this header.
    bool isFree;            // 1 byte: true(1) false(0)
                            // 7 bytes: padding for 8 byte alignment
    Header* next;           // 8 bytes: 64 bit pointer to next free memory block.
};                          // 24 bytes. Must always be multiple of 8.