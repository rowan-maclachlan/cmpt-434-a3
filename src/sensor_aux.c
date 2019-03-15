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

bool init_args(int argc, char **argv, struct sensor *s, int *DISTANCE) {
    if (argc != 7) {
        return false;
    }

    if (0 >= sscanf(argv[1], "%d", &s->id)
            || (0 > s->id) || (MAX_SENSORS <= s->id)) {
        fprintf(stderr, "Invalid ID. ID must be 0 <= %d < %d.\n", s->id, MAX_SENSORS);
        return false;
    }

    if (NULL == (strcpy(s->payload, argv[2]))) {
        fprintf(stderr, "Invalid data \"%s\".\n", argv[2]);
        return false;
    }

    if (0 >= sscanf(argv[3], "%d", DISTANCE)) {
        fprintf(stderr, "Invalid distance.\n");
        return false;
    }

    strcpy(s->port, argv[4]);

    get_random_position(&s->p);

    return true;
}

int send_id_msg(int sockfd, struct sensor *s) {
    char ID_MSG_BUF[ID_MSG_SIZE] = { 0 };
    if (NULL == s) {
        fprintf(stderr, "sensor: invalid sensor.\n");
        return -1;
    }

    if (0 >= serialize_id_msg(ID_MSG_BUF, s->id, s->port, &s->p)) {
        fprintf(stderr, "sensor: failed to serialize ID message.\n");
        return -1;
    }
    // send id msg
    if (0 >= send(sockfd, ID_MSG_BUF, ID_MSG_SIZE, 0)) {
        perror("sensor: send (ID)");
        return -1;
    }

    return 0;
}

int send_data_msg(int sockfd, struct sensor *s) {
    char DATA_MSG_BUF[DATA_MSG_SIZE] = { 0 };
    if (NULL == s) {
        fprintf(stderr, "sensor: invalid sensor.\n");
        return -1;
    }

    if (0 >= serialize_data_msg(DATA_MSG_BUF, s->id, s->payload)) {
        fprintf(stderr, "sensor: failed to serialize data message.\n");
        return -1;
    }
    if (0 >= send(sockfd, DATA_MSG_BUF, DATA_MSG_SIZE, 0)) {
        perror("sensor: send (DATA)");
        return -1;
    }

    return 0;
}

int send_info_msg(int sockfd, int mine, int theirs, int orig) {
    char INFO_MSG_BUF[INFO_MSG_SIZE] = { 0 };

    if (0 >= serialize_info_msg(INFO_MSG_BUF, mine, theirs, orig)) {
        fprintf(stderr, "sensor: failed to serialize info message.\n");
        return -1;
    }
    if (0 >= send(sockfd, INFO_MSG_BUF, INFO_MSG_SIZE, 0)) {
        perror("sensor: send (INFO)");
        return -1;
    }

    return 0;
}


int recv_conf_msg(int sockfd, unsigned int *conf) {
    char CONF_MSG_BUF[CONF_MSG_SIZE] = { 0 };

    // listen for confirmation msg
    if (0 >= recv(sockfd, CONF_MSG_BUF, CONF_MSG_SIZE, 0)) {
        perror("sensor: recv (CONF)");
        return -1;
    }

    deserialize_conf_msg(CONF_MSG_BUF, conf);

    return 0;
}
