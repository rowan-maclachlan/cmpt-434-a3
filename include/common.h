/* 
 * Rowan MacLachlan
 * rdm695 11165820
 * CMPT 434 Eager
 * assignment 3
 * Monday March 18th 2019
 */

#pragma once

#include <sys/types.h>
#include <netinet/in.h>
#include <stdbool.h>

#include "messages.h" 

#define PORT_SIZE 16 
#define HOSTNAME_SIZE 64
#define MAX_SENSORS 64

struct sensor {
    int id;
    position p;
    char payload[PAYLOAD_SIZE];
    char port[PORT_SIZE];
    struct sockaddr host;
    enum received rcvd;
};

/* 
 * Copy the data arguments into the sensor struct (must be allocated)
 */
void save_sensor(struct sensor *sensor, 
                 struct sockaddr *their_addr,
                 int their_port, 
                 int their_id, 
                 position their_position);

int client_connect_to(char *host, char *port, struct addrinfo *dest);

char *get_ip_str(const struct sockaddr *sa, char *s, size_t maxlen);

void *get_in_addr(struct sockaddr *sa);
