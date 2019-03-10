/* 
 * Rowan MacLachlan
 * rdm695 11165820
 * CMPT 434 Eager
 * assignment 3
 * Monday March 18th 2019
 */

#include "distance.h"

#define ID_MSG_SIZE 64
#define CONF_MSG_SIZE 64
#define DATA_MSG_SIZE 64
#define PAYLOAD_SIZE 32

enum confirmation {
    IN_RANGE,
    OUT_OF_RANGE
};

int serialize_id_msg(char *buf, int id, char *port_num, position *p);

int deserialize_id_msg(char *buf, int *id, int *port_num, position *p);

int serialize_conf_msg(char *buf, enum confirmation conf);

int deserialize_conf_msg(char *buf, enum confirmation *conf);

int serialize_data_msg(char *buf, int id, char *payload);

int deserialize_data_msg(char *buf, int *id, char *payload);
