/* 
 * Rowan MacLachlan
 * rdm695 11165820
 * CMPT 434 Eager
 * assignment 3
 * Monday March 18th 2019
 */

#pragma once

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <stdbool.h>

#include "messages.h" 
#include "distance.h"

#define PORT_SIZE 16 
#define HOSTNAME_SIZE 64
#define MAX_SENSORS 64

enum ip_type {
    IPV4 = 1,
    IPV6 = 2
};

struct sensor {
    int id;
    position p;
    char payload[PAYLOAD_SIZE];
    char port[PORT_SIZE]; // What port does this sensor listen on?
    enum ip_type ip_type;      // What is the type of the IP address?
    char ip[INET6_ADDRSTRLEN]; // What is this sensors's network address?
    bool rcvd;   // Has this sensors's message been received?
    bool active;   // Is this sensor active in the simulation?
};

void print_bytes(char *buf, int len);

void print_string(char *buf, int len);

enum ip_type get_ip_type(sa_family_t sa_ft);

/* 
 * Copy the data arguments into the sensor struct (must be allocated)
 */
void save_sensor(struct sensor *sensor, 
                 enum ip_type,
                 char *ip,
                 int their_port, 
                 int their_id, 
                 position their_position);

void get_hostname_from(char *their_ip, struct sockaddr *host);

int server_connect_with(char *port);

int client_connect_to(char *host, char *port);

char *get_ip_str(const struct sockaddr *sa, char *s, size_t maxlen);

void *get_in_addr(struct sockaddr *sa);
