#ifndef CONFIG_H
#define CONFIG_H

#include <stdbool.h>

#define FPS 60
#define FRAME_TIME_MS (1000 / FPS)

#define ASSETS_DIR "assets/"
#define CONFIG_PATH ASSETS_DIR "config/"
#define SFX_DIR ASSETS_DIR "sfx/"
#define MUSIC_DIR ASSETS_DIR "music/"
#define PATH_SIZE(a) sizeof(SFX_DIR) + sizeof(MUSIC_DIR) + sizeof(a)

#define CFG_FILE "hd.ini"

typedef struct {
    char name[128];
    int WIN_W;
    int WIN_H;
    bool fullscreen;
    bool vsync;
    int fps;
    
    int master_volume;
    int music_volume;
    int sfx_volume;
    int audio_frequency;

    bool show_fps;
    bool debug_mode;
} GameConfig;

extern GameConfig config;

bool loadConfig(GameConfig *cfg, char *cfg_name);
void printConfig(GameConfig *cfg);

#endif