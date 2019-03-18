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
#include <pthread.h>

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
/* thread listening for connections */
pthread_t listening_thread;
/* thread data */
struct thread_data { 
    int id;
    char port[PORT_SIZE];
    int ret;
} thread_data;

void *listen_loop(void *arg) {
    char s[INET6_ADDRSTRLEN];
    struct sockaddr_storage their_addr;
    socklen_t sin_size = 0;
    struct thread_data *this = (struct thread_data *)arg;
    char DATA_MSG_BUF[DATA_MSG_SIZE] = { 0 };
    int myfd, sensorfd = 0;
    char PAYLOAD[PAYLOAD_SIZE] = { 0 };
    int their_id = 0;
    int cancel_type = 0;
    /* Initialize return value to success */
    this->ret = 0;

    pthread_setcanceltype(PTHREAD_CANCEL_DEFERRED, &cancel_type);
    // open up for connections with other sensors
    if (-1 == (myfd = server_connect_with(this->port))) {
        fprintf(stderr, "sensor %d: failed to listen for incoming connections.\n", this->id);
        this->ret = -1;
        pthread_exit(NULL);
    }
    
    printf("sensor %d listening on port %s\n", this->id, this->port);

    if (listen(myfd, MAX_SENSORS) == -1) {
        perror("sensor: listen");
        this->ret = -1;
        pthread_exit(NULL);
    }

    printf("sensor: waiting for connections...\n");

    while(1) {
        pthread_setcancelstate(PTHREAD_CANCEL_DISABLE, &cancel_type);
        sensorfd = accept(myfd, (struct sockaddr *)&their_addr, &sin_size);
        if (sensorfd == -1) {
            perror("accept");
            continue;
        }

        void * in_addr = get_in_addr((struct sockaddr *)&their_addr);
        inet_ntop(their_addr.ss_family, in_addr, s, sizeof s);
        printf("sensor: got connection from %s on socket %d\n", s, sensorfd);

        // listen for data msg
        // TODO we should listen for MANY data
        if (0 >= recv(sensorfd, (void*)DATA_MSG_BUF, DATA_MSG_SIZE, 0)) {
            perror("sensor: recv (DATA_MSG)");
            continue;
        }

       if (0 != deserialize_data_msg(DATA_MSG_BUF, &their_id, PAYLOAD)) {
           perror("sensor: failed to deserialize data message.\n");
           continue;
       }

       log_data_msg("sensor", DATA_MSG_BUF);
       strncpy(sensors[their_id].payload, PAYLOAD, PAYLOAD_SIZE);
       sensors[their_id].rcvd = true;
       pthread_setcancelstate(PTHREAD_CANCEL_DISABLE, &cancel_type);
    }

    pthread_exit(NULL);
}

int connect_loop(char *logger_hostname, char *logger_port) {
    int loggerfd, sensorfd;

    while(1) {
        sleep(1);

        move(DISTANCE, get_random_direction(), &me.p);

        thread_data.id = me.id;
        strncpy(thread_data.port, me.port, PORT_SIZE);
        thread_data.ret = 0;
        if (0 != pthread_create(&listening_thread, NULL, listen_loop, (void*)&thread_data)) {
            perror("sensor: pthread_create:");
            fprintf(stderr, "Error: sensor failed to create listening thread.\n");
            fprintf(stderr, "*** FATAL ***\n");
            return -1;
        }

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
        if (1 == conf) {
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
                fprintf(stderr, "Invalid loggerfd: %d\n", loggerfd);
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

    if (0 != pthread_cancel(listening_thread)) {
        perror("sensor: pthread_cancel");
    }
    if (0 != pthread_join(listening_thread, NULL)) {
        perror("sensor: pthread_join");
    }
    if (thread_data.ret != 0) {
        fprintf(stderr, "listening thread failed.\n");
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

