/**
 * @file config.c
 * @brief Implementacion de la carga e impresion de configuracion del motor.
 *        Lee archivos .ini con secciones [Video], [Audio], [Game] y [Debug],
 *        y almacena los valores en una estructura GameConfig.
 */

// ============================================================
// Includes
// ============================================================

#include <stdio.h>
#include <string.h>

#include "tools.h"
#include "config.h"

// ============================================================
// Variables globales
// ============================================================

/** @brief Instancia global de la configuracion, inicializada a cero. */
GameConfig config = {0};

// #define CFG_DEBUG

// ============================================================
// Funciones publicas
// ============================================================

/**
 * @brief Abre y parsea un archivo .ini linea por linea, llenando la
 *        estructura GameConfig con los valores encontrados en cada seccion.
 *        Ignora lineas vacias y comentarios que empiezan con '#'.
 */
bool loadConfig(GameConfig *cfg, char *cfg_name)
{
    FILE *cfg_file = fopen(cfg_name, "r");
    if(!cfg_file)
    {
        printDebug(LOG_ERROR, "No se pudo cargar el archivo '%s', no se encontro o no existe\n", cfg_name);
        return false;
    }
    char title[64];
    char line[256];

    while(fgets(line, sizeof(line), cfg_file))
    {
        if(line[0] == '#' || line[0] == '\n')
            continue;

        if(sscanf(line, " [%[^]]]\n", title) == 1)
            continue;

        int temp = 0;
        char temp_str[128];

        if(!strcmp(title, "Video"))
        {
            if(sscanf(line, "window_name=%s", temp_str) == 1)
                strcpy(cfg->name, temp_str);
            sscanf(line, "width=%d", &cfg->WIN_W);
            sscanf(line, "height=%d", &cfg->WIN_H);
            if(sscanf(line, "fullscreen=%d", &temp) == 1)
                cfg->fullscreen = temp;
            if(sscanf(line, "vsync=%d", &temp) == 1)
                cfg->vsync = temp;
            sscanf(line, "fps=%d", &cfg->fps);
            sscanf(line, "default_monitor=%d", &cfg->defaultMonitor);
        }
        else if(!strcmp(title, "Audio"))
        {
            sscanf(line, "master_volume=%d", &cfg->master_volume);
            sscanf(line, "music_volume=%d", &cfg->music_volume);
            sscanf(line, "sfx_volume=%d", &cfg->sfx_volume);
            sscanf(line, "audio_frequency=%d", &cfg->audio_frequency);
        }
        else if(!strcmp(title, "Game"))
        {
            if(sscanf(line, "show_fps=%d", &temp) == 1)
                cfg->show_fps = temp;
        }
        else if(!strcmp(title, "Debug"))
        {
            if(sscanf(line, "debug_mode=%d", &temp) == 1)
                cfg->debug_mode = temp;
        }
    }
    fclose(cfg_file);
    printf("\n");
    return true;
}

/**
 * @brief Imprime todos los campos de la configuracion a stdout, agrupados
 *        por seccion ([Video], [Audio], [Game], [Debug]). Si el puntero
 *        es NULL, muestra un mensaje de error via printDebug.
 */
void printConfig(GameConfig *cfg)
{
    if(!cfg)
    {
        printDebug(LOG_ERROR, "No se pudo leer el archivo de configuracion, no se encuentra, no existe, o esta corrupto\n");
        return;
    }
    printf("[Video]\n");
    printf("name=%s\n", cfg->name);
    printf("width=%d\n", cfg->WIN_W);
    printf("height=%d\n", cfg->WIN_H);
    printf("fullscreen=%d\n", cfg->fullscreen);
    printf("vsync=%d\n", cfg->vsync);
    printf("fps=%d\n", cfg->fps);
    printf("default_monitor=%d\n\n", cfg->defaultMonitor);
    printf("[Audio]\n");
    printf("master_volume=%d\n", cfg->master_volume);
    printf("music_volume=%d\n", cfg->music_volume);
    printf("sfx_volume=%d\n", cfg->sfx_volume);
    printf("audio_frequency=%d\n\n", cfg->audio_frequency);
    printf("[Game]\n");
    printf("show_fps=%d\n\n", cfg->show_fps);
    printf("[Debug]\n");
    printf("debug_mode=%d\n", cfg->debug_mode);
}

#ifdef CFG_DEBUG
int main()
{
    GameConfig gamecfg = {0};
    loadConfig(&gamecfg, CONFIG_DIR "hd.ini");
    printConfig(&gamecfg);
    printf("\nFPS: %d\n", gamecfg.fps);

    return 0;
}
#endif
