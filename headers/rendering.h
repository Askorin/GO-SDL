#ifndef RENDERING_H
#define RENDERING_H
#include <SDL2/SDL.h>
#include <stdio.h>
#include "enums.h"

/* Renderiza las piezas en base a la matriz de juego */
void render_game_state(int len, int[len][len], SDL_Renderer*, SDL_Texture*[OBJ_QTY], SDL_Rect*);

/* Renderiza los botones del menú principal */
void render_menu_buttons(SDL_Renderer*, SDL_Texture*[OBJ_QTY], SDL_Rect*, SDL_Rect*);


#endif

