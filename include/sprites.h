/**
 * @file sprites.h
 * @brief Definiciones de estructuras para sprites, animaciones y gestion de sprites.
 *
 * Declara los tipos basicos utilizados por el motor para representar
 * sprites estaticos, animaciones por frames y el gestor global de sprites.
 */

#ifndef SPRITES_H
#define SPRITES_H

// ============================================================
// Includes
// ============================================================
#include <SDL_image.h>
#include <stdbool.h>

// ============================================================
// Estructuras
// ============================================================

/**
 * @brief Datos de una animacion basada en secuencia de frames.
 */
typedef struct {
    SDL_Rect *frames;        /**< @brief Array de rectangulos que definen cada frame en el spritesheet. */
    int frame_count;         /**< @brief Numero total de frames en la animacion. */
    int current_frame;       /**< @brief Indice del frame actualmente visible. */
    float animation_timer;   /**< @brief Tiempo acumulado desde el ultimo cambio de frame (segundos). */
    float frame_duration;    /**< @brief Duracion de cada frame en segundos. */
} animation;

/**
 * @brief Sprite estatico: una textura con su region fuente y dimensiones.
 */
typedef struct {
    SDL_Texture *texture;    /**< @brief Textura SDL asociada al sprite. */
    SDL_Rect src_rect;       /**< @brief Rectangulo fuente dentro de la textura. */
    int width;               /**< @brief Ancho del sprite en pixeles. */
    int height;              /**< @brief Alto del sprite en pixeles. */
} sprite;

/**
 * @brief Gestor global de sprites cargados.
 */
typedef struct {
    SDL_Texture *sheets;     /**< @brief Textura(s) del spritesheet cargado. */
    bool loaded;             /**< @brief Indica si el spritesheet fue cargado correctamente. */
} SpriteManager;

// ============================================================
// Variables globales
// ============================================================

/** @brief Instancia global del gestor de sprites. */
extern SpriteManager sprite_manager;

/*
typedef struct {
    SDL_Rect hitbox;
    sprite main;
    animation run;
}obj;
*/

#endif
