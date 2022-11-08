/*
 * access_latency.c: measures latency for individual integer accesses to main memory and the L1 and L2 caches
 *
 * Author: Anze Xie <a1xie@ucsd.edu>
*/

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

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

#define NUM_RUN 8  // number of runs for each experiment
#define STRIDE_SIZE_BASE 2 
#define ARRAY_SIZE_BASE 2
#define NUM_LOAD 1000000
#define MAX_ARRAY_SIZE_POWER 30

int64_t stride_size;
int64_t stride_num; // stride_size / sizeof(int*)
int64_t num_int; // number of integers in array
int** current_access_ptr; // ptr that is being accessing
int64_t array_size;
int64_t num_ptr;

int64_t total_time = 0;
int64_t loop_time = 0;
u_int32_t cycles_high0;
u_int32_t cycles_low0;
u_int32_t cycles_high1;
u_int32_t cycles_low1;

FILE *results_file;

int main(int argc, char* argv[]){
    printf("start experiment\n");
    fflush(stdout);
    results_file = fopen("access_latency_results.txt", "a");

    for (int i = 9; i <= MAX_ARRAY_SIZE_POWER; i++){
        array_size = pow(ARRAY_SIZE_BASE, i);
        num_ptr = array_size / sizeof(int*);
        int** array = (int**)malloc(sizeof(int*) * num_ptr);

        for (int j = 6; j < 13; j++){
            stride_size = pow(STRIDE_SIZE_BASE, j);
            stride_num = stride_size / sizeof(int*);

            printf("Preparing [array_size, num_ptr, stride_size] = [2^(%i), %li, %i]\n", i, num_ptr, stride_size);
            fflush(stdout);
            for (int k = 0; k < (array_size / stride_size); k++){
                array[(k * stride_num) % num_ptr] = (int*)&array[(k * stride_num + stride_num) % num_ptr];
                // printf("[%p], [%p], [%li, %li]\n", 
                //                             &array[(k * stride_num) % num_ptr], // element addr
                //                             array[(k * stride_num) % num_ptr],  // addr pointing to
                //                             (k * stride_num)%num_ptr,
                //                             (k * stride_num));
            }

            printf("Walking [array_size, num_ptr, stride_size] = [2^(%i), %li, %i]\n", i, num_ptr, stride_size);
            fflush(stdout);
            
            for (int l = 0; l < NUM_RUN; l++){
                total_time = 0;
                current_access_ptr = array[0];
                // printf("**%p**\n", current_access_ptr);
                START_MEASUREMENT();
                for (int k = 0; k < NUM_LOAD; k++){
                    current_access_ptr = *current_access_ptr;
                    // printf("**%p**\n", current_access_ptr);
                }
                END_MEASUREMENT();
                total_time = (((int64_t)cycles_high1 << 32) | cycles_low1) - (((int64_t)cycles_high0 << 32) | cycles_low0);
            }

            loop_time = 0;
            START_MEASUREMENT();
            for (int k = 0; k < NUM_LOAD; k++){}
            END_MEASUREMENT();

            total_time = total_time - loop_time;
            if (NULL == results_file){
                perror("Error opening result saving file.");
            }
            else{
                fprintf(results_file, "(%i,%i,%lf)", i, stride_size, ((double)total_time / NUM_LOAD));
            }
        }

        printf("array_size %i / %i completed\n\n", i, MAX_ARRAY_SIZE_POWER);
        if (NULL == results_file){
            perror("Error opening result saving file.");
        }
        else{
            fprintf(results_file, "\n");
        }

    }

    return(0);
    
    
    
    // load integer array size from 512 bytes to  MBs. (128 -  integers)
    
    
    // for (int i = 9; i <= MAX_ARRAY_SIZE_LOG; i++ ){ // max array size is  MB, min is 512B
    //     array_size = pow(ARRAY_SIZE_BASE, i);
    //     num_int = array_size / sizeof(int32_t); 
    //     int* array = (int*)calloc(num_int, sizeof(int32_t));

    //     for (int j = 6; j < 8; j++){ // use 6 different stride sizes, min is 64B, max is 4KB
    //         stride_size = pow(STRIDE_SIZE_BASE, j); // unit is int size
    //         printf("running [2^(%i), %i]Bytes\n", i, stride_size);
    //         fflush(stdout);

    //         for (int l = 0; l < NUM_RUN; l++){ // run a few times and take the last result
    //             total_time = 0;
    //             START_MEASUREMENT();
    //             for (int k = 0; k < NUM_LOAD; k++){
    //                 current_access_num = array[(k * stride_size / sizeof(int32_t)) % num_int]; // load instruction
    //                 // printf("current_access_num=%i, pos=%i  ", current_access_num, (k * stride_size) % array_size);
    //             }
    //             END_MEASUREMENT();
    //             total_time = (((int64_t)cycles_high1 << 32) | cycles_low1) - (((int64_t)cycles_high0 << 32) | cycles_low0);
    //         }

    //         if (NULL == results_file){
    //             perror("Error opening result saving file.");
    //         }
    //         else{
    //             fprintf(results_file, "(%i,%i,%lf)", i, stride_size, ((double)total_time / NUM_LOAD));
    //         }

    //     }

    //     printf("array_size %i / %i completed\n\n", i, MAX_ARRAY_SIZE_LOG);
    //     if (NULL == results_file){
    //         perror("Error opening result saving file.");
    //     }
    //     else{
    //         fprintf(results_file, "\n");
    //     }

    //     //free(array);
    // }

}