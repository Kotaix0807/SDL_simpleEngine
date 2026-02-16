/**
 * @file tools.h
 * @brief Funciones y macros de utilidad general para el motor SDL.
 *
 * Incluye algoritmos, manejo de archivos, utilidades de strings,
 * funciones de debug y helpers para SDL.
 */

#ifndef TOOLS_H
#define TOOLS_H

#include <SDL.h>

// ============================================================
// Macros de utilidad
// ============================================================

/** @brief Obtiene la cantidad de elementos de un arreglo estatico. */
#define ARRAY_L(a) (sizeof(a) / sizeof(a[0]))

/** @brief Concatena dos tokens en tiempo de compilacion. */
#define CAT(a, b) a ## b

/** @brief Marca una variable como intencionalmente no utilizada. */
#define UNUSED(x) ((void)(x))

// ============================================================
// Tipos
// ============================================================

/**
 * @brief Tipos de recurso validos para la carga de archivos.
 */
typedef enum
{
    IMAGE = 0, /**< @brief Recurso de tipo imagen. */
    SOUND      /**< @brief Recurso de tipo sonido. */
}valid_type;

// ============================================================
// Algoritmos
// ============================================================

/**
 * @brief Busqueda binaria recursiva sobre un arreglo de enteros.
 *
 * @param arr   Arreglo base ordenado.
 * @param left  Limite izquierdo (indice inicial).
 * @param right Limite derecho (indice final).
 * @param key   Dato a encontrar.
 * @return int  Indice del dato encontrado, o -1 si no existe.
 */
int recBinarySearch(int arr[], int left, int right, int key);

// ============================================================
// Sistema de archivos
// ============================================================

/**
 * @brief Cuenta la cantidad de archivos en un directorio.
 *
 * @param path Ruta del directorio.
 * @return int Cantidad de archivos, o -1 en caso de error.
 */
int filesInDir(char *path);

/**
 * @brief Obtiene un arreglo con los nombres de archivo de un directorio,
 *        filtrados por extensiones validas.
 *
 * @param path       Ruta del directorio.
 * @param outCount   Puntero donde se almacena la cantidad de archivos encontrados.
 * @param extensions Arreglo de extensiones permitidas (ej. ".png", ".wav").
 * @param extCount   Cantidad de extensiones en el arreglo.
 * @param type       Tipo de recurso para mensajes de debug.
 * @return char**    Arreglo de strings con los nombres de archivo, o NULL en caso de error.
 */
char **getFilesFromDir(char *path, int *outCount, const char **extensions, int extCount, valid_type type);

/**
 * @brief Libera un arreglo de strings asignado dinamicamente.
 *
 * @param array Arreglo de strings.
 * @param n     Cantidad de elementos a liberar.
 */
void freeStringArray(char **array, int n);

// ============================================================
// Debug
// ============================================================

/**
 * @brief Imprime texto en stderr solo si el modo debug esta activado.
 *
 * @param error Formato de texto estilo printf.
 * @param ...   Argumentos variables del formato.
 */
void printDebug(char *error, ...);

/**
 * @brief Devuelve un string legible del tipo de recurso admitido.
 *
 * Ejemplo: valid_type IMAGE retorna "imagen".
 *
 * @param type Tipo de recurso.
 * @return char* Nombre del tipo como cadena estatica.
 */
char *typeAdmited(valid_type type);

// ============================================================
// Utilidades SDL
// ============================================================

/**
 * @brief Obtiene el ancho y alto de una textura SDL.
 *
 * @param texture Textura SDL de la cual obtener las dimensiones.
 * @param width   Puntero donde se almacena el ancho.
 * @param height  Puntero donde se almacena el alto.
 */
void GetTextureSize(SDL_Texture* texture, int* width, int* height);

/**
 * @brief Crea y devuelve un SDL_Color con los componentes indicados.
 *
 * @param r Componente rojo   (0-255).
 * @param g Componente verde  (0-255).
 * @param b Componente azul   (0-255).
 * @param a Componente alfa   (0-255).
 * @return SDL_Color Estructura de color resultante.
 */
SDL_Color setColour(Uint8 r, Uint8 g, Uint8 b, Uint8 a);

// ============================================================
// Utilidades de strings
// ============================================================

/**
 * @brief Devuelve la longitud (en bytes) del string mas largo de un arreglo.
 *
 * @param arr Arreglo de strings.
 * @param n   Cantidad de elementos en el arreglo.
 * @return int Longitud del string mas largo.
 */
int largestStr(char *arr[], int n);

/**
 * @brief Devuelve la longitud (en caracteres UTF-8) del string mas largo de un arreglo.
 *
 * @param arr Arreglo de strings.
 * @param n   Cantidad de elementos en el arreglo.
 * @return int Longitud en caracteres del string mas largo.
 */
int largestStr_bra(char *arr[], int n);

/**
 * @brief Calcula el ancho optimo para mostrar un menu de opciones con titulo.
 *
 * @param choices  Arreglo de strings con las opciones.
 * @param n_choice Cantidad de opciones.
 * @param title    Titulo del menu (puede ser NULL).
 * @return int     Ancho calculado en caracteres.
 */
int largestOpt(char *choices[], int n_choice, const char *title);

/**
 * @brief Cuenta la longitud de un string en caracteres multibyte (UTF-8).
 *
 * @param s String de entrada codificado en UTF-8.
 * @return size_t Cantidad de caracteres (no bytes).
 */
size_t u8_len(const char *s);

/**
 * @brief Reemplaza un elemento del arreglo aplicando su formato con un argumento.
 *
 * Usa el string en arr[idx] como plantilla printf y lo sustituye
 * por el resultado formateado con arg.
 *
 * @param arr Arreglo de strings.
 * @param idx Indice del elemento a formatear.
 * @param arg Argumento para el formato.
 * @return int 0 en exito, -1 en error.
 */
int replace_fmt(char **arr, int idx, const char *arg);

// ============================================================
// Lectura de archivos de texto
// ============================================================

/**
 * @brief Obtiene informacion sobre un archivo de texto.
 *
 * Si opt == 0 devuelve la cantidad de lineas.
 * Si opt != 0 devuelve el ancho maximo de linea en caracteres.
 *
 * @param file Ruta del archivo de texto.
 * @param opt  Selector: 0 para lineas, otro valor para ancho maximo.
 * @return unsigned long Cantidad de lineas o ancho maximo, 0 en error.
 */
unsigned long fileLines(const char *file, int opt);

/**
 * @brief Lee un archivo de texto completo y devuelve un arreglo de strings (una por linea).
 *
 * @param file Ruta del archivo de texto.
 * @return char** Arreglo de strings con cada linea, o NULL en error.
 */
char **readText(const char *file);

int centerI(int a, int b);

#endif
