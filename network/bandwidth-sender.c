#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>
#include <math.h>

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
#define ONCE (64 * 1024)
#define TOTAL (ONCE * 1024)

int sockfd;
struct sockaddr_in servaddr;

double run_test() {
    char *buffer = (char *)malloc(TOTAL);  // 64 MB
    memset(buffer, 'a', TOTAL);

    int n = 0;
    uint64_t cycles = 0;

    // wait for receiver ready
    int ready = 0;
    recv(sockfd, &ready, sizeof(int), 0);
    while (ready != 1) {
        recv(sockfd, &ready, sizeof(int), 0);
    }


    for (int i = 0; i < TOTAL / ONCE; ++i) {
        START_MEASUREMENT();
        n = send(sockfd, buffer + ONCE * i, ONCE, 0);
        END_MEASUREMENT();

        if (n < 0) {
            perror("Write failed\n");
            close(sockfd);
            return -1;
        }
        cycles += GET_MEASUREMENT();
    }


    puts("finish send data");

    printf("Cycles: %lu\n", cycles);
    printf("Time: %f s\n", cycles / (FREQ * 1e9));
    printf("Bandwidth: %f MB/s\n", 64 / (cycles / (FREQ * 1e9)));
    free(buffer);

    return 64 / (cycles / (FREQ * 1e9));
}

int main(int argc, char *argv[]) {
    sockfd = socket(AF_INET, SOCK_STREAM, 0);

    bzero(&servaddr, sizeof(servaddr));

    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = inet_addr("127.0.0.1");
    servaddr.sin_port = htons(5000);

    if (connect(sockfd, (struct sockaddr *)&servaddr, sizeof(servaddr)) < 0) {
        perror("Connection failed\n");
        close(sockfd);
        return -1;
    }

    // start testing
    double res[10];
    for (int i = 0; i < 10; ++i) {
        res[i] = run_test();
        puts("");
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

    printf("Bandwidth: %lf +- %lf MB/s\n", mean, std);

    close(sockfd);
    return 0;
}

