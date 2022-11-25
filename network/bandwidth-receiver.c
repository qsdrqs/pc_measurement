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

#define TOTAL (1024 * 1024 * 1024)

int main(int argc, char *argv[]) {
    int sockfd;
    struct sockaddr_in servaddr, cliaddr;
    socklen_t cliaddr_len;

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
    printf("client connected from %s:%d\n", inet_ntoa(cliaddr.sin_addr),
           ntohs(cliaddr.sin_port));
    if (connfd < 0) {
        perror("accept failed");
        close(connfd);
        close(sockfd);
        return -1;
    }

    // test for 10 times
    for (int i = 0; i < 10; ++i) {
        char *buffer = (char *)malloc(TOTAL);  // 1GB
        memset(buffer, 0, TOTAL);

        if (recv(connfd, buffer, TOTAL, MSG_WAITALL) < 0) {
            perror("receive failed\n");
            close(connfd);
            close(sockfd);
            return -1;
        }

        puts("fininsh receive data");


        // check the correctness of the data
        for (uint64_t i = 0; i < TOTAL; i++) {
            if (buffer[i] != 'a') {
                fprintf(stderr, "Data is corrupted at %ld, which is %d\n", i, (int)buffer[i]);
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
