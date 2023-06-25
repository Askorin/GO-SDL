#include "headers/rendering.h"

extern const int SCREEN_WIDTH, SCREEN_HEIGHT, B_PAD; 

void render_menu_buttons(SDL_Renderer* renderer, SDL_Texture* textures[OBJ_QTY],
        button_t* button_ptrs[4])
{

    for (int i = 0; i < 4; ++i) {
        button_t* btn_ptr = button_ptrs[i];
        SDL_RenderCopy(renderer, textures[btn_ptr->txt_enum], NULL, &(btn_ptr->rect));
    }
    

}


void render_board(int len, SDL_Renderer* renderer, SDL_Texture* textures[OBJ_QTY])
{
    /* 
     * Un pad  entre tablero y tableros laterales, x_border es donde empezar a 
     * renderizar el tablero
     */

    /* Esto para que quede un cuadrado en medio */
    int x_border = (SCREEN_WIDTH - SCREEN_HEIGHT) / 2 + B_PAD;
    int y_border = B_PAD;
    /* 
     * Tendremos que renderizar len * len - 2 * len + 1 grids, asi que queremos que el ancho
     * de los grids multiplicado por len - 1 sea igual a SCREEN_H - 2 * pad, que es la dimensión
     * del espacio que tenemos para hacer el tablero.
     */
    SDL_Rect grid_rect = {
        .x = 0,
        .y = 0, 
        .w = (SCREEN_HEIGHT - 2 * B_PAD) / (len - 1),
        .h = (SCREEN_HEIGHT - 2 * B_PAD) / (len - 1)
    };

    /* 
     * Es cierto que podría ocupar +=, pero prefiero escribir el código así porque es
     * más entendible
     */
    for (int row = 0; row < len - 1; ++row) {
        grid_rect.y = y_border + row * grid_rect.h;
        for (int col = 0; col < len - 1; ++col) {
            grid_rect.x = x_border + col * grid_rect.w;
            SDL_RenderCopy(renderer, textures[grid], NULL, &grid_rect);
        }
    }

}

void render_game_ui(SDL_Renderer* renderer, SDL_Texture* textures[OBJ_QTY], button_t* btn_ptrs[4],
        game_stats_t* game_stats_ptr)
{
    /* Ancho de los paneles */
    int panel_width = (SCREEN_WIDTH - SCREEN_HEIGHT) / 2;
    /* Rectangulo panel lateral izquierdo para jugador negro */
    SDL_Rect black_panel_rect = {
        .x = 0,
        .y = 0,
        .w = panel_width,
        .h = SCREEN_HEIGHT
    };
    SDL_Rect white_panel_rect = {
        .x = SCREEN_WIDTH - panel_width,
        .y = 0,
        .w = panel_width,
        .h = SCREEN_HEIGHT
    };

    SDL_RenderCopy(renderer, textures[black_panel], NULL, &black_panel_rect);
    SDL_RenderCopy(renderer, textures[white_panel], NULL, &white_panel_rect);
    for (int i = 0; i < 4; ++i) {
        button_t* btn_ptr = btn_ptrs[i];
        SDL_RenderCopy(renderer, textures[btn_ptr->txt_enum], NULL, &(btn_ptr->rect)); 
    }

    if (game_stats_ptr->player == 1) {
        SDL_RenderCopy(renderer, textures[panel_mask], NULL, &white_panel_rect);
    } else {
        SDL_RenderCopy(renderer, textures[panel_mask], NULL, &black_panel_rect);
    }

}

void render_game_state(game_stats_t* game_stats_ptr, int game_arr[19][19], SDL_Renderer* renderer,
        SDL_Texture* textures[OBJ_QTY], SDL_Rect* window_rectangle, button_t* button_ptrs[4])
{

    /* Borde de renderizado tablero */
    int x_border = (SCREEN_WIDTH - SCREEN_HEIGHT) / 2 + B_PAD;
    int y_border = B_PAD;
    
    /* Ancho cuardiculas del tablero */
    int grid_w = (SCREEN_HEIGHT - 2 * B_PAD) / (game_stats_ptr->len - 1);
    int grid_h = (SCREEN_HEIGHT - 2 * B_PAD) / (game_stats_ptr->len - 1);

    /* 
     * El ancho de las piezas debería ser el mismo que el de una cuadricula menos un delta, que
     * resulta ser (SCREEN_HEIGHT - 2 * B_PAD) / (len - 1) - delta, esto ya calculado en funciones
     * de renderizado, elegimos un delta arbitrario = 5 o algo
     */
    SDL_Rect pc_rectangle = {
        .x = 0,
        .y = 0,
        .w = (SCREEN_HEIGHT - 2 * B_PAD) / (game_stats_ptr->len - 1) - 5,
        .h = (SCREEN_HEIGHT - 2 * B_PAD) / (game_stats_ptr->len - 1) - 5
    };


    /* Limpiar la pantalla */
    SDL_RenderClear(renderer);

    /* Renderizamos el fondo */
    SDL_RenderCopy(renderer, textures[menu_bck], NULL, window_rectangle);

    /* Renderizamos el tablero */ 
    render_board(game_stats_ptr->len, renderer, textures);

    /* Renderizamos el UI */
    render_game_ui(renderer, textures, button_ptrs, game_stats_ptr) ;

    /* Flag de éxito para la actualización del estado del juego */
    for (int i = 0; i < game_stats_ptr->len; ++i) {
        for (int j = 0; j < game_stats_ptr->len; ++j) {
            if (game_arr[i][j] == 0) continue;
            
            /* 
             * Es renderizar en x_border, más la cantidad de columnas multiplicado por el ancho
             * de las cuadriculas, y después centramos el rectangulo con pc_rectangle.w / 2. lo 
             * mismo con el eje y.
             */
            pc_rectangle.x = x_border + j * grid_w - pc_rectangle.w / 2.0;
            pc_rectangle.y = y_border + i * grid_h - pc_rectangle.h / 2.0;

            if (game_arr[i][j] == 1) {
                /* Dibujamos pieza negra en coordenada correspondiente */
                SDL_RenderCopy(renderer, textures[black_pc], NULL, &pc_rectangle);
            }
            else if (game_arr[i][j] == 2) {
                /* Dibujamos pieza blanca en coordenada correspondiente */
                SDL_RenderCopy(renderer, textures[white_pc], NULL, &pc_rectangle);
            }
        }
    }

    SDL_RenderPresent(renderer);
    
}



void render_game_set_buttons(SDL_Renderer* renderer, SDL_Texture* textures[OBJ_QTY],
        button_t* start_btn_obj, toggle_button_group_t* board_size_btns_ptr,
            SDL_Rect* window_rectangle)
{
    /* Limpiar y dibujar a la pantalla */
    SDL_RenderClear(renderer);
    /* Renderizamos el fondo del menu */
    SDL_RenderCopy(renderer, textures[menu_bck], NULL, window_rectangle);
    /* Renderizamos botones del game_setting */

    SDL_RenderCopy(renderer, textures[start_btn_obj->txt_enum], NULL, &(start_btn_obj->rect));
    for (int i = 0; i < board_size_btns_ptr->len; ++i) {
        toggle_button_t* btn_obj_ptr = board_size_btns_ptr->toggle_button_ptrs[i];
        sf_and_txt_enum_t id = btn_obj_ptr->txt_enum;
        if (btn_obj_ptr->toggle) {
            /* 
             * Si está presionado, renderizamos la versión con la textura presionada, es sumarle
             * uno 
             */
            SDL_RenderCopy(renderer, textures[id + 1], NULL, &(btn_obj_ptr->rect));
        } else {
            /* En caso contrario, renderizamos la versión normal */
            SDL_RenderCopy(renderer, textures[id], NULL, &(btn_obj_ptr->rect));
        }
    }

    SDL_RenderPresent(renderer);
}
