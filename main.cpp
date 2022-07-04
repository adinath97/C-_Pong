#include <iostream>

#include <SDL.h>
#include <stdio.h>

#include <glad/glad.h>

#include "Game.h"

int main(int argc, char** argv) {
    Game game;
    bool success = game.Initialize();

    if(success) {
        game.RunLoop();
    }

    game.ShutDown();
    return 0;
}

