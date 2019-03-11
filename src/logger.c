/* 
 * Rowan MacLachlan
 * rdm695 11165820
 * CMPT 434 Eager
 * assignment 3
 * Monday March 18th 2019
 */

#include <arpa/inet.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "common.h"
#include "distance.h"
#include "messages.h"

#define USAGE "./logger <port_number> <T> <number of sensors>"

bool _init_args(int argc, 
                char **argv, 
                unsigned int *transmission_range, 
                unsigned int *num_nodes, 
                char *port) {
    if (argc != 4) {
        fprintf(stderr, "Not enough arguments.\n");
        return false;
    }

    strcpy(port, argv[1]);
    if (0 >= sscanf(argv[2], "%u", transmission_range)) {
        fprintf(stderr, "Invalid transmission range.\n");
        return false;
    }
    if (0 >= sscanf(argv[3], "%u", num_nodes) || MAX_SENSORS < *num_nodes) {
        fprintf(stderr, "Invalid number of nodes: set 0 < %u < %u.\n", *num_nodes, MAX_SENSORS);
        return false;
    }
    
    return true;
}


int main(int argc, char **argv) {
    char s[INET6_ADDRSTRLEN];
    struct sockaddr_storage their_addr;
    struct addrinfo *servinfo, *p, hints;
    socklen_t sin_size;
    int sock_fd, new_fd, status;
    int yes = 1;
    char hostname[HOSTNAME_SIZE];

    unsigned int transmission_range, num_nodes;
    char port[PORT_SIZE];

    if (!_init_args(argc, argv, &transmission_range, &num_nodes, port)) {
        fprintf(stderr, USAGE);
        exit(1);
    }

    printf("logger process listening on port %s with transmission range %u and %u nodes\n",
           port, transmission_range, num_nodes);

	memset(&hints, 0, sizeof hints);
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_flags = AI_PASSIVE;

	if ((status = getaddrinfo(NULL, port, &hints, &servinfo)) != 0) {
		perror("server: getaddrinfo");
		return 1;
	}

    gethostname(hostname, HOSTNAME_SIZE);
    printf("Running on host %s and listening on port %s.\n", hostname, port);

	// loop through all the results and bind to the first we can
	for(p = servinfo; p != NULL; p = p->ai_next) {
		if ((sock_fd = socket(p->ai_family, p->ai_socktype,
				p->ai_protocol)) == -1) {
			perror("server: socket");
			continue;
		}

		if (setsockopt(sock_fd, SOL_SOCKET, SO_REUSEADDR, &yes,
				sizeof(int)) == -1) {
			perror("setsockopt");
			exit(1); }

		if (bind(sock_fd, p->ai_addr, p->ai_addrlen) == -1) {
			close(sock_fd);
			perror("server: bind");
			continue;
		}

		break;
	}

    if (p == NULL) {
        fprintf(stderr, "server: failed to bind to valid addrinfo");
        exit(1);
    }

    if (listen(sock_fd, 10) == -1) {
        perror("listen");
        exit(1);
    }

    printf("server: waiting for connections...\n");

    free(servinfo);

    return 0;
}

