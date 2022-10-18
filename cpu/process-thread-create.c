/*
 * process-thread-create.c: time to create and run both a process and a kernel
 * thread
 *
 * Author: Tianyang Zhou <t7zhou@ucsd.edu>
 *
 */

#include <pthread.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <unistd.h>

#define N 10000

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

uint32_t cycles_low0;
uint32_t cycles_high0;
uint32_t cycles_low1;
uint32_t cycles_high1;

void *thread_function(void *arg) {
    MEASURE_END();
    return NULL;
}

int main(int argc, char *argv[]) {
    /***********************
     * Process Measurement *
     ***********************/
    int clock_cycle[2];  // this pipe is used to transmit measured time from
                         // child to parent

    if (pipe(clock_cycle) == -1) {
        perror("pipe");
        exit(EXIT_FAILURE);
    }
    // create a process
    uint64_t process_cycles = 0;
    for (int i = 0; i < N; ++i) {
        MEASURE_START();
        if (fork() == 0) {
            // child process
            MEASURE_END();
            uint64_t this_cycles =
                (((uint64_t)cycles_high1 << 32) | cycles_low1) -
                (((uint64_t)cycles_high0 << 32) | cycles_low0);
            write(clock_cycle[1], &this_cycles, sizeof(this_cycles));
            exit(0);
        } else {
            uint64_t this_cycles = 0;
            read(clock_cycle[0], &this_cycles, sizeof(this_cycles));
            wait(NULL);
            process_cycles += this_cycles;
        }
    }

    printf("process creation and run time: %lu\n", process_cycles / N - 47);

    /***********************
     * Thread Measurement *
     ***********************/
    uint64_t thread_cycles = 0;
    for (int i = 0; i < N; ++i) {
        pthread_t thread;
        MEASURE_START();
        pthread_create(&thread, NULL, thread_function, NULL);
        pthread_join(thread, NULL);
        thread_cycles += (((uint64_t)cycles_high1 << 32) | cycles_low1) -
                         (((uint64_t)cycles_high0 << 32) | cycles_low0);
    }

    printf("thread creation and run time: %lu\n", thread_cycles / N - 47);

}
