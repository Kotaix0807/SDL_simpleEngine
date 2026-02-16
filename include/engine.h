/**
 * @file engine.h
 * @brief Motor principal del juego. Ciclo de vida, ventana, render y variables globales.
 */
#ifndef ENGINE_H
#define ENGINE_H

#include "SDL_pixels.h"
#include <SDL.h>
#include <SDL_ttf.h>
#include <SDL_mixer.h>
#include <stdbool.h>

// ============================================================
// Variables globales del motor
// ============================================================

extern bool INSTANCE;              /**< @brief Controla el game loop. false = salir. */
extern int last_frame;             /**< @brief Timestamp del ultimo frame (en ms). */
extern float deltatime;            /**< @brief Tiempo entre frames (en segundos). */
extern SDL_Renderer *render;       /**< @brief Renderer principal de SDL. */
extern SDL_Window *window;         /**< @brief Ventana principal de SDL. */
extern SDL_Color renderColor;      /**< @brief Color de fondo del render. */

extern int MouseX;                 /**< @brief Posicion X del mouse. */
extern int MouseY;                 /**< @brief Posicion Y del mouse. */

extern TTF_Font *font;             /**< @brief Fuente global (legacy). */

// ============================================================
// Ciclo de vida del juego
// ============================================================

/**
 * @brief Inicializa todos los subsistemas: SDL, ventana, render, audio, texto, GUI.
 * @return true si todo se inicializo correctamente, false en caso de error.
 */
bool Game_Init();

/**
 * @brief Configura los elementos iniciales del juego (textos HUD, etc).
 */
void Game_Setup();

/**
 * @brief Procesa los eventos de teclado, mouse y ventana.
 */
void Game_KeyboardInput();

/**
 * @brief Calcula deltatime y controla el framerate.
 */
void Game_UpdateFrame();

/**
 * @brief Renderiza un frame completo: limpia, dibuja, presenta.
 */
void Game_Render();

/**
 * @brief Libera todos los recursos y cierra los subsistemas.
 */
void Game_Destroy();

#endif
