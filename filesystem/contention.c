#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
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

void run_test(double* result) {
    // 1 to 21 processes, one master and 1-20 slaves
    for (int i = 0; i < 20; ++i) {
        CLEAR_CACHE();
        const char* base = "./build/contention-files/";
        char filename[100];
        int id = 0;
        int syncpipe[2];  // 0: read, 1: write, first pipe is used to read from
                          // child, second pipe is used to write to child
        int respipe[2];   // 0: read, 1: write
        int status = pipe(syncpipe);
        if (status == -1) {
            perror("pipe");
            exit(1);
        }
        status = pipe(respipe);
        if (status == -1) {
            perror("pipe");
            exit(1);
        }
        for (int j = 0; j <= i; ++j) {
            int pid = fork();
            if (pid == 0) {
                // child process
                id = j + 1;
                break;
            }
        }
        if (id == 0) {
            char buf[i + 1];
            usleep(1000);  // wait for all processes to be ready
            write(syncpipe[1], buf, i + 1);
        } else {
            sprintf(filename, "%s%d", base, id - 1);
            int fd;
            fd = open(filename, O_DIRECT);  // the file size is 50MB

            if (fd == -1) {
                printf("Error opening file %s\n", filename);
                exit(1);
            }

            char buf[512];  // block size is 512 bytes
            lseek(fd, 0, SEEK_SET);

            uint64_t clocks = 0;

            char syncbuf[1];
            read(syncpipe[0], syncbuf, 1);  // sync with other processes
            for (int j = 0; j < 102400; ++j) {
                START_MEASUREMENT();
                status = read(fd, buf, 512);
                END_MEASUREMENT();
                if (status == -1) {
                    printf("Error reading file %s\n", filename);
                    exit(1);
                }
                clocks += GET_MEASUREMENT();
            }

            close(fd);
            write(respipe[1], &clocks, sizeof(clocks));
        }

        // get the result
        if (id == 0) {
            // only parent process need to report the result
            uint64_t result_clocks[i + 1];
            uint64_t total_clocks = 0;
            for (int j = 0; j <= i; ++j) {
                read(respipe[0], &result_clocks[j], sizeof(uint64_t));
                total_clocks += result_clocks[j];
            }
            double avg_clocks = (double)total_clocks / (i + 1);
            printf("Number of processes: %d, clocks: %lf, time: %f us\n", i + 1,
                   avg_clocks, avg_clocks / 102400.0 / (FREQ * 1e3));
            result[i] = avg_clocks / (102400.0 * FREQ * 1e3);
            close(syncpipe[0]);
            close(syncpipe[1]);
            close(respipe[0]);
            close(respipe[1]);
        } else {
            // child process
            exit(0);
        }
    }
}

int main(int argc, char* argv[]) {
    if (getuid() != 0) {
        printf("You need to be root to run this test\n");
        exit(1);
    }
    double result[20];
    run_test(result);
    return 0;
}
