// Copyright (c) 2022 Berk Kırtay

#include "Board.h"

void Board::initializeBoard()
{
    for (int i = 0; i < 7; i++)
    {
        mainBoard.push_back(std::vector<int>(8, -1));
    }
}

void Board::printBoard()
{
    for (int j = 0; j <= mainBoard[0].size() * 2; j++)
    {
        std::cout << "-";
    }
    std::cout << std::endl;
    for (int i = 0; i < mainBoard.size(); i++)
    {
        for (int j = 0; j < mainBoard[0].size(); j++)
        {
            std::cout << "|";
            if (mainBoard[i][j] == -1)
                std::cout << " ";
            else
            {
                if (mainBoard[i][j] == 0)
                    std::cout << "+";
                else if (mainBoard[i][j] == 1)
                    std::cout << "-";
            }
            if (j == mainBoard[0].size() - 1)
                std::cout << "|";
            ;
        }
        std::cout << std::endl;
    }
    for (int j = 0; j <= mainBoard[0].size() * 2; j++)
    {
        std::cout << "-";
    }
    std::cout << std::endl;
}

// @put Searchs for a free space in a given column and puts the piece.
bool Board::put(int playerNumber, int pos, std::vector<std::vector<int>> &board)
{
    for (int i = board.size() - 1; i >= 0; i--)
    {
        if (board[i][pos] == -1)
        {
            board[i][pos] = playerNumber;
            return true;
        }
    }
    return false;
}

int Board::calculateContiguousRows(int playerNumber, std::vector<std::vector<int>> &board, int count)
{
    int res = 0;
    for (int i = 0; i < board.size(); i++)
    {
        for (int j = 0; j < board[0].size(); j++)
        {
            bool isFound = false;
            if (board[i][j] == playerNumber)
            {
                if (lookVertical(playerNumber, std::make_pair(i, j), board) == count)
                {
                    isFound = true;
                    res++;
                }
                if (lookHorizontal(playerNumber, std::make_pair(i, j), board) == count)
                {
                    isFound = true;
                    res++;
                }
                if (lookDiagonal(playerNumber, std::make_pair(i, j), board) == count)
                {
                    isFound = true;
                    res++;
                }
                if (lookReverseDiagonal(playerNumber, std::make_pair(i, j), board) == count)
                {
                    isFound = true;
                    res++;
                }
            }
            if (isFound == true)
            {
                if (j + count < board[0].size())
                {
                    j += count;
                }
                else
                {
                    break;
                }
            }
        }
    }
    return res;
}

int Board::calculateScoreWithAllIndexes(int playerNumber, std::vector<std::vector<int>> &board)
{
    int score = 0;
    for (int i = 0; i < board[0].size(); i++)
    {
        score = std::max(score, calculateScore(playerNumber, i, board));
    }
    return score;
}
// @calculateScore calculates the scores of the players pieces in the given column
// by checking vertically, horizontally and diagonally.
int Board::calculateScore(int playerNumber, int column, std::vector<std::vector<int>> &board)
{
    int score = 0;
    int row = 0;
    for (int i = 0; i < board.size(); i++)
    {
        if (board[i][column] == playerNumber)
        {
            row = i;
            break;
        }
    }

    score = std::max(score, lookVertical(playerNumber, std::make_pair(row, column), board));
    score = std::max(score, lookHorizontal(playerNumber, std::make_pair(row, column), board));
    score = std::max(score, lookDiagonal(playerNumber, std::make_pair(row, column), board));
    return std::max(score, lookReverseDiagonal(playerNumber, std::make_pair(row, column), board));
}

// Looking vertical:
int Board::lookVertical(int playerNumber, std::pair<int, int> pos, std::vector<std::vector<int>> &board)
{
    int score = 0;
    int i = pos.first;
    while (i >= 0)
    {
        if (board[i][pos.second] != playerNumber)
        {
            break;
        }
        i--;
    }
    i++;
    while (i < board.size())
    {
        if (board[i][pos.second] != playerNumber)
        {
            break;
        }
        i++;
        score++;
    }
    return score;
}

// Looking horizontal:
int Board::lookHorizontal(int playerNumber, std::pair<int, int> pos, std::vector<std::vector<int>> &board)
{
    int score = 0;
    int i = pos.second;
    while (i >= 0)
    {
        if (board[pos.first][i] != playerNumber)
        {
            break;
        }
        i--;
    }
    i++;
    while (i < board[0].size())
    {
        if (board[pos.first][i] != playerNumber)
        {
            break;
        }
        i++;
        score++;
    }
    return score;
}

// Looking diagonal:
int Board::lookDiagonal(int playerNumber, std::pair<int, int> pos, std::vector<std::vector<int>> &board)
{
    int score = 0;
    int i = pos.first;
    int j = pos.second;
    while (i >= 0 && j >= 0)
    {
        if (board[i][j] != playerNumber)
        {
            break;
        }
        i--;
        j--;
    }
    i++;
    j++;
    while (i < board.size() && j < board[0].size())
    {
        if (board[i][j] != playerNumber)
        {
            break;
        }
        i++;
        j++;
        score++;
    }
    return score;
}

// Looking reverse diagonal:
int Board::lookReverseDiagonal(int playerNumber, std::pair<int, int> pos, std::vector<std::vector<int>> &board)
{
    int score = 0;
    int i = pos.first;
    int j = pos.second;
    while (i >= 0 && j < board[0].size())
    {
        if (board[i][j] != playerNumber)
        {
            break;
        }
        i--;
        j++;
    }
    i++;
    j--;
    while (i < board.size() && j >= 0)
    {
        if (board[i][j] != playerNumber)
        {
            break;
        }
        i++;
        j--;
        score++;
    }
    return score;
}
