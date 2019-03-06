/* 
 * Rowan MacLachlan
 * rdm695 11165820
 * CMPT 434 Eager
 * assignment 3
 * Monday March 18th 2019
 */

#include "distance.h"

#define ID_MSG_SIZE 64

int serialize_id_msg(char *buf, int id, char *port_num, position *p);

int deserialize_id_msg(char *buf, int *id, int *port_num, position *p);
