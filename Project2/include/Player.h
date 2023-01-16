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
#include "Enum.h"
#include "Heuristic.h"

class Player
{
public:
    int playerNumber;
    int numberOfPiecesLeft;
    int currentScore = 0;
    int tempScore = 0;
    std::string playerName;
    std::shared_ptr<Board> board;

    Player(int playerNumber, std::string playerName, std::shared_ptr<Board> board);
    virtual void gameTurn(std::shared_ptr<Player> opponent) = 0;
};

class Human : virtual public Player
{
public:
    Human(int playerNumber, std::string playerName, std::shared_ptr<Board> board);
    void gameTurn(std::shared_ptr<Player> opponent);
};

class AI : virtual public Player
{
private:
    int depthLimit = 5;
    int chosenColumnByAI = 0;
    int maxVal = INT32_MIN;
    std::shared_ptr<Heuristic> heuristic;

public:
    AI(int playerNumber, std::string playerName, std::shared_ptr<Board> board, AILevel level);
    void gameTurn(std::shared_ptr<Player> opponent);
    int alphaBetaSearch(std::vector<std::vector<int>> state, std::shared_ptr<Player> opponent);
    int maxValue(std::vector<std::vector<int>> &state,
                 int a, int b, int depth, int maxDepth, int lastColumnChange, std::shared_ptr<Player> opponent);
    int minValue(std::vector<std::vector<int>> &state,
                 int a, int b, int depth, int maxDepth, int lastColumnChange, std::shared_ptr<Player> opponent);
    int evaluationFunction(std::vector<std::vector<int>> &state, std::shared_ptr<Player> opponent);
};