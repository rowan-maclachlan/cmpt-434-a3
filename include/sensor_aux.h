/*
 * Rowan MacLachlan
 * rdm695 11165820
 * CMPT 434 Eager
 * assignment 3
 * Monday March 18th 2019
 */

#pragma once

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

bool init_args(int argc, char **argv, struct sensor *s, int *DISTANCE);

int send_info_msg(int sockfd, int mine, int theirs, int orig);

int send_data_msg(int sockfd, struct sensor *s);

int send_id_msg(int sockfd, struct sensor *s);

int recv_conf_msg(int sockfd, unsigned int *conf);
