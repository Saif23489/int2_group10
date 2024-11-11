#include <stdio.h>
#include <stdlib.h>
#include "map.h"
#include "loc.h"
#include "queue.h"

/**
 * @brief Function to get the position of the base station.
 * @param map : the map
 * @return the position of the base station
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

    if (!found) {
        fprintf(stderr, "Error: Base station not found in the map.\n");
        exit(1);
    }
    return pos;
}

/**
 * @brief Function to remove false crevasses by recalculating costs.
 * @param map : the map structure
 */
void removeFalseCrevasses(t_map map) {
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
            t_position pos = {jmin, imin};
            t_position neighbors[4] = {LEFT(pos), RIGHT(pos), UP(pos), DOWN(pos)};
            int min_neighbour = COST_UNDEF;

            for (int k = 0; k < 4; k++) {
                if (isValidLocalisation(neighbors[k], map.x_max, map.y_max)) {
                    min_neighbour = (map.costs[neighbors[k].y][neighbors[k].x] < min_neighbour)
                                    ? map.costs[neighbors[k].y][neighbors[k].x]
                                    : min_neighbour;
                }
            }

            int self_cost = _soil_cost[map.soils[imin][jmin]];
            map.costs[imin][jmin] = (min_neighbour + self_cost < map.costs[imin][jmin])
                                    ? min_neighbour + self_cost
                                    : map.costs[imin][jmin];
        } else {
            over = 1;
        }
    }
}

/**
 * @brief Function to calculate traversal costs for the map.
 * @param map : the map structure
 */
void calculateCosts(t_map map) {
    t_position baseStation = getBaseStationPosition(map);
    t_queue queue = createQueue(map.x_max * map.y_max);
    enqueue(&queue, baseStation);

    while (queue.first != queue.last) {
        t_position pos = dequeue(&queue);
        int self_cost = _soil_cost[map.soils[pos.y][pos.x]];
        t_position neighbors[4] = {LEFT(pos), RIGHT(pos), UP(pos), DOWN(pos)};

        for (int i = 0; i < 4; i++) {
            if (isValidLocalisation(neighbors[i], map.x_max, map.y_max)) {
                int new_cost = map.costs[pos.y][pos.x] + self_cost;
                if (map.costs[neighbors[i].y][neighbors[i].x] == COST_UNDEF || new_cost < map.costs[neighbors[i].y][neighbors[i].x]) {
                    map.costs[neighbors[i].y][neighbors[i].x] = new_cost;
                    enqueue(&queue, neighbors[i]);
                }
            }
        }
    }

    free(queue.values);
}

/**
 * @brief Function to create a map from a file.
 * @param filename : the file name containing the map
 * @return the created map
 */
t_map createMapFromFile(char *filename) {
    FILE *file = fopen(filename, "r");
    if (!file) {
        fprintf(stderr, "Error: Cannot open file %s\n", filename);
        exit(1);
    }

    t_map map;
    if (fscanf(file, "%d %d", &map.y_max, &map.x_max) != 2 || map.x_max <= 0 || map.y_max <= 0) {
        fprintf(stderr, "Error: Invalid map dimensions in %s\n", filename);
        fclose(file);
        exit(1);
    }

    map.soils = malloc(map.y_max * sizeof(t_soil *));
    map.costs = malloc(map.y_max * sizeof(int *));
    for (int i = 0; i < map.y_max; i++) {
        map.soils[i] = malloc(map.x_max * sizeof(t_soil));
        map.costs[i] = malloc(map.x_max * sizeof(int));
    }

    for (int i = 0; i < map.y_max; i++) {
        for (int j = 0; j < map.x_max; j++) {
            if (fscanf(file, "%d", (int *)&map.soils[i][j]) != 1) {
                fprintf(stderr, "Error: Failed to read soil data at (%d, %d) in %s\n", i, j, filename);
                fclose(file);
                exit(1);
            }
            map.costs[i][j] = COST_UNDEF;
        }
    }

    fclose(file);
    return map;
}

/**
 * @brief Function to display the map's soil grid.
 * @param map : the map structure
 */
void displayMap(t_map map) {
    for (int i = 0; i < map.y_max; i++) {
        for (int j = 0; j < map.x_max; j++) {
            printf("%d ", map.soils[i][j]);
        }
        printf("\n");
    }
}
