#ifndef TERMINATION_FUNCS_H
#define TERMINATION_FUNCS_H

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <stdio.h>
#include "enums.h"

/* Libera las texturas del tablero, piezas de juego, etc */
void free_asset_ptrs(SDL_Texture*[OBJ_QTY], TTF_Font*);

/* Libera window, renderer, cierra TTF y SDL */
void close_sdl(SDL_Window*, SDL_Renderer*);

#endif



