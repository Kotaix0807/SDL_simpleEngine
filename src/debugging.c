/**
 * @file debugging.c
 * @brief Herramientas de depuracion visual: frame debug, metricas de
 *        rendimiento, font debug y menu centralizado.
 */

// ============================================================
// Includes
// ============================================================

#define _POSIX_C_SOURCE 200809L
#include <stdio.h>

#include "config.h"
#include "debugging.h"
#include "engine.h"
#include "gui.h"
#include "img.h"
#include "text.h"
#include "tools.h"

// ============================================================
// Estados de los modulos (privados)
// ============================================================

static bool debugMenuActive   = false;
static bool frameDebugActive  = false;
static bool perfMetricsActive = false;
static bool fontDebugActive   = false;

// ============================================================
// Variables - Frame Debug (privadas)
// ============================================================

static SDL_Rect *framePointer = NULL;
static texture sprites   = {0};
static int inputImageNum = 0;
static int inputFrameW   = 16;
static int inputFrameH   = 16;
static float zoom        = 1.0f;
static int panX          = 0;
static int panY          = 0;
static bool dragging     = false;
static int lastMouseX    = 0;
static int lastMouseY    = 0;

// ============================================================
// Variables - Font Debug (privadas)
// ============================================================

static const char *fontNames[] = {"PressStart2P", "LcdSolid", "VT323", "Jersey10"};
static const char *fontFiles[] = {ARCADE_FONT, LCD_FONT, VT_FONT, JERSEY_FONT};
#define FONT_COUNT 4

static int fontIndex           = 0;
static int fontSize            = 24;
static TTF_Font *debugFont     = NULL;
static SDL_Texture *previewTex = NULL;
static char previewText[128]   = "AaBbCc 0123456789 !@#";
static int previewLen          = 21;

// ============================================================
// Forward declarations (funciones static)
// ============================================================

static void toggleFrameDebug(void);
static void toggleFontDebug(void);
static void exitFontDebug(void);

// ============================================================
// Helpers (privados)
// ============================================================

/// Renderiza la preview del font debug.
static void rebuildFontPreview(void)
{
    if (previewTex)
    {
        SDL_DestroyTexture(previewTex);
        previewTex = NULL;
    }

    if (!debugFont || previewText[0] == '\0')
        return;

    SDL_Color white = {255, 255, 255, 255};
    SDL_Surface *srf = TTF_RenderUTF8_Blended(debugFont, previewText, white);
    if (!srf)
        return;

    previewTex = SDL_CreateTextureFromSurface(render, srf);
    SDL_FreeSurface(srf);
}

/// Recarga la fuente con el indice y tamanho actuales.
static void reloadDebugFont(void)
{
    if (debugFont)
    {
        TTF_CloseFont(debugFont);
        debugFont = NULL;
    }

    char path[256];
    snprintf(path, sizeof(path), "%s%s", FONTS_DIR, fontFiles[fontIndex]);
    debugFont = TTF_OpenFont(path, fontSize);
    if (!debugFont)
        printDebug(LOG_WARN, "No se pudo cargar fuente '%s': %s\n", path, TTF_GetError());

    rebuildFontPreview();
}

// ============================================================
// Debug Menu
// ============================================================

static void toggleDebugMenu(void)
{
    exitFrameDebug();
    perfMetricsActive = false;
    fontDebugActive = false;

    debugMenuActive = !debugMenuActive;
}

static void closeDebugMenu(struct nk_context *ctx)
{
    debugMenuActive = false;
    ctx->style.window.header.title_align = NK_TEXT_LEFT;
    nk_end(ctx);
}

static void renderDebugMenu(void)
{
    if (!debugMenuActive)
        return;

    int winW = 250;
    int winH = 170;

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
            toggleFrameDebug();
            closeDebugMenu(ctx);
            return;
        }

        nk_layout_row_dynamic(ctx, 30, 1);
        if (nk_button_label(ctx, "Perf. Metrics"))
        {
            perfMetricsActive = !perfMetricsActive;
            closeDebugMenu(ctx);
            return;
        }

        nk_layout_row_dynamic(ctx, 30, 1);
        if (nk_button_label(ctx, "Font Debug"))
        {
            toggleFontDebug();
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

// ============================================================
// Frame Debug
// ============================================================

static void toggleFrameDebug(void)
{
    if (frameDebugActive)
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
    zoom          = 1.0f;
    panX          = 0;
    panY          = 0;

    framePointer = malloc(sizeof(SDL_Rect));
    if (!framePointer)
        return;

    *framePointer = (SDL_Rect){0, 0, inputFrameW, inputFrameH};
    frameDebugActive = true;
}

void exitFrameDebug(void)
{
    if (framePointer)
    {
        free(framePointer);
        framePointer = NULL;
    }

    freeTextureLib(&sprites);
    frameDebugActive = false;
}

static void renderFrameDebug(void)
{
    if (!frameDebugActive)
        return;

    framePointer->w = inputFrameW;
    framePointer->h = inputFrameH;

    // --- Panel Nuklear ---
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

        if (inputImageNum != prevImageNum)
        {
            framePointer->x = 0;
            framePointer->y = 0;
            int newW = sprites.rects[inputImageNum]->w;
            int newH = sprites.rects[inputImageNum]->h;
            if (inputFrameW > newW) inputFrameW = newW;
            if (inputFrameH > newH) inputFrameH = newH;
        }
    }
    else
    {
        nk_end(ctx);
        exitFrameDebug();
        return;
    }
    nk_end(ctx);

    // --- Dibujar sprite + selector ---
    int imgW = sprites.rects[inputImageNum]->w * zoom;
    int imgH = sprites.rects[inputImageNum]->h * zoom;
    int imgX = (config.WIN_W - imgW) / 2 + panX;
    int imgY = (config.WIN_H - imgH) / 2 + panY;

    SDL_Rect spriteRect = {imgX, imgY, imgW, imgH};
    SDL_RenderCopy(render, sprites.textures_array[inputImageNum],
                   sprites.rects[inputImageNum], &spriteRect);

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

static void renderPerfMetrics(void)
{
    if (!perfMetricsActive)
        return;

    int winW = 350;
    int winH = 140;

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
        perfMetricsActive = false;
        return;
    }
    nk_end(ctx);
}

// ============================================================
// Font Debug
// ============================================================

static void toggleFontDebug(void)
{
    if (fontDebugActive)
    {
        exitFontDebug();
        return;
    }

    fontIndex = 0;
    fontSize  = 24;
    reloadDebugFont();
    fontDebugActive = true;
}

static void exitFontDebug(void)
{
    if (debugFont)
    {
        TTF_CloseFont(debugFont);
        debugFont = NULL;
    }
    if (previewTex)
    {
        SDL_DestroyTexture(previewTex);
        previewTex = NULL;
    }
    fontDebugActive = false;
}

static void renderFontDebug(void)
{
    if (!fontDebugActive)
        return;

    int winW = 400;
    int winH = 250;

    struct nk_context *ctx = GUI_GetContext();
    if (nk_begin(ctx, "Font Debug",
                 nk_rect(centerI(config.WIN_W, winW),
                         centerI(config.WIN_H, winH),
                         winW, winH),
                 NK_WINDOW_BORDER | NK_WINDOW_MOVABLE | NK_WINDOW_TITLE))
    {
        int prevIndex = fontIndex;
        int prevSize  = fontSize;

        // Selector de fuente
        nk_layout_row_dynamic(ctx, 30, 1);
        fontIndex = nk_combo(ctx, fontNames, FONT_COUNT, fontIndex, 25, nk_vec2(200, 120));

        // Tamanho
        nk_layout_row_dynamic(ctx, 30, 1);
        nk_property_int(ctx, "Size:", 8, &fontSize, 72, 1, 1);

        // Campo de texto para preview
        nk_layout_row_dynamic(ctx, 30, 1);
        nk_edit_string(ctx, NK_EDIT_SIMPLE, previewText, &previewLen,
                       (int)sizeof(previewText) - 1, nk_filter_default);

        // Recargar si cambio fuente o tamanho
        if (fontIndex != prevIndex || fontSize != prevSize)
            reloadDebugFont();

        // Info de la fuente
        if (debugFont)
        {
            char info[64];
            snprintf(info, sizeof(info), "%s  %dpx", fontNames[fontIndex], fontSize);
            nk_layout_row_dynamic(ctx, 20, 1);
            nk_label(ctx, info, NK_TEXT_CENTERED);
        }
    }
    else
    {
        nk_end(ctx);
        exitFontDebug();
        return;
    }
    nk_end(ctx);

    // --- Preview renderizada con SDL ---
    previewText[previewLen] = '\0';
    rebuildFontPreview();

    if (previewTex)
    {
        int texW, texH;
        SDL_QueryTexture(previewTex, NULL, NULL, &texW, &texH);

        SDL_Rect dst = {
            centerI(config.WIN_W, texW),
            config.WIN_H / 2 + winH / 2 + 20,
            texW, texH
        };
        SDL_RenderCopy(render, previewTex, NULL, &dst);
    }
}

// ============================================================
// Eventos
// ============================================================

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

            if (!frameDebugActive)
                return;

            int maxX = sprites.rects[inputImageNum]->w - framePointer->w;
            int maxY = sprites.rects[inputImageNum]->h - framePointer->h;

            if (key == SDLK_LEFT)  framePointer->x -= framePointer->w;
            if (key == SDLK_RIGHT) framePointer->x += framePointer->w;
            if (key == SDLK_UP)    framePointer->y -= framePointer->h;
            if (key == SDLK_DOWN)  framePointer->y += framePointer->h;

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
            if (!frameDebugActive || event.button.button != SDL_BUTTON_LEFT)
                break;

            int imgW = sprites.rects[inputImageNum]->w * zoom;
            int imgH = sprites.rects[inputImageNum]->h * zoom;
            int imgX = (config.WIN_W - imgW) / 2 + panX;
            int imgY = (config.WIN_H - imgH) / 2 + panY;

            SDL_Point mousePoint = {event.button.x, event.button.y};
            SDL_Rect  spriteRect = {imgX, imgY, imgW, imgH};

            if (SDL_PointInRect(&mousePoint, &spriteRect))
            {
                dragging   = true;
                lastMouseX = event.button.x;
                lastMouseY = event.button.y;
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
            if (!frameDebugActive)
                break;

            float dy = event.wheel.preciseY;
            if (event.wheel.direction == SDL_MOUSEWHEEL_FLIPPED)
                dy *= -1;

            zoom += dy * 0.1f;
            if (zoom < 0.1f)  zoom = 0.1f;
            if (zoom > 10.0f) zoom = 10.0f;
            break;
        }
    }
}

// ============================================================
// Master
// ============================================================

void renderDebug(void)
{
    renderFrameDebug();
    renderPerfMetrics();
    renderFontDebug();
    renderDebugMenu();
}
