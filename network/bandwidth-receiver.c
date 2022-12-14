#include <arpa/inet.h>
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

#define ONCE (64 * 1024)
#define TOTAL (ONCE * 1024)

int main(int argc, char *argv[]) {
    int sockfd;
    struct sockaddr_in servaddr, cliaddr;
    socklen_t cliaddr_len = sizeof(cliaddr);

    sockfd = socket(AF_INET, SOCK_STREAM, 0);

    bzero(&servaddr, sizeof(servaddr));
    bzero(&cliaddr, sizeof(cliaddr));

    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = INADDR_ANY;
    servaddr.sin_port = htons(5000);

    if (bind(sockfd, (struct sockaddr *)&servaddr, sizeof(servaddr)) < 0) {
        perror("bind failed");
        return -1;
    }

    puts("start listening on port 5000");
    listen(sockfd, 1);  // only one client
    int connfd = accept(sockfd, (struct sockaddr *)&cliaddr, &cliaddr_len);
    if (connfd < 0) {
        perror("accept failed");
        close(connfd);
        close(sockfd);
        return -1;
    }
    printf("client connected from %s:%d\n", inet_ntoa(cliaddr.sin_addr),
           ntohs(cliaddr.sin_port));

    // test for 10 times
    for (int i = 0; i < 10; ++i) {
        char *buffer = (char *)malloc(TOTAL);  // 64 MB
        memset(buffer, 0, TOTAL);

        // tell sender ready to receive
        int ready = 1;
        send(connfd, &ready, sizeof(int), 0);

        for (int i = 0; i < TOTAL / ONCE; ++i) {
            recv(connfd, buffer + ONCE * i, ONCE, MSG_WAITALL);
        }

        puts("fininsh receive data");


        // check the correctness of the data
        for (uint64_t i = 0; i < TOTAL; i++) {
            if (buffer[i] != 'a') {
                fprintf(stderr, "Data is corrupted at %ld, which is %d\n", i, (int)buffer[i]);
                close(connfd);
                close(sockfd);
                return -1;
            }
        }
        puts("check successful");
        free(buffer);
    }

    close(connfd);
    close(sockfd);

    return 0;
}
