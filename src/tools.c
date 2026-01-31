#define _POSIX_C_SOURCE 200809L

#include <stdio.h>
#include <dirent.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <SDL.h>
#include <wchar.h>

#include "config.h"
#include "tools.h"

//#define TOOLS_DEBUG

int binarySearch(int arr[], int left, int right, int key)
{
    if (right >= left)
    {
        int mid = left + (right - left) / 2;

        if (arr[mid] == key)
            return mid;

        if (arr[mid] > key)
            return binarySearch(arr, left, mid - 1, key);
        
        return binarySearch(arr, mid + 1, right, key);
    }
    return -1;
}

int filesInDir(char *path)
{
    DIR *dr = opendir(path);
    struct dirent *de;
    if (dr == NULL)
    {
        printDebug("No se pudo abirir '%s'\n", path);
        return -1;
    }

    int count = 0;
    while ((de = readdir(dr)) != NULL)
    {
        if (strcmp(de->d_name, ".") != 0 && strcmp(de->d_name, "..") != 0)
            count++;
        #ifdef TOOLS_DEBUG
        printf("[ARCHIVOS DETECTADOS]\n");
        printf("%s\n", de->d_name);
        #endif
    }
    closedir(dr);
    return count;
}

static int hasValidExtension(const char *filename, const char **extensions, int extCount, valid_type type)
{
    if (extensions == NULL || extCount <= 0)
        return 1;

    const char *ext = strrchr(filename, '.');
    if (ext == NULL)
        return 0;

    for (int i = 0; i < extCount; i++)
    {
        if (strcmp(ext, extensions[i]) == 0)
            return 1;
    }
    printDebug("El archivo '%s' no es %s\n", filename, typeAdmited(type));
    return 0;
}

char **getFilesFromDir(char *path, int *outCount, const char **extensions, int extCount, valid_type type)
{
    DIR *dr = opendir(path);
    struct dirent *de;
    if (dr == NULL)
    {
        printDebug("No se pudo abrir '%s'\n", path);
        if (outCount)
            *outCount = 0;
        return NULL;
    }

    int capacity = 16;
    int count = 0;
    char **fileArray = malloc(sizeof(char *) * capacity);
    if (fileArray == NULL)
    {
        printDebug("Error al asignar memoria\n");
        closedir(dr);
        if (outCount)
            *outCount = 0;
        return NULL;
    }

    while ((de = readdir(dr)) != NULL)
    {
        if (strcmp(de->d_name, ".") != 0 && strcmp(de->d_name, "..") != 0 &&
            hasValidExtension(de->d_name, extensions, extCount, type))
        {
            if (count >= capacity)
            {
                capacity *= 2;
                char **tmp = realloc(fileArray, sizeof(char *) * capacity);
                if (tmp == NULL)
                {
                    printDebug("Error al reasignar memoria\n");
                    freeStringArray(fileArray, count);
                    closedir(dr);
                    if (outCount)
                        *outCount = 0;
                    return NULL;
                }
                fileArray = tmp;
            }

            fileArray[count] = strdup(de->d_name);
            if (fileArray[count] == NULL)
            {
                printDebug("Error al copiar nombre de archivo\n");
                for (int i = 0; i < count; i++)
                    free(fileArray[i]);
                free(fileArray);
                closedir(dr);
                if (outCount)
                    *outCount = 0;
                return NULL;
            }
            count++;
        }
    }
    closedir(dr);

    if (outCount)
        *outCount = count;
    return fileArray;
}

void freeStringArray(char **array, int n)
{
    for (int i = 0; i < n; i++)
    {
        if(array[i])
            free(array[i]);
    }
    if(array)
        free(array);
}

void printDebug(char *error, ...)
{
    if(config.debug_mode)
    {
        va_list args;
        va_start(args, error);
        vfprintf(stderr, error, args);
        va_end(args);
    }
}

char *typeAdmited(valid_type type)
{
    switch(type)
    {
        case IMAGE:
            return "imagen";
        case SOUND:
            return "sonido";
        default:
            return "desconocido";
    }
}

void GetTextureSize(SDL_Texture* texture, int* width, int* height)
{
    SDL_QueryTexture(texture, NULL, NULL, width, height);
}

#ifdef TOOLS_DEBUG

GameConfig config = {0};  // Definición temporal para el test
int main()
{   
    config.debug_mode = true;
    printError("Error: %d, %d\n", 5, 3);
    return 0;
}

#endif
