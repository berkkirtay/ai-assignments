// Copyright (c) 2022 Berk Kırtay

#include "Player.h"
#include <algorithm>
#include <cmath>
#include <iostream>
#include <memory>
#include <random>
#include <vector>

Player::Player(int playerNumber, std::string playerName,
               std::shared_ptr<Board> board)
{
    this->playerNumber = playerNumber;
    this->playerName = playerName;
    this->board = board;
    numberOfPiecesLeft = 28;
}

Human::Human(int playerNumber, std::string playerName,
             std::shared_ptr<Board> board)
    : Player(playerNumber, playerName, board) {}

// Game turn of a real player. User makes a move
// and new board is printed by updating the
// players score.
void Human::gameTurn(std::shared_ptr<Player> opponent)
{
    int column;
    std::cout << playerName << " (" << currentScore
              << "): Enter the column number to "
                 "place your piece."
              << std::endl;
    std::cin >> column;
    if (column < 0 || column >= board->mainBoard[0].size())
    {
        std::cout << "Invalid index for board "
                     "column! Try again."
                  << std::endl;
        gameTurn(opponent);
        return;
    }
    auto res = board->put(playerNumber, column, board->mainBoard);
    if (res == true)
    {
        numberOfPiecesLeft--;
    }
    else
    {
        std::cout << "Given column is full! Try again." << std::endl;
        gameTurn(opponent);
        return;
    }

    std::cout << std::endl;
    board->printBoard();
    int score = board->calculateScore(playerNumber, column, board->mainBoard);
    currentScore = std::max(currentScore, score);
}

// Initializing the AI based on the level info
// given by the user.
AI::AI(int playerNumber, std::string playerName, std::shared_ptr<Board> board,
       AILevel level)
    : Player(playerNumber, playerName, board)
{
    switch (level)
    {
    case NOVICE:
        depthLimit = 2;
        heuristic = std::make_shared<H1>(board);
        this->playerName += " NOVICE";
        break;
    case REGULAR:
        depthLimit = 5;
        heuristic = std::make_shared<H1>(board);
        this->playerName += " REGULAR";
        break;
    case HARDENED:
        depthLimit = 5;
        heuristic = std::make_shared<H2>(board);
        this->playerName += " HARDENED";
        break;
    case VETERAN:
        depthLimit = 8;
        heuristic = std::make_shared<H2>(board);
        this->playerName += " VETERAN";
        break;
    case GODLIKE:
        depthLimit = 8;
        heuristic = std::make_shared<H3>(board);
        this->playerName += " GODLIKE";
        break;
    default:
        break;
    }
}

/*
 * AI game turn: After running minimax
 * algorithm on the current state and evaluating
 * the branches, AI selects the best move
 * available.
 */
void AI::gameTurn(std::shared_ptr<Player> opponent)
{
    std::cout << playerName << " (" << currentScore << "): Playing... "
              << std::endl;
    int column = alphaBetaSearch(board->mainBoard, opponent);
    auto res = board->put(playerNumber, column, board->mainBoard);
    if (res == true)
    {
        numberOfPiecesLeft--;
    }
    else
    {
        // If AI gets stuck in a full column, it should
        // manually put a piece to a column and continue.
        // This can happen when two AIs with same levels
        // play against each other. This will lead the first
        // AI to a win against the second AI because of the
        // advantage of first move.
        for (int i = 0; i < board->mainBoard[0].size(); i++)
        {
            auto res = board->put(playerNumber, i, board->mainBoard);
            if (res == true)
            {
                numberOfPiecesLeft--;
                break;
            }
        }
    }
    std::cout << std::endl;
    board->printBoard();
    int score = board->calculateScore(playerNumber, column, board->mainBoard);
    currentScore = std::max(currentScore, score);
}

/*
 * Minimax algorithm with a-b pruning
 * implementation: With the given depth limit, we
 * run the minimax algorithm. In the maxValue
 * function, we select the best branch and set its
 * state to the variable named chosenColumnByAI.
 */
int AI::alphaBetaSearch(std::vector<std::vector<int>> state,
                        std::shared_ptr<Player> opponent)
{
    chosenColumnByAI = 0;
    maxVal = INT32_MIN;
    maxValue(state, INT32_MIN, INT32_MAX, 0, depthLimit, 0, opponent);
    return chosenColumnByAI;
}

// For the recursive minimax functions, terminal states are determined
// by checking if the tree reached to the max depth and by checking
// if player or opponent has finished 4 rows.
int AI::maxValue(std::vector<std::vector<int>> &state, int a, int b, int depth,
                 int maxDepth, int lastColumnChange,
                 std::shared_ptr<Player> opponent)
{
    if (depth >= maxDepth ||
        board->calculateScore(playerNumber, lastColumnChange, state) >= 4 ||
        board->calculateScore(opponent->playerNumber, lastColumnChange, state) >= 4)
    {
        return evaluationFunction(state, opponent);
    }

    int v = INT32_MIN;
    for (int i = 0; i < state[0].size(); i++)
    {
        std::vector<std::vector<int>> tempState = state;
        auto res = board->put(playerNumber, i, tempState);
        if (res == false)
        {
            v = INT32_MIN;
            continue;
        }
        v = std::max(v, minValue(tempState, a, b, depth + 1, maxDepth, i, opponent));
        // Here, we select the most promising branch at
        // the first depth of the search tree and
        // assign it to the variable chosenColumnByAI.
        if (depth == 0 && maxVal < v)
        {
            maxVal = v;
            chosenColumnByAI = i;
        }
        if (v >= b)
            return v;
        a = std::max(a, v);
    }
    return v;
}

int AI::minValue(std::vector<std::vector<int>> &state, int a, int b, int depth,
                 int maxDepth, int lastColumnChange,
                 std::shared_ptr<Player> opponent)
{
    if (depth >= maxDepth ||
        board->calculateScore(playerNumber, lastColumnChange, state) >= 4 ||
        board->calculateScore(opponent->playerNumber, lastColumnChange, state) >= 4)
    {
        return evaluationFunction(state, opponent);
    }
    int v = INT32_MAX;
    for (int i = 0; i < state[0].size(); i++)
    {
        std::vector<std::vector<int>> tempState = state;
        auto res = board->put(opponent->playerNumber, i, tempState);
        if (res == false)
        {
            continue;
        }
        v = std::min(v, maxValue(tempState, a, b, depth + 1, maxDepth, i, opponent));
        if (v <= a)
            return v;
        b = std::min(b, v);
    }
    return v;
}
// @evaluationFunction calls the heuristic
// function to evaluate the given state.
int AI::evaluationFunction(std::vector<std::vector<int>> &state,
                           std::shared_ptr<Player> opponent)
{
    return heuristic->utility(state, playerNumber, opponent->playerNumber);
}
