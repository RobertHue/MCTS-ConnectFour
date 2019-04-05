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
public:

    GameKI(Player tokenKI);
    int calculateNextTurn(const GamePanel &gPanel);

private:
    size_t MAX_NUM_OF_ITERATIONS = 2000;
    // const GamePanel &actualGamePanel;
    GamePanel simulatedGamePanel;
    Player AI_Player, OP_Player; // The KI knows about opponent and user player token
    Tree gameTree;

    int pickBestColMove(const GamePanel &gp);
    int pickRandomTurn(const GamePanel &gp);
    int doRandomTurnH(GamePanel &gp);
    void expandAllChildrenOf(NodeType *Node);
    NodeType *UCTSelect(NodeType *Node);
    NodeType *recursive_selection(NodeType *Node);
    NodeType *selection(NodeType *Node);
    NodeType *expansion(NodeType *leaf_node);
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
    double simulation(NodeType *expanded_node);
    void backpropagation(NodeType *expanded_node, double bewertung);
    NodeType *selectSaveChild(NodeType *Node);
    NodeType *selectRobustChild(NodeType *Node);
};
