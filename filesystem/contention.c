#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define __USE_GNU
#include <fcntl.h>
#include <math.h>
#include <time.h>

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
    const char* home = getenv("HOME");
    char filename[100];
    sprintf(filename, "%s/%s", home, "tmp-files/14");

    int fd;
    fd = open(filename, O_DIRECT);  // the file size is 16GB
    if (fd == -1) {
        printf("Error opening file %s\n", filename);
    }

    char buf[512];  // block size is 512 bytes
    // start sequencial testing
    uint64_t sequencial_clocks = 0;
    srand(time(NULL));
    int status;
    status = lseek(fd, 0, SEEK_SET);
    if (status == -1) {
        printf("Error seeking file %s\n", filename);
        exit(1);
    }
    for (int i = 0; i < 1024; ++i) {
        CLEAR_CACHE();
        START_MEASUREMENT();
        // read will automatically seek to the next block
        status = read(fd, buf, 512);
        END_MEASUREMENT();
        CLEAR_CACHE();
        if (status == -1) {
            printf("Error reading file %s\n", filename);
            exit(1);
        }
        for (int i = 0; i < 512; ++i) {
            if (buf[i] != 'a') {
                printf("Error checking buffer at %d\n", i);
            }
        }
        sequencial_clocks += GET_MEASUREMENT();
    }

    uint64_t random_clocks = 0;
    int seek_status;
    // start random testing
    for (int i = 0; i < 1024; ++i) {
        uint64_t offset = rand() * 512;
        while (offset > (uint64_t)1024 * 1024 * 1024 * 16) {
            offset = rand() * 512;
        }
        CLEAR_CACHE();
        START_MEASUREMENT();
        seek_status = lseek(fd, offset, SEEK_SET);
        status = read(fd, buf, 512);
        END_MEASUREMENT();
        CLEAR_CACHE();
        if (seek_status == -1) {
            printf("Error seeking file %s\n", filename);
            exit(1);
        }
        if (status == -1) {
            printf("Error reading file %s\n", filename);
            exit(1);
        }
        for (int i = 0; i < 512; ++i) {
            if (buf[i] != 'a') {
                printf("Error checking buffer at %d\n", i);
            }
        }
        random_clocks += GET_MEASUREMENT();
    }

    close(fd);

    printf("sequencial_clocks: %lu, random_clocks: %lu\n",
           sequencial_clocks / 1024, random_clocks / 1024);

    // transform to nanoseconds
    printf("sequencial time per block: %f us, random time per block: %f us\n",
           sequencial_clocks / 1024.0 / (FREQ * 1e3), random_clocks / 1024.0 / (FREQ * 1e3));
}

int main(int argc, char* argv[]) {
    if (getuid() != 0) {
        printf("You need to be root to run this test\n");
        exit(1);
    }
    float result[15];
    run_test(result);
    return 0;
}
