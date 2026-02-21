/**
 * @file config.h
 * @brief Definiciones de configuracion del motor: macros de rutas, estructura
 *        GameConfig y funciones para cargar e imprimir la configuracion desde
 *        archivos .ini.
 */

#ifndef CONFIG_H
#define CONFIG_H

#include <stdbool.h>

// ============================================================
// Macros de utilidad
// ============================================================

/** @brief Convierte FPS deseados a milisegundos por frame. */
#define FRAME_TIME_MS(a) (1000 / a)

/**
 * @brief Calcula el tamano maximo de un path compuesto por un directorio
 *        de assets y un nombre de archivo.
 */
#define PATH_SIZE(a) sizeof(SFX_DIR) + sizeof(MUSIC_DIR) + sizeof(a)

// ============================================================
// Rutas de assets
// ============================================================

/** @brief Directorio raiz de assets. */
#define ASSETS_DIR "assets/"

/** @brief Directorio de archivos de configuracion (.ini). */
#define CONFIG_DIR ASSETS_DIR "config/"

/** @brief Directorio de efectos de sonido. */
#define SFX_DIR ASSETS_DIR "sfx/"

/** @brief Directorio de musica. */
#define MUSIC_DIR ASSETS_DIR "music/"

/** @brief Directorio de sprites e imagenes. */
#define SPRITES_DIR ASSETS_DIR "sprites/"

/** @brief Directorio de fuentes tipograficas. */
#define FONTS_DIR ASSETS_DIR "fonts/"

/** @brief Directorio de logs. */
#define LOGS_DIR "logs/"

// ============================================================
// Archivos de configuracion
// ============================================================

/** @brief Nombre del archivo de configuracion para resolucion 4K. */
#define FOUR_K "4k.ini"

/** @brief Nombre del archivo de configuracion para resolucion HD. */
#define HD "hd.ini"

/** @brief Nombre del archivo de configuracion para resolucion arcade (Pac-Man). */
#define ARCADE "arcade.ini"

/** @brief Archivo de configuracion activo seleccionado para la compilacion. */
#define CFG_FILE HD

// ============================================================
// Tipos
// ============================================================

/**
 * @brief Estructura que almacena toda la configuracion del juego, leida
 *        desde un archivo .ini.
 */
typedef struct {
    char name[128];      /**< @brief Nombre de la ventana. */
    int WIN_W;           /**< @brief Ancho de la ventana en pixeles. */
    int WIN_H;           /**< @brief Alto de la ventana en pixeles. */
    bool fullscreen;     /**< @brief Activar pantalla completa. */
    bool vsync;          /**< @brief Activar sincronizacion vertical. */
    int fps;             /**< @brief Frames por segundo objetivo. */
    int defaultMonitor;  /**< @brief Indice del monitor por defecto. */

    int master_volume;   /**< @brief Volumen maestro (0-100). */
    int music_volume;    /**< @brief Volumen de la musica (0-100). */
    int sfx_volume;      /**< @brief Volumen de efectos de sonido (0-100). */
    int audio_frequency; /**< @brief Frecuencia de audio en Hz. */

    bool show_fps;       /**< @brief Mostrar contador de FPS en pantalla. */
    bool debug_mode;     /**< @brief Activar modo de depuracion. */
} GameConfig;

// ============================================================
// Variables globales
// ============================================================

/** @brief Instancia global de la configuracion del juego. */
extern GameConfig config;

// ============================================================
// Funciones
// ============================================================

/**
 * @brief Carga la configuracion desde un archivo .ini y la almacena en
 *        la estructura proporcionada.
 * @param cfg      Puntero a la estructura GameConfig donde se guardaran
 *                 los valores leidos.
 * @param cfg_name Ruta al archivo .ini de configuracion.
 * @return true si el archivo se abrio y leyo correctamente, false en
 *         caso contrario.
 */
bool loadConfig(GameConfig *cfg, char *cfg_name);

/**
 * @brief Imprime por consola todos los valores de la configuracion,
 *        organizados por seccion (Video, Audio, Game, Debug).
 * @param cfg Puntero a la estructura GameConfig a imprimir.
 */
void printConfig(GameConfig *cfg);

#endif
