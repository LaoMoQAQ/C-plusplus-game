#include <SDL.h>

#include "core/Game.h"


// [新增] Windows 专有头文件条件编译
#ifdef _WIN32
#include <windows.h>
#endif


int main(
    int argc,
    char* argv[]
)
{

    // [新增] 控制台 UTF-8 只在 Windows 需要
#ifdef _WIN32
    SetConsoleOutputCP(CP_UTF8);
    SetConsoleCP(CP_UTF8);
#endif


    Game game;

    if(!game.Init())
    {
        return -1;
    }

    game.Run();

    return 0;
}