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
#include "Board.h"

class Heuristic
{
public:
    std::shared_ptr<Board> board;
    virtual int utility(std::vector<std::vector<int>> &state, int playerNumber, int opponentNumber) = 0;
};

class H1 : virtual public Heuristic
{
public:
    H1(std::shared_ptr<Board> board);
    int utility(std::vector<std::vector<int>> &state, int playerNumber, int opponentNumber);
};

class H2 : virtual public Heuristic
{
public:
    H2(std::shared_ptr<Board> board);
    int utility(std::vector<std::vector<int>> &state, int playerNumber, int opponentNumber);
};

class H3 : virtual public Heuristic
{
public:
    H3(std::shared_ptr<Board> board);
    int utility(std::vector<std::vector<int>> &state, int playerNumber, int opponentNumber);
};