/**
 * @file engine.c
 * @brief Implementacion del motor principal. Ciclo de vida, input, render y framerate.
 */

// ============================================================
// Includes
// ============================================================
#include "SDL_video.h"
#include <SDL_mixer.h>
#include <SDL_image.h>

//#define ARDUINO_ON

#ifdef ARDUINO_ON
#include "arduino.h"
#endif
#include "gui.h"
#include "engine.h"
#include "config.h"
#include "img.h"
#include "sound.h"
#include "tools.h"
#include "debugging.h"
#include "text.h"

// ============================================================
// Variables
// ============================================================

// -- Publicas (accesibles via extern en engine.h) --
bool INSTANCE = true;
int last_frame = 0;
float deltatime = 0.0f;
SDL_Renderer *render = NULL;
SDL_Window *window = NULL;
SDL_Color renderColor = {
	.r = 0,
	.g = 0,
	.b = 0,
	.a = 255
};

int MouseX = 0;
int MouseY = 0;

TTF_Font *font = NULL;

// ============================================================
// Funciones publicas - Ciclo de vida
// ============================================================

// Inicializa SDL, ventana, render, audio, texto y GUI.
// Orden: config -> SDL -> IMG/Audio -> ventana -> render -> TTF -> Text -> GUI -> Arduino.
bool Game_Init()
{
	// Cargar configuracion desde archivo .ini
	if(loadConfig(&config, CONFIG_DIR CFG_FILE) != true)
		return false;

	Uint32 windowFlags = SDL_WINDOW_RESIZABLE | (config.fullscreen ? SDL_WINDOW_FULLSCREEN : 0);

	// Iniciar SDL (video)
	if (SDL_Init(SDL_INIT_VIDEO) != 0)
	{
		printDebug("No se pudo iniciar SDL: %s\n", SDL_GetError());
		return false;
	}

	// Iniciar subsistemas de textura y audio
	initTexture();
	initAudio();

	// Validar monitor: si no existe el configurado, usar el default (0)
	if(SDL_GetNumVideoDisplays() < config.defaultMonitor)
		config.defaultMonitor = 0;

	// Crear ventana
	window = SDL_CreateWindow(config.name, SDL_WINDOWPOS_CENTERED_DISPLAY(config.defaultMonitor), SDL_WINDOWPOS_CENTERED_DISPLAY(config.defaultMonitor), config.WIN_W, config.WIN_H, windowFlags);
	if (!window)
	{
		printDebug("No se pudo crear ventana: %s\n", SDL_GetError());
		return false;
	}

	// Crear renderer con aceleracion por hardware (y vsync si esta habilitado en config)
	Uint32 renderFlags = SDL_RENDERER_ACCELERATED | (config.vsync ? SDL_RENDERER_PRESENTVSYNC : 0);
	render = SDL_CreateRenderer(window, -1, renderFlags);
	if (!render)
	{
		printDebug("No se pudo crear render: %s\n", SDL_GetError());
		return false;
	}

	// Escala inicial 1:1 (ventana arranca al tamanho configurado)
	SDL_RenderSetScale(render, 1.0f, 1.0f);

	// Iniciar SDL_ttf
	if (TTF_Init() == -1)
	{
		printDebug("No se pudo iniciar TTF: %s\n", TTF_GetError());
		return false;
	}

	// Iniciar sistema de texto
	if (!Text_InitSystem(FONTS_DIR JERSEY_FONT, 24))
		return false;

	// Iniciar GUI (Nuklear)
	if (!GUI_Init(window, render, FONTS_DIR JERSEY_FONT, 30))
	{
		printDebug("No se pudo iniciar GUI\n");
		return false;
	}

	#ifdef ARDUINO_ON
	if (!arduinoConnect())
	{
		printDebug("No se pudo conectar con Arduino (continuando sin el)\n");
	}
	#endif

	return true;
}

// Crea los textos del HUD (FPS, mouse).
void Game_Setup()
{
	// Setup del juego
}

// Procesa eventos SDL: cierre, teclas, mouse.
// Delega a GUI y debugging antes de procesar los propios.
void Game_KeyboardInput()
{
	SDL_Event event;
	GUI_InputBegin();
	while (SDL_PollEvent(&event))
	{
		GUI_HandleEvent(&event);
		keyEventHandlerDebug(event);
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
				if(KEY == SDLK_F11)
				{
					config.fullscreen = !config.fullscreen;
					SDL_SetWindowFullscreen(window, config.fullscreen ? SDL_WINDOW_FULLSCREEN : 0);
				}
				break;
			}
			case(SDL_KEYUP):
			{
				break;
			}
			case(SDL_MOUSEWHEEL):
			{
				break;
			}
			case(SDL_WINDOWEVENT):
			{
				if (event.window.event == SDL_WINDOWEVENT_RESIZED)
				{
					float sx = (float)event.window.data1 / config.WIN_W; // ej: 1920 / 960 = 2.0 <- Escala horizontal del juego
					float sy = (float)event.window.data2 / config.WIN_H; // Escala vertical del juego
					SDL_RenderSetScale(render, sx, sy);
				}
				break;
			}
			default:
				break;
		}
	}
	GUI_InputEnd();
}

// Calcula deltatime y espera el tiempo restante para cumplir el framerate objetivo.
void Game_UpdateFrame()
{
	Uint32 actualTime = SDL_GetTicks();
	deltatime = (actualTime - last_frame) / 1000.0f;
	last_frame = actualTime;

	SDL_GetMouseState(&MouseX, &MouseY);

	float sx, sy;
	SDL_RenderGetScale(render, &sx, &sy);
	MouseX = (int)(MouseX / sx);
	MouseY = (int)(MouseY / sy);

	int WaitTime = FRAME_TIME_MS(config.fps) - (SDL_GetTicks() - actualTime);
	if (WaitTime > 0 && WaitTime <= FRAME_TIME_MS(config.fps))
		SDL_Delay(WaitTime);
}

// Limpia pantalla, dibuja debug/HUD/GUI y presenta el frame.
void Game_Render()
{
	SDL_SetRenderDrawColor(render, 0, 0, 0, 255);
	SDL_RenderClear(render);

	renderFrameDebug();

	renderPerfMetrics();

	renderDebugMenu();

	GUI_Render();
	SDL_RenderPresent(render);
}

// Libera todos los recursos en orden inverso a la inicializacion.
void Game_Destroy()
{
	Text_QuitSystem();
	exitFrameDebug();

	#ifdef ARDUINO_ON
	arduinoDisconnect();
	#endif

	GUI_Destroy();

	SDL_DestroyRenderer(render);
	SDL_DestroyWindow(window);

	quitTexture();
	quitAudio();
	SDL_Quit();
}
