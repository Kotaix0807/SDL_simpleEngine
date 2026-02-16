// ============================================================
// Includes
// ============================================================
#include <stdbool.h>
#include <SDL_render.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include "SDL_events.h"
#include "SDL_rect.h"
#include "engine.h"
#include "debugging.h"
#include "config.h"
#include "img.h"
#include "gui.h"
#include "tools.h"

// ============================================================
// Frame Debug - Variables
// ============================================================

// -- Publicas (accesibles desde otros .c via extern en debugging.h) --
FrameDebugState fdState = FDEBUG_OFF;  // Estado actual del frame debug
PerfMetricsState pmState = PERF_METRICS_OFF;
DebMenuState mnState = DEB_MENU_OFF;
SDL_Rect *framePointer = NULL;         // Rectangulo selector de frame

// -- Privadas (solo visibles en este archivo) --
static texture sprites = {0};     // Sprites cargados desde SPRITES_DIR
static int inputImageNum = 0;     // Indice de imagen seleccionada en el setup
static int inputFrameW = 16;      // Ancho de frame ingresado en el setup
static int inputFrameH = 16;      // Alto de frame ingresado en el setup
static float zoom = 1.0f;         // Nivel de zoom (controlado con rueda del mouse)
static int panX = 0;              // Desplazamiento horizontal por arrastre del mouse
static int panY = 0;              // Desplazamiento vertical por arrastre del mouse
static bool dragging = false;     // Indica si se esta arrastrando con el mouse
static int lastMouseX = 0;        // Ultima posicion X del mouse durante el arrastre
static int lastMouseY = 0;        // Ultima posicion Y del mouse durante el arrastre

// ============================================================
// Frame Debug - Funciones internas (static)
// ============================================================

// Inicializa el frame pointer y pasa al estado FDEBUG_ACTIVE.
// Se llama desde la ventana de setup al presionar "Start".
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

// ============================================================
// Frame Debug - Funciones publicas
// ============================================================

// Carga los sprites y abre la ventana de setup.
// Si ya esta activo, lo cierra (toggle con F1).
void loadFrameDebug()
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
    inputFrameW = 16;
    inputFrameH = 16;

    activateFrameDebug();
}

// Maneja los eventos de teclado y mouse para el frame debug.
// F1: toggle del frame debug.
// Flechas: mover el frame pointer (con wrap-around).
// Rueda del mouse: zoom (0.1x - 5.0x).
void keyEventHandlerDebug(SDL_Event event)
{
    SDL_Keycode key = event.key.keysym.sym;
    switch (event.type)
	{
        case(SDL_KEYDOWN):
        {
            // F1 activa/desactiva el frame debug desde cualquier estado
            if (key == SDLK_F1)
            {
                loadFrameDebug();
                return;
            }
            if (key == SDLK_F3)
            {
                loadPerfMetrics();
                //loadDebugMenu();
                return;
            }
            if (fdState != FDEBUG_ACTIVE)
                return;

            // Limites maximos basados en el tamanho de la imagen y el frame
            int maxX = sprites.rects[inputImageNum]->w - framePointer->w;
            int maxY = sprites.rects[inputImageNum]->h - framePointer->h;

            // Mover frame pointer en pasos del tamanho del frame (coordenadas fuente, sin zoom)
            if(key == SDLK_LEFT)
                framePointer->x -= framePointer->w;
            if(key == SDLK_RIGHT)
                framePointer->x += framePointer->w;
            if(key == SDLK_UP)
                framePointer->y -= framePointer->h;
            if(key == SDLK_DOWN)
                framePointer->y += framePointer->h;

            // Wrap-around: al pasar del borde, aparece en el lado opuesto
            if(framePointer->x < 0)
                framePointer->x = maxX;
            else if(framePointer->x > maxX)
                framePointer->x = 0;

            if(framePointer->y < 0)
                framePointer->y = maxY;
            else if(framePointer->y > maxY)
                framePointer->y = 0;
            break;
        }
        case(SDL_MOUSEBUTTONDOWN):
        {
            if (fdState != FDEBUG_ACTIVE)
                break;
            if (event.button.button == SDL_BUTTON_LEFT)
            {
                // Calcular posicion actual del sprite en pantalla
                int imgW = sprites.rects[inputImageNum]->w * zoom;
                int imgH = sprites.rects[inputImageNum]->h * zoom;
                int imgX = (config.WIN_W - imgW) / 2 + panX;
                int imgY = (config.WIN_H - imgH) / 2 + panY;

                SDL_Point mousePoint = {event.button.x, event.button.y};
                SDL_Rect spriteRect = {imgX, imgY, imgW, imgH};

                // Solo arrastrar si el mouse esta sobre la textura
                if (SDL_PointInRect(&mousePoint, &spriteRect))
                {
                    dragging = true;
                    lastMouseX = event.button.x;
                    lastMouseY = event.button.y;
                }
            }
            break;
        }
        case(SDL_MOUSEBUTTONUP):
        {
            if (event.button.button == SDL_BUTTON_LEFT)
                dragging = false;
            break;
        }
        case(SDL_MOUSEMOTION):
        {
            if (dragging)
            {
                panX += event.motion.x - lastMouseX;
                panY += event.motion.y - lastMouseY;
                lastMouseX = event.motion.x;
                lastMouseY = event.motion.y;
            }
            break;
        }
        case(SDL_MOUSEWHEEL):
        {
            // Zoom con rueda del mouse, limitado entre 0.1x y 5.0x
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

// Libera todos los recursos del frame debug y restaura el estado.
void exitFrameDebug()
{
    if (framePointer)
    {
        free(framePointer);
        framePointer = NULL;
    }

    freeTextureLib(&sprites);

    fdState = FDEBUG_OFF;
}

// Renderiza el frame debug: panel Nuklear + sprite con rectangulo selector.
void renderFrameDebug()
{
    if (fdState == FDEBUG_OFF)
        return;

    // Aplicar cambios en vivo al framePointer
    framePointer->w = inputFrameW;
    framePointer->h = inputFrameH;
    // --- Ventana Nuklear de configuracion (panel de control en vivo) ---
    struct nk_context *ctx = GUI_GetContext();
    if (nk_begin(ctx, "Frame Debug",
                 nk_rect(0, 0, 300, 220),
                 NK_WINDOW_BORDER | NK_WINDOW_MOVABLE | NK_WINDOW_TITLE))
    {
        int prevImageNum = inputImageNum;

        nk_layout_row_dynamic(ctx, 30, 1);
        nk_property_int(ctx, "Image #:", 0, &inputImageNum, sprites.n - 1, 1, 1);

        nk_layout_row_dynamic(ctx, 30, 1);
        nk_property_int(ctx, "Frame W:", 1, &inputFrameW, sprites.rects[inputImageNum]->w, 1, 1);

        nk_layout_row_dynamic(ctx, 30, 1);
        nk_property_int(ctx, "Frame H:", 1, &inputFrameH, sprites.rects[inputImageNum]->h, 1, 1);

        char buffer[64];
        snprintf(buffer, sizeof(buffer), "X: %d, Y: %d", 
                framePointer->x / framePointer->w,
                framePointer->y / framePointer->h);
        nk_layout_row_dynamic(ctx, 50, 1);
        nk_label(ctx, buffer, NK_TEXT_LEFT);

        // Si cambio la imagen, resetear posicion y ajustar tamanho del frame
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
        // Si el usuario cierra la ventana con X, salir del frame debug
        nk_end(ctx);
        exitFrameDebug();
        return;
    }
    nk_end(ctx);

    // --- Dibujar sprite + rectangulo selector ---

    int imgW = sprites.rects[inputImageNum]->w * zoom;
    int imgH = sprites.rects[inputImageNum]->h * zoom;
    // Centrar la imagen en la ventana + desplazamiento por arrastre
    int imgX = (config.WIN_W - imgW) / 2 + panX;
    int imgY = (config.WIN_H - imgH) / 2 + panY;

    SDL_Rect spriteRect = {
        .x = imgX,
        .y = imgY,
        .w = imgW,
        .h = imgH
    };

    SDL_RenderCopy(render, sprites.textures_array[inputImageNum], sprites.rects[inputImageNum], &spriteRect);
    // Dibujar rectangulo rojo sobre el frame seleccionado
    SDL_Rect drawRect = {
        .x = (int)(framePointer->x * zoom) + imgX,
        .y = (int)(framePointer->y * zoom) + imgY,
        .w = (int)(framePointer->w * zoom),
        .h = (int)(framePointer->h * zoom)
    };
    renderRect(&drawRect, 255, 0, 0, 255);
}

// Lee utime y stime de /proc/self/stat y calcula el % de CPU
// comparando el delta de tiempo de CPU con el delta de tiempo real.
float getCpuUsage()
{
    static unsigned long prev_utime = 0, prev_stime = 0;
    static Uint32 prev_ticks = 0;

    FILE *f = fopen("/proc/self/stat", "r");
    if(!f)
        return -1.0f;

    char buf[512];
    if(!fgets(buf, sizeof(buf), f))
    {
        fclose(f);
        return -1.0f;
    }

    fclose(f);

    // El campo "comm" puede contener espacios, buscar el ultimo ')'
    char *p = strrchr(buf, ')');
    if (!p) 
        return -1.0f;

    p += 2;

    // Saltar 11 campos (state..cmajflt) para llegar a utime(14) y stime(15)
    unsigned long dummy, utime, stime;
    char state;
    sscanf(p, "%c %lu %lu %lu %lu %lu %lu %lu %lu %lu %lu %lu %lu",
           &state, &dummy, &dummy, &dummy, &dummy, &dummy, &dummy,
           &dummy, &dummy, &dummy, &dummy, &utime, &stime);

    Uint32 now = SDL_GetTicks();
    float cpu = 0.0f;

    if (prev_ticks > 0)
    {
        unsigned long delta_cpu = (utime + stime) - (prev_utime + prev_stime);
        float delta_wall = (now - prev_ticks) / 1000.0f;
        long ticks_per_sec = sysconf(_SC_CLK_TCK);

        if (delta_wall > 0)
            cpu = ((float)delta_cpu / ticks_per_sec) / delta_wall * 100.0f;
    }

    prev_utime = utime;
    prev_stime = stime;
    prev_ticks = now;

    return cpu;
}

long getMemoryUsageMB()
{
    FILE *f = fopen("/proc/self/status", "r");
    if (!f) return -1;

    char line[128];
    while (fgets(line, sizeof(line), f))
    {
        long val;
        //if (sscanf(line, "VmRSS: %ld", &val) == 1)
        if(sscanf(line, "RssAnon: %ld", &val) == 1)
        {
            fclose(f);
            return (val / 1024); // en MB
        }
    }
    fclose(f);
    return -1;
}

void loadPerfMetrics()
{
    if(pmState == PERF_METRICS_ON)
    {
        pmState = PERF_METRICS_OFF;
        return;
    }
    else {
        pmState = PERF_METRICS_ON;
    }
}

void renderPerfMetrics()
{
    if(pmState == PERF_METRICS_OFF)
        return;
    static int win_w = 350;
    static int win_h = 140;
    struct nk_context *ctx = GUI_GetContext();
    if (nk_begin(ctx, "Performance Metrics", nk_rect(config.WIN_W - win_w, 0, win_w, win_h),
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
        //Se cierra la ventana
        nk_end(ctx);
        return;
    }
    nk_end(ctx);
}

void loadDebugMenu()
{
    exitFrameDebug();
    pmState = PERF_METRICS_OFF;
    if(mnState == DEB_MENU_ON)
    {
        mnState = DEB_MENU_OFF;
        return;
    }
    else {
        mnState = DEB_MENU_ON;
    }
}

void renderDebugMenu()
{
    if(mnState == DEB_MENU_OFF)
        return;
    static int mnDebWin_w = 250;
    static int mnDebWin_h = 140;
    struct nk_context *ctx = GUI_GetContext();
    ctx->style.window.header.title_align = NK_TEXT_CENTERED;
    if (nk_begin(ctx, "Debug Menu", nk_rect(
        centerI(config.WIN_W, mnDebWin_w),
        centerI(config.WIN_H, mnDebWin_h),
        mnDebWin_w, mnDebWin_h),
    NK_WINDOW_BORDER | NK_WINDOW_TITLE))
    {
        nk_layout_row_dynamic(ctx, 30, 1);
        if (nk_button_label(ctx, "Frame Debug"))
        {
            // se ejecuta cuando se hace click
        }
        nk_layout_row_dynamic(ctx, 30, 1);
        if (nk_button_label(ctx, "Perf. Metrics"))
        {
            mnState = DEB_MENU_OFF;
            ctx->style.window.header.title_align = NK_TEXT_LEFT;
            nk_end(ctx);
            return;
        }
    }
    else
    {
        //Se cierra la ventana
        nk_end(ctx);
        ctx->style.window.header.title_align = NK_TEXT_LEFT;
        return;
    }
    nk_end(ctx);
    ctx->style.window.header.title_align = NK_TEXT_LEFT;
}