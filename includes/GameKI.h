#pragma once

#include <queue>
#include <stack>
#include <stdlib.h>
#include <time.h>
#include <cmath>
#include "GamePanel.h"
#include "Tree.h"




// Quadratwurzel aus 2 als C_FACTOR
#define C_FACTOR 1.41421L

class GameKI {
private:
    size_t MAX_NUM_OF_ITERATIONS = 2000;
    // const GamePanel &actualGamePanel;   
    GamePanel simulatedGamePanel;
    Player AI_Player, OP_Player; // The KI knows about opponent and user player token 
    Tree gameTree;

    int pickBestColMove(const GamePanel &gp);
    int pickRandomTurn(const GamePanel &gp);
    int doRandomTurnH(GamePanel &gp);
    void expandAllChildrenOf(t_node *node);
    t_node *UCTSelect(t_node *node);
    t_node *recursive_selection(t_node *node);
    t_node *selection(t_node *node);
    t_node *expansion(t_node *leaf_node);
    //---------------------------------- 
#define VALUE_WIN_IMM  3000.0;
#define VALUE_LOOSE_IMM  0.0;
    //----------------------------------
#define VALUE_WIN   1.0;
    //----------------------------------
#define VALUE_FULL_TIE  0.0;
#define VALUE_TIE   0.0;
    //---------------------------------- 
#define VALUE_LOOSE   0.0;
    //---------------------------------- 
    double simulation(t_node *expanded_node);
    void backpropagation(t_node *expanded_node, double bewertung);
    t_node *selectSaveChild(t_node *node);
    t_node *selectRobustChild(t_node *node);
public:

    GameKI(Player tokenKI);
    int calculateNextTurn(const GamePanel &gPanel);


};