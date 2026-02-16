/**
 * @file text.h
 * @brief Sistema de renderizado de texto con cache de texturas SDL.
 *
 * Proporciona una API para crear, manipular y dibujar texto en pantalla
 * utilizando SDL_ttf. Las texturas se cachean y solo se re-renderizan
 * cuando el contenido cambia.
 */

#ifndef TEXT_H
#define TEXT_H

#include <SDL.h>
#include <SDL_ttf.h>
#include <stdbool.h>

// ============================================================
//  Fuentes disponibles
// ============================================================

/** @brief Fuente estilo arcade retro (Press Start 2P). */
#define ARCADE_FONT "PressStart2P-Regular.ttf"

/** @brief Fuente estilo pantalla LCD. */
#define LCD_FONT "LcdSolid.ttf"

/** @brief Fuente estilo terminal VT323. */
#define VT_FONT "VT323-Regular.ttf"

/** @brief Fuente Jersey 10 Regular. */
#define JERSEY_FONT "Jersey10-Regular.ttf"

// ============================================================
//  Tipos
// ============================================================

/**
 * @brief Estructura de texto con cache de textura.
 *
 * Almacena toda la informacion necesaria para renderizar y dibujar
 * un texto en pantalla, incluyendo una textura cacheada que solo
 * se regenera cuando el contenido cambia.
 */
typedef struct {
    SDL_Texture *texture;   /**< @brief Textura cacheada del texto renderizado. */
    SDL_Rect rect;          /**< @brief Posicion (x, y) y dimensiones (w, h) en pantalla. */
    SDL_Color color;        /**< @brief Color RGBA del texto. */
    TTF_Font *font;         /**< @brief Puntero a la fuente TTF utilizada. */
    char *content;          /**< @brief Cadena con el texto actual (usada para comparar cambios). */
} Text;

// ============================================================
//  Sistema de texto
// ============================================================

/**
 * @brief Inicializa el sistema de texto cargando la fuente por defecto.
 *
 * Debe llamarse una vez antes de crear cualquier objeto Text.
 *
 * @param fontPath Ruta al archivo de fuente TTF.
 * @param defaultSize Tamano en puntos de la fuente por defecto.
 * @return true si la fuente se cargo correctamente, false en caso de error.
 */
bool Text_InitSystem(const char *fontPath, int defaultSize);

/**
 * @brief Cierra el sistema de texto y libera la fuente por defecto.
 *
 * Llama a TTF_Quit() internamente. Debe invocarse al finalizar el uso
 * del modulo de texto.
 */
void Text_QuitSystem(void);

// ============================================================
//  Creacion y manipulacion de texto
// ============================================================

/**
 * @brief Crea un texto en la posicion indicada con el color por defecto (blanco).
 *
 * @param content Cadena de texto a mostrar.
 * @param x Coordenada X en pantalla.
 * @param y Coordenada Y en pantalla.
 * @return Estructura Text inicializada y renderizada.
 */
Text Text_Create(const char *content, int x, int y);

/**
 * @brief Crea un texto en la posicion indicada con un color personalizado.
 *
 * @param content Cadena de texto a mostrar.
 * @param x Coordenada X en pantalla.
 * @param y Coordenada Y en pantalla.
 * @param color Color RGBA para el texto.
 * @return Estructura Text inicializada y renderizada.
 */
Text Text_CreateColored(const char *content, int x, int y, SDL_Color color);

/**
 * @brief Actualiza el contenido del texto (solo re-renderiza si cambia).
 *
 * Compara el contenido nuevo con el actual; si son iguales no realiza
 * ninguna operacion, evitando re-renderizados innecesarios.
 *
 * @param text Puntero al objeto Text a actualizar.
 * @param content Nueva cadena de texto. Puede ser NULL para limpiar.
 */
void Text_Set(Text *text, const char *content);

/**
 * @brief Dibuja el texto en pantalla usando el renderer global.
 *
 * @param text Puntero al objeto Text a dibujar.
 */
void Text_Draw(Text *text);

/**
 * @brief Libera todos los recursos asociados a un objeto Text.
 *
 * Destruye la textura y libera la cadena de contenido.
 *
 * @param text Puntero al objeto Text a liberar.
 */
void Text_Free(Text *text);

#endif
