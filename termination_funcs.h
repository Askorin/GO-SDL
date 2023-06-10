#ifndef TERMINATION_FUNCS_H
#define TERMINATION_FUNCS_H

#include <SDL2/SDL.h>
#include <stdio.h>
#include "enums.h"

#define OBJ_QTY 10
#define BMP_LEN 50

/* Libera las texturas del tablero, piezas de juego, etc */
void free_texture_ptrs(SDL_Texture*[OBJ_QTY]);

/* Libera window, renderer y cierra SDL */
void close_sdl();

#endif



