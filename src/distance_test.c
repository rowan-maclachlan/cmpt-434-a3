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

#include "distance.h"

void closest_test() {
    position to = { 500, 500 };
    position a;
    position b;

    a.x = 100;
    a.y = 100;

    b.x = 99;
    b.y = 99;

    assert(closest(&to, &a, &b));

    b.x = 101;
    b.y = 101;

    assert(!closest(&to, &a, &b));

    // wrap around
    to.x = 900;
    to.y = 900;
    a.x = 10;
    a.y = 10;
    b.x = 700;
    b.y = 700;

    assert(closest(&to, &a, &b));
}

void in_range_test() {
    position p1;
    position p2;
    int T = 100;

    p1.x = 50;
    p1.y = 50;

    p2.x = 50;
    p2.y = 50;

    assert(in_range(T, &p1, &p2));

    p2.x = 500;
    p2.y = 500;

    assert(!in_range(T, &p1, &p2));

    p1.x = 400;
    p1.y = 400;

    assert(!in_range(T, &p1, &p2));

    p1.x = 1;
    p1.y = 1;
    p2.x = 999;
    p2.y = 999;

    assert(in_range(T, &p1, &p2));
}

void move_test() {
    position p;
    int D = 0;
    direction dir = NORTH;

    p.x = 50;
    p.y = 50;

    move(D, dir, &p);
    assert(p.x == 50);
    assert(p.y == 50);

    D = U_LEFT;

    move(D, dir, &p);
    assert(p.x == 50);
    assert(p.y == 50);

    dir = EAST;
    D = 500;

    move(D, dir, &p);
    assert(p.x == 550);
    assert(p.y == 50);

    dir = SOUTH;
    move(D, dir, &p);
    assert(p.x == 550);
    assert(p.y == 550);

}

int main(int argc, char *argv[]) {

    closest_test();

    in_range_test();

    move_test();

    printf("All tests passed!\n");

    return 0;
}

