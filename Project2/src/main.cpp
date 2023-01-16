// Copyright (c) 2022 Berk Kırtay

#include "Game.h"

int main(int argc, char **argv)
{
    Game game;
    std::cout << "Please choose a game type:" << std::endl;
    std::cout << "1 for Player vs Player" << std::endl;
    std::cout << "2 for Player vs AI" << std::endl;
    std::cout << "3 for AI vs AI" << std::endl;
    int choice;
    std::cin >> choice;
    GameType type{};
    switch (choice)
    {
    case 1:
        type = HUMAN_VS_HUMAN;
        break;
    case 2:
        type = HUMAN_VS_AI;
        break;
    case 3:
        type = AI_VS_AI;
        break;
    default:
        std::cout << "Wrong value is entered! Exiting..";
        exit(EXIT_FAILURE);
        break;
    }
    game.initializeGame(type);
    game.gameLoop();
}
