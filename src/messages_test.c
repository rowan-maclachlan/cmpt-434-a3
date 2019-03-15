/*
 * Rowan MacLachlan
 * rdm695 11165820
 * CMPT 434 Eager
 * assignment 3
 * Monday March 18th 2019
 */

#include <assert.h>
#include <stdio.h>
#include <string.h>

#include "distance.h"
#include "messages.h"
#include "common.h"

void test() {
    int id, ip_type, port;
    char ip[INET6_ADDRSTRLEN] = { 0 };
    char buf[CONTACT_MSG_SIZE]
        = "(123/2/ffe80::85f:61cc:9965:58fb/32000)";

    /* ID, IP, port number */
    assert (4 == sscanf(buf, "(%d/%d/%[.:abcdef0-9]/%d)", &id, &ip_type, ip, &port));
    assert (0 == strcmp(ip, "ffe80::85f:61cc:9965:58fb"));
    assert (port == 32000);

    bzero(buf, CONTACT_MSG_SIZE);
    strcpy(buf, "(123/2/192.168.12.1/32000)");
    assert (4 == sscanf(buf, "(%d/%d/%[.:abcdef0-9]/%d)", &id, &ip_type, ip, &port));

}

void serialize_info_msg_test() {
    char buf[INFO_MSG_SIZE] = { 0 };
    int sender = 1;
    int receiver = 123;
    int original = 12;
    char *expected = "(1/123/12)";

    assert(10 == serialize_info_msg(buf, sender, receiver, original));
    assert(0 == strcmp(buf, expected));
}

void deserialize_info_msg_test() {
    char buf[INFO_MSG_SIZE] = "(1/123/12)";
    int sender = 0;
    int receiver = 0;
    int original = 0;

    assert(0 == deserialize_info_msg(buf, &sender, &receiver, &original));
    assert(sender == 1);
    assert(receiver == 123);
    assert(original == 12);
}

void serialize_contact_msg_test() {
    char buf[CONTACT_MSG_SIZE] = { 0 };
    struct sensor sensor;
    sensor.id = 123;
    sensor.p.x = 333;
    sensor.p.y = 333;
    strncpy(sensor.payload, "TESTTESTTEST", PAYLOAD_SIZE);
    strncpy(sensor.port, "32000", PORT_SIZE);
    sensor.ip_type = IPV6;
    sensor.rcvd = false;
    bzero(sensor.ip, INET6_ADDRSTRLEN);
    // { A, B, C, D, E, F, G, 0, 0, 0, 0, 0, 0, 0}
    for (int i = 0; i < INET6_ADDRSTRLEN - 1; i++) {
        // 46...
        sensor.ip[i] = 0x41 + i;
    }

    // "(123/2/ABCDEFGHIJKLMNOPQRSTUV.../32000)"
    char expected[CONTACT_MSG_SIZE]
        = { 0x28, 0x31, 0x32, 0x33, 0x2f, 0x32, 0x2f,

            0x41, 0x42, 0x43, 0x44, 0x45, 0x46, 0x47,
            0x48, 0x49, 0x4a, 0x4b, 0x4c, 0x4d, 0x4e,
            0x4f, 0x50, 0x51, 0x52, 0x53, 0x54, 0x55,
            0x56, 0x57, 0x58, 0x59, 0x5a, 0x5b, 0x5c,
            0x5d, 0x5e, 0x5f, 0x60, 0x61, 0x62, 0x63,
            0x64, 0x65, 0x66, 0x67, 0x68, 0x69, 0x6a,
            0x6b, 0x6c, 0x6d,

            0x2f, 0x33, 0x32, 0x30, 0x30, 0x30, 0x29 };

    /* ID, IP, port number - (%d/%d/%s/%d) */
    assert(59 == serialize_contact_msg(buf, &sensor));
    assert(0 == bcmp(buf, expected, 59));
}

void deserialize_contact_msg_test() {
    char buf[CONTACT_MSG_SIZE]
        = "(123/2/fe80::85f:61cc:9965:58fb/32000)";
//        = { 0x28, 0x31, 0x32, 0x33, 0x2f, 0x32, 0x2f,
//            0x41, 0x42, 0x43, 0x44, 0x45, 0x46, 0x47,
//            0x48, 0x49, 0x4a, 0x4b, 0x4c, 0x4d, 0x4e,
//            0x4f, 0x50, 0x51, 0x52, 0x53, 0x54, 0x55,
//            0x56, 0x57, 0x58, 0x59, 0x5a, 0x5b, 0x5c,
//            0x5d, 0x5e, 0x5f, 0x60, 0x61, 0x62, 0x63,
//            0x64, 0x65, 0x66, 0x67, 0x68, 0x69, 0x6a,
//            0x6b, 0x6c, 0x6d,
//            0x2f, 0x33, 0x32, 0x30, 0x30, 0x30, 0x29 };
    struct sensor sensor = { 0 };
    char ip[INET6_ADDRSTRLEN] = "fe80::85f:61cc:9965:58fb";
    /* ID, IP, port number */
    assert(0 == deserialize_contact_msg(buf, &sensor));

    assert(sensor.id == 123);
    assert(sensor.ip_type == IPV6);
    assert(0 == memcmp(sensor.ip, ip, INET6_ADDRSTRLEN));
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
    unsigned int conf = 1;
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
    unsigned int conf = 1;
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

    test();

    serialize_info_msg_test();

    deserialize_info_msg_test();

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
