#include <arpa/inet.h>
#include <math.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>

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

// 3.6 GHz
#define FREQ 3.6

int sockfd;
struct sockaddr_in servaddr;

void run_test(double *res) {
    const int rounds = 100;
    int n = 0;
    uint64_t connect_cycles = 0;
    uint64_t disconnect_cycles = 0;

    for (int i = 0; i < rounds; ++i) {
        // setup
        sockfd = socket(AF_INET, SOCK_STREAM, 0);
        // wait until server start to listen
        usleep(100);
        START_MEASUREMENT();
        n = connect(sockfd, (struct sockaddr *)&servaddr, sizeof(servaddr));
        END_MEASUREMENT();
        if (n < 0) {
            perror("Connection failed\n");
            close(sockfd);
            exit(-1);
        }
        connect_cycles += GET_MEASUREMENT();

        // send data (b means not to end the server)
        send(sockfd, "b", 1, 0);

        // teardown
        START_MEASUREMENT();
        n = close(sockfd);
        END_MEASUREMENT();

        if (n < 0) {
            perror("Close failed\n");
            exit(-1);
        }
        disconnect_cycles += GET_MEASUREMENT();
    }

    printf("Connect Cycles: %lu\n", connect_cycles / rounds);
    printf("Disconnect Cycles: %lu\n", disconnect_cycles / rounds);
    printf("Connect Time: %f ms\n",
           ((double)connect_cycles / rounds) / (FREQ * 1e6));
    printf("Disconnect Time: %f ms\n",
           ((double)disconnect_cycles / rounds) / (FREQ * 1e6));

    res[0] = ((double)connect_cycles / rounds) / (FREQ * 1e6);
    res[1] = ((double)disconnect_cycles / rounds) / (FREQ * 1e6);
}

int main(int argc, char *argv[]) {
    sockfd = socket(AF_INET, SOCK_STREAM, 0);

    bzero(&servaddr, sizeof(servaddr));

    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = inet_addr("127.0.0.1");
    servaddr.sin_port = htons(5000);

    // start testing
    double res[10];
    double res2[10];
    for (int i = 0; i < 10; ++i) {
        double tmp[2];
        run_test(tmp);
        res[i] = tmp[0];
        res2[i] = tmp[1];
        puts("");
    }

    // calculate standard deviation
    double mean = 0;
    double mean2 = 0;
    for (int i = 0; i < 10; ++i) {
        mean += res[i];
        mean2 += res2[i];
    }
    mean /= 10;
    mean2 /= 10;

    double std = 0;
    double std2 = 0;
    for (int i = 0; i < 10; ++i) {
        std += (res[i] - mean) * (res[i] - mean);
        std2 += (res2[i] - mean2) * (res2[i] - mean2);
    }
    std /= 10;
    std2 /= 10;
    std = sqrt(std);
    std2 = sqrt(std2);

    printf("connect time: %lf +- %lf ms\n", mean, std);
    printf("disconnect time: %lf +- %lf ms\n", mean2, std2);

    // end server
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    int n = connect(sockfd, (struct sockaddr *)&servaddr, sizeof(servaddr));
    if (n < 0) {
        perror("Connection failed\n");
        close(sockfd);
        exit(-1);
    }
    char end[1] = "a"; // a means end the server
    while(send(sockfd, end, 1, 0) >= 0) {
        usleep(100);
    }
    close(sockfd);

    return 0;
}
