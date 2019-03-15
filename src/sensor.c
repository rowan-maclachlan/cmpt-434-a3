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
#include "sensor_aux.h"

#define USAGE "./sensor_node <id> <payload> <D> <listening_port> <logger_hostname> <logger_port>\n"

/* How far do I travel every tick? */
int DISTANCE = 0;
/* Save data about me, the sensor node */
struct sensor me;
/* To keep track of alternate sensors */
static struct sensor sensors[MAX_SENSORS] = { 0 };
/* When do we timeout on select? */
struct timeval poll_val = { 1, 0 };

int connect_loop(char *logger_hostname, char *logger_port) {
    int myfd, loggerfd, sensorfd;
    // fd_set master_set;

    while(1) {
        sleep(1);

        move(DISTANCE, get_random_direction(), &me.p);

        // open up for connections with other sensors
        if (-1 == (myfd = server_connect_with(me.port))) {
            fprintf(stderr, "sensor %d: failed to listen for incoming connections.\n", me.id);
        }

        // open connection
        if (-1 == (loggerfd = client_connect_to(logger_hostname, logger_port))) {
            fprintf(stderr, "sensor %d: failed to connect to logger.\n", me.id);
            fprintf(stderr, "*** FATAL ***\n");
            return -1;
        }

        //FD_ZERO(&master_set);
        //FD_SET(myfd, &master_set);
        //FD_SET(loggerfd, &master_set);
        //int maxfd = loggerfd > myfd ? loggerfd : myfd;

        //if (-1 == select(maxfd+1, &master_set, NULL, NULL, &poll_val)) {
        //    perror("sensor: select\n");
        //    goto _done;
        //}

        if (-1 == send_id_msg(loggerfd, &me)) {
            goto _done;
        }

        bool conf = false;
        if (0 > recv_conf_msg(loggerfd, &conf)) {
            goto _done;
        }
        
        // if in range, send data msg
        // TODO we should send ALL messages we are carrying
        if (conf) {
            if (0 > send_data_msg(loggerfd, &me)) {
                goto _done;
            }

            // Successfully delivered messages.  Quit
            close(loggerfd);
            break;
        }
        else { // setup alternate contact
            struct sensor s;
            char CONTACT_MSG_BUF[CONTACT_MSG_SIZE] = { 0 };
            if (0 >= recv(loggerfd, CONTACT_MSG_BUF, CONTACT_MSG_SIZE, 0)) {
                perror("sensor: recv (CONTACT)");
                goto _done;
            }
            
            deserialize_contact_msg(CONTACT_MSG_BUF, &s);
            memcpy(&sensors[s.id], &s, sizeof(s));

            // don't connect if the contact is ourselves
            if (me.id == s.id) {
                goto _done;
            }
            
            // open tcp with s
            if(-1 == (sensorfd = client_connect_to(s.ip, s.port))) {
                fprintf(stderr, "sensor %d: Failed to open connection to sensor %d\n", me.id, s.id);
                // log communication to logger process
                if (0 > send_info_msg(loggerfd, me.id, me.id, me.id)) {
                    goto _done;
                }
                goto _done;
            }

            // transmit message to s
            // TODO this should be ALL the messages I am carrying
            if (0 > send_data_msg(sensorfd, &me)) {
                goto _done;
            }

            // log communication to logger process
            if (0 > send_info_msg(loggerfd, me.id, s.id, me.id)) {
                goto _done;
            }
        }

_done:  
        close(loggerfd);
        close(sensorfd);
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

    if (!init_args(argc, argv, &me, &DISTANCE)) {
        fprintf(stderr, USAGE);
        exit(EXIT_FAILURE);
    }
    else {
        strcpy(logger_hostname, argv[5]);
        strcpy(logger_port, argv[6]);
        printf("Node %d connecting to logger host %s, port %s with port %s. "
               "Using payload \"%s\", distance %d.\n",
               me.id, logger_hostname, logger_port, me.port, me.payload, DISTANCE);
        printf("Using position (%d, %d)\n", me.p.x, me.p.y);
    }

    if (-1 == connect_loop(logger_hostname, logger_port)) {
        fprintf(stderr, "Failed in connect loop.  Exiting.\n");
        exit(1);
    }

    printf("Successfully transmitted to base station.\n");

    exit(0);
}

