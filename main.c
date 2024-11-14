#include <stdio.h>
#include <stdlib.h>
#include "map.h"
#include "loc.h"
#include "moves.h"
#include "queue.h"
#include "stack.h"

int main() {
    
    t_map map = createMapFromFile("C:\\Users\\malom\\CLionProjects\\int2_group10\\maps\\example1.map");
    printf("Map created with dimensions %d x %d\n", map.y_max, map.x_max);

   
    printf("Map Soils:\n");
    displayMap(map);

   
    t_localisation loc = loc_init(5, 5, NORTH);
    printf("\nInitial Robot Position: (%d, %d), Orientation: %d\n", loc.pos.x, loc.pos.y, loc.ori);

  
    updateLocalisation(&loc, F_10);
    printf("After Move F_10: (%d, %d), Orientation: %d\n", loc.pos.x, loc.pos.y, loc.ori);

    updateLocalisation(&loc, T_RIGHT);
    printf("After Turn Right: (%d, %d), Orientation: %d\n", loc.pos.x, loc.pos.y, loc.ori);

  
    t_queue queue = createQueue(10);
    t_position start_pos = {2, 3};

    enqueue(&queue, start_pos);  // Ensure enqueue is done before dequeuing
    printf("Position enqueued: (%d, %d)\n", start_pos.x, start_pos.y);

    if (queue.first != queue.last) {
        t_position dequeued_pos = dequeue(&queue);
        printf("Dequeued Position: (%d, %d)\n", dequeued_pos.x, dequeued_pos.y);
    } else {
        printf("Queue is empty; cannot dequeue.\n");
    }

   
    free(queue.values);

    return 0;
}
