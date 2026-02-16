/*
 * ============================================================================
 *                          SDL2 - NOTAS DE REFERENCIA
 * ============================================================================
 *
 *  Archivo de referencia personal. Extensión .c para resaltado de sintaxis.
 *  No se compila.
 */

#include <SDL.h>
#include <SDL_image.h>
#include <SDL_ttf.h>
#include <SDL_mixer.h>

/* ============================================================================
 *  VENTANAS
 * ============================================================================ */

/*
 * SDL_CreateWindow()
 * Crea una nueva ventana con título, posición, tamaño y flags.
 */
SDL_Window *window = SDL_CreateWindow(
    "Mi Juego",                            // Título
    SDL_WINDOWPOS_CENTERED,                // Posición X
    SDL_WINDOWPOS_CENTERED,                // Posición Y
    1920, 1080,                            // Ancho, Alto
    SDL_WINDOW_RESIZABLE                   // Flags
);

/*
 * Flags de ventana (se pueden combinar con |):
 *
 *   SDL_WINDOW_FULLSCREEN            Fullscreen real (cambia resolución del monitor)
 *   SDL_WINDOW_FULLSCREEN_DESKTOP    Fullscreen sin cambiar resolución (borderless)
 *   SDL_WINDOW_RESIZABLE             Permite redimensionar la ventana
 *   SDL_WINDOW_BORDERLESS            Sin bordes ni barra de título
 *   SDL_WINDOW_MINIMIZED             Inicia minimizada
 *   SDL_WINDOW_MAXIMIZED             Inicia maximizada
 *   SDL_WINDOW_HIDDEN                Inicia oculta
 *   SDL_WINDOW_SHOWN                 Inicia visible (por defecto)
 *   SDL_WINDOW_ALWAYS_ON_TOP         Siempre encima de otras ventanas
 */
Uint32 flags = SDL_WINDOW_RESIZABLE | SDL_WINDOW_SHOWN;

/*
 * SDL_SetWindowSize(window, w, h)
 * Cambia el tamaño FISICO de la ventana (los píxeles reales en el escritorio).
 * No afecta las coordenadas lógicas del renderer.
 */
SDL_SetWindowSize(window, 1280, 720);

/*
 * SDL_GetWindowSize(window, &w, &h)
 * Obtiene el tamaño actual de la ventana en píxeles de pantalla.
 */
int win_w, win_h;
SDL_GetWindowSize(window, &win_w, &win_h);

/*
 * SDL_SetWindowPosition(window, x, y)
 * Mueve la ventana a la posición (x, y) en pantalla.
 *
 * Valores especiales para x/y:
 *   100, 200                              Posición absoluta en píxeles
 *   SDL_WINDOWPOS_CENTERED                Centra en el monitor principal
 *   SDL_WINDOWPOS_CENTERED_DISPLAY(n)     Centra en el monitor n
 *   SDL_WINDOWPOS_UNDEFINED               El SO decide la posición
 *   SDL_WINDOWPOS_UNDEFINED_DISPLAY(n)    El SO decide, pero en el monitor n
 *
 * Se pueden mezclar valores entre X e Y:
 */
SDL_SetWindowPosition(window, SDL_WINDOWPOS_CENTERED_DISPLAY(1), 100);

/*
 * SDL_SetWindowFullscreen(window, flags)
 * Cambia el modo fullscreen en runtime.
 *
 *   SDL_WINDOW_FULLSCREEN              Fullscreen real (cambia resolución)
 *   SDL_WINDOW_FULLSCREEN_DESKTOP      Borderless fullscreen
 *   0                                  Vuelve a modo ventana
 */
SDL_SetWindowFullscreen(window, SDL_WINDOW_FULLSCREEN);
SDL_SetWindowFullscreen(window, 0);   // Salir de fullscreen

/*
 * SDL_RestoreWindow(window)
 * Restaura una ventana minimizada o maximizada a su tamaño y posición normales.
 * Si ya estaba en estado normal, no hace nada.
 * Equivale a pulsar el botón "restaurar" de la barra de título.
 */
SDL_RestoreWindow(window);

/*
 * SDL_MinimizeWindow / SDL_MaximizeWindow
 */
SDL_MinimizeWindow(window);
SDL_MaximizeWindow(window);

/*
 * SDL_SetWindowTitle(window, "Nuevo título")
 * Cambia el título de la ventana en runtime.
 */
SDL_SetWindowTitle(window, "Nuevo título");

/* ============================================================================
 *  MONITORES
 * ============================================================================ */

/*
 * SDL_GetNumVideoDisplays()
 * Devuelve la cantidad de monitores conectados.
 */
int numDisplays = SDL_GetNumVideoDisplays();

/*
 * SDL_GetDisplayName(index)
 * Devuelve el nombre del monitor.
 */
const char *name = SDL_GetDisplayName(0);

/*
 * SDL_GetDisplayBounds(index, &rect)
 * Obtiene la posición y resolución de un monitor.
 * rect.x, rect.y = posición en el escritorio virtual.
 */
SDL_Rect bounds;
SDL_GetDisplayBounds(0, &bounds);
// bounds.w = ancho, bounds.h = alto

/*
 * SDL_GetCurrentDisplayMode(index, &mode)
 * Obtiene el modo actual del monitor (resolución + refresh rate).
 */
SDL_DisplayMode mode;
SDL_GetCurrentDisplayMode(0, &mode);
// mode.w, mode.h, mode.refresh_rate

/*
 * SDL_GetWindowDisplayIndex(window)
 * Devuelve en qué monitor está la ventana actualmente.
 */
int currentDisplay = SDL_GetWindowDisplayIndex(window);

/*
 * SDL_WINDOWPOS_CENTERED_DISPLAY(n)
 * Macro para centrar la ventana en el monitor n (0, 1, 2...).
 * Se usa en SDL_CreateWindow o SDL_SetWindowPosition.
 */
SDL_SetWindowPosition(window,
    SDL_WINDOWPOS_CENTERED_DISPLAY(1),
    SDL_WINDOWPOS_CENTERED_DISPLAY(1)
);

/* ============================================================================
 *  RENDERER
 * ============================================================================ */

/*
 * SDL_CreateRenderer(window, index, flags)
 *
 *   index: -1 para que SDL elija el mejor driver disponible.
 *
 *   Flags:
 *     SDL_RENDERER_ACCELERATED      Usa GPU (hardware)
 *     SDL_RENDERER_SOFTWARE         Usa CPU (software, lento)
 *     SDL_RENDERER_PRESENTVSYNC     Sincroniza con el refresh del monitor
 *     SDL_RENDERER_TARGETTEXTURE    Permite render a textura
 */
SDL_Renderer *render = SDL_CreateRenderer(window, -1,
    SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC
);

/*
 * SDL_RenderSetLogicalSize(renderer, w, h)
 * Define el espacio de coordenadas LOGICO del renderer.
 * Todo lo que dibujes usa estas coordenadas; SDL lo escala al tamaño físico.
 * Mantiene el aspect ratio (añade barras negras si no coincide).
 *
 * Ejemplo:
 *   Ventana física:  1920x1080
 *   Logical size:     960x540
 *
 *   Dibujas en (0, 0, 960, 540) -> SDL lo escala a toda la ventana.
 *   Dibujas en (480, 270)       -> aparece en el centro (píxel real 960, 540).
 *
 * Pasar (0, 0) desactiva el escalado lógico.
 */
SDL_RenderSetLogicalSize(render, 1920, 1080);
SDL_RenderSetLogicalSize(render, 0, 0);    // Desactivar

/*
 * SDL_RenderGetLogicalSize(renderer, &w, &h)
 * Obtiene el tamaño lógico actual. Devuelve (0,0) si no está configurado.
 */
int log_w, log_h;
SDL_RenderGetLogicalSize(render, &log_w, &log_h);

/*
 * SDL_SetRenderDrawColor(renderer, r, g, b, a)
 * Establece el color para las operaciones de dibujo (Clear, DrawRect, etc).
 */
SDL_SetRenderDrawColor(render, 0, 0, 0, 255);

/*
 * SDL_RenderClear(renderer)
 * Limpia toda la pantalla con el color actual (SetRenderDrawColor).
 */
SDL_RenderClear(render);

/*
 * SDL_RenderPresent(renderer)
 * Muestra en pantalla todo lo dibujado desde el último Clear.
 * (Intercambia el back buffer con el front buffer)
 */
SDL_RenderPresent(render);

/*
 * SDL_RenderCopy(renderer, texture, src_rect, dst_rect)
 *
 *   src_rect: Porción de la textura a dibujar (NULL = toda).
 *   dst_rect: Dónde dibujarla en pantalla (NULL = llena todo).
 */
SDL_Rect src = {0, 0, 32, 32};     // Recorte del spritesheet
SDL_Rect dst = {100, 100, 64, 64}; // Posición y tamaño en pantalla
SDL_RenderCopy(render, NULL, &src, &dst);

/*
 * SDL_RenderCopyEx(renderer, texture, src, dst, angle, center, flip)
 * Igual que RenderCopy pero con rotación y espejo.
 *
 *   angle:  Rotación en grados (sentido horario).
 *   center: Punto de rotación (NULL = centro de dst).
 *   flip:   SDL_FLIP_NONE, SDL_FLIP_HORIZONTAL, SDL_FLIP_VERTICAL.
 */
SDL_RenderCopyEx(render, NULL, &src, &dst, 45.0, NULL, SDL_FLIP_HORIZONTAL);

/* ============================================================================
 *  TEXTURAS
 * ============================================================================ */

/*
 * SDL_CreateTexture(renderer, format, access, w, h)
 * Crea una textura vacía.
 *
 *   Acceso:
 *     SDL_TEXTUREACCESS_STATIC     Solo escritura inicial (no se modifica)
 *     SDL_TEXTUREACCESS_STREAMING  Se puede modificar píxel a píxel
 *     SDL_TEXTUREACCESS_TARGET     Se puede usar como destino de render
 */
SDL_Texture *tex = SDL_CreateTexture(render,
    SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, 256, 256
);

/*
 * IMG_LoadTexture(renderer, path)
 * Carga una imagen (PNG, JPG, BMP, etc.) directamente como textura.
 * Requiere SDL_image.
 */
SDL_Texture *sprite = IMG_LoadTexture(render, "assets/sprites/mario.png");

/*
 * SDL_QueryTexture(texture, &format, &access, &w, &h)
 * Obtiene las propiedades de una textura. Se puede pasar NULL en lo que no interese.
 */
int tex_w, tex_h;
SDL_QueryTexture(sprite, NULL, NULL, &tex_w, &tex_h);

/*
 * SDL_SetTextureBlendMode(texture, blendMode)
 *
 *   SDL_BLENDMODE_NONE     Sin transparencia
 *   SDL_BLENDMODE_BLEND    Alpha blending (transparencia normal)
 *   SDL_BLENDMODE_ADD      Aditivo (brillo, partículas)
 *   SDL_BLENDMODE_MOD      Multiplicativo (sombras)
 */
SDL_SetTextureBlendMode(sprite, SDL_BLENDMODE_BLEND);

/*
 * SDL_SetTextureColorMod(texture, r, g, b)
 * Tiñe la textura multiplicando cada píxel por el color dado.
 * (255,255,255) = sin cambio. (255,0,0) = solo canal rojo.
 */
SDL_SetTextureColorMod(sprite, 255, 100, 100);  // Tinte rojizo

/*
 * SDL_SetTextureAlphaMod(texture, alpha)
 * Cambia la opacidad global de la textura (0 = invisible, 255 = opaco).
 */
SDL_SetTextureAlphaMod(sprite, 128);  // 50% transparente

/*
 * SDL_DestroyTexture(texture)
 * Libera la memoria de una textura. Siempre llamar al terminar.
 */
SDL_DestroyTexture(sprite);

/* ============================================================================
 *  EVENTOS E INPUT
 * ============================================================================ */

/*
 * SDL_PollEvent(&event)
 * Saca el siguiente evento de la cola. Devuelve 1 si había evento, 0 si no.
 * Se usa en un while para procesar todos los eventos del frame.
 */
SDL_Event event;
while (SDL_PollEvent(&event)) {
    switch (event.type) {
        case SDL_QUIT:           break;  // Cerrar ventana
        case SDL_KEYDOWN:        break;  // Tecla presionada
        case SDL_KEYUP:          break;  // Tecla soltada
        case SDL_MOUSEBUTTONDOWN:break;  // Click del ratón
        case SDL_MOUSEBUTTONUP:  break;  // Soltar click
        case SDL_MOUSEMOTION:    break;  // Movimiento del ratón
        case SDL_MOUSEWHEEL:     break;  // Rueda del ratón
        case SDL_WINDOWEVENT:    break;  // Eventos de ventana (resize, focus, etc)
    }
}

/*
 * Teclas - event.key.keysym.sym (tipo SDL_Keycode)
 *
 *   SDLK_a ... SDLK_z          Letras
 *   SDLK_0 ... SDLK_9          Números
 *   SDLK_RETURN, SDLK_ESCAPE   Enter, Escape
 *   SDLK_SPACE                  Espacio
 *   SDLK_UP/DOWN/LEFT/RIGHT     Flechas
 *   SDLK_F1 ... SDLK_F12       Teclas de función
 *   SDLK_LSHIFT, SDLK_LCTRL    Modificadores
 */

/*
 * SDL_GetMouseState(&x, &y)
 * Obtiene la posición actual del ratón. Devuelve un bitmask de botones.
 * Cuando SDL_RenderSetLogicalSize está activo, las coordenadas se
 * reportan en el espacio lógico.
 */
int mx, my;
Uint32 buttons = SDL_GetMouseState(&mx, &my);
int leftPressed  = (buttons & SDL_BUTTON(SDL_BUTTON_LEFT));
int rightPressed = (buttons & SDL_BUTTON(SDL_BUTTON_RIGHT));

/*
 * SDL_GetKeyboardState(&numkeys)
 * Devuelve un array con el estado de TODAS las teclas (1 = presionada).
 * Útil para movimiento continuo (no depende de eventos).
 */
const Uint8 *keys = SDL_GetKeyboardState(NULL);
if (keys[SDL_SCANCODE_W]) { /* mover arriba */ }
if (keys[SDL_SCANCODE_A]) { /* mover izquierda */ }

/*
 * SDL_MOUSEWHEEL - Evento de rueda del ratón
 * Se dispara cuando el usuario gira la rueda o hace scroll en el touchpad.
 *
 * Campos de event.wheel:
 *   x         Scroll horizontal (positivo = derecha, negativo = izquierda)
 *   y         Scroll vertical   (positivo = arriba,  negativo = abajo)
 *   direction SDL_MOUSEWHEEL_NORMAL o SDL_MOUSEWHEEL_FLIPPED
 *   preciseX  Scroll horizontal con precisión flotante (SDL 2.0.18+)
 *   preciseY  Scroll vertical con precisión flotante   (SDL 2.0.18+)
 *   mouseX    Posición X del ratón al momento del scroll (SDL 2.26.0+)
 *   mouseY    Posición Y del ratón al momento del scroll (SDL 2.26.0+)
 *
 * IMPORTANTE: Algunos sistemas (macOS "natural scrolling") invierten la
 * dirección. SDL lo indica con event.wheel.direction:
 *
 *   SDL_MOUSEWHEEL_NORMAL   y > 0 = scroll arriba (alejarse de usuario)
 *   SDL_MOUSEWHEEL_FLIPPED  y > 0 = scroll abajo  (dirección invertida)
 *
 * Para normalizar la dirección y que funcione igual en todos los sistemas:
 */
case SDL_MOUSEWHEEL: {
    float scroll_x = event.wheel.preciseX;
    float scroll_y = event.wheel.preciseY;
    if (event.wheel.direction == SDL_MOUSEWHEEL_FLIPPED) {
        scroll_x *= -1;
        scroll_y *= -1;
    }
    // scroll_y > 0 = arriba, scroll_y < 0 = abajo (siempre consistente)
    break;
}

/*
 * Ejemplo: Zoom con la rueda del ratón
 */
float zoom = 1.0f;
case SDL_MOUSEWHEEL: {
    float dy = event.wheel.preciseY;
    if (event.wheel.direction == SDL_MOUSEWHEEL_FLIPPED) dy *= -1;
    zoom += dy * 0.1f;
    if (zoom < 0.1f) zoom = 0.1f;
    if (zoom > 5.0f) zoom = 5.0f;
    break;
}

/*
 * Ejemplo: Scroll de una lista o menú
 */
int scroll_offset = 0;
int item_height = 32;
case SDL_MOUSEWHEEL: {
    int dy = event.wheel.y;
    if (event.wheel.direction == SDL_MOUSEWHEEL_FLIPPED) dy *= -1;
    scroll_offset -= dy * item_height;  // dy>0 (arriba) mueve contenido hacia arriba
    if (scroll_offset < 0) scroll_offset = 0;
    break;
}

/*
 * Nota: event.wheel.y es entero (pasos discretos, normalmente 1 o -1).
 *       event.wheel.preciseY es float (permite scroll suave en touchpads).
 *       Usa preciseY cuando necesites scroll fino (zoom, paneos).
 *       Usa y cuando necesites pasos discretos (navegar ítems de un menú).
 */

/* ============================================================================
 *  TIEMPO
 * ============================================================================ */

/*
 * SDL_GetTicks()
 * Devuelve los milisegundos desde que se inició SDL.
 */
Uint32 now = SDL_GetTicks();

/*
 * SDL_GetTicks64()
 * Igual que GetTicks pero devuelve Uint64 (no se desborda en ~49 días).
 */
Uint64 now64 = SDL_GetTicks64();

/*
 * SDL_Delay(ms)
 * Pausa la ejecución durante ms milisegundos.
 * Útil para limitar FPS manualmente.
 */
SDL_Delay(16);  // ~60 FPS

/*
 * Delta time (patrón típico):
 */
Uint32 lastFrame = SDL_GetTicks();
// En el game loop:
Uint32 currentFrame = SDL_GetTicks();
float deltatime = (currentFrame - lastFrame) / 1000.0f;
lastFrame = currentFrame;
// Uso: posicion.x += velocidad * deltatime;

/* ============================================================================
 *  AUDIO (SDL_mixer)
 * ============================================================================ */

/*
 * Mix_OpenAudio(frequency, format, channels, chunksize)
 * Inicializa el sistema de audio.
 *
 *   frequency: 44100 (CD quality) o 48000
 *   format:    MIX_DEFAULT_FORMAT
 *   channels:  2 (stereo)
 *   chunksize: 2048 (buffer, más bajo = menos latencia)
 */
Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048);

/*
 * Música (para BGM, solo 1 a la vez):
 */
Mix_Music *music = Mix_LoadMUS("assets/music/theme.ogg");
Mix_PlayMusic(music, -1);     // -1 = loop infinito
Mix_PauseMusic();
Mix_ResumeMusic();
Mix_HaltMusic();               // Detener
Mix_VolumeMusic(64);           // Volumen 0-128
Mix_FreeMusic(music);

/*
 * Efectos de sonido (múltiples simultáneos):
 */
Mix_Chunk *sfx = Mix_LoadWAV("assets/sfx/jump.wav");
Mix_PlayChannel(-1, sfx, 0);  // -1 = primer canal libre, 0 = sin loop
Mix_VolumeChunk(sfx, 100);    // Volumen 0-128
Mix_FreeChunk(sfx);

/*
 * Mix_CloseAudio()
 * Cierra el sistema de audio. Llamar al final.
 */
Mix_CloseAudio();

/* ============================================================================
 *  TEXTO (SDL_ttf)
 * ============================================================================ */

/*
 * TTF_Init() / TTF_Quit()
 * Inicializa y cierra el subsistema de fuentes.
 */
TTF_Init();

/*
 * TTF_OpenFont(path, size)
 * Carga una fuente .ttf a un tamaño dado en puntos.
 */
TTF_Font *font = TTF_OpenFont("assets/fonts/PressStart2P-Regular.ttf", 24);

/*
 * TTF_RenderUTF8_Blended(font, text, color)
 * Renderiza texto con anti-aliasing a una SDL_Surface.
 *
 * Métodos de render (de peor a mejor calidad):
 *   TTF_RenderUTF8_Solid      Rápido, sin anti-aliasing, fondo transparente
 *   TTF_RenderUTF8_Shaded     Anti-aliasing, requiere color de fondo
 *   TTF_RenderUTF8_Blended    Mejor calidad, anti-aliasing con alpha blending
 */
SDL_Color white = {255, 255, 255, 255};
SDL_Surface *surface = TTF_RenderUTF8_Blended(font, "Hola mundo", white);
SDL_Texture *textTex = SDL_CreateTextureFromSurface(render, surface);
SDL_FreeSurface(surface);
// Dibujar con SDL_RenderCopy(render, textTex, NULL, &dstRect);

/*
 * TTF_CloseFont(font)
 * Libera la fuente. Llamar al final.
 */
TTF_CloseFont(font);
TTF_Quit();

/* ============================================================================
 *  DIBUJO DE PRIMITIVAS
 * ============================================================================ */

/*
 * Todas usan el color establecido por SDL_SetRenderDrawColor.
 */
SDL_SetRenderDrawColor(render, 255, 0, 0, 255);  // Rojo

SDL_Rect rect = {100, 100, 200, 150};
SDL_RenderDrawRect(render, &rect);     // Rectángulo (solo borde)
SDL_RenderFillRect(render, &rect);     // Rectángulo relleno

SDL_RenderDrawLine(render, 0, 0, 800, 600);       // Línea
SDL_RenderDrawPoint(render, 400, 300);             // Punto individual

/* ============================================================================
 *  DETECCION DE COLISIONES
 * ============================================================================ */

/*
 * SDL_PointInRect(point, rect)
 * Comprueba si un punto (SDL_Point) esta dentro de un rectangulo (SDL_Rect).
 * Devuelve SDL_TRUE si el punto esta dentro, SDL_FALSE si no.
 *
 * Uso tipico: detectar si el mouse esta sobre un objeto.
 */
int mx, my;
SDL_GetMouseState(&mx, &my);

SDL_Point mousePoint = {mx, my};
SDL_Rect objeto = {100, 100, 64, 64};

if (SDL_PointInRect(&mousePoint, &objeto)) {
    // El mouse esta sobre el objeto
}

/*
 * Deteccion manual (equivalente sin SDL_PointInRect):
 *
 *   if (mx >= rect.x && mx <= rect.x + rect.w &&
 *       my >= rect.y && my <= rect.y + rect.h)
 *
 * SDL_PointInRect es una macro inline, no tiene overhead.
 */

/*
 * SDL_HasIntersection(rectA, rectB)
 * Comprueba si dos rectangulos se superponen (colision AABB).
 * Devuelve SDL_TRUE si hay interseccion, SDL_FALSE si no.
 *
 * Es la forma mas comun de detectar colisiones en juegos 2D.
 */
SDL_Rect jugador  = {100, 100, 32, 32};
SDL_Rect enemigo  = {120, 110, 32, 32};

if (SDL_HasIntersection(&jugador, &enemigo)) {
    // Hay colision entre jugador y enemigo
}

/*
 * SDL_IntersectRect(rectA, rectB, result)
 * Igual que HasIntersection, pero ademas calcula el rectangulo de
 * interseccion (el area donde se superponen).
 * Devuelve SDL_TRUE si hay interseccion.
 *
 * Util para saber CUANTO se superponen (por ejemplo, para empujar
 * al jugador fuera de una pared).
 */
SDL_Rect overlap;
if (SDL_IntersectRect(&jugador, &enemigo, &overlap)) {
    // overlap.w y overlap.h indican cuanto se superponen
    // overlap.x y overlap.y indican donde empieza la superposicion
}

/*
 * SDL_UnionRect(rectA, rectB, result)
 * Calcula el rectangulo minimo que contiene a ambos rectangulos.
 * No indica colision; es util para bounding boxes combinados.
 */
SDL_Rect combined;
SDL_UnionRect(&jugador, &enemigo, &combined);
// combined contiene a ambos rectangulos

/*
 * SDL_EnclosePoints(points, count, clip, result)
 * Calcula el rectangulo minimo que encierra un conjunto de puntos.
 *
 *   points: Array de SDL_Point.
 *   count:  Cantidad de puntos.
 *   clip:   Rectangulo de recorte (NULL = sin recorte).
 *   result: Rectangulo resultante.
 *
 * Devuelve SDL_TRUE si al menos un punto queda dentro del clip.
 */
SDL_Point puntos[] = {{10, 20}, {50, 80}, {30, 10}};
SDL_Rect bounding;
SDL_EnclosePoints(puntos, 3, NULL, &bounding);
// bounding = rectangulo minimo que contiene los 3 puntos

/*
 * Ejemplo completo: arrastrar un objeto solo si el mouse lo toca
 *
 *   SDL_MOUSEBUTTONDOWN:
 *     1. Obtener posicion del mouse
 *     2. Verificar si esta dentro del rectangulo del objeto
 *     3. Si esta dentro, activar arrastre
 *
 *   SDL_MOUSEMOTION:
 *     Si esta arrastrando, mover el objeto con el delta del mouse
 *
 *   SDL_MOUSEBUTTONUP:
 *     Desactivar arrastre
 */
SDL_Rect sprite = {200, 150, 64, 64};
SDL_bool arrastrando = SDL_FALSE;

// En el event loop:
case SDL_MOUSEBUTTONDOWN: {
    SDL_Point p = {event.button.x, event.button.y};
    if (SDL_PointInRect(&p, &sprite))
        arrastrando = SDL_TRUE;
    break;
}
case SDL_MOUSEMOTION: {
    if (arrastrando) {
        sprite.x += event.motion.xrel;
        sprite.y += event.motion.yrel;
    }
    break;
}
case SDL_MOUSEBUTTONUP: {
    arrastrando = SDL_FALSE;
    break;
}

/* ============================================================================
 *  CICLO DE VIDA TIPICO
 * ============================================================================ */

/*
 *   SDL_Init()
 *   SDL_CreateWindow()
 *   SDL_CreateRenderer()
 *   SDL_RenderSetLogicalSize()     // Opcional
 *   TTF_Init()
 *   Mix_OpenAudio()
 *
 *   while (running) {
 *       SDL_PollEvent()            // Input
 *       Update()                   // Lógica + delta time
 *       SDL_RenderClear()          // Limpiar
 *       SDL_RenderCopy()           // Dibujar
 *       SDL_RenderPresent()        // Mostrar
 *   }
 *
 *   Mix_CloseAudio()
 *   TTF_Quit()
 *   SDL_DestroyRenderer()
 *   SDL_DestroyWindow()
 *   SDL_Quit()
 */