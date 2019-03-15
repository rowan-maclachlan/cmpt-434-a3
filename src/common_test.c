/* 
 * Rowan MacLachlan
 * rdm695 11165820
 * CMPT 434 Eager
 * assignment 3
 * Monday March 18th 2019
 */

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <netdb.h>
#include <time.h>

#include "common.h"
#include "distance.h"
#include "messages.h"

void save_sensor_tests() {
    struct sensor sensor;
    enum ip_type ip_type = IPV4;
    char ip[INET6_ADDRSTRLEN] = "::ffff:172.16.1.76";
    int port = 123;
    int id = 321;
    position p = { 987, 789 };

    save_sensor(&sensor, ip_type, ip, port, id, p);

    assert(0 == strcmp(sensor.port, "123"));
    assert(sensor.id == id);
    assert(sensor.p.x = 987);
    assert(sensor.p.y = 789);
    assert(sensor.ip_type == IPV4);
    assert(0 == strcmp(sensor.ip, ip));
}

int main(void) {

    save_sensor_tests();

    printf("All common tests passed!\n");

    return 0;
}
