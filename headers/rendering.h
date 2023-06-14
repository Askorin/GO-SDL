#ifndef RENDERING_H
#define RENDERING_H
#include <SDL2/SDL.h>
#include <stdio.h>
#include "enums.h"
#include "buttons.h"

void render_board(int len, SDL_Renderer*, SDL_Texture*[OBJ_QTY]);

/* Renderiza las piezas en base a la matriz de juego */
void render_game_state(int len, int[19][19], SDL_Renderer*, SDL_Texture*[OBJ_QTY], SDL_Rect*);

/* Renderiza los botones del menú principal */
void render_menu_buttons(SDL_Renderer*, SDL_Texture*[OBJ_QTY], button_t*[4]);

void render_game_set_buttons(SDL_Renderer*, SDL_Texture*[OBJ_QTY], button_t*,
        toggle_button_group_t*, SDL_Rect*);

#endif

