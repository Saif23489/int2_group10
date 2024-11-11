#include <stdio.h>
#include <stdlib.h>
#include "map.h"
#include "loc.h"
#include "queue.h"

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
        fprintf(stderr, "Error: base station not found in the map\n");
        exit(1);
    }
    return pos;
}

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
            t_position lp = LEFT(pos), rp = RIGHT(pos), up = UP(pos), dp = DOWN(pos);
            int min_neighbour = COST_UNDEF;

            if (isValidLocalisation(lp, map.x_max, map.y_max))
                min_neighbour = (map.costs[lp.y][lp.x] < min_neighbour) ? map.costs[lp.y][lp.x] : min_neighbour;

            if (isValidLocalisation(rp, map.x_max, map.y_max))
                min_neighbour = (map.costs[rp.y][rp.x] < min_neighbour) ? map.costs[rp.y][rp.x] : min_neighbour;

            if (isValidLocalisation(up, map.x_max, map.y_max))
                min_neighbour = (map.costs[up.y][up.x] < min_neighbour) ? map.costs[up.y][up.x] : min_neighbour;

            if (isValidLocalisation(dp, map.x_max, map.y_max))
                min_neighbour = (map.costs[dp.y][dp.x] < min_neighbour) ? map.costs[dp.y][dp.x] : min_neighbour;

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

        if (isValidLocalisation(lp, map.x_max, map.y_max))
            min_cost = (map.costs[lp.y][lp.x] < min_cost) ? map.costs[lp.y][lp.x] : min_cost;

        if (isValidLocalisation(rp, map.x_max, map.y_max))
            min_cost = (map.costs[rp.y][rp.x] < min_cost) ? map.costs[rp.y][rp.x] : min_cost;

        if (isValidLocalisation(up, map.x_max, map.y_max))
            min_cost = (map.costs[up.y][up.x] < min_cost) ? map.costs[up.y][up.x] : min_cost;

        if (isValidLocalisation(dp, map.x_max, map.y_max))
            min_cost = (map.costs[dp.y][dp.x] < min_cost) ? map.costs[dp.y][dp.x] : min_cost;

        map.costs[pos.y][pos.x] = (map.soils[pos.y][pos.x] == BASE_STATION) ? 0 : min_cost + self_cost;

        if (isValidLocalisation(lp, map.x_max, map.y_max) && map.costs[lp.y][lp.x] == COST_UNDEF) {
            enqueue(&queue, lp);
        }
        if (isValidLocalisation(rp, map.x_max, map.y_max) && map.costs[rp.y][rp.x] == COST_UNDEF) {
            enqueue(&queue, rp);
        }
        if (isValidLocalisation(up, map.x_max, map.y_max) && map.costs[up.y][up.x] == COST_UNDEF) {
            enqueue(&queue, up);
        }
        if (isValidLocalisation(dp, map.x_max, map.y_max) && map.costs[dp.y][dp.x] == COST_UNDEF) {
            enqueue(&queue, dp);
        }
    }
}

t_map createMapFromFile(char *filename) {
    FILE *file = fopen(filename, "r");
    if (!file) {
        fprintf(stderr, "Error: Cannot open file %s\n", filename);
        exit(1);
    }

    t_map map;
    fscanf(file, "%d %d", &map.y_max, &map.x_max);
    if (map.x_max <= 0 || map.y_max <= 0) {
        fprintf(stderr, "Error: Invalid map dimensions\n");
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
            fscanf(file, "%d", (int *)&map.soils[i][j]);
            map.costs[i][j] = COST_UNDEF;
        }
    }

    fclose(file);
    return map;
}

void displayMap(t_map map) {
    for (int i = 0; i < map.y_max; i++) {
        for (int j = 0; j < map.x_max; j++) {
            printf("%d ", map.soils[i][j]);
        }
        printf("\n");
    }
}
