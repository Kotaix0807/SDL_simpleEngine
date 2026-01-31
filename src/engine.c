#include <SDL_mixer.h>
#include <SDL_image.h>

#include "arduino.h"
#include "engine.h"
#include "SDL_mouse.h"
#include "SDL_render.h"
#include "config.h"
#include "img.h"
#include "sound.h"
#include "tools.h"

bool INSTANCE = true;
int last_frame = 0;
float deltatime = 0.0f;
SDL_Renderer *render = NULL;
SDL_Window *window = NULL;
texture fotos = {0};

int MouseX = 0;
int MouseY = 0;

TTF_Font *font = NULL;

bool Game_Init()
{
	if(loadConfig(&config, CONFIG_DIR CFG_FILE) != true)
		return false;
	Uint32 windowFlags = config.fullscreen ? SDL_WINDOW_FULLSCREEN : 0;
    if (SDL_Init(SDL_INIT_VIDEO) != 0)
	{
		printDebug("No se pudo iniciar SDL: %s\n", SDL_GetError());
		return false;
	}
	initTexture();
    initAudio();
	window = SDL_CreateWindow(config.name, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, config.WIN_W, config.WIN_H, windowFlags);
	if (!window)
	{
		printDebug("No se pudo crear ventana: %s\n", SDL_GetError());
		return false;
	}
	render = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
	if (!render)
	{
		printDebug("No se pudo crear render: %s\n", SDL_GetError());
		return false;
	}
	fotos = initTextureLib(SPRITES_DIR);
	if (TTF_Init() == -1)
	{
		printDebug("No se pudo iniciar TTF: %s\n", TTF_GetError());
		return false;
	}
	if (!arduinoConnect())
	{
		printDebug("No se pudo conectar con Arduino (continuando sin él)\n");
	}
	return true;
}

void Game_Setup()
{
	return;
}
void Game_KeyboardInput()
{
	SDL_Event event;
	while (SDL_PollEvent(&event))
	{
		switch (event.type)
		{
			case(SDL_QUIT):
			{
				INSTANCE = false;
				break;
			}
			case(SDL_KEYDOWN):
			{
				SDL_Keycode KEY = event.key.keysym.sym;

				if(KEY == SDLK_ESCAPE)
					INSTANCE = false;
				break;
			}
			case(SDL_KEYUP):
			{
				//SDL_Keycode KEY = event.key.keysym.sym;


				break;
			}
			case(SDL_MOUSEWHEEL):
			{ 
				break;
			}
			default:
				break;
		}
	}
}
void Game_UpdateFrame()
{
	Uint32 actualTime = SDL_GetTicks();
	deltatime = (actualTime - last_frame) / 1000.0f;
	last_frame = actualTime;


	int WaitTime = FRAME_TIME_MS(config.fps) - (SDL_GetTicks() - actualTime);
	if (WaitTime > 0 && WaitTime <= FRAME_TIME_MS(config.fps))
		SDL_Delay(WaitTime);
}

void Game_Render()
{
	SDL_SetRenderDrawColor(render, 0, 0, 0, 255);
	SDL_RenderClear(render);

	SDL_GetMouseState(&MouseX, &MouseY);
	SDL_RenderPresent(render);
}

void Game_Destroy()
{
	arduinoDisconnect();
    SDL_DestroyRenderer(render);
    SDL_DestroyWindow(window);

	freeTextureLib(&fotos);
	quitTexture();
	quitAudio();
	TTF_Quit();
	SDL_QuitSubSystem(SDL_INIT_VIDEO);
    SDL_Quit();
}