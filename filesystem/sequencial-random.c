#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <stdint.h>

#define __USE_GNU
#include <fcntl.h>
#include <time.h>
#include <math.h>

uint32_t cycles_low0;
uint32_t cycles_high0;
uint32_t cycles_low1;
uint32_t cycles_high1;

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

#define FREQ 3.6
#define CLEAR_CACHE() system("sync; echo 3 > /proc/sys/vm/drop_caches")

void run_test(float* result) {
    const char* filename = "/mnt/Users/qsdrqs/14";

    int fd;
    fd = open(filename, O_RDONLY);
    if (fd == -1) {
        printf("Error opening file %s\n", filename);
    }

    char buf[512]; // block size is 512 bytes
    // start sequencial testing
    uint64_t sequencial_clocks = 0;
    srand(time(NULL));
    uint64_t base = 0;
    for (int i = 0; i < 1024; ++i) {
        uint64_t offset = base + i * 512;
        int status;
        START_MEASUREMENT();
        status = lseek(fd, offset, SEEK_SET);
        status = read(fd, buf, 512);
        END_MEASUREMENT();
        if (status == -1) {
            printf("Error reading file %s\n", filename);
        }

        sequencial_clocks += GET_MEASUREMENT();
    }

    uint64_t random_clocks = 0;
    // start random testing
    for (int i = 0; i < 1024; ++i) {
        uint64_t offset = rand();
        int a, b;
        START_MEASUREMENT();
        a = lseek(fd, offset, SEEK_SET);
        b = read(fd, buf, 512);
        END_MEASUREMENT();
        if (a < 0) {
            perror("lseek2");
            printf("offset: %lu\n", offset);
            exit(-1);
        }
        if (b < 0) {
            perror("read2");
            exit(-1);
        }

        random_clocks += GET_MEASUREMENT();
    }

    close(fd);


    printf("sequencial_clocks: %lu, random_clocks: %lu\n", sequencial_clocks, random_clocks);

    // transform to nanoseconds
    printf("sequencial time per block: %f ns, random time per block: %f ns\n", sequencial_clocks / 1000.0 / FREQ, random_clocks / 1000.0 / FREQ);
}

int main(int argc, char *argv[])
{
    if (getuid() != 0) {
        printf("You need to be root to run this test\n");
        exit(1);
    }
    float result[15];
    run_test(result);
    return 0;
}
