/*
 * sys_call.c: measures the overhead of system call time()
 * Author: Anze Xie <a1xie@ucsd.edu>
 * 
*/

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <float.h>

#define START_MEASUREMENT() \
    asm volatile ("cpuid\n\t" \
		  "rdtsc\n\t" \
		  "mov %%edx, %0\n\t" \
		  "mov %%eax, %1\n\t" \
		  : "=r" (cycles_high0), "=r" (cycles_low0) \
		  :: "%rax", "%rbx", "%rcx", "%rdx")

#define END_MEASUREMENT()  \
    asm volatile ("rdtscp\n\t"  \
		  "mov %%edx, %0\n\t"  \
		  "mov %%eax, %1\n\t"  \
		  "cpuid\n\t"  \
		  : "=r" (cycles_high1), "=r" (cycles_low1)  \
		  :: "%rax", "%rbx", "%rcx", "%rdx")

int const NUM_ITER = 1000000;  // number of iteration per experiment run
int const NUM_TRIAL = 10; // number of trials

u_int32_t cycles_high0;
u_int32_t cycles_low0;
u_int32_t cycles_high1;
u_int32_t cycles_low1;

int64_t total_time = 0;

FILE *results_file;

int main()
{
    results_file = fopen("sys_call_results.txt", "a");

    for (int j=0; j < NUM_TRIAL; j++){
        for (int i=0; i < NUM_ITER; i++){
            // start measurement, took reference from https://cseweb.ucsd.edu//classes/fa18/cse221-a/timing.html
            START_MEASUREMENT();

            // make system call
            time(NULL);

            // end measurement
            END_MEASUREMENT();

            total_time = total_time + (((int64_t)cycles_high1 << 32) | cycles_low1) - (((int64_t)cycles_high0 << 32) | cycles_low0);
        }

        // save results
        if (NULL == results_file){
            perror("Error opening result saving file.");
        }
        else{
            fprintf(results_file, "%f, ", ((double)total_time / NUM_ITER)); 
        }
        total_time = 0;
    }

    return 0;
}
