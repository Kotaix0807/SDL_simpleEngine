/**
 * @file text.c
 * @brief Implementacion del sistema de renderizado de texto con cache de texturas.
 *
 * Gestiona la carga de fuentes, la creacion de objetos Text y el
 * renderizado eficiente mediante texturas cacheadas que solo se
 * regeneran cuando el contenido cambia.
 */

// ============================================================
//  Includes
// ============================================================

#define _POSIX_C_SOURCE 200809L
#include "text.h"
#include "engine.h"
#include "tools.h"
#include <string.h>
#include <stdlib.h>

// ============================================================
//  Variables privadas
// ============================================================

/** @brief Fuente TTF cargada por defecto para todos los objetos Text. */
static TTF_Font *defaultFont = NULL;

/** @brief Color por defecto (blanco opaco) usado al crear texto sin color explicito. */
static SDL_Color defaultColor = {255, 255, 255, 255};

// ============================================================
//  Sistema de texto
// ============================================================

/** @brief Inicializa el sistema de texto cargando la fuente por defecto. */
bool Text_InitSystem(const char *fontPath, int defaultSize)
{
    defaultFont = TTF_OpenFont(fontPath, defaultSize);
    if (!defaultFont)
    {
        printDebug("No se pudo cargar fuente: %s\n", TTF_GetError());
        return false;
    }
    return true;
}

/** @brief Cierra el sistema de texto y libera la fuente por defecto. */
void Text_QuitSystem(void)
{
    if (defaultFont)
    {
        TTF_CloseFont(defaultFont);
        defaultFont = NULL;
    }
    TTF_Quit();
}

// ============================================================
//  Funciones internas (static)
// ============================================================

/**
 * @brief Renderiza el contenido de un Text a una textura SDL.
 *
 * Destruye la textura previa si existe, genera una nueva superficie
 * con TTF_RenderUTF8_Blended y la convierte en textura. Actualiza
 * las dimensiones del rect del texto.
 */
static void Text_Render(Text *text)
{
    if (text->texture)
    {
        SDL_DestroyTexture(text->texture);
        text->texture = NULL;
    }

    if (!text->content || text->content[0] == '\0')
        return;

    SDL_Surface *surface = TTF_RenderUTF8_Blended(text->font, text->content, text->color);
    if (!surface)
        return;

    text->texture = SDL_CreateTextureFromSurface(render, surface);
    text->rect.w = surface->w;
    text->rect.h = surface->h;
    SDL_FreeSurface(surface);
}

// ============================================================
//  Creacion y manipulacion de texto
// ============================================================

/** @brief Crea un texto en la posicion indicada con el color por defecto. */
Text Text_Create(const char *content, int x, int y)
{
    return Text_CreateColored(content, x, y, defaultColor);
}

/** @brief Crea un texto con color personalizado en la posicion indicada. */
Text Text_CreateColored(const char *content, int x, int y, SDL_Color color)
{
    Text text = {0};
    text.rect.x = x;
    text.rect.y = y;
    text.color = color;
    text.font = defaultFont;

    if (content)
    {
        text.content = strdup(content);
        Text_Render(&text);
    }

    return text;
}

/** @brief Actualiza el contenido del texto, re-renderizando solo si cambia. */
void Text_Set(Text *text, const char *content)
{
    // Si el contenido es igual, no hacer nada (optimización clave)
    if (text->content && content && strcmp(text->content, content) == 0)
        return;

    free(text->content);
    text->content = content ? strdup(content) : NULL;
    Text_Render(text);
}

/** @brief Dibuja el texto en pantalla usando el renderer global. */
void Text_Draw(Text *text)
{
    if (text->texture)
        SDL_RenderCopy(render, text->texture, NULL, &text->rect);
}

/** @brief Libera la textura y la cadena de contenido de un objeto Text. */
void Text_Free(Text *text)
{
    if (text->texture)
        SDL_DestroyTexture(text->texture);
    free(text->content);
    text->texture = NULL;
    text->content = NULL;
}
