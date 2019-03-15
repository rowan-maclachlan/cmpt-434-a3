/* 
 * Rowan MacLachlan
 * rdm695 11165820
 * CMPT 434 Eager
 * assignment 3
 * Monday March 18th 2019
 */

#pragma once

#include <stdbool.h>
#include "distance.h"

#define INFO_MSG_SIZE 32
#define REQUEST_MSG_SIZE 64
#define CONTACT_MSG_SIZE 128
#define ID_MSG_SIZE 64
#define CONF_MSG_SIZE 64
#define DATA_MSG_SIZE 64
#define PAYLOAD_SIZE 32

struct sensor;

int serialize_info_msg(char *buf, int sender, int receiver, int original);

int deserialize_info_msg(char *buf, int *sender, int *receiver, int *original);

void log_info_msg(char *buf);

int serialize_request_msg(char *buf, bool req);

int deserialize_request_msg(char *buf, bool *req);

int serialize_contact_msg(char *buf, struct sensor *sensor);

int deserialize_contact_msg(char *buf, struct sensor *sensor);

int serialize_id_msg(char *buf, int id, char *port_num, position *p);

int deserialize_id_msg(char *buf, int *id, int *port_num, position *p);

void log_id_msg(char *buf, char *src);

int serialize_conf_msg(char *buf, bool conf);

int deserialize_conf_msg(char *buf, bool *conf);

void log_conf_msg(char *buf, char *src);

int serialize_data_msg(char *buf, int id, char *payload);

int deserialize_data_msg(char *buf, int *id, char *payload);

void log_data_msg(char *buf, char *src);
