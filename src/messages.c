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
