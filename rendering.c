#include "headers/rendering.h"

void render_menu_buttons(SDL_Renderer* renderer, SDL_Texture* textures[OBJ_QTY],
        button_t* button_ptrs[4])
{

    for (int i = 0; i < 4; ++i) {
        button_t* btn_ptr = button_ptrs[i];
        SDL_RenderCopy(renderer, textures[btn_ptr->txt_enum], NULL, &(btn_ptr->rect));
    }
    

}

void render_game_state(int len, int game_arr[19][19], SDL_Renderer* renderer,
        SDL_Texture* textures[OBJ_QTY], SDL_Rect* window_rectangle)
{


    SDL_Rect pc_rectangle;
    
    /* 
     * El ancho óptimo para las piezas es 50x50 en ventanas de 1280x720, mantendrémos
     * ese aspect ratio.
     */

    
    pc_rectangle.w = 65;
    pc_rectangle.h = 65;
     

    /* 
     * Posición centrada de x e y en donde colocar la pieza, tener en consideración que
     * el eje y positivo va hacia abajo, por eso se resta a ambos la mitad de height y
     * width.
     * */
    
    /* Limpiar la pantalla */
    SDL_RenderClear(renderer);

    /* Renderizamos el tablero */ 
    SDL_RenderCopy(renderer, textures[board], NULL, window_rectangle);

    /* Flag de éxito para la actualización del estado del juego */
    for (int i = 0; i < len; ++i) {
        for (int j = 0; j < len; ++j) {
            if (game_arr[i][j] == 0) continue;
            
            pc_rectangle.x = 363 + j*69 - pc_rectangle.w / 2.0;
            pc_rectangle.y = 82 + i*69 - pc_rectangle.h / 2.0;

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
