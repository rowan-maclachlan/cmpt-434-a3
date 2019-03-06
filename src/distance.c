/* 
 * Rowan MacLachlan
 * rdm695 11165820
 * CMPT 434 Eager
 * assignment 3
 * Monday March 18th 2019
 */

#include <math.h>
#include <stdbool.h>
#include <stdlib.h>

#include "distance.h"

/*
 * Calculate whether p1 and p2 are at least as near to each other as the
 * distance T.  Do this by taking the floor of the hypothenus derived from
 * squaring their position's differences, and comparing that value to T.
 */
bool in_range(int T, position *p1, position *p2) {
    return floor(sqrt( pow((p1->x - p2->x), 2) + pow((p1->y - p2->y), 2))) <= T;
}

/*
 * Move position by traveling a distance of D in a direction dir.  Update the
 * position parameter p with the new position.  Boundaries of the map should be
 * toroidal - as though the map is continuous.
 */
void move(int D, direction dir, position *p) {
    if (dir == NORTH) {
        p->y = (p->y + D) % U_LEFT;
    } 
    else if (dir == EAST) {
        p->x = (p->x + D) % B_RIGHT;
    }
    else if (dir == SOUTH) {
        p->y = ((p->y + U_LEFT) - D) % U_LEFT;
    }
    else if (dir == WEST) {
        p->x = ((p->x + B_RIGHT) - D) % B_RIGHT;
    }
}

/*
 * Get a random direction
 */
direction get_random_direction() {
    int dir = arc4random() % (WEST + 1);
    switch (dir) {
        case NORTH:
            return NORTH;
        case EAST:
            return EAST;
        case SOUTH:
            return SOUTH;
        default:
            return WEST;
    }
}
