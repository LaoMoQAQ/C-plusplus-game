#include <windows.h>

#include <SDL.h>

#include "core/Game.h"


int main(
    int argc,
    char* argv[]
)
{

    SetConsoleOutputCP(CP_UTF8);
    SetConsoleCP(CP_UTF8);

    Game game;


    if(!game.Init())
    {

        return -1;

    }



    game.Run();



    return 0;

}