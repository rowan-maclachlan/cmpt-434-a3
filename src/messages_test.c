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
#include "common.h"

void serialize_contact_msg_test() {
    char buf[CONTACT_MSG_SIZE] = { 0 };
    struct sensor sensor;
    sensor.id = 123;
    sensor.p.x = 333;
    sensor.p.y = 333;
    strncpy(sensor.payload, "TESTTESTTEST", PAYLOAD_SIZE);
    strncpy(sensor.port, "32000", PORT_SIZE);
    sensor.host.sa_family = AF_INET6;
    sensor.rcvd = false;
    bzero(sensor.host.sa_data, 14);
    // { A, B, C, D, E, F, G, 0, 0, 0, 0, 0, 0, 0}
    for (int i = 0; i < 14; i++) {
        sensor.host.sa_data[i] = 0x41 + i;
    }
    
    // "(123/30/ABCDEFGHIJKLMNO/32000)"
    char expected[CONTACT_MSG_SIZE]
        = { 0x28, 0x31, 0x32, 0x33, 0x2f, 0x33, 0x30, 0x2f,
            0x41, 0x42, 0x43, 0x44, 0x45, 0x46, 0x47, 0x48, 
            0x49, 0x4a, 0x4b, 0x4c, 0x4d, 0x4e, 0x2f, 0x33, 
            0x32, 0x30, 0x30, 0x30, 0x29 };

    /* ID, IP, port number - (%d/%u/%s/%d) */
    assert(29 == serialize_contact_msg(buf, &sensor));
    assert(0 == bcmp(buf, expected, 29));

    sensor.host.sa_data[0] = 0x0;
    sensor.host.sa_data[5] = 0x0;
    expected[8] = 0x0;
    expected[13] = 0x0;

    assert(29 == serialize_contact_msg(buf, &sensor));
    assert(0 == bcmp(buf, expected, 29));

}

void deserialize_contact_msg_test() {
    char buf[CONTACT_MSG_SIZE] 
        = { 0x28, 0x31, 0x32, 0x33, 0x2f, 0x33, 0x30, 0x2f,
            0x00, 0x42, 0x43, 0x44, 0x00, 0x46, 0x47, 0x48, 
            0x49, 0x4a, 0x4b, 0x4c, 0x4d, 0x4e, 0x2f, 0x33,
            0x32, 0x30, 0x30, 0x30, 0x29 };
    struct sensor sensor = { 0 };
    char data[sizeof(sensor.host.sa_data)]
        = { 0x00, 0x42, 0x43, 0x44, 0x00, 0x46, 0x47, 
            0x48, 0x49, 0x4a, 0x4b, 0x4c, 0x4d, 0x4e};
    /* ID, IP, port number */
    assert(0 == deserialize_contact_msg(buf, &sensor));

    assert(sensor.id == 123);
    assert(sensor.host.sa_family == AF_INET6);
    assert(0 == memcmp(sensor.host.sa_data, data, sizeof(sensor.host.sa_data)));
    assert(0 == strcmp(sensor.port, "32000"));
}

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

void serialize_confirmation_msg_test() {
    bool conf = true;
    char buf[CONF_MSG_SIZE];
    char *expected = "1";

    assert(1 == serialize_conf_msg(buf, conf));
    assert(0 == strcmp(buf, expected)); 

    conf = false;
    expected = "0";
    assert(1 == serialize_conf_msg(buf, conf));
    assert(0 == strcmp(buf, expected)); 
}

void deserialize_confirmation_msg_test() {
    bool conf = true;
    char *buf = "1";

    assert(0 == deserialize_conf_msg(buf, &conf));
    assert(conf); 

    buf = "0";
    conf = false;

    assert(0 == deserialize_conf_msg(buf, &conf));
    assert(!conf); 
}

void serialize_data_msg_test() {
    char buf[DATA_MSG_SIZE];
    char *expected = "123/the_payload";
    int id = 123;
    char *payload = "the_payload";

    assert(15 == serialize_data_msg(buf, id, payload));
    assert(0 == strcmp(buf, expected)); 
}

void deserialize_data_msg_test() {
    char payload[PAYLOAD_SIZE];
    int id = 0;
    char *buf = "123/the_payload";

    assert(0 == deserialize_data_msg(buf, &id, payload));
    assert(0 == strcmp(payload, "the_payload")); 
    assert(id == 123); 
}

int main(void) {

    serialize_contact_msg_test();

    deserialize_contact_msg_test();

    serialize_identity_msg_test();

    deserialize_identity_msg_test();

    serialize_confirmation_msg_test();

    deserialize_confirmation_msg_test();

    serialize_data_msg_test();

    deserialize_data_msg_test();

    printf("All messages tests passed!\n");

    return 0;
}
