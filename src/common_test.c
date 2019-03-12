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
    struct sockaddr addr;
    int port = 123;
    int id = 321;
    position p = { 987, 789 };
    addr.sa_family = AF_INET;
    addr.sa_data[0] = 1;

    save_sensor(&sensor, &addr, port, id, p);

    assert(0 == strcmp(sensor.port, "123"));
    assert(sensor.id == id);
    assert(sensor.p.x = 987);
    assert(sensor.p.y = 789);
    assert(sensor.host.sa_family == AF_INET);
    assert(sensor.host.sa_data[0] == 1);
    assert(sensor.rcvd == RECEIVED);

}

int main(void) {

    save_sensor_tests();

    printf("All common tests passed!\n");

    return 0;
}
