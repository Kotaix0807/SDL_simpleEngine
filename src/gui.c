/**
 * @file gui.c
 * @brief Implementacion del modulo GUI basado en Nuklear + SDL2 renderer.
 *
 * Contiene la logica de inicializacion, manejo de eventos, renderizado
 * y destruccion del contexto Nuklear, asi como la carga de fuentes TTF.
 */

// ============================================================
// Includes
// ============================================================
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wpedantic"
#pragma GCC diagnostic ignored "-Wunused-function"
#pragma GCC diagnostic ignored "-Wsign-compare"
#pragma GCC diagnostic ignored "-Wmissing-field-initializers"

#define NK_IMPLEMENTATION
#define NK_SDL_RENDERER_IMPLEMENTATION

#include "gui.h"
#include "nuklear_sdl_renderer.h"

#pragma GCC diagnostic pop

// ============================================================
// Variables privadas
// ============================================================
static struct nk_context *ctx = NULL;
static SDL_Renderer *sdl_renderer = NULL;
static SDL_Window *sdl_window = NULL;

// ============================================================
// Funciones publicas
// ============================================================

/// Inicializa Nuklear con el renderer SDL y carga la fuente TTF indicada.
bool GUI_Init(SDL_Window *win, SDL_Renderer *ren, const char *font_path, float font_size)
{
    sdl_window = win;
    sdl_renderer = ren;
    ctx = nk_sdl_init(win, ren);
    if (!ctx)
        return false;

    struct nk_font_atlas *atlas;
    struct nk_font *nk_font = NULL;
    nk_sdl_font_stash_begin(&atlas);
    if (font_path)
        nk_font = nk_font_atlas_add_from_file(atlas, font_path, font_size, 0);
    nk_sdl_font_stash_end();

    if (nk_font)
        nk_style_set_font(ctx, &nk_font->handle);

    return true;
}

/// Delega el evento SDL al backend de entrada de Nuklear.
/// Transforma coordenadas de mouse de espacio de ventana a espacio logico
/// para que Nuklear funcione correctamente con SDL_RenderSetScale.
int GUI_HandleEvent(SDL_Event *event)
{
    float sx, sy;
    SDL_RenderGetScale(sdl_renderer, &sx, &sy);

    switch (event->type)
    {
        case SDL_MOUSEBUTTONDOWN:
        case SDL_MOUSEBUTTONUP:
        {
            event->button.x = (int)(event->button.x / sx);
            event->button.y = (int)(event->button.y / sy);
            break;
        }
        case SDL_MOUSEMOTION:
        {
            event->motion.x = (int)(event->motion.x / sx);
            event->motion.y = (int)(event->motion.y / sy);
            event->motion.xrel = (int)(event->motion.xrel / sx);
            event->motion.yrel = (int)(event->motion.yrel / sy);
            break;
        }
    }
    return nk_sdl_handle_event(event);
}

/// Inicia la captura de entrada de Nuklear para este frame.
void GUI_InputBegin(void)
{
    nk_input_begin(ctx);
}

/// Finaliza la captura de entrada de Nuklear para este frame.
void GUI_InputEnd(void)
{
    nk_input_end(ctx);
}

/// Renderiza los comandos de dibujo acumulados con anti-aliasing activado.
void GUI_Render(void)
{
    nk_sdl_render(NK_ANTI_ALIASING_ON);
}

/// Libera los recursos de Nuklear y resetea los punteros internos.
void GUI_Destroy(void)
{
    nk_sdl_shutdown();
    ctx = NULL;
    sdl_renderer = NULL;
    sdl_window = NULL;
}

/// Devuelve el contexto Nuklear activo (o NULL si no esta inicializado).
struct nk_context* GUI_GetContext(void)
{
    return ctx;
}
