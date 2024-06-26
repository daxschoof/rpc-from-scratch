#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <sys/wait.h>
#include <signal.h>

#include "../is_prime.h"

#define SERVERPORT "5005"  // The port the server will be listening on.

// Gets the IPv4 or IPv6 sockaddr.
void *get_in_addr(struct sockaddr *sa) {
  if (sa->sa_family == AF_INET) {
    return &(((struct sockaddr_in*)sa)->sin_addr);
  } else {
    return &(((struct sockaddr_in6*)sa)->sin6_addr);
  }
}

int unpack(int packed_input) {
    return ntohs(packed_input);
}

int get_and_bind_socket() {
    int sockfd;
    struct addrinfo hints, *server_info, *p;

    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE;
    
    int ret = getaddrinfo(NULL, SERVERPORT, &hints, &server_info);
    if (ret != 0) {
        fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(ret));
        exit(1);
    }

    for (p = server_info; p != NULL; p = p->ai_next) {
        if ((sockfd = socket(p->ai_family, p->ai_socktype, p->ai_protocol)) == -1) {
            perror("server: socket");
            continue;
        }

        int yes = 1;
        if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int)) == -1) {
            perror("setsockopt");
            exit(1);
        }

        if (bind(sockfd, p->ai_addr, p->ai_addrlen) == -1) {
            close(sockfd);
            perror("server: bind");
            continue;
        }

        break;
    }

    if (p == NULL) {
        fprintf(stderr, "server: failed to bind\n");
        exit(2);
    }

    return sockfd;
}

int main(void) {
    int sockfd = get_and_bind_socket();

    if (listen(sockfd, /*backlog=*/1) == -1) {
        perror("listen");
        exit(1);
    }

    printf("Server waiting for connections.\n");

    struct sockaddr their_addr;
    socklen_t sin_size;
    int new_fd;

    while (1) {
        sin_size = sizeof their_addr;

        new_fd = accept(sockfd, (struct sockaddr *)&their_addr, &sin_size);
        if (new_fd == -1) {
            perror("accept");
            continue;
        }

        int buffer;
        int bytes_received = recv(new_fd, &buffer, sizeof buffer, 0);
        if (bytes_received == -1) {
            perror("recv");
            continue;
        }

        int num = unpack(buffer);
        printf("Received a request: is %d prime?\n", num);

        bool num_is_prime = is_prime(num);
        printf("Sending response: %s\n", num_is_prime ? "true" : "false");

        if (send(new_fd, &num_is_prime, sizeof num_is_prime, 0) == -1) {
            perror("send");
        }

        close(new_fd);
    }
}