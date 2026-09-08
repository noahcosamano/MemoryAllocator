#include "nalloc.h"

int main() {
    int* number = nalloc(sizeof(int));
    int* number2 = nalloc(sizeof(int) *2);
    char* letter = nalloc(sizeof(char));

    *number = 5;

    printf("Number: %d, Address: %p\n", *number, number);

    int isFree = nfree(number);

    char* letter2 = nalloc(sizeof(char) * 2);

    return 0;
}