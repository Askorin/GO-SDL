#ifndef INIT_FUNCS_H
#define INIT_FUNCS_H

#include <SDL2/SDL.h>
#include <stdio.h>
#include <stdbool.h>
#include "enums.h"

/* Incializa SDL y crea una ventana */
bool init_sdl(SDL_Window**);

/* Crea el renderer */
bool create_renderer(SDL_Window*, SDL_Renderer**);

/* Carga todas las superficies y texturas necesarias */
bool load_surfaces_and_textures(SDL_Surface*[OBJ_QTY], SDL_Texture*[OBJ_QTY], char[OBJ_QTY][BMP_LEN],
        SDL_Renderer*); 
#endif



