/**
 * @file sprites.h
 * @brief Sistema de sprites, animaciones y sprites animados.
 *
 * Provee las estructuras y funciones para dibujar sprites estáticos,
 * animaciones por frames desde spritesheets, y sprites animados con
 * múltiples estados (idle, walk, etc).
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
 * @brief Animación basada en secuencia de frames de un spritesheet.
 */
typedef struct {
    SDL_Rect *frames;         // Array de regiones (cada frame en el spritesheet)
    int       frame_count;    // Total de frames
    int       current_frame;  // Frame actual
    float     timer;          // Tiempo acumulado
    float     frame_duration; // Segundos por frame (ej: 0.1 = 10 FPS)
    bool      loop;           // Si repite al terminar
    bool      finished;       // true cuando terminó (si loop == false)
} Animation;

/**
 * @brief Sprite: región de una textura con posición, flip y rotación.
 */
typedef struct {
    SDL_Texture     *texture; // Spritesheet o imagen individual
    SDL_Rect         src;     // Región fuente (qué recortar de la textura)
    SDL_FRect        dst;     // Posición y tamaño en pantalla (float)
    SDL_RendererFlip flip;    // SDL_FLIP_NONE, SDL_FLIP_HORIZONTAL, SDL_FLIP_VERTICAL
    double           angle;   // Rotación en grados (centro del sprite)
} Sprite;

/**
 * @brief Sprite animado: sprite con múltiples animaciones (estados).
 */
typedef struct {
    Sprite     sprite;        // Sprite base (textura + posición + flip)
    Animation *animations;    // Array de animaciones
    int        anim_count;    // Total de animaciones
    int        current_anim;  // Índice de la animación activa
} AnimatedSprite;

// ============================================================
// Animation
// ============================================================

Animation Anim_Create(SDL_Rect *frames, int count, float fps, bool loop);
Animation Anim_CreateFromSheet(int frameW, int frameH, int cols, int row, int count, float fps, bool loop);
void      Anim_Update(Animation *a, float dt);
void      Anim_Reset(Animation *a);
SDL_Rect  Anim_CurrentFrame(Animation *a);
void      Anim_Free(Animation *a);

// ============================================================
// Sprite
// ============================================================

Sprite Sprite_Create(SDL_Texture *tex, SDL_Rect src, float x, float y);
Sprite Sprite_CreateFull(SDL_Texture *tex, float x, float y);
void   Sprite_Draw(Sprite *s);
void   Sprite_SetPos(Sprite *s, float x, float y);
void   Sprite_SetFlip(Sprite *s, SDL_RendererFlip flip);

// ============================================================
// AnimatedSprite
// ============================================================

AnimatedSprite ASprite_Create(SDL_Texture *tex, Animation *anims, int count, float x, float y);
void ASprite_Play(AnimatedSprite *as, int animIndex);
void ASprite_Update(AnimatedSprite *as, float dt);
void ASprite_Draw(AnimatedSprite *as);
void ASprite_Free(AnimatedSprite *as);

#endif
