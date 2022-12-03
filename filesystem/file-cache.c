#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <stdint.h>
#include <fcntl.h>

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

void run_test(double* result) {
    const char* home = getenv("HOME");
    const char* filepath = "tmp-files";
    for (int i = 0; i < 16; ++i) {
        char filename[100];
        sprintf(filename, "%s/%s/%d", home, filepath, i);
        CLEAR_CACHE();

        FILE* stream;
        char buf[512];
        int status;
        printf("current file: %s\n", filename);
        // warm up cache
        stream = fopen(filename, "r");
        if (stream == NULL) {
            printf("Error opening file\n");
            exit(1);
        }
        status = fseek(stream, 0, SEEK_SET);
        if (status != 0) {
            printf("Error seeking file\n");
            exit(1);
        }
        while (fread(buf, 1, 512, stream) > 0) {
        }

        // start testing
        stream = fopen(filename, "r");
        if (stream == NULL) {
            printf("Error opening file\n");
            exit(1);
        }
        status = fseek(stream, 0, SEEK_SET);
        if (status != 0) {
            printf("Error seeking file\n");
            exit(1);
        }
        START_MEASUREMENT();
        status = fread(buf, 1, 512, stream);
        END_MEASUREMENT();
        if (status != 512) {
            printf("Error reading file\n");
            exit(1);
        }

        uint64_t cycles = GET_MEASUREMENT();
        printf("cycles: %lu\n", cycles);

        // transform to us
        printf("time: %f us\n", cycles / (FREQ * 1e3));
        result[i] = cycles / (FREQ * 1e3);
    }
}

int main(int argc, char *argv[])
{
    if (getuid() != 0) {
        printf("You need to be root to run this test\n");
        exit(1);
    }
    double result[16];
    run_test(result);
    FILE* res = fopen("./file-cache-res.csv", "w");
    for (int i = 0; i < 16; ++i) {
        fprintf(res, "%d, %f\n", i, result[i]);
    }
    return 0;
}
