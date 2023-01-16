// Copyright (c) 2022 Berk Kırtay

#pragma once
#include <vector>
#include <iostream>
#include <cmath>
#include <stack>
#include <queue>
#include <algorithm>
#include <random>
#include <chrono>
#include <memory>

class Board
{
public:
    std::vector<std::vector<int>> mainBoard;
    void initializeBoard();
    void printBoard();
    bool put(int playerNumber, int pos, std::vector<std::vector<int>> &board);
    int calculateContiguousRows(int playerNumber, std::vector<std::vector<int>> &board, int count);
    int calculateScoreWithAllIndexes(int playerNumber, std::vector<std::vector<int>> &board);
    int calculateScore(int playerNumber, int column, std::vector<std::vector<int>> &board);

    // Looking vertical:
    int lookVertical(int playerNumber, std::pair<int, int> pos, std::vector<std::vector<int>> &board);

    // Looking horizontal:
    int lookHorizontal(int playerNumber, std::pair<int, int> pos, std::vector<std::vector<int>> &board);

    // Looking diagonal:
    int lookDiagonal(int playerNumber, std::pair<int, int> pos, std::vector<std::vector<int>> &board);

    // Looking reverse diagonal:
    int lookReverseDiagonal(int playerNumber, std::pair<int, int> pos, std::vector<std::vector<int>> &board);
};
