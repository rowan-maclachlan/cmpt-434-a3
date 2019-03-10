/* 
 * Rowan MacLachlan
 * rdm695 11165820
 * CMPT 434 Eager
 * assignment 3
 * Monday March 18th 2019
 */

#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <unistd.h>
#include <errno.h> 
#include <string.h>
#include <netdb.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h>

#include "common.h"

#define PORT_SIZE 16 
#define HOSTNAME_SIZE 64
#define USAGE "./sensor_node <id> <payload> <D> <listening_port> <logger_hostname> <logger_port>\n"

bool _init_args(int argc, char **argv, int *id, int *distance) {

    if (argc != 7) {
        return false;
    }

    if (0 >= sscanf(argv[1], "%d", id) || (0 > *id)) {
        fprintf(stderr, "Invalid ID: 0 > %d .\n", *id);
        return false;
    }
    if (0 >= sscanf(argv[3], "%d", distance)) {
        fprintf(stderr, "Invalid distance.\n");
        return false;
    }

    return true;
}

/*
 * argv should contain: 
 *  uint32 id, 
 *  char *data, 
 *  uint32 distance,
 *  uint32 listening_port, 
 *  char *logger_name, 
 *  uint32 logger_port
 * "./sensor_node <id> <payload> <D> <listening_port> <logger_hostname> <logger_port>\n"
 */
int main(int argc, char **argv) {
    int sockfd;
    struct addrinfo hints;
    struct addrinfo *servinfo;
    struct addrinfo *p;
    char s[INET6_ADDRSTRLEN];
    int status;

    int id = 0;
    char *data = "";
    int distance = 0;
    char my_hostname[HOSTNAME_SIZE];
    char my_port[PORT_SIZE];
    char logger_hostname[HOSTNAME_SIZE];
    char logger_port[PORT_SIZE];

    if (!_init_args(argc, argv, &id, &distance)) {
        fprintf(stderr, USAGE);
        exit(EXIT_FAILURE);
    }
    else {
        data = strdup(argv[2]);
        strcpy(my_port, argv[4]);
        strcpy(logger_hostname, argv[5]);
        strcpy(logger_port, argv[6]);
        printf("Node %d connecting to logger host %s, port %s with port %s. "
               "Using payload \"%s\", distance %d.\n",
               id, logger_hostname, logger_port, my_port, data, distance);
    }

    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE;

    if ((status = getaddrinfo(logger_hostname, logger_hostname, &hints, &servinfo)) != 0) {
        fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(status));
        exit(1);
    }

    // loop through all the results and connect to the first we can
    for(p = servinfo; p != NULL; p = p->ai_next) {
        if ((sockfd = socket(p->ai_family, p->ai_socktype, p->ai_protocol)) == -1) {
            perror("sensor: socket");
            continue;
        }

        if (connect(sockfd, p->ai_addr, p->ai_addrlen) == -1) {
            perror("sensor: connect");
            close(sockfd);
            continue;
        }

        break;
    }

    if (p == NULL) {
        fprintf(stderr, "sensor: failed to connect\n");
        status = 1;
        goto _end;
    }

    inet_ntop(p->ai_family, get_in_addr((struct sockaddr *)p->ai_addr), s, sizeof s);
    printf("sensor: connecting to %s on socket %d\n", s, sockfd);

_end:
    freeaddrinfo(servinfo); // all done with this structure
    free(data);
    close(sockfd);
    exit(status);
}

