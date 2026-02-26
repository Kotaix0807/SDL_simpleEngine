/**
 * @file img.h
 * @brief Sistema de texturas e imagenes. Carga, dibujo y gestion de librerias de texturas.
 */
#ifndef IMG_H
#define IMG_H

#include <SDL_image.h>
#include <stdbool.h>

// ============================================================
// Tipos
// ============================================================

/**
 * @brief Libreria de texturas cargadas desde un directorio.
 */
typedef struct texture_{
    SDL_Texture **textures_array;  /**< @brief Array de texturas SDL. */
    SDL_Rect **rects;              /**< @brief Array de rectangulos (tamano de cada textura). */
    int n;                         /**< @brief Cantidad de texturas cargadas. */
}texture;

// ============================================================
// Inicializacion y cierre
// ============================================================

/**
 * @brief Inicializa SDL_image (soporte PNG).
 * @return true si se inicializo correctamente, false en caso de error.
 */
bool initTexture(void);

/**
 * @brief Cierra SDL_image y el subsistema de video.
 */
void quitTexture(void);

// ============================================================
// Gestion de librerias de texturas
// ============================================================

/**
 * @brief Carga todas las imagenes de un directorio en una libreria de texturas.
 * @param path Ruta del directorio con las imagenes.
 * @return texture Libreria cargada (n=0 si fallo).
 */
texture initTextureLib(char *path);

/**
 * @brief Libera todos los recursos de una libreria de texturas.
 * @param txr Puntero a la libreria a liberar.
 */
void freeTextureLib(texture *txr);

// ============================================================
// Utilidades de textura
// ============================================================

/**
 * @brief Asigna las dimensiones de una textura a un rectangulo.
 * @param texture Textura fuente.
 * @param rect Rectangulo destino (se sobreescribe x, y, w, h).
 */
void assignRectToTexture(SDL_Texture *texture, SDL_Rect *rect);

// ============================================================
// Dibujo
// ============================================================

/**
 * @brief Dibuja una textura en pantalla con coordenadas float.
 * @param x Posicion X destino.
 * @param y Posicion Y destino.
 * @param w Ancho destino (-1 para usar el original).
 * @param h Alto destino (-1 para usar el original).
 * @param texture Textura a dibujar.
 */
void drawImageF(float x, float y, float w, float h, SDL_Texture *texture);

/**
 * @brief Dibuja una textura en pantalla con coordenadas enteras.
 * @param x Posicion X destino.
 * @param y Posicion Y destino.
 * @param w Ancho destino (-1 para usar el original).
 * @param h Alto destino (-1 para usar el original).
 * @param texture Textura a dibujar.
 */
void drawImage(int x, int y, int w, int h, SDL_Texture *texture);

/**
 * @brief Dibuja las aristas de un rectangulo con un color RGBA.
 * @param rect Rectangulo vacio a dibujar.
 * @param fill Rectangulo relleno.
 * @param r Componente rojo (0-255).
 * @param g Componente verde (0-255).
 * @param b Componente azul (0-255).
 * @param a Componente alpha (0-255).
 */
void renderRect(SDL_Rect *rect, bool fill, Uint8 r, Uint8 g, Uint8 b, Uint8 a);

#endif
