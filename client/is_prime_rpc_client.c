#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>

#define SERVERPORT "5005" // server will listen on port 5005
#define SERVER "localhost"

#include "is_prime_rpc_client.h"

// converts int from host order to network order
int pack (int input) {
    return htons(input);
}

// get IPv4 or IPv6 sockaddr
void* get_in_addr(struct sockaddr *sa) {
    if (sa->sa_family == AF_INET) {
        return &(((struct sockaddr_in*)sa)->sin_addr);
    } else {
        return &(((struct sockaddr_in6*)sa)->sin6_addr);
    }
}

// get socket to connect with
int get_socket() {
    int sockfd;
    struct addrinfo hints, *server_info, *p;

    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;

    int return_value = getaddrinfo(SERVER, SERVERPORT, &hints, &server_info);
    if (return_value != 0) {
        fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(return_value));
        exit(1);
    }

    // linked-list of addresses, connecting to first one possible
    for(p = server_info; p != NULL; p = p->ai_next) {
        if ((sockfd = socket(p->ai_family, p->ai_socktype, p->ai_protocol)) == -1) {
            perror("client: socket");
            continue;            
        }

        if (connect(sockfd, p->ai_addr, p->ai_addrlen) == -1) {
            close(sockfd);
            perror("client: connect");
            continue;
        }

        break; // have a valid socket
    }

    if (p == NULL) {
        perror("client, failed to connect\n");
    }

    return sockfd;
}

bool is_prime_rpc(int num) {
    int packed_num = pack(num);

    int sockfd = get_socket();

    if (send(sockfd, &packed_num, sizeof packed_num, 0) == -1) {
        perror("send");
        close(sockfd);
        exit(0);
    }

    int buf[1];

    int bytes_received = recv(sockfd, &buf, 1, 0);
    if (bytes_received == -1) {
        perror("recv");
        exit(1);
    }

    bool result = buf[0];
    printf("%d\n",result);
    close(sockfd);
    return result;
}