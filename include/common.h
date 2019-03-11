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

#define PORT_SIZE 16 
#define HOSTNAME_SIZE 64
#define MAX_SENSORS 64

char *get_ip_str(const struct sockaddr *sa, char *s, size_t maxlen);

void *get_in_addr(struct sockaddr *sa);
