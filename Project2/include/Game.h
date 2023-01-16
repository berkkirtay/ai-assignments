// Copyright (c) 2022 Berk Kırtay

#pragma once
#include <vector>
#include <iostream>
#include <cmath>
#include <algorithm>
#include <random>
#include <memory>
#include "Player.h"
#include "Board.h"
#include "Enum.h"

class Game
{
private:
    std::shared_ptr<Player> firstPlayer;
    std::shared_ptr<Player> secondPlayer;
    std::vector<std::shared_ptr<Player>> players;
    std::shared_ptr<Board> board;
    int playingPlayer = 0;

public:
    void initializeGame(GameType gameType);
    AILevel selectGameLevel();
    void gameLoop();
};