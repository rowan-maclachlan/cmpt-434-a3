/* 
 * Rowan MacLachlan
 * rdm695 11165820
 * CMPT 434 Eager
 * assignment 3
 * Monday March 18th 2019
 */

#include <string.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <inttypes.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <time.h>
#include <stdbool.h>

#include <arpa/inet.h>

#include "distance.h"
#include "messages.h"
#include "common.h"

int serialize_info_msg(char *buf, int sender, int receiver, int original) {
    if (NULL == buf) {
        fprintf(stderr, "input buffer cannot be NULL.\n");
        return -1;
    }

    return sprintf(buf, "(%d/%d/%d)", sender, receiver, original);
}

int deserialize_info_msg(char *buf, int *sender, int *receiver, int *original) {
    if (NULL == buf) {
        fprintf(stderr, "input buffer cannot be NULL.\n");
        return -1;
    }
    if (NULL == sender) {
        fprintf(stderr, "Listening port number cannot be NULL.\n");
        return -1;
    }
    if (NULL == receiver) {
        fprintf(stderr, "Sensor position cannot be NULL.\n");
        return -1;
    }
    if (NULL == original) {
        fprintf(stderr, "Sensor position cannot be NULL.\n");
        return -1;
    }

    if (3 != sscanf(buf, "(%d/%d/%d)", sender, receiver, original)) {
        perror("sscanf");
        fprintf(stderr, "sscanf failed to scan input: \"%s\"\n", buf);
        return -1;
    }

    return 0;
}

void log_info_msg(char *buf) {
    int sender, receiver, original = 0;

    deserialize_info_msg(buf, &sender, &receiver, &original);

    printf("Sensor node %d passed sensor %d's payload to sensor %d.\n",
           sender, receiver, original);
}

int serialize_request_msg(char *buf, bool req) {
    if (NULL == buf) {
        fprintf(stderr, "input buffer cannot be NULL.\n");
        return -1;
    }

    return sprintf(buf, "%d", req);
}

int deserialize_request_msg(char *buf, bool *req) {
    if (NULL == buf) {
        fprintf(stderr, "Output buffer cannot be NULL.\n");
        return -1;
    }
    if (NULL == req) {
        fprintf(stderr, "Output request cannot be NULL.\n");
        return -1;
    }

    if (1 != sscanf(buf, "%d", (unsigned int*)req)) {
        perror("sscanf");
        fprintf(stderr, "sscanf failed to scan input: \"%s\"\n", buf);
        return -1;
    }

    return 0;
}

/* ID, IP, port number */
int serialize_contact_msg(char *buf, struct sensor *sensor) {
    int bytes = 0;

    if (NULL == buf) {
        fprintf(stderr, "input buffer cannot be NULL.\n");
        return -1;
    }
    if (NULL == sensor) {
        fprintf(stderr, "input sensor cannot be NULL.\n");
        return -1;
    }
    
    bytes += sprintf(buf, "(%d/%u/",
            sensor->id, sensor->host.sa_family);
    memcpy(&buf[bytes], sensor->host.sa_data, sizeof(sensor->host.sa_data));
    bytes += sizeof(sensor->host.sa_data);
    bytes += sprintf(&buf[bytes], "/%s)", sensor->port);

    return bytes;
}

/* ID, IP, port number */
int deserialize_contact_msg(char *buf, struct sensor *sensor) {
    int bytes = 0;
    int port = 0;
    if (NULL == buf) {
        fprintf(stderr, "Output buffer cannot be NULL.\n");
        return -1;
    }
    if (NULL == sensor) {
        fprintf(stderr, "Output sensor cannot be NULL.\n");
        return -1;
    }

    if (2 != sscanf(buf, "(%d/%u/", &sensor->id, &sensor->host.sa_family)) {
        perror("sscanf");
        fprintf(stderr, "sscanf failed to scan input: \"%s\"\n", buf);
        return -1;
    }

    // copy sa_data byte buffer to sensor field
    bytes = strlen(buf);
    memcpy(sensor->host.sa_data, &buf[bytes], sizeof(sensor->host.sa_data));
    bytes += sizeof(sensor->host.sa_data);

    // scan port and print into sensor field
    if (1 != sscanf(&buf[bytes], "/%d)", &port)) {
        perror("sscanf");
        fprintf(stderr, "sscanf failed to scan input: \"%s\"\n", buf);
        return -1;
    }
    sprintf(sensor->port, "%d", port);

    return 0;
}
/*
 * Serialize the identify message.
 * This message is sent by the sensor node to the logger process once a second
 * to initiate contact and determine following steps.
 *
 * identify = { ID, listening_port_number, (x,y) }
 *
 * returns a serialize form of the message.
 */
int serialize_id_msg(char *buf, int id, char *port_num, position *p) {
    if (NULL == buf) {
        fprintf(stderr, "input buffer cannot be NULL.\n");
        return -1;
    }
    if (NULL == port_num) {
        fprintf(stderr, "Listening port number cannot be NULL.\n");
        return -1;
    }
    if (NULL == p) {
        fprintf(stderr, "Sensor position cannot be NULL.\n");
        return -1;
    }

    return sprintf(buf, "%d/%s/(%d,%d)",
            id, port_num, p->x, p->y);
}

/*
 * Deserialize the identify message.
 * This message is received by the logging process from the sensor node at the
 * beginning of every tick, to initiate contact. 
 *
 * identify = { ID, listening_port_number, (x,y) }
 *          = "123/32001/(50,50)"
 *
 * returns the deserialized components of the message.
 */
int deserialize_id_msg(char *buf, int *id, int *port_num, position *p) {
    if (NULL == buf) {
        fprintf(stderr, "Output buffer cannot be NULL.\n");
        return -1;
    }
    if (NULL == id) {
        fprintf(stderr, "Output ID cannot be NULL.\n");
        return -1;
    }
    if (NULL == port_num) {
        fprintf(stderr, "Output listening port number cannot be NULL.\n");
        return -1;
    }
    if (NULL == p) {
        fprintf(stderr, "Output sensor position cannot be NULL.\n");
        return -1;
    }

    if (4 != sscanf(buf, "%d/%d/(%d,%d)", 
                id, port_num, &p->x, &p->y)) {
        perror("sscanf");
        fprintf(stderr, "sscanf failed to scan input: \"%s\"\n", buf);
        return -1;
    }

    return 0;
}

/*
 * Log a message.
 */
void log_id_msg(char *buf, char *src) {
    printf("%s: received ID message \"%s\"\n", src, buf);
}

/*
 * Serialize the in range confirmation message.
 * This message is received by the sensor node from the logger to inform it
 * whether it is in range or not.
 *
 * confirmation = { in_range/out_of_range }
 *
 * returns a serialize form of the message.
 */
int serialize_conf_msg(char *buf, bool conf) {
    if (NULL == buf) {
        fprintf(stderr, "input buffer cannot be NULL.\n");
        return -1;
    }

    return sprintf(buf, "%d", (unsigned int)conf);
}

/*
 * Deserialize the range confirmation message.
 * This message is received by the sensor node from the logger to inform it
 * whether it is in range or not.
 *
 * confirmation = { [in_range|out_of_range] }
 *
 * returns the deserialized components of the message.
 */
int deserialize_conf_msg(char *buf, bool *conf) {
    if (NULL == buf) {
        fprintf(stderr, "Output buffer cannot be NULL.\n");
        return -1;
    }
    if (NULL == conf) {
        fprintf(stderr, "Output confirmation value cannot be NULL.\n");
        return -1;
    }

    if (1 != sscanf(buf, "%d", (unsigned int*)conf)) {
        perror("sscanf");
        fprintf(stderr, "sscanf failed to scan input: \"%s\"\n", buf);
        return -1;
    }

   return 0;
}

/*
 * Log a message.
 */
void log_conf_msg(char *buf, char *src) {
    printf("%s: received confirmation message \"%s\"\n", src, buf);
}

/*
 * Serialize the payload message.
 * This message is received by the logger node from the sensor node to
 * communicate its payload
 *
 * data = { id/payload }
 *
 * returns a serialize form of the message.
 */
int serialize_data_msg(char *buf, int id, char *payload) {
    if (NULL == buf) {
        fprintf(stderr, "input buffer cannot be NULL.\n");
        return -1;
    }
    if (NULL == payload) {
        fprintf(stderr, "input payload buffer cannot be NULL.\n");
        return -1;
    }

    return sprintf(buf, "%d/%s", id, payload);
}

/*
 * Serialize the payload message.
 * This message is received by the logger node from the sensor node to
 * communicate its payload
 *
 * data = { id/payload }
 *
 * returns a serialize form of the message.
 */
int deserialize_data_msg(char *buf, int *id, char *payload) {
    if (NULL == buf) {
        fprintf(stderr, "Output buffer cannot be NULL.\n");
        return -1;
    }
    if (NULL == id) {
        fprintf(stderr, "Output id value cannot be NULL.\n");
        return -1;
    }
    if (NULL == payload) {
        fprintf(stderr, "Output payload value cannot be NULL.\n");
        return -1;
    }

    if (2 != sscanf(buf, "%d/%s", id, payload)) {
        perror("sscanf");
        fprintf(stderr, "sscanf failed to scan input: \"%s\"\n", buf);
        return -1;
    }

    return 0;
}

/*
 * Log a message.
 */
void log_data_msg(char *buf, char *src) {
    printf("%s: received data message \"%s\"\n", src, buf);
}
