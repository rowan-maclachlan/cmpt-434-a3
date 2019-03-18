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
    int loggerfd;

    while(1) {
        sleep(1);

        move(DISTANCE, get_random_direction(), &me.p);

        // open connection
        if (-1 == (loggerfd = client_connect_to(logger_hostname, logger_port))) {
            fprintf(stderr, "sensor %d: failed to connect to logger.\n", me.id);
            fprintf(stderr, "*** FATAL ***\n");
            return -1;
        }

        if (-1 == send_id_msg(loggerfd, &me)) {
            goto _done;
        }

        unsigned int conf = 0;
        if (0 > recv_conf_msg(loggerfd, &conf)) {
            goto _done;
        }

        // if in range, send data msg
        // TODO we should send ALL messages we are carrying
        if (1 == conf) {
            if (0 > send_data_msg(loggerfd, &me)) {
                goto _done;
            }

            // Successfully delivered messages.  Quit
            close(loggerfd);
            break;
        }

_done:
        close(loggerfd);
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

