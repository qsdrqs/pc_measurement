#include <stdio.h>
#include <linux/module.h> 
#include <linux/kernel.h> 
#include <linux/sched.h>
#include <stdint.h>

int main(int argc, char *argv[]){

	int record[1000000];
	long long unsigned int record_sum = 0;
	unsigned low0, high0, low1, high1;
	uint64_t start, end;

	for(int i=0;i<1000000;i++){
		asm volatile ("cpuid\n\t"
		    "rdtsc\n\t"
			"mov %%edx, %0\n\t"
			"mov %%eax, %1\n\t"
			: "=r" (high0), "=r" (low0)
			:: "%rax", "%rbx", "%rcx", "%rdx");
			
		asm volatile ("rdtscp\n\t"
	        "mov %%edx, %0\n\t"
			"mov %%eax, %1\n\t"
		    "cpuid\n\t"
			: "=r" (high1), "=r" (low1)
			:: "%rax"
			, "%rbx", "%rcx", "%rdx");

		start = (((uint64_t)high0 << 32) | low0);
		end = (((uint64_t)high1 << 32) | low1);
		record[i] = end - start;
		
	}
	
	for(int i=0;i<1000000;i++){
		record_sum += record[i];
	}

	printf("\n %llu", record_sum/1000000);

	//for(range(1000))(start end)

/*	start
		for(i=0;i<1000;i++){}
	end
*/
		//start for(range(1000)) end
	return 0;
}
