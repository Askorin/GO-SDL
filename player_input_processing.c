#include "headers/player_input_processing.h"

bool check_mdown(int len, int game_arr[19][19], SDL_MouseButtonEvent* mouse_event, int player,
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

void check_menu_btn_press(button_t* button_ptrs[4], SDL_MouseButtonEvent* mouse_event,
        state_t* state_ptr)
{

    int x = mouse_event->x;
    int y = mouse_event->y;

    for (int i = 0; i < 4; ++i) {
        button_t* btn_ptr = button_ptrs[i];
        bool in_x_range = x >= btn_ptr->rect.x && x <= btn_ptr->rect.x + btn_ptr->rect.w;
        bool in_y_range = y >= btn_ptr->rect.y && y <= btn_ptr->rect.y + btn_ptr->rect.h;
        if (in_x_range && in_y_range) *state_ptr = btn_ptr->st_event;
    }

    printf("State is now of index: %d\n", *state_ptr);

}

bool check_game_set_btn_press(button_t* start_btn_obj, toggle_button_group_t* board_size_btns,
        SDL_MouseButtonEvent* mouse_event)
{
    bool start_game = false;
    /* Chequeamos si es que se presionó el botón start */
    int x = mouse_event->x;
    int y = mouse_event->y;
    bool in_x_range = x >= start_btn_obj->rect.x &&  x <= start_btn_obj->rect.x + start_btn_obj->rect.w;
    bool in_y_range = y >= start_btn_obj->rect.y && y <= start_btn_obj->rect.y + start_btn_obj->rect.h;
    if (in_x_range && in_y_range) {
        start_game = true;
        //*state_ptr = start_btn_obj->st_event;
    }

    /* Loopeamos sobre los botones toggle */
    for (int i = 0; i < board_size_btns->len; ++i) {
        toggle_button_t* btn_obj_ptr = board_size_btns->toggle_button_ptrs[i];
        bool in_x_range = x >= btn_obj_ptr->rect.x && x <= btn_obj_ptr->rect.x + btn_obj_ptr->rect.w;
        bool in_y_range = y >= btn_obj_ptr->rect.y && y <= btn_obj_ptr->rect.y + btn_obj_ptr->rect.h;
        if (in_x_range && in_y_range) {
            /* presionamos el botón, USAR LA FUNCIÓN POR FAVOR */
            press_toggle_btn(board_size_btns, i);
            //btn_obj_ptr->toggle = true;
        }
    }

    return start_game;

}
