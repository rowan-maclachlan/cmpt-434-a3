/* 
 * Rowan MacLachlan
 * rdm695 11165820
 * CMPT 434 Eager
 * assignment 3
 * Monday March 18th 2019
 */

#pragma once

#include <stdbool.h>

#define B_LEFT 0
#define B_RIGHT 1000
#define U_LEFT 1000
#define U_RIGHT 1000

#define BASE_STATION_X 500
#define BASE_STATION_Y 500

typedef struct position {
    int x;
    int y;
} position;

typedef enum direction {
    NORTH,
    EAST,
    SOUTH,
    WEST
} direction;

bool in_range(int T, position *p1, position *p2);

void move(int D, direction dir, position *p);

direction get_random_direction();

void get_random_position(position *p);

