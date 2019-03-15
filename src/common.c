/* 
 * Rowan MacLachlan
 * rdm695 11165820
 * CMPT 434 Eager
 * assignment 3
 * Monday March 18th 2019
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <stdbool.h>
#include <netdb.h>
#include <inttypes.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <time.h>

#include <arpa/inet.h>

#include "common.h"
#include "messages.h"

void print_bytes(char *buf, int len) {
    for (int i = 0; i < len; i++) {
        if (i > 0) printf(":");
        printf("%02X", buf[i]);
    }
    printf("\n");
}

void print_string(char *buf, int len) {
    for (int i = 0; i < len; i++) {
        if ('\0' == buf[i]) {
            printf("%c", '*');
        } else {
            printf("%c", buf[i]);
        }
    }
    printf("\n");
}

enum ip_type get_ip_type(sa_family_t sa_ft) {
    return sa_ft == AF_INET ? IPV4 : IPV6;
}

void save_sensor(struct sensor *sensor, 
                 enum ip_type ip_type,
                 char *their_addr,
                 int their_port, 
                 int their_id, 
                 position their_position) {
    sensor->id = their_id;
    sensor->p = their_position;
    sprintf(sensor->port, "%d", their_port);
    sensor->ip_type = ip_type;
    strncpy(sensor->ip, their_addr, INET6_ADDRSTRLEN);
    sensor->active = true;
}

int server_connect_with(char *port) {
    struct addrinfo *servinfo, *p, hints;
    int sockfd, status;
    int yes = 1;
    char s[INET6_ADDRSTRLEN];

	memset(&hints, 0, sizeof hints);
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_flags = AI_PASSIVE;

	if ((status = getaddrinfo(NULL, port, &hints, &servinfo)) != 0) {
		perror("getaddrinfo");
		return -1;
	}

	// loop through all the results and bind to the first we can
	for(p = servinfo; p != NULL; p = p->ai_next) {
		if ((sockfd = socket(p->ai_family, p->ai_socktype, p->ai_protocol)) == -1) {
			perror("socket");
			continue;
		}

		if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int)) == -1) {
			perror("setsockopt");
			continue;
        }

		if (bind(sockfd, p->ai_addr, p->ai_addrlen) == -1) {
            get_ip_str(p->ai_addr, s, INET6_ADDRSTRLEN);
            fprintf(stderr, "\nFAILED WITH: %d, %s\n", sockfd, s);
			close(sockfd);
			perror("bind");
			continue;
		}

		break;
	}

    if (p == NULL) {
        fprintf(stderr, "failed to bind to valid addrinfo\n");
        sockfd = -1;
    }

    freeaddrinfo(servinfo);

    return sockfd;
}

int client_connect_to(char *host, char *port) {
    int sockfd = 0;
    struct addrinfo hints, *servinfo, *p;
    char s[INET6_ADDRSTRLEN];
    int status;

    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE;

    if ((status = getaddrinfo(host, port, &hints, &servinfo)) != 0) {
        fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(status));
        return -1;
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
        close(sockfd);
        freeaddrinfo(servinfo);
        return -1;
    }

    inet_ntop(p->ai_family, get_in_addr((struct sockaddr *)p->ai_addr), s, sizeof s);
    printf("sensor: connecting to %s on socket %d\n", s, sockfd);

    // Free unused server info structs
    freeaddrinfo(servinfo);

    return sockfd;
}


/**
 * From Beej's Networking guide.
 */
char *get_ip_str(const struct sockaddr *sa, char *s, size_t maxlen)
{
    switch(sa->sa_family) {
        case AF_INET:
            inet_ntop(AF_INET, &(((struct sockaddr_in *)sa)->sin_addr),
                    s, maxlen);
            break;

        case AF_INET6:
            inet_ntop(AF_INET6, &(((struct sockaddr_in6 *)sa)->sin6_addr),
                    s, maxlen);
            break;

        default:
            strncpy(s, "Unknown AF", maxlen);
            return NULL;
    }

    return s;
}

/**
 * From Beej's Guide
 * get sockaddr, IPv4 or IPv6:
 */
void *get_in_addr(struct sockaddr *sa)
{
    if (sa->sa_family == AF_INET) { // IPv4
        return &(((struct sockaddr_in*)sa)->sin_addr);
    }

    return &(((struct sockaddr_in6*)sa)->sin6_addr); // IPv6
}

