#define _POSIX_C_SOURCE 200809L

#include <stdio.h>
#include <dirent.h>
#include <stdlib.h>
#include <string.h>

#include "tools.h"

// #define TOOLS_DEBUG

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
        fprintf(stderr, "No se pudo abirir '%s'\n", path);
        return -1;
    }

    int count = 0;
    while ((de = readdir(dr)) != NULL)
    {
        if(strcmp(de->d_name, ".") || strcmp(de->d_name, ".."))
            count++;
        #ifdef TOOLS_DEBUG
        printf("%s\n", de->d_name);
        #endif
    }
    closedir(dr);
    return count;
}

char **fillArr(char *path, int *outCount)
{
    int fileCount = filesInDir(path);
    if (fileCount <= 0)
    {
        if (outCount)
            *outCount = 0;
        return NULL;
    }

    char **fileArray = malloc(sizeof(char *) * fileCount);
    if (fileArray == NULL)
    {
        fprintf(stderr, "Error al asignar memoria\n");
        if (outCount)
            *outCount = 0;
        return NULL;
    }

    DIR *dr = opendir(path);
    struct dirent *de;
    if (dr == NULL)
    {
        fprintf(stderr, "No se pudo abrir '%s'\n", path);
        free(fileArray);
        if (outCount)
            *outCount = 0;
        return NULL;
    }

    int count = 0;
    while ((de = readdir(dr)) != NULL)
    {
        if (strcmp(de->d_name, ".") != 0 && strcmp(de->d_name, "..") != 0)
        {
            fileArray[count] = strdup(de->d_name);
            if (fileArray[count] == NULL)
            {
                fprintf(stderr, "Error al copiar nombre de archivo\n");
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
#ifdef TOOLS_DEBUG

int main()
{
    printf("Se leyo %d archivos en '%s'\n", filesInDir(ASSETS_DIR "sfx"), ASSETS_DIR "sfx");
    return 0;
}


#endif