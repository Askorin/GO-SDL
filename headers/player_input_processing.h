#ifndef PLAYER_INPUT_PROCESSING_H
#define PLAYER_INPUT_PROCESSING_H
#include <SDL2/SDL.h>
#include <stdio.h>
#include <stdbool.h>
#include "enums.h" 
#include "struct_objects.h"

/* Revisa si el click izquierdo colisionó con un "hitbox" válido */
bool check_mdown(int len, int[len][len], SDL_MouseButtonEvent*, int, int*, int*);

/* Chequea cuál botón se presionó en el menú */
void check_menu_btn_press(button*, button*, SDL_MouseButtonEvent*, state_type*);

#endif

