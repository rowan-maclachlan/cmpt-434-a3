/* 
 * Rowan MacLachlan
 * rdm695 11165820
 * CMPT 434 Eager
 * assignment 3
 * Monday March 18th 2019
 */

#include <string.h>
#include <stdio.h>

#include "distance.h"
#include "messages.h"

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
        fprintf(stderr, "sscanf failed to scan input: \"%s\"\n", buf);
        return -1;
    }

    return 0;
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
int serialize_conf_msg(char *buf, enum confirmation conf) {
    if (NULL == buf) {
        fprintf(stderr, "input buffer cannot be NULL.\n");
        return -1;
    }

    return sprintf(buf, "%d", conf);
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
int deserialize_conf_msg(char *buf, enum confirmation *conf) {
    if (NULL == buf) {
        fprintf(stderr, "Output buffer cannot be NULL.\n");
        return -1;
    }
    if (NULL == conf) {
        fprintf(stderr, "Output confirmation value cannot be NULL.\n");
        return -1;
    }

    if (1 != sscanf(buf, "%d", conf)) {
        fprintf(stderr, "sscanf failed to scan input: \"%s\"\n", buf);
        return -1;
    }

   return 0;
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
        fprintf(stderr, "sscanf failed to scan input: \"%s\"\n", buf);
        return -1;
    }

    return 0;
}
