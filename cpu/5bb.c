/*
 * 5bb.c: context switching between kernel threads
 *
 * Author: Tianyang Zhou <t7zhou@ucsd.edu>
 *
 */
#include <inttypes.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define N 100000

int pipefd[2];   // pipefd[0] is for reading, pipefd[1] is for writing
int pipefd2[2];  // pipefd2[0] is for reading, pipefd2[1] is for writing

uint32_t cycles_low0;
uint32_t cycles_high0;
uint32_t cycles_low1;
uint32_t cycles_high1;

uint64_t rw_sum;
uint64_t srw2_sum;

void *context_switch(void *thread_id) {
    int id = *(int *)thread_id;
    if (id == 1) {
        int random_buf[1] = {0};

        for (int i = 0; i < N; ++i) {
            asm volatile(
                "cpuid\n\t"
                "rdtsc\n\t"
                "mov %%edx, %0\n\t"
                "mov %%eax, %1\n\t"
                : "=r"(cycles_high0), "=r"(cycles_low0)::"%rax", "%rbx", "%rcx",
                  "%rdx");
            write(pipefd[1], random_buf, sizeof(random_buf));
            read(pipefd2[0], random_buf, sizeof(random_buf));
        }
    } else if (id == 2) {
        int random_buf[1] = {0};

        for (int i = 0; i < N; ++i) {
            read(pipefd[0], random_buf, sizeof(random_buf));
            asm volatile(
                "rdtscp\n\t"
                "mov %%edx, %0\n\t"
                "mov %%eax, %1\n\t"
                "cpuid\n\t"
                : "=r"(cycles_high1), "=r"(cycles_low1)::"%rax", "%rbx", "%rcx",
                  "%rdx");
            srw2_sum+= (((uint64_t)cycles_high1 << 32) | cycles_low1) -
                      (((uint64_t)cycles_high0 << 32) | cycles_low0);
            write(pipefd2[1], random_buf, sizeof(random_buf));
        }
    } else {
        perror("Invalid thread id");
    }
    return 0;
}

int main(int argc, char *argv[]) {
    printf("test\n");
    // create two threads
    pthread_t thread1, thread2;
    int a = 1;  // tmp var
    int b = 2;  // tmp var
    pthread_create(&thread1, NULL, context_switch, (void *)&a);
    pthread_create(&thread2, NULL, context_switch, (void *)&b);

    // pipe
    if (pipe(pipefd) != 0) {
        perror("pipe creation failed");
        return 1;
    }
    if (pipe(pipefd2) != 0) {
        perror("pipe creation failed");
        return 1;
    }

    pthread_join(thread1, NULL);
    pthread_join(thread2, NULL);

    uint64_t context_switching_time = srw2_sum / N;
    printf("average time of context switching: %lu cycles\n", context_switching_time);
    return 0;
}
