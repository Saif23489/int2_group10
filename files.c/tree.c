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
}


/**
 * Recursively creates a tree of nodes from the given parent node.
 * This method terminates the recursion based on the specified depth or
 * if the node is deemed invalid.
 *
 * @param parent A pointer to the parent node from which the tree will be constructed.
 * @param map The map containing details about soil types and costs.
 * @param k Maximum depth for the tree creation. The recursion will stop when this depth is reached.
 */
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
}

void findMinCostLeaf(p_node root, p_node *leaf) {
    if (root == NULL)
        return;

    findMinCostLeafInNode(root, leaf);
}



void printTree(p_node root, int level) {
    if (root == NULL)
        return;

    for (int i = 0; i <= level; i++) printf("  ");
    printf("Position (%d, %d), Orientation %d, Cost: %d\n", root->loc.pos.x, root->loc.pos.y, root->loc.ori, root->cost);


    for (int i = 0; i < root->nbSons; i++) {
        printTree(root->sons[i], level + 1);
    }
}


void freeTree(p_node node) {
    if (node == NULL) return;

    for (int i = 0; i < node->nbSons; i++) {
        freeTree(node->sons[i]);
    }
    free(node->sons);
    free(node->avails_moves);
    free(node);
}


void printPathToRoot(p_node leaf) {
    if (leaf == NULL) return;

    if (leaf->parent != NULL) {
        printPathToRoot(leaf->parent);

        printf(" -> %s", getMoveAsString(leaf->move));
    } else {

        printf("START");
    }
}

int hasReachedBase(t_map map, t_position pos){
    t_position base = getBaseStationPosition(map);
    if(pos.x == base.x && pos.y == base.y)
        return 1;
    return 0;
}
/***********************************************************************************************************************/
// Stacks functions definition

t_stack_node createStackNode(int size)
{
    // the size of the stack must be positive
    assert(size > 0);
    t_stack_node s;
    s.size = size;
    s.nbElts = 0;
    s.values = malloc(size * sizeof(p_node));
    return s;
}


void pushNode(t_stack_node *p_stack, p_node value)
{
    // the stack must not be full
    assert(p_stack->nbElts < p_stack->size);
    p_stack->values[p_stack->nbElts] = value;
    p_stack->nbElts++;

}


p_node popNode(t_stack_node *p_stack) {
    // the stack must not be empty
    assert(p_stack->nbElts > 0);
    p_node value = p_stack->values[p_stack->nbElts - 1];
    p_stack->nbElts--;
    return value;
}


p_node topNode(t_stack_node stack) {
    // the stack must not be empty
    assert(stack.nbElts > 0);
    return stack.values[stack.nbElts - 1];
}

int isStackEmptyNode(t_stack_node s) {
    return s.nbElts == 0;
}
/**********************************************************************************************************************/

void executePath(t_stack_node *s, t_localisation *start_loc) {
    while (!isStackEmptyNode(*s)) {
        p_node current = popNode(s);
        updateLocalisation(start_loc, current->move);
    }

}


void getPath(p_node min_leaf, t_stack_node* s) {
    if (min_leaf == NULL) return;
    p_node current = min_leaf;
    while (current != NULL) {
        pushNode(s, current);
        current = current->parent;
    }
}

void lauchedPhase(t_map map, t_localisation *start_loc, t_stack_node *s) {
    t_move *moves = getRandomMoves(9);
    p_node tree = createNode(*start_loc, 9, moves, 9, 0, NULL, START, map);
    int nbMovementsInPhase = 5;
    if (tree->type_of_soil == REG) {
        nbMovementsInPhase = 4;
    }
    createTreeRecursivity(tree, map, nbMovementsInPhase);
    //printTree(tree,0);
    p_node minLeaf = NULL;
    findMinCostLeaf(tree, &minLeaf);
    if(minLeaf != NULL) {
        printf("\nminimal coast leave : %d\n", minLeaf->cost);
        printPathToRoot(minLeaf);
        getPath(minLeaf, s);
        executePath(s, start_loc);
    }
    freeTree(tree);
}


void drivingToBase(t_map map, t_localisation *start_loc, t_stack_node *s) {
    if (!isValidLocalisation(start_loc->pos, map.x_max, map.y_max) ||
        map.soils[start_loc->pos.y][start_loc->pos.x] == CREVASSE) {
        printf("Position invalide \n");
        return;
    }
    while (!hasReachedBase(map, start_loc->pos)) {
        lauchedPhase(map, start_loc, s);
    }
}