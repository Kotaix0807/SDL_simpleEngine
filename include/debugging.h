/**
 * @file debugging.h
 * @brief Interfaz publica del modulo de depuracion.
 *
 * Proporciona herramientas de depuracion visual: un selector de frames
 * sobre spritesheets, metricas de rendimiento (FPS, CPU, memoria),
 * debug de fuentes TTF y un menu centralizado para activar cada herramienta.
 *
 * Internamente gestiona sus propios estados; la API publica se reduce
 * a manejo de eventos, renderizado y limpieza.
 */

#ifndef DEBUGGING_H
#define DEBUGGING_H

#include "SDL_events.h"

// ============================================================
// API publica
// ============================================================

/**
 * @brief Gestiona los eventos SDL para el modulo de depuracion.
 *
 * Procesa eventos de teclado, mouse y rueda para controlar el
 * frame debug y el menu:
 * - F3: toggle del menu de depuracion.
 * - Flechas: mover el selector de frame (con wrap-around).
 * - Click izquierdo + arrastre: desplazar la imagen (pan).
 * - Rueda del mouse: zoom (0.1x - 10.0x).
 *
 * @param event Evento SDL a procesar.
 */
void handleDebugEvent(SDL_Event event);

/**
 * @brief Renderiza todos los modulos de depuracion activos.
 *
 * Llama en orden a los renders de frame debug, metricas de
 * rendimiento, font debug y debug menu.
 */
void renderDebug(void);

/**
 * @brief Libera todos los recursos de depuracion.
 *
 * Cierra frame debug, font debug y desactiva todos los modulos.
 * Llamar desde Game_Destroy().
 */
void exitDebug(void);

#endif
