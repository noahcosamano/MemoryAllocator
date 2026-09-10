#include "nalloc.h"

int main() {
    printf("\nCreating 'number' of 4 bytes\n");
    int* number = nalloc(sizeof(int));

    printf("\nCreating 'number2' of 8 bytes\n");
    int* number2 = nalloc(sizeof(int) *2);

    printf("\nCreating 'letter' of 1 bytes\n");
    char* letter = nalloc(sizeof(char));

    printf("\nCreating 'letter2' of 50000 bytes\n");
    char* letter2 = nalloc(50000);

    printf("\nGetting 'number' memory address\n");
    *number = 5;

    printf("Number: %d, Address: %p\n", *number, number);

    printf("\nFreeing 'number' of 4 bytes\n");
    nfree(number);

    printf("\nCreating 'letter3' of 2 bytes\n");
    char* letter3 = nalloc(sizeof(char) * 2);

    printf("\nFreeing 'number2' of 8 bytes\n");
    nfree(number2);

    printf("\nFreeing 'letter' of 41 bytes\n");
    nfree(letter);

    printf("\nFreeing 'letter2' of 50000 bytes\n");
    nfree(letter2);

    printf("\nFreeing 'letter3' of 2 bytes\n");
    nfree(letter3);

    return 0;
}