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
#include <time.h>

#include "common.h"
#include "distance.h"
#include "messages.h"

#define USAGE "./logger <port_number> <T> <number of sensors>"

/* A global value to hold the tranmission range of the objects, defined at
 * runtime */
unsigned int TRANS_RANGE = 0;
/* Define the position of the base station */
static position BASE_STATION = { BASE_STATION_X, BASE_STATION_Y };
/* Track which sensors have delivered their payloads */
static struct sensor sensors[MAX_SENSORS] = { 0 };

bool _init_args(int argc, 
                char **argv, 
                unsigned int *num_nodes, 
                char *port) {
    if (argc != 4) {
        fprintf(stderr, "Not enough arguments.\n");
        return false;
    }

    strcpy(port, argv[1]);
    if (0 >= sscanf(argv[2], "%u", &TRANS_RANGE)) {
        fprintf(stderr, "Invalid transmission range.\n");
        return false;
    }
    if (0 >= sscanf(argv[3], "%u", num_nodes) || MAX_SENSORS < *num_nodes) {
        fprintf(stderr, "Invalid number of nodes: set 0 < %u < %u.\n", *num_nodes, MAX_SENSORS);
        return false;
    }
    
    return true;
}

int listen_loop(int sockfd, int num_nodes) {
    char s[INET6_ADDRSTRLEN];
    struct sockaddr_storage their_addr;
    socklen_t sin_size = 0;
    int sensorfd = 0;

    char ID_MSG_BUF[ID_MSG_SIZE] = { 0 };
    char CONF_MSG_BUF[CONF_MSG_SIZE] = { 0 };
    char DATA_MSG_BUF[DATA_MSG_SIZE] = { 0 };
    char CONTACT_MSG_BUF[CONTACT_MSG_SIZE] = { 0 };

    if (listen(sockfd, MAX_SENSORS) == -1) {
        perror("listen");
        return -1;
    }

    printf("logger: waiting for connections...\n");

    while(num_nodes > 0) {
        sin_size = sizeof their_addr;
        sensorfd = accept(sockfd, (struct sockaddr *)&their_addr, &sin_size);
        if (sensorfd == -1) {
            perror("accept");
            continue;
        }

        void * in_addr = get_in_addr((struct sockaddr *)&their_addr);
        inet_ntop(their_addr.ss_family, in_addr, s, sizeof s);
        printf("logger: got connection from %s on socket %d\n", s, sensorfd);

        // listen for id msg
        if (0 >= recv(sensorfd, (void*)ID_MSG_BUF, ID_MSG_SIZE, 0)) {
            perror("logger: recv (ID_MSG)");
            goto _done;
        }

        log_id_msg(ID_MSG_BUF, "logger");
        
        int their_id = 0;
        int their_port = 0;
        position their_position = { 0, 0 };
        deserialize_id_msg(ID_MSG_BUF, &their_id, &their_port, &their_position);

        save_sensor(&(sensors[their_id]),
                    (struct sockaddr *)&their_addr, 
                    their_port, their_id, their_position);
        
        // send confirmation message
        bool conf = false;
        if (in_range(TRANS_RANGE, &their_position, &BASE_STATION)) {
            conf = true;
        }
        if (0 >= serialize_conf_msg(CONF_MSG_BUF, conf)) {
            fprintf(stderr, "logger: failed to serialize confirmation message.\n");
            goto _done;
        }
        if (0 >= send(sensorfd, CONF_MSG_BUF, CONF_MSG_SIZE, 0)) {
            perror("logger: send (CONF_MSG)");
            goto _done;
        }

        // if in range, recv data msg
        char PAYLOAD[PAYLOAD_SIZE] = { 0 };
        if (conf) {
            if (0 >= recv(sensorfd, DATA_MSG_BUF, DATA_MSG_SIZE, 0)) {
                perror("logger: recv (DATA_MSG)");
                goto _done;
            }

            log_data_msg(DATA_MSG_BUF, "logger");

            if (0 != deserialize_data_msg(DATA_MSG_BUF, &their_id, PAYLOAD)) {
                perror("logger: failed to deserialize data message.\n");
                goto _done;
            }

            strncpy(sensors[their_id].payload, PAYLOAD, PAYLOAD_SIZE);

            if (sensors[their_id].rcvd) {
                printf("logger: received duplicate payload for sensor %d.\n", their_id);
            }
            else {
                sensors[their_id].rcvd = true;
                num_nodes--;
            }
        }
        // if not in range, send alternate message and receive logs
        else {
            int closer = their_id;
            for (int i = 0; i < MAX_SENSORS; i++) {
                struct sensor s = sensors[i];
                if (s.active == INACTIVE) {
                    continue;
                }
                if (closest(&BASE_STATION, &s.p, &sensors[closer].p)
                  && in_range(TRANS_RANGE, &s.p, &sensors[closer].p)) {
                    closer = i;
                }
            }
            if (0 >= serialize_contact_msg(CONTACT_MSG_BUF, &sensors[closer])) {
                fprintf(stderr, "logger: failed to serialize confirmation message.\n");
                goto _done;
            }
            if (0 >= send(sensorfd, CONTACT_MSG_BUF, CONTACT_MSG_SIZE, 0)) {
                perror("logger: send (CONF_MSG)");
                goto _done;
            }

            char INFO_MSG_BUF[INFO_MSG_SIZE] = { 0 };
            // TODO receive as many logs as arrive (INFO messages)
            if (0 >= recv(sensorfd, INFO_MSG_BUF, INFO_MSG_SIZE, 0)) {
                perror("logger: recv (INFO)");
                goto _done;
            }

            log_info_msg(INFO_MSG_BUF);
        }

_done: 
        close(sensorfd);
    }

    return 0;
}


int main(int argc, char **argv) {
    int sockfd;
    time_t start_time = 0;

    unsigned int num_nodes;
    char port[PORT_SIZE];

    if (!_init_args(argc, argv, &num_nodes, port)) {
        fprintf(stderr, USAGE);
        exit(1);
    }

    printf("logger process listening on port %s with transmission range %u and %u nodes\n",
           port, TRANS_RANGE, num_nodes);

    if (-1 == (sockfd = server_connect_with(port))) {
        fprintf(stderr, "server: Failed to setup.\n");
        fprintf(stderr, " *** FATAL *** \n");
        exit(1);
    }

    start_time = time(NULL);
    if (-1 == listen_loop(sockfd, num_nodes)) {
        fprintf(stderr, "logger: failed in listen loop.\n");
        fprintf(stderr, " *** FATAL *** \n");
        exit(1);
    }
    start_time = time(NULL) - start_time;
    printf("Simulation terminated in %ld seconds.\n", start_time);

    return 0;
}

