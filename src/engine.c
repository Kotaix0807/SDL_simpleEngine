#include "engine.h"
#include "SDL_mixer.h"
#include "SDL_video.h"
#include "config.h"
#include "sound.h"

bool INSTANCE = true;
int Last_frame = 0;
float deltatime = 0.0f;
SDL_Renderer *render = NULL;
SDL_Window *window = NULL;

int MouseX = 0;
int MouseY = 0;

sfx *MarioLib = NULL;

TTF_Font *font = NULL;

bool Game_Init()
{
	if(loadConfig(&config, CONFIG_PATH CFG_FILE) == false)
		return false;
	Uint32 windowFlags = config.fullscreen ? SDL_WINDOW_FULLSCREEN : 0;
    if (SDL_Init(SDL_INIT_VIDEO) != 0)
	{
		fprintf(stderr, "No se pudo iniciar SDL: %s\n", SDL_GetError());
		return false;
	}
	if (IMG_Init(IMG_INIT_PNG) == 0)
	{
		fprintf(stderr, "No se pudo iniciar IMG: %s\n", SDL_GetError());
		return false;
	}
    initAudio();
	window = SDL_CreateWindow(config.name, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, config.WIN_W, config.WIN_H, windowFlags);
	if (!window)
	{
		fprintf(stderr, "No se pudo crear ventana: %s\n", SDL_GetError());
		return false;
	}
	render = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
	if (!render)
	{
		fprintf(stderr, "No se pudo crear render: %s\n", SDL_GetError());
		return false;
	}
	if (TTF_Init() == -1)
	{
		fprintf(stderr, "No se pudo iniciar TTF: %s\n", TTF_GetError());
		return false;
	}
	/*if(!loadConfig(&config, CONFIG_PATH CFG_FILE))
	{
		fprintf(stderr, "Fallo al cargar archivo de configuracion '%s'", CFG_FILE);
		return false;
	}*/
	return true;
}

void Game_Setup()
{
	MarioLib = initSfxLib(SFX_DIR);
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
				if(KEY >= SDLK_1 && KEY <= SDLK_9)
				{
					int index = KEY - SDLK_1;
					if(MarioLib && index < MarioLib->n && MarioLib->chunks[index])
						Mix_PlayChannel(-1, MarioLib->chunks[index], 0);
				}
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
	deltatime = (actualTime - Last_frame) / 1000.0f;
	Last_frame = actualTime;

	//Eventos aqui.


	int WaitTime = FRAME_TIME_MS - (SDL_GetTicks() - actualTime);
	if (WaitTime > 0 && WaitTime <= FRAME_TIME_MS)
		SDL_Delay(WaitTime);
}
void Game_Render()
{
	SDL_SetRenderDrawColor(render, 0, 0, 0, 255);
	SDL_RenderClear(render);

	//Render otros aqui.

	SDL_RenderPresent(render);
}
void Game_Destroy()
{
    SDL_DestroyRenderer(render);
    SDL_DestroyWindow(window);

	freeSfxLib(MarioLib);
    quitAudio();
	MarioLib = NULL;

    IMG_Quit();
	TTF_Quit();
	SDL_QuitSubSystem(SDL_INIT_VIDEO);
    SDL_Quit();
}