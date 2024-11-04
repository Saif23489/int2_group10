#include <stdio.h>
#include <stdlib.h>
#include "map.h"
#include "loc.h"
#include "queue.h"

/* prototypes of local functions */
/* local functions are used only in this file, as helper functions */

/**
 * @brief : function to get the position of the base station
 * @param map : the map
 * @return : the position of the base station
 */
t_position getBaseStationPosition(t_map map) {
    t_position pos;
    int i = 0;
    int found = 0;
    while (i < map.y_max && !found) {
        int j = 0;
        while (j < map.x_max && !found) {
            if (map.soils[i][j] == BASE_STATION) {
                pos.x = j;
                pos.y = i;
                found = 1;
            }
            j++;
        }
        i++;
    }
    // if the base station is not found, we exit the program
    if (!found) {
        fprintf(stderr, "Error: base station not found in the map\n");
        exit(1);
    }
    return pos;
}

void removeFalseCrevasses(t_map map) {
    // step 1 : find the minimal cost > 10000 in the costs array where the soil is not a crevasse
    int over = 0;
    int imin, jmin;
    while (!over) {
        int min_cost = COST_UNDEF;
        imin = map.y_max;
        jmin = map.x_max;
        for (int i = 0; i < map.y_max; i++) {
            for (int j = 0; j < map.x_max; j++) {
                if (map.soils[i][j] != CREVASSE && map.costs[i][j] > 10000 && map.costs[i][j] < min_cost) {
                    min_cost = map.costs[i][j];
                    imin = i;
                    jmin = j;
                }
            }
        }
        if (imin < map.y_max && jmin < map.x_max) {
            // step 2 : calculate the costs of the neighbours of the position
            t_position pos;
            pos.x = jmin;
            pos.y = imin;
            t_position lp, rp, up, dp;
            lp = LEFT(pos);
            rp = RIGHT(pos);
            up = UP(pos);
            dp = DOWN(pos);
            int min_neighbour = COST_UNDEF;
            if (isValidLocalisation(lp, map.x_max, map.y_max)) {
                min_neighbour = (map.costs[lp.y][lp.x] < min_neighbour) ? map.costs[lp.y][lp.x] : min_neighbour;
            }
            if (isValidLocalisation(rp, map.x_max, map.y_max)) {
                min_neighbour = (map.costs[rp.y][rp.x] < min_neighbour) ? map.costs[rp.y][rp.x] : min_neighbour;
            }
            if (isValidLocalisation(up, map.x_max, map.y_max)) {
                min_neighbour = (map.costs[up.y][up.x] < min_neighbour) ? map.costs[up.y][up.x] : min_neighbour;
            }
            if (isValidLocalisation(dp, map.x_max, map.y_max)) {
                min_neighbour = (map.costs[dp.y][dp.x] < min_neighbour) ? map.costs[dp.y][dp.x] : min_neighbour;
            }
            int self_cost = _soil_cost[map.soils[imin][jmin]];
            map.costs[imin][jmin] = (min_neighbour + self_cost < map.costs[imin][jmin]) ? min_neighbour + self_cost : map.costs[imin][jmin];
        } else {
            over = 1;
        }
    }
}

void calculateCosts(t_map map) {
    t_position baseStation = getBaseStationPosition(map);
    t_queue queue = createQueue(map.x_max * map.y_max);
    enqueue(&queue, baseStation);

    while (queue.first != queue.last) {
        t_position pos = dequeue(&queue);
        int self_cost = _soil_cost[map.soils[pos.y][pos.x]];
        t_position lp = LEFT(pos), rp = RIGHT(pos), up = UP(pos), dp = DOWN(pos);
        int min_cost = COST_UNDEF;

        // Update costs by examining valid neighbors
        if (isValidLocalisation(lp, map.x_max, map.y_max))
            min_cost = (map.costs[lp.y][lp.x] < min_cost) ? map.costs[lp.y][lp.x] : min_cost;
        if (isValidLocalisation(rp, map.x_max, map.y_max))
            min_cost = (map.costs[rp.y][rp.x] < min_cost) ? map.costs[rp.y][rp.x] : min_cost;
        if (isValidLocalisation(up, map.x_max, map.y_max))
            min_cost = (map.costs[up.y][up.x] < min_cost) ? map.costs[up.y][up.x] : min_cost;
        if (isValidLocalisation(dp, map.x_max, map.y_max))
            min_cost = (map.costs[dp.y][dp.x] < min_cost) ? map.costs[dp.y][dp.x] : min_cost;

        // Set the cost of the current position based on terrain type and neighbors
        map.costs[pos.y][pos.x] = (map.soils[pos.y][pos.x] == BASE_STATION) ? 0 : min_cost + self_cost;

        // Enqueue valid neighboring positions if not yet visited
        if (isValidLocalisation(lp, map.x_max, map.y_max) && map.costs[lp.y][lp.x] == COST_UNDEF) {
            map.costs[lp.y][lp.x] = COST_UNDEF - 1;
            enqueue(&queue, lp);
        }
        if (isValidLocalisation(rp, map.x_max, map.y_max) && map.costs[rp.y][rp.x] == COST_UNDEF) {
            map.costs[rp.y][rp.x] = COST_UNDEF - 1;
            enqueue(&queue, rp);
        }
        if (isValidLocalisation(up, map.x_max, map.y_max) && map.costs[up.y][up.x] == COST_UNDEF) {
            map.costs[up.y][up.x] = COST_UNDEF - 1;
            enqueue(&queue, up);
        }
        if (isValidLocalisation(dp, map.x_max, map.y_max) && map.costs[dp.y][dp.x] == COST_UNDEF) {
            map.costs[dp.y][dp.x] = COST_UNDEF - 1;
            enqueue(&queue, dp);
        }
    }
}

t_map createMapFromFile(char *filename) {
    /* Original createMapFromFile code remains unchanged */
}

t_map createTrainingMap() {
    /* Original createTrainingMap code remains unchanged */
}

void displayMap(t_map map) {
    /* Original displayMap code remains unchanged */
}
