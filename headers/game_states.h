#ifndef GAME_STATES_H
#define GAME_STATES_H

#include <SDL2/SDL.h>
#include <stdio.h>
#include "enums.h"
#include "struct_objects.h"

/* Función de estado del menu */
void menu(SDL_Renderer*, SDL_Texture*[OBJ_QTY], state_t*, SDL_Rect*);

/* Función de estado de juego en tablero */
void play(SDL_Renderer*, SDL_Texture*[OBJ_QTY], int len, int[len][len], state_t*, SDL_Rect*,
        game_stats_t*, int[len][len]);

/* Función previa al juego, elección de tamaño de tablero, settings, etc... */
void game_set(SDL_Renderer*, SDL_Texture*[OBJ_QTY], state_t*, SDL_Rect*, toggle_button_t*[3]);

#endif
