#pragma once

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
