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
#include "distance.h"
#include "messages.h"

#define USAGE "./sensor_node <id> <payload> <D> <listening_port> <logger_hostname> <logger_port>\n"

int ID = 0;
char *DATA = "";
int DISTANCE = 0;
position my_position = { 0, 0 };
char LISTENING_PORT[PORT_SIZE] = { 0 };

bool _init_args(int argc, char **argv) {

    if (argc != 7) {
        return false;
    }

    if (0 >= sscanf(argv[1], "%d", &ID) 
            || (0 > ID) || (MAX_SENSORS <= ID)) {
        fprintf(stderr, "Invalid ID. ID must be 0 <= %d < %d.\n", ID, MAX_SENSORS);
        return false;
    }

    if (NULL == (DATA = strdup(argv[2]))) {
        fprintf(stderr, "Invalid data \"%s\".\n", argv[2]);
        return false;
    }

    if (0 >= sscanf(argv[3], "%d", &DISTANCE)) {
        fprintf(stderr, "Invalid distance.\n");
        return false;
    }

    strcpy(LISTENING_PORT, argv[4]);

    return true;
}

int connect_loop(char *logger_hostname, char *logger_port) {
    struct addrinfo dest;
    int sockfd;
    char ID_MSG_BUF[ID_MSG_SIZE] = { 0 };
    char CONF_MSG_BUF[CONF_MSG_SIZE] = { 0 };
    char DATA_MSG_BUF[DATA_MSG_SIZE] = { 0 };

    while(1) {

        sleep(1);

        move(DISTANCE, get_random_direction(), &my_position);

        // open connection
        if (-1 == (sockfd = client_connect_to(logger_hostname, logger_port, &dest))) {
            fprintf(stderr, "sensor: failed to connect to logger.\n");
            continue;
        }

        if (0 >= serialize_id_msg(ID_MSG_BUF, ID, LISTENING_PORT, &my_position)) {
            fprintf(stderr, "sensor: failed to serialize ID message.\n");
            goto _done;
        }

        // send id msg
        if (0 >= send(sockfd, ID_MSG_BUF, ID_MSG_SIZE, 0)) {
            perror("sensor: send (ID)");
            goto _done;
        }
        // listen for confirmation msg
        if (0 >= recv(sockfd, CONF_MSG_BUF, CONF_MSG_SIZE, 0)) {
            perror("sensor: recv (CONF)");
            goto _done;
        }
        
        log_conf_msg(CONF_MSG_BUF, "sensor");

        enum confirmation conf = OUT_OF_RANGE;
        deserialize_conf_msg(CONF_MSG_BUF, &conf);

        // if in range, send data msg
        if (IN_RANGE == conf) {
            if (0 >= serialize_data_msg(DATA_MSG_BUF, ID, DATA)) {
                fprintf(stderr, "sensor: failed to serialize data message.\n");
                goto _done;
            }
            if (0 >= send(sockfd, DATA_MSG_BUF, DATA_MSG_SIZE, 0)) {
                perror("sensor: send (DATA)");
                goto _done;
            }

            close(sockfd);
            break;
        }

_done:  
        close(sockfd);

    }

    return 0;
}

/*
 * argv should contain: 
 *  uint32 id, 
 *  char *data, 
 *  uint32 distance,
 *  uint32 listening_port, 
 *  char *logger_name, 
 *  uint32 logger_port
 */
int main(int argc, char **argv) {

    char logger_hostname[HOSTNAME_SIZE];
    char logger_port[PORT_SIZE];

    if (!_init_args(argc, argv)) {
        fprintf(stderr, USAGE);
        exit(EXIT_FAILURE);
    }
    else {
        strcpy(logger_hostname, argv[5]);
        strcpy(logger_port, argv[6]);
        get_random_position(&my_position);
        printf("Node %d connecting to logger host %s, port %s with port %s. "
               "Using payload \"%s\", distance %d.\n",
               ID, logger_hostname, logger_port, LISTENING_PORT, DATA, DISTANCE);
        printf("Using position (%d, %d)\n", my_position.x, my_position.y);
    }

    if (-1 == connect_loop(logger_hostname, logger_port)) {
        fprintf(stderr, "Failed in connect loop.  Exiting.");
        exit(1);
    }

    printf("Successfully transmitted to base station.\n");

    exit(0);
}

