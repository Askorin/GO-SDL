#ifndef PLAYER_INPUT_PROCESSING_H
#define PLAYER_INPUT_PROCESSING_H
#include <SDL2/SDL.h>
#include <stdio.h>
#include <stdbool.h>
#include "enums.h" 
#include "buttons.h"
#include "struct_objects.h"

/* Revisa si el click izquierdo colisionó con un "hitbox" válido */
bool check_mdown(game_stats_t*, int[19][19], int[19][19], SDL_MouseButtonEvent*, button_t*[2]);

/* Chequea cuál botón se presionó en el juego */
int check_game_btn_press(button_t*[2], SDL_MouseButtonEvent*);

/* Chequea cuál botón se presionó en el menú */
void check_menu_btn_press(button_t*[4], SDL_MouseButtonEvent*, state_t*);

/* Chequea cuál botón se presionó en el setting de partida, si se presionó start, se retorna true */
bool check_game_set_btn_press(button_t*, toggle_button_group_t*, SDL_MouseButtonEvent*);

#endif

