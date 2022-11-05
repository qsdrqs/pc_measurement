/*
 * ram-bandwidth.c: measure the bandwidth of memory
 *
 * Author: Tianyang Zhou <t7zhou@ucsd.edu>
 *
 */

#include <stdint.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

// 1 Mega
#define N 1024 * 1024

#define START_MEASUREMENT() \
    asm volatile ("cpuid\n\t" \
		  "rdtsc\n\t" \
		  "mov %%edx, %0\n\t" \
		  "mov %%eax, %1\n\t" \
		  : "=r" (cycles_high0), "=r" (cycles_low0) \
		  :: "%rax", "%rbx", "%rcx", "%rdx")

#define END_MEASUREMENT()  \
    asm volatile ("rdtscp\n\t"  \
		  "mov %%edx, %0\n\t"  \
		  "mov %%eax, %1\n\t"  \
		  "cpuid\n\t"  \
		  : "=r" (cycles_high1), "=r" (cycles_low1)  \
		  :: "%rax", "%rbx", "%rcx", "%rdx")

#define GET_MEASUREMENT() \
	(( (uint64_t)cycles_high1 << 32) | cycles_low1) - \
	(( (uint64_t)cycles_high0 << 32) | cycles_low0)

int main(int argc, char *argv[])
{
    uint32_t cycles_low0;
    uint32_t cycles_high0;
    uint32_t cycles_low1;
    uint32_t cycles_high1;

    char* buf = malloc(N * 1024 * sizeof(char)); // malloc 1GB
    char* buf2 = malloc(N * 1024 * sizeof(char)); // malloc 1GB
    uint64_t freq = 3.6 * pow(10, 9);

    START_MEASUREMENT();
#include "ram-bandwidth-w-helper.h"
    END_MEASUREMENT();

    uint64_t cycles = GET_MEASUREMENT();
    printf("write of 1MB: %lu cycles\n", cycles / 1024);
    printf("estimate write bandwidth: %lu MB/s\n",  (freq * 1024) / cycles);

    START_MEASUREMENT();
#include "ram-bandwidth-rw-helper.h"
    END_MEASUREMENT();

    uint64_t cycles_rw = GET_MEASUREMENT();
    printf("read and write of 1MB: %lu cycles\n", cycles_rw / 1024);

    printf("read of 1MB: %lu cycles\n", (cycles_rw - cycles) / 1024);
    printf("estimate read bandwidth: %lu MB/s\n",  (freq * 1024) / (cycles_rw - cycles));

    free(buf);
    free(buf2);

    return 0;
}
