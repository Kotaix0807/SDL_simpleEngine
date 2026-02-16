/**
 * @file sound.h
 * @brief Interfaz del modulo de audio: tipos, inicializacion, reproduccion y gestion de librerias de sonido.
 */

#ifndef SOUND_H
#define SOUND_H

// ============================================================
// Includes
// ============================================================
#include <SDL_mixer.h>
#include <SDL.h>
#include <stdbool.h>

// ============================================================
// Tipos
// ============================================================

/**
 * @brief Libreria de efectos de sonido (SFX).
 */
typedef struct sounds_{
    Mix_Chunk **chunks; /**< @brief Array de chunks de audio cargados. */
    int n;              /**< @brief Numero de chunks en el array. */
}sfx;

/**
 * @brief Libreria de musica (pistas largas).
 */
typedef struct songs{
    Mix_Music **music;  /**< @brief Array de pistas de musica cargadas. */
    int n;              /**< @brief Numero de pistas en el array. */
}music;

// ============================================================
// Inicializacion y cierre
// ============================================================

/**
 * @brief Inicializa el subsistema de audio de SDL y abre el dispositivo de mezcla.
 * @return true si la inicializacion fue exitosa, false en caso de error.
 */
bool initAudio(void);

/**
 * @brief Cierra el dispositivo de mezcla y libera el subsistema de audio de SDL.
 */
void quitAudio(void);

// ============================================================
// Reproduccion
// ============================================================

/**
 * @brief Reproduce un efecto de sonido una vez y lo libera automaticamente al terminar.
 * @param sound Nombre del archivo de sonido (relativo a SFX_DIR).
 */
void playAndFreeSfx(const char *sound);

// ============================================================
// Gestion de librerias de audio
// ============================================================

/**
 * @brief Crea una libreria de efectos de sonido cargando todos los archivos de audio de un directorio.
 * @param path Ruta del directorio que contiene los archivos de audio.
 * @return Puntero a la libreria sfx creada, o NULL si fallo.
 */
sfx *initSfxLib(char *path);

/**
 * @brief Crea una libreria de musica cargando todos los archivos de audio de un directorio.
 * @param path Ruta del directorio que contiene los archivos de musica.
 * @return Puntero a la libreria music creada, o NULL si fallo.
 */
music *initMusicLib(char *path);

/**
 * @brief Libera todos los recursos de una libreria de efectos de sonido.
 * @param cur Puntero a la libreria sfx a liberar (puede ser NULL).
 */
void freeSfxLib(sfx *cur);

/**
 * @brief Libera todos los recursos de una libreria de musica.
 * @param cur Puntero a la libreria music a liberar (puede ser NULL).
 */
void freeMusicLib(music *cur);

#endif
