#include "moves.h"
#include "map.h"  // Include map.h to define t_map
#include <stdlib.h>
#include <stdio.h>

/* prototypes of local functions */
/* local functions are used only in this file, as helper functions */

/**
 * @brief Function to rotate the robot according to a move and its actual orientation
 * @param ori : the actual orientation of the robot
 * @param move : the move to do
 * @return the new orientation of the robot
 */
t_orientation rotate(t_orientation ori, t_move move) {
    int rst;
    switch (move) {
        case T_LEFT:
            rst = 3;
            break;
        case T_RIGHT:
            rst = 1;
            break;
        case U_TURN:
            rst = 2;
            break;
        default:
            break;
    }
    return (ori + rst) % 4;
}

/**
 * @brief function to translate the robot according to a move and its actual position
 * @param loc : the actual localisation of the robot
 * @param move : the move to do
 * @return the new localisation of the robot
 */
t_localisation translate(t_localisation loc, t_move move) {
    /** rules for coordinates:
     *  - x grows to the right with step of +1
     *  - y grows to the bottom with step of +1
     *  - the origin (x=0, y=0) is at the top left corner
     */
    t_position res = loc.pos;  // Initialize res with loc's current position
    switch (move) {
        case F_10:
            switch (loc.ori) {
                case NORTH:
                    res.y = loc.pos.y - 1;
                    break;
                case EAST:
                    res.x = loc.pos.x + 1;
                    break;
                case SOUTH:
                    res.y = loc.pos.y + 1;
                    break;
                case WEST:
                    res.x = loc.pos.x - 1;
                    break;
                default:
                    break;
            }
            break;
        case F_20:
            switch (loc.ori) {
                case NORTH:
                    res.y = loc.pos.y - 2;
                    break;
                case EAST:
                    res.x = loc.pos.x + 2;
                    break;
                case SOUTH:
                    res.y = loc.pos.y + 2;
                    break;
                case WEST:
                    res.x = loc.pos.x - 2;
                    break;
                default:
                    break;
            }
            break;
        case F_30:
            switch (loc.ori) {
                case NORTH:
                    res.y = loc.pos.y - 3;
                    break;
                case EAST:
                    res.x = loc.pos.x + 3;
                    break;
                case SOUTH:
                    res.y = loc.pos.y + 3;
                    break;
                case WEST:
                    res.x = loc.pos.x - 3;
                    break;
                default:
                    break;
            }
            break;
        case B_10:
            switch (loc.ori) {
                case NORTH:
                    res.y = loc.pos.y + 1;
                    break;
                case EAST:
                    res.x = loc.pos.x - 1;
                    break;
                case SOUTH:
                    res.y = loc.pos.y - 1;
                    break;
                case WEST:
                    res.x = loc.pos.x + 1;
                    break;
                default:
                    break;
            }
            break;
        default:
            break;
    }
    return loc_init(res.x, res.y, loc.ori);
}

/* definitions of exported functions */

char *getMoveAsString(t_move move) {
    return _moves[move];
}

t_localisation move(t_localisation loc, t_move move) {
    t_localisation new_loc;
    new_loc.ori = rotate(loc.ori, move);
    new_loc = translate(loc, move);
    return new_loc;
}

void updateLocalisation(t_localisation *p_loc, t_move m) {
    *p_loc = move(*p_loc, m);
    return;
}

/* Additional functions for movement selection using a tree structure */

// Struct for a node in the movement tree
typedef struct s_move_node {
    int cost;
    t_move move;
    struct s_move_node *children[9];  // 9 possible moves at each level
} t_move_node;

/**
 * @brief Creates a new move node with specified cost and move type.
 * @param cost : the cost associated with the node's position
 * @param move : the type of move for this node
 * @return Pointer to the newly created node
 */
t_move_node* createMoveNode(int cost, t_move move) {
    t_move_node *node = (t_move_node *)malloc(sizeof(t_move_node));
    node->cost = cost;
    node->move = move;
    for (int i = 0; i < 9; i++) node->children[i] = NULL;  // Initialize all children to NULL
    return node;
}

/**
 * @brief Builds a tree for selecting moves with minimum cost, given a depth limit.
 * @param depth : the current depth in the tree
 * @param max_depth : the maximum depth for tree expansion
 * @param map : the Mars map to check for costs
 * @param start_pos : the starting position on the map
 * @return Pointer to the root of the move tree
 */
t_move_node* buildMoveTree(int depth, int max_depth, t_map map, t_position start_pos) {
    if (depth == max_depth) return NULL;  // Stop if max depth is reached

    // Create a root node at the starting position with its associated cost
    t_move_node *root = createMoveNode(map.costs[start_pos.y][start_pos.x], F_10);  // Use F_10 as a placeholder move

    // Build child nodes for each of the 9 possible moves
    for (int i = 0; i < 9; i++) {
        // Calculate the next position based on the move
        t_position next_pos = calculateNextPosition(start_pos, i);
        if (isValidLocalisation(next_pos, map.x_max, map.y_max)) {
            root->children[i] = buildMoveTree(depth + 1, max_depth, map, next_pos);  // Recursive call for child nodes
        }
    }
    return root;
}