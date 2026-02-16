/**
 * @file debugging.h
 * @brief Interfaz publica del modulo de depuracion de frames (Frame Debug).
 *
 * Permite seleccionar visualmente frames dentro de un spritesheet,
 * con navegacion por teclado y una ventana de configuracion Nuklear.
 */

#ifndef DEBUGGING_H
#define DEBUGGING_H

// ============================================================
// Includes
// ============================================================
#include <SDL_image.h>
#include <stdbool.h>

// ============================================================
// Enumeraciones
// ============================================================

/**
 * @brief Estados posibles del sistema de depuracion de frames.
 */
typedef enum {
    FDEBUG_OFF,
    FDEBUG_ACTIVE
} FrameDebugState;

/**
 * @brief Estados posibles del sistema de depuracion de metricas.
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

// ============================================================
// Variables globales
// ============================================================

/** @brief Indica si el sistema de frame debug esta habilitado (legacy, ver fdState). */
extern bool frameDebug;

/**
 * @brief Puntero al rectangulo selector de frame actual.
 *
 * @note Si se utiliza un puntero a NULL, son 8 bytes de memoria en stack.
 *       A la hora de alocar memoria, se utilizan 64 bytes adicionales en heap.
 */
extern SDL_Rect *framePointer;

// ============================================================
// Funciones publicas
// ============================================================

/**
 * @brief Carga el frame debug (toggle con F1).
 *
 * Si el frame debug esta desactivado, carga los sprites y activa
 * el modo debug. Si ya esta activo, lo cierra.
 */
void loadFrameDebug();

/**
 * @brief Gestiona los eventos de teclado y mouse para el frame debug.
 *
 * F1 activa/desactiva el frame debug. Las flechas mueven el selector
 * de frame. La rueda del mouse controla el zoom.
 *
 * @param event Evento SDL a procesar.
 */
void keyEventHandlerDebug(SDL_Event event);

/**
 * @brief Sale del modo debug de frames y libera los recursos asociados.
 *
 * Libera el frame pointer, la libreria de texturas y restaura el
 * estado a FDEBUG_OFF.
 */
void exitFrameDebug();

/**
 * @brief Renderiza el frame debug segun el estado actual.
 *
 * En FDEBUG_ACTIVE dibuja el spritesheet con el rectangulo selector,
 * las coordenadas del frame y el panel de configuracion Nuklear.
 */
void renderFrameDebug();

long getMemoryUsageMB();

float getCpuUsage();

void loadPerfMetrics();

void renderPerfMetrics();

void loadDebugMenu();

void renderDebugMenu();

#endif
