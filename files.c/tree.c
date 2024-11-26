//
// Created by ibr on 23/11/2024.

#include "../files.h//tree.h"
#include "moves.h"
#include "map.h"
#include "stdlib.h"
#include "assert.h"
#include <stdio.h>

p_node createNode(t_localisation loc, int nbSons, const t_move *available_moves, int nbMoves, int depth, p_node parent, t_move move_to_do, t_map map) {
    p_node node = malloc(sizeof(t_node));
    if (node == NULL )
        return NULL;
    if (!isValidLocalisation(loc.pos,map.x_max , map.y_max)) {
        printf("invalide Position  \n");
        return NULL;
    }

    node->loc = loc;
    node->type_of_soil = map.soils[loc.pos.y][loc.pos.x];
    node->cost = map.costs[loc.pos.y][loc.pos.x];
    node->depth = depth;
    node->nbSons = nbSons;
    node->parent = parent;
    node->move = move_to_do;
    node->nbMoves = nbMoves;
    node->avails_moves = (t_move *)malloc(nbMoves *sizeof(t_move));

    if (node->avails_moves == NULL) {
        free(node);
        printf("Moves configuration failed\n");
        return NULL;
    }

    for (int i = 0; i < nbMoves; i++) {
        node->avails_moves[i] = available_moves[i];
    }
    node->sons = (p_node *) malloc(nbSons * sizeof(p_node));

    if (node->sons == NULL) {
        free(node);
        printf("Sons configuration failed\n");
        return NULL;
    }

    for (int i = 0; i < nbSons; i++){
        node->sons[i] = NULL;
    }

    return node;
    // Complexity: O(nbMoves + nbSons) - Allocates memory for moves and sons, then initializes.
}

void createTreeRecursivity(p_node parent, t_map map, int depth) {
    if (parent == NULL || parent->depth >= depth || !isValidLocalisation(parent->loc.pos, map.x_max, map.y_max))
        return;

    if (map.soils[parent->loc.pos.y][parent->loc.pos.x] == CREVASSE) {
        parent->nbSons = 0;
        parent->nbMoves = 0;
        return;
    }

    for (int i = 0; i < parent->nbSons; i++) {
        int nbAvailsMove = 0;
        t_move *availsMove = malloc((parent->nbMoves - 1) * sizeof(t_move));
        if (availsMove == NULL)
            return;

        for (int j = 0; j < parent->nbMoves; j++) {
            int movement = parent->avails_moves[j];
            if (j != i) {
                if (parent->type_of_soil == CREVASSE)
                    continue;
                if (parent->type_of_soil == ERG) {

                    if (movement == F_10 || movement == B_10) {
                        continue;
                    }
                    if (movement == F_20) {
                        movement = F_10;
                    } else if (movement == F_30) {
                        movement = F_20;
                    }

                    // Empêche les rotations de quart de tour
                    if (movement == T_LEFT || movement == T_RIGHT) {
                        continue;  // Tourner d’un quart de tour est interdit
                    }
                    if (movement == U_TURN) {
                        movement = T_LEFT;  // On choisit de tourner à gauche à chaque fois dans ce cas
                    }
                }
                availsMove[nbAvailsMove++] = movement;
            }
        }

        t_localisation son_loc = move(parent->loc, parent->avails_moves[i]);
        if (!isValidLocalisation(son_loc.pos, map.x_max, map.y_max)) {
            free(availsMove);
            continue;
        }


        p_node son = createNode(son_loc,
                                nbAvailsMove,
                                availsMove,
                                parent->nbSons - 1,
                                parent->depth + 1,
                                parent,
                                parent->avails_moves[i],
                                map);
        if (son == NULL) {
            free(availsMove);
            continue;
        }

        parent->sons[i] = son;
        createTreeRecursivity(son, map, depth);

        free(availsMove);
    }
    // Complexity: O(branching_factor^depth) - Exponential growth of nodes as recursion progresses.
}

void findMinCostLeafInNode(p_node node, p_node *leaf) {
    if (node == NULL )
        return;

    if (node->nbSons == 0) {
        if (*leaf == NULL || node->cost < (*leaf)->cost) {
            *leaf = node;
        }
    }

    for (int i = 0; i < node->nbSons; i++) {
        findMinCostLeafInNode(node->sons[i], leaf);
    }
    // Complexity: O(total_nodes) - Traverses all nodes in the tree to find the leaf with the minimum cost.
}

void findMinCostLeaf(p_node root, p_node *leaf) {
    if (root == NULL)
        return;

    findMinCostLeafInNode(root, leaf);
    // Complexity: O(total_nodes) - Delegates to `findMinCostLeafInNode`.
}

void printTree(p_node root, int level) {
    if (root == NULL)
        return;

    for (int i = 0; i <= level; i++) printf("  ");
    printf("Position (%d, %d), Orientation %d, Cost: %d\n", root->loc.pos.x, root->loc.pos.y, root->loc.ori, root->cost);


    for (int i = 0; i < root->nbSons; i++) {
        printTree(root->sons[i], level + 1);
    }
    // Complexity: O(total_nodes) - Visits each node once to print its data.
}

void freeTree(p_node node) {
    if (node == NULL) return;

    for (int i = 0; i < node->nbSons; i++) {
        freeTree(node->sons[i]);
    }
    free(node->sons);
    free(node->avails_moves);
    free(node);
    // Complexity: O(total_nodes) - Frees all nodes recursively.
}

void printPathToRoot(p_node leaf) {
    if (leaf == NULL) return;

    if (leaf->parent != NULL) {
        printPathToRoot(leaf->parent);

        printf(" -> %s", getMoveAsString(leaf->move));
    } else {

        printf("START");
    }
    // Complexity: O(depth) - Traverses up the tree from the leaf to the root.
}
