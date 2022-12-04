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

void run_test(double* result_seq, double* result_rand) {
    for (int i = 0; i < 11; ++i) {
        char filename[100];
        sprintf(filename, "%s/%d", "./build/nfs-files", i);
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
        uint64_t size = lseek(fd, 0, SEEK_END) + 1;
        if (size == -1) {
            printf("Error seeking file %s\n", filename);
            exit(1);
        }
        printf("File size: %lu KB\n", size / 1024);

        status = lseek(fd, 0, SEEK_SET);
        if (status == -1) {
            printf("Error seeking file %s\n", filename);
            exit(1);
        }
        for (int i = 0; i < size / 512; ++i) {
            CLEAR_CACHE();
            START_MEASUREMENT();
            // read will automatically seek to the next block
            status = read(fd, buf, 512);
            END_MEASUREMENT();
            if (status != 512) {
                printf("Error reading file %s\n", filename);
                printf("read: %d\n", status);
                exit(1);
            }
            for (int j = 0; j < 512; ++j) {
                if (buf[j] != 'a') {
                    printf("Error checking buffer at %d, it's %d\n", j, buf[j]);
                    exit(1);
                }
            }
            sequencial_clocks += GET_MEASUREMENT();
        }

        uint64_t random_clocks = 0;
        int seek_status;
        // start random testing
        for (int i = 0; i < size / 512; ++i) {
            uint64_t offset = rand() % (size - 512);
            CLEAR_CACHE();
            START_MEASUREMENT();
            seek_status = lseek(fd, offset, SEEK_SET);
            status = read(fd, buf, 512);
            END_MEASUREMENT();
            if (seek_status == -1) {
                printf("Error seeking file %s\n", filename);
                exit(1);
            }
            if (status != 512) {
                printf("Error reading file %s\n", filename);
                exit(1);
            }
            for (int j = 0; j < 512; ++j) {
                if (buf[j] != 'a') {
                    printf("Error checking buffer at %d, it's %d\n", j, buf[j]);
                    exit(1);
                }
            }
            random_clocks += GET_MEASUREMENT();
        }

        close(fd);

        printf("sequencial_clocks: %lu, random_clocks: %lu\n",
               sequencial_clocks / (size / 512), random_clocks / (size / 512));

        // transform to us
        printf(
            "sequencial time per block: %f us, random time per block: %f us\n",
            sequencial_clocks / (double)(size / 512.0) / (FREQ * 1e3),
            random_clocks / (double)(size / 512.0) / (FREQ * 1e3));
        result_seq[i] =
            sequencial_clocks / (double)(size / 512.0) / (FREQ * 1e3);
        result_rand[i] = random_clocks / (double)(size / 512.0) / (FREQ * 1e3);
    }
}

int main(int argc, char* argv[]) {
    if (getuid() != 0) {
        printf("You need to be root to run this test\n");
        exit(1);
    }
    double result_seq[11];
    double result_rand[11];
    run_test(result_seq, result_rand);
    FILE* res = fopen("./nfs-res.csv", "w");
    for (int i = 0; i < 11; ++i) {
        fprintf(res, "%d, %f, %f\n", i, result_seq[i], result_rand[i]);
    }
    return 0;
    }
