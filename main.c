//#define MAIN_DEBUG
#ifndef MAIN_DEBUG

#include "engine.h"

int main()
{
    INSTANCE = Game_Init();
    if(INSTANCE)
    {
        Game_Setup();
        while(INSTANCE)
        {
            Game_KeyboardInput();
            Game_UpdateFrame();
            Game_Render();
        }
    }
    else
    {
        perror("No se pudo iniciar el programa");
        exit(EXIT_FAILURE);
    }
	Game_Destroy();
    return 0;
}

#else

int main()
{
    return 0;
}

#endif