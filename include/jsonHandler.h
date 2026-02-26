#ifndef JSON_H
#define JSON_H

#include <cjson/cJSON.h>
#include <stdbool.h>

#include "config.h"

#define JSON_MAIN_DIR ASSETS_DIR "data/"

#define JSON_SPRITE_DIR JSON_MAIN_DIR "sprites/"

#define JSON_PACMAN JSON_SPRITE_DIR "pacman.json"

bool readASpriteFromJSON(const char *jsonFileName);

#endif