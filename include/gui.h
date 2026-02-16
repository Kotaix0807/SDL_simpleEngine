/**
 * @file gui.h
 * @brief Interfaz publica del modulo GUI basado en Nuklear + SDL2.
 *
 * Proporciona funciones para inicializar, procesar eventos, renderizar
 * y destruir el contexto de interfaz grafica inmediata (Nuklear).
 */

#ifndef GUI_H
#define GUI_H

// ============================================================
// Includes
// ============================================================
#include <SDL.h>
#include <stdbool.h>

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wpedantic"
#define NK_INCLUDE_FIXED_TYPES
#define NK_INCLUDE_STANDARD_IO
#define NK_INCLUDE_STANDARD_VARARGS
#define NK_INCLUDE_DEFAULT_ALLOCATOR
#define NK_INCLUDE_VERTEX_BUFFER_OUTPUT
#define NK_INCLUDE_FONT_BAKING
#define NK_INCLUDE_DEFAULT_FONT
#include "nuklear.h"
#pragma GCC diagnostic pop

// ============================================================
// Declaraciones forward
// ============================================================
struct nk_context;

// ============================================================
// Funciones publicas
// ============================================================

/**
 * @brief Inicializa el subsistema GUI (Nuklear + SDL2 renderer).
 *
 * Crea el contexto Nuklear, carga la fuente indicada y la establece
 * como fuente por defecto.
 *
 * @param win       Ventana SDL sobre la que se dibujara la GUI.
 * @param ren       Renderer SDL asociado a la ventana.
 * @param font_path Ruta al archivo de fuente TTF (puede ser NULL para fuente por defecto).
 * @param font_size Tamano en puntos de la fuente.
 * @return true si la inicializacion fue exitosa, false en caso contrario.
 */
bool GUI_Init(SDL_Window *win, SDL_Renderer *ren, const char *font_path, float font_size);

/**
 * @brief Pasa un evento SDL al sistema de entrada de Nuklear.
 *
 * @param event Puntero al evento SDL a procesar.
 * @return Distinto de 0 si Nuklear consumio el evento.
 */
int GUI_HandleEvent(SDL_Event *event);

/**
 * @brief Inicia la captura de entrada de Nuklear para el frame actual.
 */
void GUI_InputBegin(void);

/**
 * @brief Finaliza la captura de entrada de Nuklear para el frame actual.
 */
void GUI_InputEnd(void);

/**
 * @brief Renderiza todos los comandos de dibujo acumulados por Nuklear.
 */
void GUI_Render(void);

/**
 * @brief Libera todos los recursos del subsistema GUI.
 */
void GUI_Destroy(void);

/**
 * @brief Obtiene el contexto Nuklear activo.
 *
 * @return Puntero al struct nk_context, o NULL si la GUI no esta inicializada.
 */
struct nk_context* GUI_GetContext(void);

#endif
