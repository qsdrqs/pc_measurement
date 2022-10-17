/*
 * process-context-switch.c: context switching between processes
 *
 * Author: Tianyang Zhou <t7zhou@ucsd.edu>
 *
 */
#include <inttypes.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <unistd.h>

#define N 1000000

#define MEASURE_START()                                                  \
    asm volatile(                                                        \
        "cpuid\n\t"                                                      \
        "rdtsc\n\t"                                                      \
        "mov %%edx, %0\n\t"                                              \
        "mov %%eax, %1\n\t"                                              \
        : "=r"(cycles_high0), "=r"(cycles_low0)::"%rax", "%rbx", "%rcx", \
          "%rdx")

#define MEASURE_END()                                                    \
    asm volatile(                                                        \
        "rdtscp\n\t"                                                     \
        "mov %%edx, %0\n\t"                                              \
        "mov %%eax, %1\n\t"                                              \
        "cpuid\n\t"                                                      \
        : "=r"(cycles_high1), "=r"(cycles_low1)::"%rax", "%rbx", "%rcx", \
          "%rdx")

int main(int argc, char *argv[]) {
    uint32_t cycles_low0;
    uint32_t cycles_high0;
    uint32_t cycles_low1;
    uint32_t cycles_high1;

    // pipe
    int pipefd[2];   // pipefd[0] is for reading, pipefd[1] is for writing
    int pipefd2[2];  // pipefd2[0] is for reading, pipefd2[1] is for writing
    if (pipe(pipefd) != 0) {
        perror("pipe creation failed");
        return 1;
    }
    if (pipe(pipefd2) != 0) {
        perror("pipe creation failed");
        return 1;
    }

    // fork
    int pid = fork();
    if (pid < 0) {
        perror("fork failed");
        return 1;
    }
    if (pid == 0) {
        // child process
        int random_buf[1] = {0};
        close(pipefd[1]);
        close(pipefd2[0]);

        for (int i = 0; i < N; ++i) {
            read(pipefd[0], random_buf, sizeof(random_buf));
            write(pipefd2[1], random_buf, sizeof(random_buf));
        }

        close(pipefd2[1]);
        close(pipefd[0]);
        exit(0);
    } else {
        // parent process
        int random_buf[1] = {0};
        close(pipefd[0]);
        close(pipefd2[1]);

        // measure the time of read and write of pipe
        int measure_rw[2];
        if (pipe(measure_rw) != 0) {
            perror("pipe creation failed");
            return 1;
        }
        uint64_t rw_sum = 0;
        for (int i = 0; i < N; ++i) {
            MEASURE_START();
            write(measure_rw[1], random_buf, sizeof(random_buf));
            read(measure_rw[0], random_buf, sizeof(random_buf));
            MEASURE_END();
            rw_sum += (((uint64_t)cycles_high1 << 32) | cycles_low1) -
                      (((uint64_t)cycles_high0 << 32) | cycles_low0);
        }
        printf("average time of read and write of pipe: %lu cycles\n",
               rw_sum / N);

        // measure 2 time of context switching plus read and write of pipe
        uint64_t srw2_sum = 0;
        for (int i = 0; i < N; ++i) {
            MEASURE_START();
            write(pipefd[1], random_buf, sizeof(random_buf));
            read(pipefd2[0], random_buf, sizeof(random_buf));
            MEASURE_END();
            srw2_sum += (((uint64_t)cycles_high1 << 32) | cycles_low1) -
                        (((uint64_t)cycles_high0 << 32) | cycles_low0);
        }
        printf(
            "average time of 2 times of context switching plus read and write "
            "of pipe: %lu cycles\n",
            srw2_sum / N);
        uint64_t context_switching_time =
            (srw2_sum / N - 2 * rw_sum / N + 48) / 2;
        printf("average time of context switching: %lu cycles\n",
               context_switching_time);
        close(pipefd[1]);
        close(pipefd2[0]);
    }

    return 0;
}
