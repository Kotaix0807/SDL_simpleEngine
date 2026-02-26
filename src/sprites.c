/**
 * @file sprites.c
 * @brief Implementación del sistema de sprites, animaciones y sprites animados.
 */

// ============================================================
// Includes
// ============================================================
#include "sprites.h"
#include "engine.h"
#include "tools.h"
#include <stdlib.h>
#include <string.h>

// ============================================================
// Animation
// ============================================================

// Crea una animación a partir de un array de rects externo.
// El caller mantiene la propiedad del array frames.
Animation Anim_Create(SDL_Rect *frames, int count, float fps, bool loop)
{
    return (Animation){
        .frames         = frames,
        .frame_count    = count,
        .current_frame  = 0,
        .timer          = 0.0f,
        .frame_duration = 1.0f / fps,
        .loop           = loop,
        .finished       = false
    };
}

// Genera automáticamente los rects desde un spritesheet con grid uniforme.
// frameW/frameH: tamaño de cada frame. cols: columnas del sheet.
// row: fila inicial. count: cantidad de frames. fps: frames por segundo.
Animation Anim_CreateFromSheet(int frameW, int frameH, int cols, int row, int count, float fps, bool loop)
{
    SDL_Rect *frames = malloc(count * sizeof(SDL_Rect));
    if (!frames)
    {
        printDebug(LOG_ERROR, "No se pudo asignar memoria para frames de animacion\n");
        return (Animation){0};
    }

    for (int i = 0; i < count; i++)
    {
        frames[i] = (SDL_Rect){
            .x = (i % cols) * frameW,
            .y = (row + i / cols) * frameH,
            .w = frameW,
            .h = frameH
        };
    }

    return (Animation){
        .frames         = frames,
        .frame_count    = count,
        .current_frame  = 0,
        .timer          = 0.0f,
        .frame_duration = 1.0f / fps,
        .loop           = loop,
        .finished       = false
    };
}

// Avanza la animación según deltatime.
void Anim_Update(Animation *a, float dt)
{
    if (!a || a->finished || a->frame_count <= 1)
        return;

    a->timer += dt;
    while (a->timer >= a->frame_duration)
    {
        a->timer -= a->frame_duration;
        a->current_frame++;

        if (a->current_frame >= a->frame_count)
        {
            if (a->loop)
                a->current_frame = 0;
            else
            {
                a->current_frame = a->frame_count - 1;
                a->finished = true;
                return;
            }
        }
    }
}

// Reinicia la animación al frame 0.
void Anim_Reset(Animation *a)
{
    if (!a) return;
    a->current_frame = 0;
    a->timer = 0.0f;
    a->finished = false;
}

// Retorna el SDL_Rect del frame actual.
SDL_Rect Anim_CurrentFrame(Animation *a)
{
    if (!a || !a->frames || a->frame_count <= 0)
        return (SDL_Rect){0};
    return a->frames[a->current_frame];
}

// Libera el array de frames (solo si fue creado con Anim_CreateFromSheet).
void Anim_Free(Animation *a)
{
    if (!a) return;
    free(a->frames);
    a->frames = NULL;
    a->frame_count = 0;
}

// ============================================================
// Sprite
// ============================================================

// Crea un sprite desde una región (src_rect) de una textura.
// El tamaño en pantalla se toma del src_rect.
Sprite Sprite_Create(SDL_Texture *tex, SDL_Rect src, float x, float y)
{
    return (Sprite){
        .texture = tex,
        .src     = src,
        .dst     = {x, y, (float)src.w, (float)src.h},
        .flip    = SDL_FLIP_NONE,
        .angle   = 0.0
    };
}

// Crea un sprite que usa la textura completa.
Sprite Sprite_CreateFull(SDL_Texture *tex, float x, float y)
{
    int w = 0, h = 0;
    GetTextureSize(tex, &w, &h);
    return (Sprite){
        .texture = tex,
        .src     = {0, 0, w, h},
        .dst     = {x, y, (float)w, (float)h},
        .flip    = SDL_FLIP_NONE,
        .angle   = 0.0
    };
}

// Dibuja el sprite con rotación y flip.
void Sprite_Draw(Sprite *s)
{
    if (!s || !s->texture) return;
    SDL_RenderCopyExF(render, s->texture, &s->src, &s->dst,
                      s->angle, NULL, s->flip);
}

// Cambia la posición del sprite.
void Sprite_SetPos(Sprite *s, float x, float y)
{
    if (!s) return;
    s->dst.x = x;
    s->dst.y = y;
}

// Cambia el flip del sprite.
void Sprite_SetFlip(Sprite *s, SDL_RendererFlip flip)
{
    if (!s) return;
    s->flip = flip;
}

// ============================================================
// AnimatedSprite
// ============================================================

// Crea un sprite animado. Copia el array de animaciones (toma ownership de los frames).
// Después de llamar, no usar Anim_Free en las animaciones originales.
AnimatedSprite ASprite_Create(SDL_Texture *tex, Animation *anims, int count, float x, float y)
{
    Animation *copy = malloc(count * sizeof(Animation));
    if (!copy)
    {
        printDebug(LOG_ERROR, "No se pudo asignar memoria para AnimatedSprite\n");
        return (AnimatedSprite){0};
    }
    memcpy(copy, anims, count * sizeof(Animation));

    // Src rect inicial = primer frame de la primera animación
    SDL_Rect initial = {0};
    if (count > 0 && anims[0].frames && anims[0].frame_count > 0)
        initial = anims[0].frames[0];

    return (AnimatedSprite){
        .sprite       = Sprite_Create(tex, initial, x, y),
        .animations   = copy,
        .anim_count   = count,
        .current_anim = 0
    };
}

// Cambia la animación activa. Si ya es la misma, no hace nada.
void ASprite_Play(AnimatedSprite *as, int animIndex)
{
    if (!as || animIndex < 0 || animIndex >= as->anim_count)
        return;
    if (as->current_anim == animIndex)
        return;
    as->current_anim = animIndex;
    Anim_Reset(&as->animations[animIndex]);
}

// Actualiza la animación activa y sincroniza el src rect del sprite.
void ASprite_Update(AnimatedSprite *as, float dt)
{
    if (!as || as->anim_count <= 0)
        return;

    Animation *current = &as->animations[as->current_anim];
    Anim_Update(current, dt);
    as->sprite.src = Anim_CurrentFrame(current);
}

// Dibuja el sprite animado (frame actual con flip/rotación).
void ASprite_Draw(AnimatedSprite *as)
{
    if (!as) return;
    Sprite_Draw(&as->sprite);
}

// Libera las animaciones y sus frames.
void ASprite_Free(AnimatedSprite *as)
{
    if (!as) return;
    for (int i = 0; i < as->anim_count; i++)
        Anim_Free(&as->animations[i]);
    free(as->animations);
    as->animations = NULL;
    as->anim_count = 0;
}
