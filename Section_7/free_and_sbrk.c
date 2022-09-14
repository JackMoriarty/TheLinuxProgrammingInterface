/**
 * @file free_and_sbrk.c
 * @author Bangduo Chen (chenbangduo@163.com)
 * @brief free() 函数对 program break 的影响
 * @version 0.1
 * @date 2018-12-30
 * 
 * @copyright Copyright (c) 2018
 * 
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define MAX_ALLOCS  1000000

int main(int argc, char *argv[])
{
    char *ptr[MAX_ALLOCS];
    int freeStep, freeMin, freeMax, blockSize, numAllocs, j;

    if (argc < 3 || strcmp(argv[1], "--help") == 0) {
        fprintf(stderr, "%s num-allocs block-size [step [min [max]]]\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    numAllocs = atoi(argv[1]);
    if (numAllocs > MAX_ALLOCS) {
        fprintf(stderr, "num-allocs > %d\n", MAX_ALLOCS);
        exit(EXIT_FAILURE);
    }

    blockSize = atoi(argv[2]);
    freeStep = (argc > 3) ? atoi(argv[3]) : 1;
    freeMin = (argc > 4) ? atoi(argv[4]) : 1;
    freeMax = (argc > 5) ? atoi(argv[5]) : numAllocs;

    if (freeMax > numAllocs) {
        fprintf(stderr, "free-max > num-allocs\n");
        exit(EXIT_FAILURE);
    }

    printf("Initial program break:\t%10p\n", sbrk(0));

    printf("Allocating %d*%d bytes\n", numAllocs, blockSize);
    for (j = 0; j < numAllocs; j++) {
        ptr[j] = malloc(blockSize);
        if (ptr[j] == NULL) {
            fprintf(stderr, "malloc\n");
            exit(EXIT_FAILURE);
        }
    }

    printf("Program break is now:\t%10p\n", sbrk(0));
    
    printf("Freeing blocks from %d to %d in steps of %d\n", freeMin, freeMax, freeStep);
    for (j = freeMin - 1; j < freeMax; j += freeStep) {
        free(ptr[j]);
    }

    printf("After free(), program break is:\t%10p\n", sbrk(0));

    exit(EXIT_SUCCESS);
}