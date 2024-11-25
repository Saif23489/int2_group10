//
// Created by ibr on 20/11/2024.
//
#include "test.h"
#include "loc.h"
#include "../files.h//moves.h"
#include "stdio.h"
#include "stdlib.h"

/**
 * @brief Tests the localization functions for the rover.
 */
void test_localisation() {
    t_localisation Rover_loc = loc_init(4, 6, NORTH);      // Initialize rover's current position.
    t_localisation Rover_init_loc = loc_init(6, 6, NORTH); // Initialize rover's initial position.

    // Test positions relative to Rover's current location.
    t_position Right_rover = RIGHT(Rover_loc.pos);
    t_position Left_rover = LEFT(Rover_loc.pos);
    t_position Up_rover = UP(Rover_loc.pos);
    t_position Down_rover = DOWN(Rover_loc.pos);

    // Print the initial and current positions of the rover.
printf("The initial location of the rover is (%d, %d) oriented towards %d\n", Rover_init_loc.pos.x, Rover_init_loc.pos.y, Rover_init_loc.ori);
printf("The current location of the rover is (%d, %d) oriented towards %d\n", Rover_loc.pos.x, Rover_loc.pos.y, Rover_loc.ori);

// Print positions around the rover.
printf("The position of the cell to the right of the rover is (%d, %d)\n", Right_rover.x, Right_rover.y);
printf("The position of the cell to the left of the rover is (%d, %d)\n", Left_rover.x, Left_rover.y);
printf("The position of the cell above the rover is (%d, %d)\n", Up_rover.x, Up_rover.y);
printf("The position of the cell below the rover is (%d, %d)\n", Down_rover.x, Down_rover.y);
printf("\n");

    // Complexity: O(1) - Each function called (e.g., loc_init, RIGHT, LEFT, UP, DOWN) is O(1), and printing is O(1) per statement.
}

/**
 * @brief Tests the movement functions for the rover.
 */
void test_move() {
    t_localisation Rover_loc = loc_init(4, 6, NORTH); // Initialize rover's position and orientation.

    // Test various movements.
    t_localisation move_1 = move(Rover_loc, F_10);  // Move forward by 10 meters.
    t_localisation move_2 = move(Rover_loc, T_LEFT); // Turn left.
    t_localisation move_3 = move(Rover_loc, F_20);  // Move forward by 20 meters.
    t_localisation move_4 = move(Rover_loc, T_RIGHT); // Turn right.
    t_localisation move_5 = move(Rover_loc, F_30);  // Move forward by 30 meters.
    t_localisation move_6 = move(Rover_loc, B_10);  // Move backward by 10 meters.
    t_localisation move_7 = move(Rover_loc, U_TURN); // Make a U-turn.

    // Print the results of each move.
   printf("The rover's location after moving 10m should be (%d, %d) oriented towards %d\n", move_1.pos.x, move_1.pos.y, move_1.ori);
printf("The rover's location after turning left should be (%d, %d) oriented towards %d\n", move_2.pos.x, move_2.pos.y, move_2.ori);
printf("The rover's location after moving 20m should be (%d, %d) oriented towards %d\n", move_3.pos.x, move_3.pos.y, move_3.ori);
printf("The rover's location after turning right should be (%d, %d) oriented towards %d\n", move_4.pos.x, move_4.pos.y, move_4.ori);
printf("The rover's location after moving 30m should be (%d, %d) oriented towards %d\n", move_5.pos.x, move_5.pos.y, move_5.ori);
printf("The rover's location after reversing 10m should be (%d, %d) oriented towards %d\n", move_6.pos.x, move_6.pos.y, move_6.ori);
printf("The rover's location after making a U-turn should be (%d, %d) oriented towards %d\n", move_7.pos.x, move_7.pos.y, move_7.ori);
printf("\n");

    // Complexity: O(1) per movement - The `move` function is O(1), and each print statement is O(1).
    // Total complexity: O(1) per line, with constant overhead for testing a fixed number of moves.
}

/**
 * @brief Tests the generation of random moves.
 */
void test_getRandomMoves() {
    // Generate two sets of random moves.
    t_move *moves1 = getRandomMoves(9); // Get 9 random moves.
    t_move *moves2 = getRandomMoves(5); // Get 5 random moves.

    // Print the first set of random moves.
    printf("[ ");
    for (int i = 0; i < 9; i++) {
        printf("%s ", getMoveAsString(moves1[i]));
    }
    printf("]\n");

    // Print the second set of random moves.
    printf("[ ");
    for (int i = 0; i < 5; i++) {
        printf("%s ", getMoveAsString(moves2[i]));
    }
    printf("]\n");

    // Free the allocated memory for moves.
    free(moves1);
    free(moves2);
    // Complexity: O(N) - The `getRandomMoves` function generates `N` moves, and printing each move is O(1).
    // Total complexity: O(N) for moves1 and O(M) for moves2, where N=9 and M=5.
}
