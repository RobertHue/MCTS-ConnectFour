# MCTS_ConnectFour

![example workflow](https://github.com/RobertHue/MCTS_ConnectFour/actions/workflows/build_and_test.yml/badge.svg)

Implementation of an Connect-Four AI using the MCTS algorithm.

## Table of contents

- [MCTS\_ConnectFour](#mcts_connectfour)
  - [Table of contents](#table-of-contents)
  - [Pre-requisites](#pre-requisites)
  - [Compilation](#compilation)
  - [MCTS-Algorithm](#mcts-algorithm)
  - [UML-class diagram](#uml-class-diagram)
  - [GUI-Design](#gui-design)
  - [Sources and Literature](#sources-and-literature)
  - [MIT](#mit)
    - [The MIT License](#the-mit-license)

## Pre-requisites

The installation pre-requisites are as follows:

- [CMake 3.20](https://cmake.org/download/)
- [Conan 2.0](https://docs.conan.io/1/installation.html)

As explained [here](https://doc.qt.io/qt-6/linux-requirements.html), on Linux the xcb platform plugin is used. It provides basic features to run against X11. To install the following development libraries run the following command::

```bash
sudo apt-get install libfontconfig1-dev libfreetype6-dev libx11-dev libx11-xcb-dev libxext-dev libxfixes-dev libxi-dev libxrender-dev libxcb1-dev libxcb-cursor-dev libxcb-glx0-dev libxcb-keysyms1-dev libxcb-image0-dev libxcb-shm0-dev libxcb-icccm4-dev libxcb-sync-dev libxcb-xfixes0-dev libxcb-shape0-dev libxcb-randr0-dev libxcb-render-util0-dev libxcb-util-dev libxcb-xinerama0-dev libxcb-xkb-dev libxkbcommon-dev libxkbcommon-x11-dev
```

## Compilation

To compile this project with the tests and the game itself execute the following commands:

```bash
mkdir build
cd build
conan install .. --output-folder=. --build=missing --profile:build=../conan-profile.txt
cmake .. -DCMAKE_TOOLCHAIN_FILE=conan_toolchain.cmake -DCMAKE_BUILD_TYPE=Release
cmake --build .
```

To execute the tests do the following:

```bash
../bin/MCTS_ConnectFour_tests
```

To execute the game do the following:

```bash
../bin/MCTS_ConnectFour
```

To clear all the whole conan cache, use the following command:

```bash
../bin/MCTS_ConnectFour
```


## MCTS-Algorithm

The algorithm works by finding the next best move the AI can do.
It does that by book-keeping information about a search tree (here called: game tree).
The root of that tree is the enemies turn and current state of the board.
The connections from that root to childNodes are the moves the current player can do.

MCTS process:

1. expand all childNodes from the root and assign a very large random initial UCTB-value
2. Repeat an iteration of Selection, Expansion, Simulation and Backpropagation as many times (here set to 10,000, but a time treshold can be chosen too)
3. Select an action to perfom (here: most visited robust childNode from root)

&nbsp;
&nbsp;
![MCTS](https://upload.wikimedia.org/wikipedia/commons/thumb/6/62/MCTS_%28English%29_-_Updated_2017-11-19.svg/1920px-MCTS_%28English%29_-_Updated_2017-11-19.svg.png)

Shows what the iteration in step 2. has to do (from wikipedia with CC BY-SA 4.0 licence).

Steps are explained below:

**Selection:**

The MCTS tries to traverses through the game tree always following paths with highest UCTB - doesn't matter whether enemy nodes or personal ones, because it needs to take into account that the enemy is also tries to maximize its winrate. This traversal stops, when some unexpanded Leaf-Nodes (L) is found.

**Expansion:**

Expand one or more Leaf-Nodes (L), unless leaf-node L ended the game.

**Simulation:**

Simulate a playout until game ended in a WIN / LOOSE / DRAW.

**Backpropagation:**

Increase visit count of every node along the followed path until the root node is reached (included) and do the following for those nodes:

- DRAW => every nodes along the path is increased by Value::DRAW
- WIN => every AI node along the path is increased by Value::WIN, whereas every enemy node is increased by Value::LOOSE
- LOOSE => every AI node along the path is increased by Value::LOOSE, whereas every enemy node is increased by Value::WIN

## UML-class diagram

![UML-class diagram](resources/UML_class_dia.png)

Shows the class structure and assoziation between the classes used. Including signal and slots for the QT-based GUI.

## GUI-Design

In this project I used a rather simple GUI design consisting of a TableView. When you click in a non-empty column for edit and click somewhere else again, the TableView receives an update and it's going to place your token in that column. When it's not your turn, you cannot place any tokens. The game is stopped when a modal dialog pops up, indicating the winner.
![GUI design](resources/GUI.PNG)

## Sources and Literature

- <https://dke.maastrichtuniversity.nl/m.winands/documents/pMCTS.pdf>
- <https://de.slideshare.net/ftgaic/mcts-ai>
- <http://jeffbradberry.com/posts/2015/09/intro-to-monte-carlo-tree-search/>
- <https://en.wikipedia.org/wiki/Monte_Carlo_tree_search>

## MIT

### The MIT License

[![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](https://opensource.org/licenses/MIT)
