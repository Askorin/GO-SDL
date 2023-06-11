#include "headers/player_input_processing.h"




bool check_mdown(int len, int game_arr[len][len], SDL_MouseButtonEvent* mouse_event, int player,
        int* row, int* col)
{

    /* Flag de validez del movimiento */
    bool is_valid = false;

    /* Se inicializa variable correspondiente a la coordenada de la esquina superior izquierda */
    int currentx= 363, currenty = 82;
    int sqrside = 557 / (len - 1);

    /* Verifica casilla por casilla si el mouse hizo click en la hitbox */
    for (int i = 0; i < len; i++) {
        currentx = 363;
        for (int j = 0; j < len; j++) {
            if (currentx - sqrside / 2 < (mouse_event->x) &&
                    (mouse_event->x) < currentx + sqrside / 2 &&
                    currenty-sqrside / 2 < (mouse_event->y) &&
                    (mouse_event->y) < currenty + sqrside / 2 &&
                    game_arr[i][j] == 0) {

                /* 
                 * TODO: A futuro, en el momento en que se identifica una "colisión", se podría re-
                 * visar que el movimiento es válido. En ese caso, esta función solo se dedicaría a
                 * chequear colisión.
                 */
                *row = i;
                *col = j;
                is_valid = true;
                /* Chequeamos las piezas muertas del tablero */
                //printf("Checkeando con (%d, %d)\n", i, j);
                //print_game_arr(len, game_arr);
                //find_dead_pieces(len, game_arr);
                goto exit;
            }
            currentx += sqrside;
        }
        currenty+=sqrside;
    }

    if (!is_valid) printf("Error. Movimiento no se encuentra dentro del tablero.\n");

    exit:
    return is_valid;
}

void check_menu_btn_press(button* play_btn_obj, button* opt_btn_obj,
        SDL_MouseButtonEvent* mouse_event, state_type* state_ptr)
{

    int x = mouse_event->x;
    int y = mouse_event->y;
    bool in_x_range = x >= play_btn_obj->rect.x &&  x <= play_btn_obj->rect.x + play_btn_obj->rect.w;
    bool in_y_range = y >= play_btn_obj->rect.y && y <= play_btn_obj->rect.y + play_btn_obj->rect.h;
    if (in_x_range && in_y_range) *state_ptr = game_st;

}
