// Copyright (c) 2022 Berk Kırtay

#include "Heuristic.h"

/*
 * H1 is basically returning a random integer between 0 and 10 and adds
 * it to the players score. This heuristic provides the most basic approach.
 */
H1::H1(std::shared_ptr<Board> board)
{
    this->board = board;
    srand(time(NULL));
}

int H1::utility(std::vector<std::vector<int>> &state, int playerNumber, int opponentNumber)
{
    auto score = board->calculateScoreWithAllIndexes(
        playerNumber, state);
    if (score >= 4)
        return INT32_MAX;
    return rand() % 10 + score;
}

/*
 * H2 evaluates both players and opponents board scores and subtract
 * from each other. If player reaches to a 4 contiguous row, it returns max value.
 * Similarly, if opponent reaches to a 4 contiguous row, it returns min value.
 */
H2::H2(std::shared_ptr<Board> board)
{
    this->board = board;
}

int H2::utility(std::vector<std::vector<int>> &state, int playerNumber, int opponentNumber)
{
    auto score = board->calculateScoreWithAllIndexes(
        playerNumber, state);
    if (score >= 4)
        return INT32_MAX;

    auto opponentScore = board->calculateScoreWithAllIndexes(
        opponentNumber, state);

    if (opponentScore >= 4)
        return INT32_MIN;

    return score - opponentScore;
}

/*
 * H3 evaluates every possible row in a table for both player and opponent.
 * It calculates a weighted sum based on the row sizes. In our case, those weights are
 * 0.2 for 2 row, 0.5 for 3 row and max value for 4 row.
 */
H3::H3(std::shared_ptr<Board> board)
{
    this->board = board;
}

int H3::utility(std::vector<std::vector<int>> &state, int playerNumber, int opponentNumber)
{
    auto score = board->calculateScoreWithAllIndexes(
        playerNumber, state);
    if (score >= 4)
        return INT32_MAX;

    auto opponentScore = board->calculateScoreWithAllIndexes(
        opponentNumber, state);

    if (opponentScore >= 4)
        return INT32_MIN;

    int weightedPlayerScore = 0;
    weightedPlayerScore += board->calculateContiguousRows(playerNumber, state, 2) * 0.2;
    weightedPlayerScore += board->calculateContiguousRows(playerNumber, state, 3) * 0.5;

    int weightedOpponentScore = 0;
    weightedOpponentScore += board->calculateContiguousRows(opponentNumber, state, 2) * 0.2;
    weightedOpponentScore += board->calculateContiguousRows(opponentNumber, state, 3) * 0.5;

    return weightedPlayerScore - weightedOpponentScore;
}
