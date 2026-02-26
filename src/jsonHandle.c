#include "jsonHandler.h"
#include "tools.h"
#include <cjson/cJSON.h>
#include <stdio.h>

//#define JSON_DEBUG

bool readASpriteFromJSON(const char *jsonFileName)
{
    // Fase 1: Sacar la informacion del archivo como un string largo:
    static char path[256];
    snprintf(path, sizeof(path), "%s%s", JSON_SPRITE_DIR, jsonFileName);
    FILE *spriteJsonFile = fopen(path, "r");
    if (!spriteJsonFile)
    {
        printDebug(LOG_ERROR, "Error al abrir archivo '%s', ruta completa: '%s'\n", jsonFileName, path);
        return false;
    }
    long int jsonFileSize = fileSize(spriteJsonFile);
    char *buffer = malloc((size_t)jsonFileSize + sizeof(char));
    fread(buffer, 1, (size_t)jsonFileSize, spriteJsonFile);
    buffer[jsonFileSize] = '\0';
    fclose(spriteJsonFile);

    printDebug(LOG_INFO, "Fase 1: Completa\n");

    // Fase 2: Parsear con cJSON
    cJSON *jsonFile = cJSON_Parse(buffer);
    if(!jsonFile)
    {
        const char *errorString = cJSON_GetErrorPtr();
        if(errorString)
            printDebug(LOG_ERROR, "No se pudo parsear el archivo '%s': %s\n", jsonFileName, errorString);
        else
            printDebug(LOG_ERROR, "No se pudo parsear el archivo '%s': Error desconocido.\n", jsonFileName);
        cJSON_Delete(jsonFile);
        free(buffer);
        return false;
    }
    free(buffer);
    printDebug(LOG_INFO, "Fase 2: completa, JSON parseado\n");
    
    cJSON *PacMan = cJSON_GetObjectItemCaseSensitive(jsonFile, "PacMan");
    printDebug(LOG_INFO, "Se obtuvo el objeto\n");

    cJSON_Delete(jsonFile);
    return true;
}

#ifdef JSON_DEBUG

#include "config.h"

int main()
{
    config.debug_mode = true;
    cleanLogFolder();
    initLog();
    readASpriteFromJSON("pacman.json");
    closeLog();
    return 0;
}

#endif