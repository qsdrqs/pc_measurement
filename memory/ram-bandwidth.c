/*
 * ram-bandwidth.c: measure the bandwidth of memory
 *
 * Author: Tianyang Zhou <t7zhou@ucsd.edu>
 *
 */

#include <math.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

uint32_t cycles_low0;
uint32_t cycles_high0;
uint32_t cycles_low1;
uint32_t cycles_high1;

// 1 Mega
#define N 1024 * 1024

#define START_MEASUREMENT()                                              \
    asm volatile(                                                        \
        "cpuid\n\t"                                                      \
        "rdtsc\n\t"                                                      \
        "mov %%edx, %0\n\t"                                              \
        "mov %%eax, %1\n\t"                                              \
        : "=r"(cycles_high0), "=r"(cycles_low0)::"%rax", "%rbx", "%rcx", \
          "%rdx")

#define END_MEASUREMENT()                                                \
    asm volatile(                                                        \
        "rdtscp\n\t"                                                     \
        "mov %%edx, %0\n\t"                                              \
        "mov %%eax, %1\n\t"                                              \
        "cpuid\n\t"                                                      \
        : "=r"(cycles_high1), "=r"(cycles_low1)::"%rax", "%rbx", "%rcx", \
          "%rdx")

#define GET_MEASUREMENT()                            \
    (((uint64_t)cycles_high1 << 32) | cycles_low1) - \
        (((uint64_t)cycles_high0 << 32) | cycles_low0)

void run_test(int* test_res) {
    char* buf = malloc(N * 1024 * sizeof(char));   // malloc 1GB
    char* buf2 = malloc(N * 1024 * sizeof(char));  // malloc 1GB
    uint64_t freq = 3.6 * pow(10, 9);

    START_MEASUREMENT();
#include "ram-bandwidth-w-helper.h"
    END_MEASUREMENT();

    uint64_t cycles = GET_MEASUREMENT();
    printf("write of 1MB: %lu cycles\n", cycles / 1024);
    printf("estimate write bandwidth: %lu MB/s\n", (freq * 1024) / cycles);

    START_MEASUREMENT();
#include "ram-bandwidth-rw-helper.h"
    END_MEASUREMENT();

    uint64_t cycles_rw = GET_MEASUREMENT();
    printf("read and write of 1MB: %lu cycles\n", cycles_rw / 1024);

    printf("read of 1MB: %lu cycles\n", (cycles_rw - cycles) / 1024);
    printf("estimate read bandwidth: %lu MB/s\n",
           (freq * 1024) / (cycles_rw - cycles));

    free(buf);
    free(buf2);

    test_res[0] = (freq * 1024) / cycles;
    test_res[1] = (freq * 1024) / (cycles_rw - cycles);
}

int main(int argc, char* argv[]) {
    int tmp[2];
    int res1[10];
    int res2[10];
    for (int i = 0; i < 10; ++i) {
        run_test(tmp);
        puts("\n");
        res1[i] = tmp[0];
        res2[i] = tmp[1];
    }

    // calculate standard deviation
    double mean1 = 0;
    double mean2 = 0;
    for (int i = 0; i < 10; ++i) {
        mean1 += res1[i];
        mean2 += res2[i];
    }
    mean1 /= 10;
    mean2 /= 10;

    double std1 = 0;
    double std2 = 0;
    for (int i = 0; i < 10; ++i) {
        std1 += (res1[i] - mean1) * (res1[i] - mean1);
        std2 += (res2[i] - mean2) * (res2[i] - mean2);
    }
    std1 /= 10;
    std2 /= 10;
    std1 = sqrt(std1);
    std2 = sqrt(std2);

    printf("write bandwidth: %lf +- %lf MB/s\n", mean1, std1);
    printf("read bandwidth: %lf +- %lf MB/s\n", mean2, std2);

    return 0;
}
