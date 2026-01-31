#ifndef IMG_H
#define IMG_H

#include "SDL_rect.h"
#include <SDL_image.h>
#include <stdbool.h>

typedef struct texture_{
    SDL_Texture **array_textures;
    SDL_Rect **rects;
    int n;
}texture;

bool initTexture(void);
void quitTexture(void);

texture initTextureLib(char *path);
void freeTextureLib(texture *txr);
void assignRect(SDL_Texture *texture, SDL_Rect *rect);

void drawImageF(float x, float y, float w, float h, SDL_Texture *texture);
void drawImage(int x, int y, int w, int h, SDL_Texture *texture);

#endif
