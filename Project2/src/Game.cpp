// Copyright (c) 2022 Berk Kırtay

#include "Game.h"

void Game::initializeGame(GameType gameType)
{
    board = std::make_shared<Board>();
    board->initializeBoard();
    if (gameType == HUMAN_VS_HUMAN)
    {
        firstPlayer = std::make_shared<Human>(0, "Player 1", board);
        secondPlayer = std::make_shared<Human>(1, "Player 2", board);
    }
    else if (gameType == HUMAN_VS_AI)
    {
        std::cout << "Please specify the AI level:" << std::endl;
        auto level = selectGameLevel();
        firstPlayer = std::make_shared<Human>(0, "Player 1", board);
        secondPlayer = std::make_shared<AI>(1, "AI", board, level);
    }
    else
    {
        std::cout << "Please specify the level of the first AI:" << std::endl;
        auto level = selectGameLevel();
        firstPlayer = std::make_shared<AI>(0, "AI 1", board, level);
        std::cout << "Please specify the level of the second AI:" << std::endl;
        level = selectGameLevel();
        secondPlayer = std::make_shared<AI>(1, "AI 2", board, level);
    }
    players = {firstPlayer, secondPlayer};
    board->printBoard();
}

// @selectGameLevel Initializes the AI by asking the level info to the user:
AILevel Game::selectGameLevel()
{
    std::cout << "1 for NOVICE" << std::endl;
    std::cout << "2 for REGULAR" << std::endl;
    std::cout << "3 for HARDENED" << std::endl;
    std::cout << "4 for VETERAN" << std::endl;
    std::cout << "5 for GODLIKE" << std::endl;
    int choice;
    std::cin >> choice;

    AILevel level{};
    switch (choice)
    {
    case 1:
        level = NOVICE;
        break;
    case 2:
        level = REGULAR;
        break;
    case 3:
        level = HARDENED;
        break;
    case 4:
        level = VETERAN;
        break;
    case 5:
        level = GODLIKE;
        break;
    default:
        std::cout << "Wrong value is entered! Exiting..";
        exit(EXIT_FAILURE);
        break;
    }
    return level;
}

// @gameLoop lets players play in turns.
void Game::gameLoop()
{
    while (firstPlayer->numberOfPiecesLeft + secondPlayer->numberOfPiecesLeft > 0)
    {
        players[playingPlayer]->gameTurn(players[1 - playingPlayer]);
        if (players[playingPlayer]->currentScore >= 4)
        {
            std::cout << players[playingPlayer]->playerName << " won." << std::endl;
            return;
        }
        playingPlayer = 1 - playingPlayer;
    }
    std::cout << "Game draw." << std::endl;
}