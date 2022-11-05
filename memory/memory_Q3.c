#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/sched.h>
#include <math.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <sys/mman.h>
// #include <iostream>
// using namespace std;
#include <ctype.h>
#include <fcntl.h>
#include <stdlib.h>
#include <unistd.h>

void run_test(int* res) {
    // FILE *my_file;
    // int mmap_length = 1;
    // char *prefix = "memory_Q3";
    // int *p;
    int record[100];
    long long unsigned int record_sum = 0;
    unsigned low0, high0, low1, high1;
    uint64_t start, end;
    double sqr_sum = 0;
    double my_std = 0;
    char loop_num[2];
    // char file_path[100] =
    // "~/cse221_project/cpu/memory_Q3_dir/memory_Q3_run1_"; char * ptr; char
    // refresh_file_path[100] =
    // "~/cse221_project/cpu/memory_Q3_dir/memory_Q3_run1_";

    //	printf("start looping");

    // make directory
    system("mkdir ./build/Q3_tmp");

    for (int i = 0; i < 100; i++) {
        FILE *my_file = NULL;
        // char file_path[100] =
        // "~/cse221_project/cpu/memory_Q3_dir/memory_Q3_run1_";
        char file_path[100] = "./build/Q3_tmp/memory_Q3_run4_";
        char *ptr;
        char *p;

        // file_path = "~/cse221_project/cpu/memory_Q3_dir/memory_Q3_run1_";

        loop_num[0] = '0' + i;
        loop_num[1] = '\0';

        //		printf("start fopen");

        ptr = strcat(file_path, loop_num);
        ptr = strcat(file_path, ".txt");
        //		printf(file_path);
        //		printf(strcat(strcat("~/cse221_project/cpu/memory_Q3_dir/memory_Q3_run1_",
        //loop_num), ".txt"));

        // my_file =
        // fopen(strcat(strcat("~/cse221_project/cpu/memory_Q3_dir/memory_Q3_run1_",
        // loop_num), ".txt"), "w");
        my_file = fopen(file_path, "w+");

        fprintf(my_file, "%d", i);  // write something random in it
        // fclose(my_file);
        //		printf("fopen done, start mmap");

        // break;

        p = mmap(NULL, 4000 * sizeof(int), PROT_READ | PROT_WRITE, MAP_SHARED,
                 fileno(my_file), 0);

        if (p == MAP_FAILED) {
            perror("failed to call mmap");
        }

        ftruncate(fileno(my_file), 4000 * sizeof(int));

        //		printf("mmap done, start page fault");

        // stop here by commenting out the rest can create files/p and fix bus
        // error, but changing the file name would cause bus error again

        asm volatile(
            "cpuid\n\t"
            "rdtsc\n\t"
            "mov %%edx, %0\n\t"
            "mov %%eax, %1\n\t"
            : "=r"(high0), "=r"(low0)::"%rax", "%rbx", "%rcx", "%rdx");

        char new = p[0];

        asm volatile(
            "rdtscp\n\t"
            "mov %%edx, %0\n\t"
            "mov %%eax, %1\n\t"
            "cpuid\n\t"
            : "=r"(high1), "=r"(low1)::"%rax", "%rbx", "%rcx", "%rdx");

        start = (((uint64_t)high0 << 32) | low0);
        end = (((uint64_t)high1 << 32) | low1);
        record[i] = end - start;
    }

    for (int i = 0; i < 100; i++) {
        record_sum += record[i];
    }

    printf("%llu\n", record_sum / 100);
    *res = record_sum / 100;

    system("rm -rf ./build/Q3_tmp");
}

int main(int argc, char *argv[]) {
    int tmp;
    int res[10];
    for (int i = 0; i < 10; ++i) {
        run_test(&tmp);
        res[i] = tmp;
    }

    // calculate standard deviation
    double mean = 0;
    for (int i = 0; i < 10; ++i) {
        mean += res[i];
    }
    mean /= 10;

    double std = 0;
    for (int i = 0; i < 10; ++i) {
        std += (res[i] - mean) * (res[i] - mean);
    }
    std /= 10;
    std = sqrt(std);

    printf("page fault service time: %lf +- %lf cpu cycles\n", mean, std);
    return 0;
}
