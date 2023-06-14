#include "headers/player_input_processing.h"
#include <stdlib.h>
#include <math.h>

extern const int SCREEN_WIDTH, SCREEN_HEIGHT, B_PAD;

bool check_mdown(int len, int game_arr[19][19], SDL_MouseButtonEvent* mouse_event, int player,
        int* row, int* col)
{

    /* Flag de validez del movimiento */
    bool is_valid = false;

    /* 
     * x_border es coordenada donde se empieza a renderiza el tablero, queremos encontrar el nodo
     * más cercano al puntero, para esto basta hacer división  de la diferencia con el ancho de cada
     * grid, que es SCREEN_H - 2*B_PAD / (len - 1)
     */
    int grid_w = (SCREEN_HEIGHT - 2 * B_PAD) / (len - 1);
    int x_border = (SCREEN_WIDTH - SCREEN_HEIGHT) / 2 + B_PAD;
    int y_border = B_PAD;

    int closest_col = round((mouse_event->x - x_border) / (float) grid_w);
    int closest_row = round((mouse_event->y - y_border) / (float) grid_w);
    //printf("(%d, %d)\n", closest_row, closest_col);
    
    /* Revisamos que el click haya mapeado a una columna y fila válidas */
    if (closest_row >= 0 && closest_row < len && closest_col >= 0 && closest_col < len) {

        /* Una vez mapeado, calculamos un delta, que será 2/5 del ancho de cada cuadricula */
        float delta = (grid_w * 2) / 5;
        
        /* Convertimos closest_row y closest_col a coordenadas en (x, y) */
        int closest_x = x_border + closest_col * grid_w;
        int closest_y = y_border + closest_row * grid_w;
        /* Condición para decir que efectivamente, se clickeo dentro de un delta alrededor del nodo */
        bool in_x_range = abs(mouse_event->x - closest_x) <= delta;
        bool in_y_range = abs(mouse_event->y - closest_y) <= delta;


        /* Está dentro del delta */
        if (in_x_range && in_y_range && game_arr[closest_row][closest_col] == 0) {
            *row = closest_row;
            *col = closest_col;
            is_valid = true;
        }
    }

    if (!is_valid) printf("Error. Movimiento no puede ser mapeado.\n");

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
