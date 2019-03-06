/* 
 * Rowan MacLachlan
 * rdm695 11165820
 * CMPT 434 Eager
 * assignment 3
 * Monday March 18th 2019
 */

#include <assert.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>

#include "distance.h"
#include "messages.h"

void serialize_identity_msg_test() {
    int id = 321;
    char *port_num = "32001";
    position p = { 50, 50 };
    char buf[ID_MSG_SIZE];
    char *expected = "321/32001/(50,50)";

    assert(17 == serialize_id_msg(buf, id, port_num, &p));

    assert(0 == strcmp(buf, expected)); 
}

void deserialize_identity_msg_test() {
    int id = 0;
    int port_num = 0;
    position p = { 0, 0 };
    char *buf = "321/32001/(50,50)";

    assert(0 == deserialize_id_msg(buf, &id, &port_num, &p));

    assert(id == 321); 
    assert(port_num == 32001);
    assert(p.x == 50); 
    assert(p.y == 50); 
}

int main(void) {

    serialize_identity_msg_test();

    deserialize_identity_msg_test();

    printf("All messages tests passed!\n");

    return 0;
}
