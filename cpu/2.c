#include <stdio.h>
#include <stdio.h>
#include <linux/module.h>
#include <linux/kernel.h>
//#include <linux/init.h>
////#include <linux/hardirq.h>
/////#include <linux/preempt.h>
#include <linux/sched.h>
#include <stdint.h>
/*asm volatile ("cpuid\n\t"
          "rdtsc\n\t"
                  "mov %%edx, %0\n\t"
                          "mov %%eax, %1\n\t"
                                  : "=r" (cycles_high0), "=r" (cycles_low0)
                                          :: "%rax", "%rbx", "%rcx", "%rdx");
                                          */
/* code to measure */
/*
  asm volatile ("rdtscp\n\t"
          "mov %%edx, %0\n\t"
                  "mov %%eax, %1\n\t"
                          "cpuid\n\t"
                                  : "=r" (cycles_high1), "=r" (cycles_low1)
                                          :: "%rax"
                                                  , "%rbx", "%rcx", "%rdx");
                                                  */
void foo0(){}
void foo1(int a){}
void foo2(int a, int b){}
void foo3(int a, int b, int c){}
void foo4(int a, int b, int c, int d){}
void foo5(int a, int b, int c, int d, int e){}
void foo6(int a, int b, int c, int d, int e, int f){}
void foo7(int a, int b, int c, int d, int e, int f, int g){}

int main(int argc, char *argv[]){

	int a,b,c,d,e,f,g;
    int record0[1000];
	int record1[1000];
	int record2[1000];
	int record3[1000];
	int record4[1000];
	int record5[1000];
	int record6[1000];
	int record7[1000];
	long long unsigned int record_sum0 = 0;
	long long unsigned int record_sum1 = 0;
	long long unsigned int record_sum2 = 0;
	long long unsigned int record_sum3 = 0;
	long long unsigned int record_sum4 = 0;
	long long unsigned int record_sum5 = 0;
	long long unsigned int record_sum6 = 0;
	long long unsigned int record_sum7 = 0;
	unsigned low0, high0, low1, high1;
	uint64_t start, end;

	for(int i=0;i<1000;i++){
		asm volatile ("cpuid\n\t"
			          "rdtsc\n\t"
					  "mov %%edx, %0\n\t"
					  "mov %%eax, %1\n\t"
				      : "=r" (high0), "=r" (low0)
					  :: "%rax", "%rbx", "%rcx", "%rdx");
		foo0();
		asm volatile ("rdtscp\n\t"
			          "mov %%edx, %0\n\t"
					  "mov %%eax, %1\n\t"
					  "cpuid\n\t"
					 : "=r" (high1), "=r" (low1)  
					  :: "%rax", "%rbx", "%rcx", "%rdx");
	 	start = (((uint64_t)high0 << 32) | low0);
	    end = (((uint64_t)high1 << 32) | low1);
//	printk(KERN_INFO "\n %llu", end-start);
    //printf("\n foo0: %lu", end-start);
		record0[i] = end-start;

		asm volatile ("cpuid\n\t"
					  "rdtsc\n\t"
					  "mov %%edx, %0\n\t"
				      "mov %%eax, %1\n\t"
					  : "=r" (high0), "=r" (low0)
					  :: "%rax", "%rbx", "%rcx", "%rdx");
		foo1(a);
		asm volatile ("rdtscp\n\t"
					  "mov %%edx, %0\n\t"
			          "mov %%eax, %1\n\t"
				      "cpuid\n\t"
					  : "=r" (high1), "=r" (low1)					
					  :: "%rax", "%rbx", "%rcx", "%rdx");
		start = (((uint64_t)high0 << 32) | low0);
		end = (((uint64_t)high1 << 32) | low1);
//	printk(KERN_INFO "\n %llu", end-start);
   // printf("\n foo1: %lu", end-start);
		record1[i] = end-start;

	    asm volatile ("cpuid\n\t"
			          "rdtsc\n\t"
					  "mov %%edx, %0\n\t"
					  "mov %%eax, %1\n\t"
					  : "=r" (high0), "=r" (low0)
					  :: "%rax", "%rbx", "%rcx", "%rdx");
		foo2(a, b);
	    asm volatile ("rdtscp\n\t"
		              "mov %%edx, %0\n\t"
					  "mov %%eax, %1\n\t"
					  "cpuid\n\t"
					  : "=r" (high1), "=r" (low1)    
					  :: "%rax", "%rbx", "%rcx", "%rdx");
		start = (((uint64_t)high0 << 32) | low0);
		end = (((uint64_t)high1 << 32) | low1);
//	printk(KERN_INFO "\n %llu", end-start);
   // printf("\n foo2: %lu", end-start);
	    record2[i] = end-start;

	    asm volatile ("cpuid\n\t"
		              "rdtsc\n\t"
					  "mov %%edx, %0\n\t"
					  "mov %%eax, %1\n\t"
					  : "=r" (high0), "=r" (low0)
					  :: "%rax", "%rbx", "%rcx", "%rdx");
		foo3(a, b, c);
		asm volatile ("rdtscp\n\t"
			          "mov %%edx, %0\n\t"
					  "mov %%eax, %1\n\t"
					  "cpuid\n\t"
					  : "=r" (high1), "=r" (low1)
					  :: "%rax", "%rbx", "%rcx", "%rdx");
	    start = (((uint64_t)high0 << 32) | low0);
		end = (((uint64_t)high1 << 32) | low1);
//	printk(KERN_INFO "\n %llu", end-start);
    //printf("\n foo3: %lu", end-start);
		record3[i] = end-start;

	    asm volatile ("cpuid\n\t"
			 		  "rdtsc\n\t"
			          "mov %%edx, %0\n\t"
					  "mov %%eax, %1\n\t"
				  	  : "=r" (high0), "=r" (low0)
					  :: "%rax", "%rbx", "%rcx", "%rdx");
		foo4(a, b, c, d);
	    asm volatile ("rdtscp\n\t"
		              "mov %%edx, %0\n\t"
					  "mov %%eax, %1\n\t"
				      "cpuid\n\t"
					  : "=r" (high1), "=r" (low1)    
					  :: "%rax", "%rbx", "%rcx", "%rdx");
	    start = (((uint64_t)high0 << 32) | low0);
		end = (((uint64_t)high1 << 32) | low1);
//	printk(KERN_INFO "\n %llu", end-start);
    //printf("\n foo4: %lu", end-start);
		record4[i] = end-start;

	    asm volatile ("cpuid\n\t"
		              "rdtsc\n\t"
					  "mov %%edx, %0\n\t"
					  "mov %%eax, %1\n\t"								                   
					  : "=r" (high0), "=r" (low0)
					  :: "%rax", "%rbx", "%rcx", "%rdx");
		foo5(a, b, c, d, e);
		asm volatile ("rdtscp\n\t"
					  "mov %%edx, %0\n\t"
					  "mov %%eax, %1\n\t"
					  "cpuid\n\t"
					  : "=r" (high1), "=r" (low1)
					  :: "%rax", "%rbx", "%rcx", "%rdx");
		start = (((uint64_t)high0 << 32) | low0);
		end = (((uint64_t)high1 << 32) | low1);
//	printk(KERN_INFO "\n %llu", end-start);
    //printf("\n foo5: %lu", end-start);
		record5[i] = end-start;

	    asm volatile ("cpuid\n\t"
		              "rdtsc\n\t"
			          "mov %%edx, %0\n\t"
				      "mov %%eax, %1\n\t"
					  : "=r" (high0), "=r" (low0)
				 	  :: "%rax", "%rbx", "%rcx", "%rdx");
		foo6(a, b, c, d, e, f);
	    asm volatile ("rdtscp\n\t"
		              "mov %%edx, %0\n\t"
			          "mov %%eax, %1\n\t"
					  "cpuid\n\t"
					  : "=r" (high1), "=r" (low1)
					  :: "%rax", "%rbx", "%rcx", "%rdx");
	    start = (((uint64_t)high0 << 32) | low0);
		end = (((uint64_t)high1 << 32) | low1);
//	printk(KERN_INFO "\n %llu", end-start);
    //printf("\n foo6: %lu", end-start);
		record6[i] = end-start;

	    asm volatile ("cpuid\n\t"
		              "rdtsc\n\t"
					  "mov %%edx, %0\n\t"
					  "mov %%eax, %1\n\t"
					  : "=r" (high0), "=r" (low0)
					  :: "%rax", "%rbx", "%rcx", "%rdx");
		foo7(a, b, c, d, e, f, g);
		asm volatile ("rdtscp\n\t"
			          "mov %%edx, %0\n\t"
					  "mov %%eax, %1\n\t"
					  "cpuid\n\t"
					  : "=r" (high1), "=r" (low1)
					  :: "%rax", "%rbx", "%rcx", "%rdx");
		start = (((uint64_t)high0 << 32) | low0);
		end = (((uint64_t)high1 << 32) | low1);
//	printk(KERN_INFO "\n %llu", end-start);
    //printf("\n foo7: %lu", end-start);
		record7[i] = end-start;
	}


	for(int i=0;i<1000;i++){
	    record_sum0 += record0[i];
    }

	printf("\n foo0: %llu", record_sum0/1000);

    for(int i=0;i<1000;i++){
	    record_sum1 += record1[i];
	}

	printf("\n foo1: %llu", record_sum1/1000);

	for(int i=0;i<1000;i++){
		record_sum2 += record2[i];
	}

	printf("\n foo2: %llu", record_sum2/1000);

    for(int i=0;i<1000;i++){
		record_sum3 += record3[i];
	}

	printf("\n foo3: %llu", record_sum3/1000);

    for(int i=0;i<1000;i++){
	    record_sum4 += record4[i];
	}

	printf("\n foo4: %llu", record_sum4/1000);

    for(int i=0;i<1000;i++){
	    record_sum5 += record5[i];
	}

	printf("\n foo5: %llu", record_sum5/1000);

	for(int i=0;i<1000;i++){
		record_sum6 += record6[i];
	}

	printf("\n foo6: %llu", record_sum6/1000);

    for(int i=0;i<1000;i++){
	    record_sum7 += record7[i];
	}

	printf("\n foo7: %llu", record_sum7/1000);

	return 0;
}
