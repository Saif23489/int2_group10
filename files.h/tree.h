//
// Created by ibr on 23/11/2024.
//


#define UNTITLED1_TREE_H

#include "loc.h"
#include "map.h"
#include "moves.h"

typedef struct s_node {
    t_localisation loc;
    t_soil type_of_soil;
    int cost;
    int depth;
    t_move move;
    struct s_node **sons;
    int nbSons;
    t_move * avails_moves;
    int nbMoves;
    struct s_node *parent;
} t_node, *p_node;

typedef struct s_stack_node
{
    p_node *values;
    int size;
    int nbElts;
}t_stack_node;

p_node createNode(t_localisation loc, int nbSons, const t_move *available_moves, int nbMoves, int depth, p_node parent, t_move move_to_do,t_map map);
void createTreeRecursivity(p_node parent, t_map map, int depth);

void findMinCostLeaf(p_node root, p_node *leaf);
void printPathToRoot(p_node leaf);
void printTree(p_node root, int level);
void lauchedPhase(t_map map, t_localisation *start_loc, t_stack_node *s);
void drivingToBase(t_map map, t_localisation *start_loc, t_stack_node *s);
void freeTree(p_node root);
void drivingToBase(t_map map, t_localisation *start_loc, t_stack_node *s);
int hasReachedBase(t_map map, t_position pos);



t_stack_node createStackNode(int);


void pushNode(t_stack_node *p_stack, p_node value);

int isStackEmptyNode(t_stack_node );

p_node popNode(t_stack_node *);


p_node topNode(t_stack_node);









