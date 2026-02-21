// ============================================================
// Includes
// ============================================================
#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>

#include <SDL_render.h>
#include "SDL_events.h"
#include "SDL_rect.h"

#include "config.h"
#include "debugging.h"
#include "engine.h"
#include "gui.h"
#include "img.h"
#include "tools.h"

// ============================================================
// Variables - Estados de los modulos de depuracion
// ============================================================

FrameDebugState  fdState  = FDEBUG_OFF;
PerfMetricsState pmState  = PERF_METRICS_OFF;
DebMenuState     mnState  = DEB_MENU_OFF;
TtfDebugState    ttfState = TTF_DEBUG_OFF;

// ============================================================
// Variables - Frame Debug (privadas)
// ============================================================

SDL_Rect *framePointer = NULL;

static texture sprites   = {0};   ///< Sprites cargados desde SPRITES_DIR
static int inputImageNum = 0;     ///< Indice de imagen seleccionada
static int inputFrameW   = 16;    ///< Ancho de frame configurado
static int inputFrameH   = 16;    ///< Alto de frame configurado
static float zoom        = 1.0f;  ///< Nivel de zoom (0.1x - 10.0x)
static int panX          = 0;     ///< Desplazamiento horizontal (arrastre)
static int panY          = 0;     ///< Desplazamiento vertical (arrastre)
static bool dragging     = false; ///< Indica si se arrastra con el mouse
static int lastMouseX    = 0;     ///< Ultima posicion X durante arrastre
static int lastMouseY    = 0;     ///< Ultima posicion Y durante arrastre

// ============================================================
// Debug Menu
// ============================================================

void toggleDebugMenu(void)
{
    exitFrameDebug();
    pmState = PERF_METRICS_OFF;

    if (mnState == DEB_MENU_ON)
    {
        mnState = DEB_MENU_OFF;
        return;
    }

    mnState = DEB_MENU_ON;
}

void renderDebugMenu(void)
{
    if (mnState == DEB_MENU_OFF)
        return;

    static int winW = 250;
    static int winH = 170;

    struct nk_context *ctx = GUI_GetContext();
    ctx->style.window.header.title_align = NK_TEXT_CENTERED;

    if (nk_begin(ctx, "Debug Menu", nk_rect(
        centerI(config.WIN_W, winW),
        centerI(config.WIN_H, winH),
        winW, winH),
        NK_WINDOW_BORDER | NK_WINDOW_TITLE | NK_WINDOW_MOVABLE))
    {
        nk_layout_row_dynamic(ctx, 30, 1);
        if (nk_button_label(ctx, "Frame Debug"))
        {
            loadFrameDebug();
            closeDebugMenu(ctx);
            return;
        }

        nk_layout_row_dynamic(ctx, 30, 1);
        if (nk_button_label(ctx, "Perf. Metrics"))
        {
            togglePerfMetrics();
            closeDebugMenu(ctx);
            return;
        }

        nk_layout_row_dynamic(ctx, 30, 1);
        if (nk_button_label(ctx, "Font Debug"))
        {
            closeDebugMenu(ctx);
            return;
        }
    }
    else
    {
        nk_end(ctx);
        ctx->style.window.header.title_align = NK_TEXT_LEFT;
        return;
    }

    nk_end(ctx);
    ctx->style.window.header.title_align = NK_TEXT_LEFT;
}

void closeDebugMenu(struct nk_context *ctx)
{
    mnState = DEB_MENU_OFF;
    ctx->style.window.header.title_align = NK_TEXT_LEFT;
    nk_end(ctx);
}

// ============================================================
// Frame Debug
// ============================================================

/// Inicializa el frame pointer y pasa al estado FDEBUG_ACTIVE.
static void activateFrameDebug(void)
{
    framePointer = malloc(sizeof(SDL_Rect));
    if (!framePointer)
        return;

    framePointer->x = 0;
    framePointer->y = 0;
    framePointer->w = inputFrameW;
    framePointer->h = inputFrameH;

    panX = 0;
    panY = 0;

    fdState = FDEBUG_ACTIVE;
}

void loadFrameDebug(void)
{
    if (fdState != FDEBUG_OFF)
    {
        exitFrameDebug();
        return;
    }

    sprites = initTextureLib(SPRITES_DIR);
    if (sprites.n <= 0)
        return;

    inputImageNum = 0;
    inputFrameW   = 16;
    inputFrameH   = 16;

    activateFrameDebug();
}

void handleDebugEvent(SDL_Event event)
{
    SDL_Keycode key = event.key.keysym.sym;

    switch (event.type)
    {
        case SDL_KEYDOWN:
        {
            if (key == SDLK_F3)
            {
                toggleDebugMenu();
                return;
            }

            if (fdState != FDEBUG_ACTIVE)
                return;

            // Limites basados en el tamanho de la imagen y el frame
            int maxX = sprites.rects[inputImageNum]->w - framePointer->w;
            int maxY = sprites.rects[inputImageNum]->h - framePointer->h;

            // Mover en pasos del tamanho del frame
            if (key == SDLK_LEFT)
                framePointer->x -= framePointer->w;
            if (key == SDLK_RIGHT)
                framePointer->x += framePointer->w;
            if (key == SDLK_UP)
                framePointer->y -= framePointer->h;
            if (key == SDLK_DOWN)
                framePointer->y += framePointer->h;

            // Wrap-around
            if (framePointer->x < 0)
                framePointer->x = maxX;
            else if (framePointer->x > maxX)
                framePointer->x = 0;

            if (framePointer->y < 0)
                framePointer->y = maxY;
            else if (framePointer->y > maxY)
                framePointer->y = 0;
            break;
        }
        case SDL_MOUSEBUTTONDOWN:
        {
            if (fdState != FDEBUG_ACTIVE)
                break;
            if (event.button.button == SDL_BUTTON_LEFT)
            {
                int imgW = sprites.rects[inputImageNum]->w * zoom;
                int imgH = sprites.rects[inputImageNum]->h * zoom;
                int imgX = (config.WIN_W - imgW) / 2 + panX;
                int imgY = (config.WIN_H - imgH) / 2 + panY;

                SDL_Point mousePoint = {event.button.x, event.button.y};
                SDL_Rect spriteRect  = {imgX, imgY, imgW, imgH};

                // Solo arrastrar si el mouse esta sobre la textura
                if (SDL_PointInRect(&mousePoint, &spriteRect))
                {
                    dragging   = true;
                    lastMouseX = event.button.x;
                    lastMouseY = event.button.y;
                }
            }
            break;
        }
        case SDL_MOUSEBUTTONUP:
        {
            if (event.button.button == SDL_BUTTON_LEFT)
                dragging = false;
            break;
        }
        case SDL_MOUSEMOTION:
        {
            if (dragging)
            {
                panX      += event.motion.x - lastMouseX;
                panY      += event.motion.y - lastMouseY;
                lastMouseX = event.motion.x;
                lastMouseY = event.motion.y;
            }
            break;
        }
        case SDL_MOUSEWHEEL:
        {
            float dy = event.wheel.preciseY;
            if (event.wheel.direction == SDL_MOUSEWHEEL_FLIPPED)
                dy *= -1;

            zoom += dy * 0.1f;
            if (zoom < 0.1f)
                zoom = 0.1f;
            if (zoom > 10.0f)
                zoom = 10.0f;
            break;
        }
    }
}

void exitFrameDebug(void)
{
    if (framePointer)
    {
        free(framePointer);
        framePointer = NULL;
    }

    freeTextureLib(&sprites);
    fdState = FDEBUG_OFF;
}

void renderFrameDebug(void)
{
    if (fdState == FDEBUG_OFF)
        return;

    framePointer->w = inputFrameW;
    framePointer->h = inputFrameH;

    // --- Panel de configuracion Nuklear ---
    struct nk_context *ctx = GUI_GetContext();
    if (nk_begin(ctx, "Frame Debug",
                 nk_rect(0, 0, 300, 220),
                 NK_WINDOW_BORDER | NK_WINDOW_MOVABLE | NK_WINDOW_TITLE))
    {
        int prevImageNum = inputImageNum;

        nk_layout_row_dynamic(ctx, 30, 1);
        nk_property_int(ctx, "Image #:", 0, &inputImageNum, sprites.n - 1, 1, 1);

        nk_layout_row_dynamic(ctx, 30, 1);
        nk_property_int(ctx, "Frame W:", 1, &inputFrameW,
                        sprites.rects[inputImageNum]->w, 1, 1);

        nk_layout_row_dynamic(ctx, 30, 1);
        nk_property_int(ctx, "Frame H:", 1, &inputFrameH,
                        sprites.rects[inputImageNum]->h, 1, 1);

        char buffer[64];
        snprintf(buffer, sizeof(buffer), "X: %d, Y: %d",
                 framePointer->x / framePointer->w,
                 framePointer->y / framePointer->h);
        nk_layout_row_dynamic(ctx, 50, 1);
        nk_label(ctx, buffer, NK_TEXT_LEFT);

        // Resetear posicion si cambio la imagen
        if (inputImageNum != prevImageNum)
        {
            framePointer->x = 0;
            framePointer->y = 0;
            int newW = sprites.rects[inputImageNum]->w;
            int newH = sprites.rects[inputImageNum]->h;

            if (inputFrameW > newW)
                inputFrameW = newW;
            if (inputFrameH > newH)
                inputFrameH = newH;
        }
    }
    else
    {
        nk_end(ctx);
        exitFrameDebug();
        return;
    }
    nk_end(ctx);

    // --- Dibujar sprite + rectangulo selector ---
    int imgW = sprites.rects[inputImageNum]->w * zoom;
    int imgH = sprites.rects[inputImageNum]->h * zoom;
    int imgX = (config.WIN_W - imgW) / 2 + panX;
    int imgY = (config.WIN_H - imgH) / 2 + panY;

    SDL_Rect spriteRect = {imgX, imgY, imgW, imgH};
    SDL_RenderCopy(render, sprites.textures_array[inputImageNum],
                   sprites.rects[inputImageNum], &spriteRect);

    // Rectangulo rojo sobre el frame seleccionado
    SDL_Rect drawRect = {
        .x = (int)(framePointer->x * zoom) + imgX,
        .y = (int)(framePointer->y * zoom) + imgY,
        .w = (int)(framePointer->w * zoom),
        .h = (int)(framePointer->h * zoom)
    };
    renderRect(&drawRect, 255, 0, 0, 255);
}

// ============================================================
// Performance Metrics
// ============================================================

void togglePerfMetrics(void)
{
    if (pmState == PERF_METRICS_ON)
    {
        pmState = PERF_METRICS_OFF;
        return;
    }

    pmState = PERF_METRICS_ON;
}

void renderPerfMetrics(void)
{
    if (pmState == PERF_METRICS_OFF)
        return;

    static int winW = 350;
    static int winH = 140;

    struct nk_context *ctx = GUI_GetContext();
    if (nk_begin(ctx, "Performance Metrics",
                 nk_rect(config.WIN_W - winW, 0, winW, winH),
                 NK_WINDOW_BORDER | NK_WINDOW_TITLE))
    {
        char buffer[32];

        snprintf(buffer, sizeof(buffer), "FPS: %d", (int)(1.0f / deltatime));
        nk_layout_row_dynamic(ctx, 20, 1);
        nk_label(ctx, buffer, NK_TEXT_LEFT);

        snprintf(buffer, sizeof(buffer), "Mem Usg: %ld MB", getMemoryUsageMB());
        nk_layout_row_dynamic(ctx, 20, 1);
        nk_label(ctx, buffer, NK_TEXT_LEFT);

        snprintf(buffer, sizeof(buffer), "CPU: %.1f%%", getCpuUsage());
        nk_layout_row_dynamic(ctx, 20, 1);
        nk_label(ctx, buffer, NK_TEXT_LEFT);
    }
    else
    {
        nk_end(ctx);
        return;
    }
    nk_end(ctx);
}

// ============================================================
// Font Debug (stub)
// ============================================================

void renderFontDebug(void)
{
    if (ttfState == TTF_DEBUG_OFF)
        return;

    static int winW = 250;
    static int winH = 140;

    struct nk_context *ctx = GUI_GetContext();
    ctx->style.window.header.title_align = NK_TEXT_CENTERED;

    if (nk_begin(ctx, "Font Debug", nk_rect(
        centerI(config.WIN_W, winW),
        centerI(config.WIN_H, winH),
        winW, winH),
        NK_WINDOW_BORDER | NK_WINDOW_MOVABLE))
    {
        // TODO: implementar contenido de Font Debug
    }
    else
    {
        nk_end(ctx);
        ctx->style.window.header.title_align = NK_TEXT_LEFT;
        return;
    }

    nk_end(ctx);
    ctx->style.window.header.title_align = NK_TEXT_LEFT;
}

void exitFontDebug(void)
{
    ttfState = TTF_DEBUG_OFF;
}

// ============================================================
// Master
// ============================================================

void renderDebug(void)
{
    renderFrameDebug();
    renderPerfMetrics();
    renderDebugMenu();
}