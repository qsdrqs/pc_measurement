/*
 * page-fault.c: measure page fault time
 *
 * Author: Tianyang Zhou <t7zhou@ucsd.edu>
 *
 */

#include <math.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/resource.h>
#include <time.h>
#include <unistd.h>

uint32_t cycles_low0;
uint32_t cycles_high0;
uint32_t cycles_low1;
uint32_t cycles_high1;
struct rusage usage;

// use system default page size, 4096B, or 4KB
#define PAGE_SIZE 4096

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

void run_test(double* result, int base) {
    char* tmpdir = "./build/tmp";
    int a = 1;
    srand(time(NULL));
    uint64_t total = 0;
    double freq = 3.6;  // frequency of the CPU, in GHz

    system("mkdir ./build/tmp");
    for (int i = 0; i < 100; ++i) {
        // get the random file name (5 char)
        char filename[5];
        for (int i = 0; i < 5; ++i) {
            filename[i] = 'a' + rand() % 26;
        }

        // create that file and fill random data in
        char* filepath = malloc(strlen(tmpdir) + 1 + 5 + 1);
        sprintf(filepath, "%s/%s", tmpdir, filename);
        FILE* fp = fopen(filepath, "w+");
        if (fp == NULL) {
            perror("Error opening file!\n");
            printf("%s\n", filepath);
        }

        fprintf(fp, "a");  // start with a constant char
        for (int i = 1; i < PAGE_SIZE - 1; ++i) {
            fprintf(fp, "%c", rand() % 256);
        }
        fprintf(fp, "a");  // end with a constant char
        fclose(fp);
        fp = fopen(filepath, "r");
        if (fp == NULL) {
            perror("Error opening file!\n");
            printf("%s\n", filepath);
        }

        // map the file to memory by mmap
        char* addr =
            mmap(NULL, PAGE_SIZE, PROT_READ, MAP_PRIVATE, fileno(fp), 0);
        fclose(fp);
        if (addr == MAP_FAILED) {
            perror("mmap failed");
            exit(1);
        }

        char c;
        int current_faults = usage.ru_majflt;
        if (current_faults > i + base * 100) {
            perror("ERROR: more page faults than expected!\n");
            continue;
        }

        // clear page cache, need to be run as root
        system("sync; echo 3 > /proc/sys/vm/drop_caches");

        getrusage(RUSAGE_SELF, &usage);
        printf("major page faults: %ld\n", usage.ru_majflt);

        // start measurement of page fault time for 4KB
        START_MEASUREMENT();
        c = addr[0];
        END_MEASUREMENT();

        getrusage(RUSAGE_SELF, &usage);
        printf("major page faults: %ld\n", usage.ru_majflt);

        // check if there is a new major page fault generated
        if (current_faults == usage.ru_majflt) {
            perror("ERROR: didn't get page fault here!\n");

            // if not, rerun this test and skip this result
            i--;
            continue;
        }
        if (c != 'a') {
            perror("error on checking the first char!\n");
            exit(1);
        }
        int res = GET_MEASUREMENT();
        total += res;
        printf("%d\n", res);

        START_MEASUREMENT();
        c = addr[PAGE_SIZE - 1];
        END_MEASUREMENT();
        if (c != 'a') {
            perror("error on checking the last char!\n");
            exit(1);
        }
        res = GET_MEASUREMENT();
        printf("%d\n", res);
        munmap(addr, PAGE_SIZE);

        free(filepath);
    }
    system("rm -rf ./build/tmp");
    *result = (double)total / 4096 / 100 / freq;  // in ns
}

int main(int argc, char* argv[]) {
    if (getuid() != 0) {
        printf("You need to be root to run this program!\n");
        exit(1);
    }
    // create dir
    double res[10];

    for (int i = 0; i < 10; ++i) {
        run_test(res + i, i);
        puts("\n");
    }

    // calculate standard deviation
    double mean = 0;
    for (int i = 0; i < 10; ++i) {
        mean += res[i];
    }
    mean /= 10;

    double std = 0;
    for (int i = 0; i < 10; ++i) {
        std += (res[i] - mean) * (res[i] - mean);
    }
    std /= 10;
    std = sqrt(std);

    printf("page fault of 1B: %lf +- %lf ns\n", mean, std);

    return 0;
}
