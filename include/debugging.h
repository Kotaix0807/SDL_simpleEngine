/**
 * @file debugging.h
 * @brief Interfaz publica del modulo de depuracion.
 *
 * Proporciona herramientas de depuracion visual: un selector de frames
 * sobre spritesheets, metricas de rendimiento (FPS, CPU, memoria),
 * y un menu centralizado para activar cada herramienta.
 */

#ifndef DEBUGGING_H
#define DEBUGGING_H

// ============================================================
// Includes
// ============================================================
#include "SDL_events.h"
#include "SDL_rect.h"
#include "gui.h"

// ============================================================
// Enumeraciones
// ============================================================

/**
 * @brief Estados posibles del selector de frames.
 */
typedef enum {
    FDEBUG_OFF,
    FDEBUG_ACTIVE
} FrameDebugState;

/**
 * @brief Estados posibles de las metricas de rendimiento.
 */
typedef enum {
    PERF_METRICS_OFF,
    PERF_METRICS_ON
} PerfMetricsState;

/**
 * @brief Estados posibles del menu de depuracion.
 */
typedef enum {
    DEB_MENU_OFF,
    DEB_MENU_ON
} DebMenuState;

/**
 * @brief Estados posibles del depurador de fuentes TTF.
 */
typedef enum {
    TTF_DEBUG_OFF,
    TTF_DEBUG_ON
} TtfDebugState;

// ============================================================
// Variables globales
// ============================================================

/**
 * @brief Puntero al rectangulo selector de frame actual.
 *
 * NULL cuando el frame debug esta desactivado. Al activarse se
 * aloca en heap (sizeof(SDL_Rect) = 16 bytes).
 */
extern SDL_Rect *framePointer;

// ============================================================
// Debug Menu
// ============================================================

/**
 * @brief Abre o cierra el menu de depuracion (toggle).
 *
 * Al abrirse, cierra cualquier herramienta de debug activa
 * (frame debug, metricas). Se activa con F3.
 */
void toggleDebugMenu(void);

/**
 * @brief Renderiza la ventana Nuklear del menu de depuracion.
 *
 * Muestra botones para activar las distintas herramientas:
 * Frame Debug, Perf. Metrics, Font Debug.
 */
void renderDebugMenu(void);

/**
 * @brief Cierra el menu de depuracion y restaura el estilo Nuklear.
 *
 * Funcion auxiliar usada al seleccionar una opcion del menu para
 * cerrar la ventana y restablecer la alineacion del titulo.
 *
 * @param ctx Contexto Nuklear activo.
 */
void closeDebugMenu(struct nk_context *ctx);

// ============================================================
// Frame Debug
// ============================================================

/**
 * @brief Carga el frame debug (toggle).
 *
 * Si esta desactivado, carga los sprites desde SPRITES_DIR y activa
 * el selector de frames. Si ya esta activo, lo cierra y libera
 * los recursos.
 */
void loadFrameDebug(void);

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
 * @brief Sale del modo frame debug y libera los recursos asociados.
 *
 * Libera el frame pointer, la libreria de texturas y restaura el
 * estado a FDEBUG_OFF.
 */
void exitFrameDebug(void);

/**
 * @brief Renderiza el frame debug segun el estado actual.
 *
 * En FDEBUG_ACTIVE dibuja el spritesheet centrado en pantalla con
 * zoom y pan, el rectangulo selector rojo, y el panel de
 * configuracion Nuklear (imagen, tamanho de frame, coordenadas).
 */
void renderFrameDebug(void);

// ============================================================
// Performance Metrics
// ============================================================

/**
 * @brief Activa o desactiva las metricas de rendimiento (toggle).
 */
void togglePerfMetrics(void);

/**
 * @brief Renderiza la ventana Nuklear de metricas de rendimiento.
 *
 * Muestra FPS, uso de memoria y porcentaje de CPU en una ventana
 * anclada a la esquina superior derecha.
 */
void renderPerfMetrics(void);

// ============================================================
// Font Debug (stub)
// ============================================================

/**
 * @brief Renderiza la ventana de depuracion de fuentes TTF.
 *
 * @note Actualmente es un placeholder sin funcionalidad.
 */
void renderFontDebug(void);

/**
 * @brief Sale del modo de depuracion de fuentes TTF.
 */
void exitFontDebug(void);

// ============================================================
// Master
// ============================================================

/**
 * @brief Renderiza todos los modulos de depuracion activos.
 *
 * Llama en orden a renderFrameDebug(), renderPerfMetrics() y
 * renderDebugMenu().
 */
void renderDebug(void);

#endif
