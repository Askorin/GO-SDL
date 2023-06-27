#include "headers/player_input_processing.h"
#include "headers/move_validation.h"
#include "headers/game_saving.h"
#include <stdlib.h>
#include <math.h>

extern const int SCREEN_WIDTH, SCREEN_HEIGHT, B_PAD, PANEL_WIDTH;


bool check_mdown(game_stats_t* game_stats_ptr, int game_arr[19][19], int prev_game_arr[19][19],
        SDL_MouseButtonEvent* mouse_event, button_t* button_ptrs[4], state_t* state_ptr,
            button_t* overlay_menu_btn_ptrs[3], bool* overlay_menu_ptr, SDL_Rect* menu_rect)
{

    /* En caso de que el input hay sido una jugada (pasar o poner pieza) */
    bool is_move = false;

    if (*overlay_menu_ptr) {
        *overlay_menu_ptr = check_menu_overlay_btn_press(overlay_menu_btn_ptrs, mouse_event,
                state_ptr, menu_rect);

        return false;
        printf("hay overlay\n");
    }

    int btn_press = check_game_btn_press(button_ptrs, mouse_event);
    switch (btn_press) {
        /* Se pasó un turno */
        case -1:
            is_move = true;
            /* Si se ha pasado dos veces el turno...*/
            if (process_pass(game_stats_ptr)) {
                /* Terminar el juego */
                *state_ptr = end_game_st;
                printf("Juego terminado con pass\n");
            }
            break;
        case end_game_st:
            /* Se resignó */
            if (process_resign(game_stats_ptr)) {
                /* Terminar el juego */
                *state_ptr = end_game_st;
                printf("Juego terminado con resign\n");
            }
            break;
        case -3:
            /* Se guardó la partida */
            break;
        default:
            break;
    }

    /* 
     * x_border es coordenada donde se empieza a renderiza el tablero, queremos encontrar el nodo
     * más cercano al puntero, para esto basta hacer división  de la diferencia con el ancho de cada
     * grid, que es SCREEN_H - 2*B_PAD / (len - 1)
     */

    int grid_w = (SCREEN_HEIGHT - 2 * B_PAD) / (game_stats_ptr->len - 1);
    int x_border = PANEL_WIDTH + B_PAD;
    int y_border = B_PAD;
    int row = -1, col = -1; 

    int closest_col = round((mouse_event->x - x_border) / (float) grid_w);
    int closest_row = round((mouse_event->y - y_border) / (float) grid_w);
    
    /* Revisamos que el click haya mapeado a una columna y fila válidas */
    if (closest_row >= 0 && closest_row < game_stats_ptr->len && closest_col >= 0 &&
            closest_col < game_stats_ptr->len) {

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
            row = closest_row;
            col = closest_col;
            is_move = process_move(game_stats_ptr, game_arr, row, col, prev_game_arr); 
            printf("Process move: %d\n", is_move);
        }
    }

    if (is_move) {
        game_stats_ptr->pass = false;
    } 

    return is_move;
}

int check_game_btn_press(button_t* button_ptrs[4], SDL_MouseButtonEvent* mouse_event)
{
    int x = mouse_event->x;
    int y = mouse_event->y;
    
    int event = 0;
        
    for (int i = 0; i < 4; ++i) {
        button_t* btn_ptr = button_ptrs[i];
        bool in_x_range = x >= btn_ptr->rect.x && x <= btn_ptr->rect.x + btn_ptr->rect.w;
        bool in_y_range = y >= btn_ptr->rect.y && y <= btn_ptr->rect.y + btn_ptr->rect.h;
        if (btn_ptr->enabled && in_x_range && in_y_range) event = btn_ptr->st_event;
    }
    
    return event;
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
        if (btn_ptr-> enabled && in_x_range && in_y_range) *state_ptr = btn_ptr->st_event;
    }
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
    if (start_btn_obj->enabled && in_x_range && in_y_range) {
        start_game = true;
    }
    /* Loopeamos sobre los botones toggle */
    for (int i = 0; i < board_size_btns->len; ++i) {
        toggle_button_t* btn_obj_ptr = board_size_btns->toggle_button_ptrs[i];
        bool in_x_range = x >= btn_obj_ptr->rect.x && x <= btn_obj_ptr->rect.x + btn_obj_ptr->rect.w;
        bool in_y_range = y >= btn_obj_ptr->rect.y && y <= btn_obj_ptr->rect.y + btn_obj_ptr->rect.h;
        if (in_x_range && in_y_range) {
            /* presionamos el botón, USAR LA FUNCIÓN POR FAVOR */
            press_toggle_btn(board_size_btns, i);
        }
    }

    return start_game;

}

bool check_menu_overlay_btn_press(button_t* btn_ptrs[3], SDL_MouseButtonEvent* mouse_event,
        state_t* state_ptr, SDL_Rect* menu_rect)
{
    int x = mouse_event->x;
    int y = mouse_event->y;
    
    bool keep_overlay = false;

    bool in_x_range_menu = x >= menu_rect->x && x <= menu_rect->x + menu_rect->w;
    bool in_y_range_menu = y >= menu_rect->y && y <= menu_rect->y + menu_rect->h;
    bool in_menu = in_x_range_menu && in_y_range_menu;
    if (in_menu) {
        keep_overlay = true;
        for (int i = 0; i < 3; ++i) {
            button_t* btn_ptr = btn_ptrs[i];
            bool in_x_range = x >= btn_ptr->rect.x && x <= btn_ptr->rect.x + btn_ptr->rect.w;
            bool in_y_range = y >= btn_ptr->rect.y && y <= btn_ptr->rect.y + btn_ptr->rect.h;
            if (btn_ptr-> enabled && in_x_range && in_y_range) *state_ptr = btn_ptr->st_event;
        }
    }
    return keep_overlay;
}

void check_save_game_mdown(game_stats_t* game_stats_ptr, int game_arr[19][19],
        int prev_game_arr[19][19], SDL_MouseButtonEvent* mouse_event, button_t* save_btn_ptr,
            state_t* state_ptr, char* save_name)
{
    int x = mouse_event->x;
    int y = mouse_event->y;
    bool in_x_range_save = x >= save_btn_ptr->rect.x && x <= save_btn_ptr->rect.x +
        save_btn_ptr->rect.w;

    bool in_y_range_save = y >= save_btn_ptr->rect.y && y <= save_btn_ptr->rect.y +
        save_btn_ptr->rect.h;

    bool in_range_save = in_x_range_save && in_y_range_save;
    if (in_range_save) {
        /* save the gaaaaameee */
        printf("Guardando el juego con nombre de archivo %s.bin\n", save_name);
        if (create_save(game_stats_ptr, prev_game_arr, game_arr, save_name)) {
            printf("Save creado con éxito\n");
        *state_ptr = save_btn_ptr->st_event;
        }
    }


}


