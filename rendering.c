#include "headers/rendering.h"

extern const int SCREEN_WIDTH, SCREEN_HEIGHT, B_PAD, PANEL_WIDTH, OVERLAY_MENU_WIDTH; 

void render_menu_buttons(SDL_Renderer* renderer, SDL_Texture* textures[OBJ_QTY],
        button_t* button_ptrs[5])
{
    for (int i = 0; i < 5; ++i) {
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
        game_stats_t* game_stats_ptr, TTF_Font* font)
{

    /* Rectangulos de paneles */
    SDL_Rect black_panel_rect = {
        .x = 0,
        .y = 0,
        .w = PANEL_WIDTH,
        .h = SCREEN_HEIGHT
    };
    SDL_Rect white_panel_rect = {
        .x = SCREEN_WIDTH - PANEL_WIDTH,
        .y = 0,
        .w = PANEL_WIDTH,
        .h = SCREEN_HEIGHT
    };
    /* Rectángulos de texto para label de contador de capturas */
    SDL_Rect black_caps_rect = {
        .x = 20,
        .y = 20 * 4,
        .w = 240,
        .h = 70
    };
    SDL_Rect white_caps_rect = {
        .x = SCREEN_WIDTH - PANEL_WIDTH + 20,
        .y = 20 * 4,
        .w = 240,
        .h = 70
    };

    /* Rectángulos para texto de capturas */
    SDL_Rect black_caps_num_rect = {
        .x = black_caps_rect.x,
        .y = black_caps_rect.y + black_caps_rect.h,
        .w = black_caps_rect.w
    };
    SDL_Rect white_caps_num_rect = {
        .x = white_caps_rect.x,
        .y = white_caps_rect.y + white_caps_rect.h,
        .w = white_caps_rect.w
    };

    /* Rectángulos para label de territorio */
    SDL_Rect black_terr_rect = {
        .x = black_caps_num_rect.x,
        .y = black_caps_num_rect.y + black_caps_num_rect.h + 20 * 4,
        .w = 240,
        .h = 70
    };
    SDL_Rect white_terr_rect = {
        .x = white_caps_num_rect.x,
        .y = white_caps_num_rect.y + white_caps_num_rect.h + 20 * 4,
        .w = 240,
        .h = 70
    };
    
    /* Rectángulos para texto de territorio */
    SDL_Rect black_terr_num_rect = {
        .x = black_terr_rect.x,
        .y = black_terr_rect.y + black_terr_rect.h,
        .w = black_terr_rect.w
    };
    SDL_Rect white_terr_num_rect = {
        .x = white_terr_rect.x,
        .y = white_terr_rect.y + white_terr_rect.h,
        .w = white_terr_rect.w
    };
        ;
    SDL_Color text_color = {255, 255, 255};

    /* Renderizamos los paneles laterales del UI */
    SDL_RenderCopy(renderer, textures[black_panel], NULL, &black_panel_rect);
    SDL_RenderCopy(renderer, textures[white_panel], NULL, &white_panel_rect);
    
    /* Renderizamos el texto de los paneles */
    SDL_RenderCopy(renderer, textures[captures_text], NULL, &black_caps_rect);
    SDL_RenderCopy(renderer, textures[captures_text], NULL, &white_caps_rect);
    SDL_RenderCopy(renderer, textures[territory_text], NULL, &black_terr_rect);
    SDL_RenderCopy(renderer, textures[territory_text], NULL, &white_terr_rect);

    /* Voy a jugar a la segura y diré que como mucho el número tendrá 4 dígitos */
    char black_caps[5], white_caps[5];
    /* En el caso del territorio si puedo decir que como máximo es de 19x19, 3 digs jaja */
    char black_terr[4], white_terr[4];
    snprintf(black_caps, 5, "%d", game_stats_ptr->black_caps);
    snprintf(white_caps, 5, "%d", game_stats_ptr->white_caps);
    snprintf(black_terr, 4, "%d", game_stats_ptr->black_terr);
    snprintf(white_terr, 4, "%d", game_stats_ptr->white_terr);
    render_text(renderer, font, black_caps, text_color, &black_caps_num_rect, true);
    render_text(renderer, font, white_caps, text_color, &white_caps_num_rect, true);
    render_text(renderer, font, black_terr, text_color, &black_terr_num_rect, true);
    render_text(renderer, font, white_terr, text_color, &white_terr_num_rect, true);

    /* Se renderizan los botones del UI */
    for (int i = 0; i < 4; ++i) {
        button_t* btn_ptr = btn_ptrs[i];
        SDL_RenderCopy(renderer, textures[btn_ptr->txt_enum], NULL, &(btn_ptr->rect)); 
    }

    /* Queremos oscurecer el panel del jugador que no está en su turno, para mayor claridad. */
    if (game_stats_ptr->player == 1) {
        SDL_RenderCopy(renderer, textures[panel_mask], NULL, &white_panel_rect);
    } else {
        SDL_RenderCopy(renderer, textures[panel_mask], NULL, &black_panel_rect);
    }

}

void render_overlay_menu(SDL_Renderer* renderer, SDL_Texture* textures[OBJ_QTY],
        button_t* overlay_menu_btn_ptrs[3], SDL_Rect* menu_rect) 
{
    

    SDL_RenderCopy(renderer, textures[overlay_menu], NULL, menu_rect);

    for (int i = 0; i < 3; ++i) {
        button_t* btn_ptr = overlay_menu_btn_ptrs[i];
        SDL_RenderCopy(renderer, textures[btn_ptr->txt_enum], NULL, &(btn_ptr->rect));
    }

}

void render_game_state(game_stats_t* game_stats_ptr, int game_arr[19][19], SDL_Renderer* renderer,
        SDL_Texture* textures[OBJ_QTY], SDL_Rect* window_rectangle, button_t* button_ptrs[4],
        bool* overlay_menu_ptr, button_t* overlay_menu_btn_ptrs[3], SDL_Rect* menu_rect,
        TTF_Font* font)
{

    /* Borde de renderizado tablero */
    int x_border = PANEL_WIDTH + B_PAD;
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
    render_game_ui(renderer, textures, button_ptrs, game_stats_ptr, font) ;

    
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

    /* Renderizamos el overlay de ser necesario */
    if (*overlay_menu_ptr) {
        render_overlay_menu(renderer, textures, overlay_menu_btn_ptrs, menu_rect);
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

void render_save_game(SDL_Renderer* renderer, SDL_Texture* textures[OBJ_QTY],
        SDL_Rect* window_rectangle, TTF_Font* font, char* save_name, int save_name_len,
            button_t* save_btn_ptr)
{

    SDL_Rect save_name_rect = {
        .w = 480,
        .x = (SCREEN_WIDTH - 480) / 2.0,
        .y = 20*2 + 5,
        .h = 140
    };

    /* Color blanco para el texto */
    SDL_Color text_color = {255, 255, 255};
    SDL_Rect text_rect = {
        .x = save_name_rect.x + 20,
        .y = save_name_rect.y + save_name_rect.h / 2.0 + 20
    };

    SDL_RenderClear(renderer);
    /* Renderizamos el fondo del menu */
    SDL_RenderCopy(renderer, textures[menu_bck], NULL, window_rectangle);
    /* Renderizamos el campo para el texto */
    SDL_RenderCopy(renderer, textures[save_name_field], NULL, &save_name_rect);
    /* Renderizamos el botón de guardado */
    SDL_RenderCopy(renderer, textures[save_btn_ptr->txt_enum], NULL, &(save_btn_ptr->rect));

    /* Renderizamos el texto de acuerdo a su tamaño */
    if (save_name_len) {
        /* Renderizamos el texto */
        render_text(renderer, font, save_name, text_color, &text_rect, false);
    } else {
        /* Renderizamos un espacio en texto */
        render_text(renderer, font, " ", text_color, &text_rect, false);
    }

    SDL_RenderPresent(renderer);


}


void render_load_game(SDL_Renderer* renderer, SDL_Texture* textures[OBJ_QTY],
        SDL_Rect* window_rectangle, TTF_Font* font, char* save_name, int save_name_len,
            button_t* load_btn_ptr) 
{
    

    SDL_Rect save_name_rect = {
        .w = 480,
        .x = (SCREEN_WIDTH - 480) / 2.0,
        .y = 20*2 + 5,
        .h = 140
    };

    /* Color blanco para el texto */
    SDL_Color text_color = {255, 255, 255};
    SDL_Rect text_rect = {
        .x = save_name_rect.x + 20,
        .y = save_name_rect.y + save_name_rect.h / 2.0 + 20
    };

    SDL_RenderClear(renderer);
    /* Renderizamos el fondo del menu */
    SDL_RenderCopy(renderer, textures[menu_bck], NULL, window_rectangle);
    /* Renderizamos el campo para el texto */
    SDL_RenderCopy(renderer, textures[save_name_field], NULL, &save_name_rect);
    /* Renderizamos el botón de guardado */
    SDL_RenderCopy(renderer, textures[load_btn_ptr->txt_enum], NULL, &(load_btn_ptr->rect));

    /* Renderizamos el texto de acuerdo a su tamaño */
    if (save_name_len) {
        /* Renderizamos el texto */
        render_text(renderer, font, save_name, text_color, &text_rect, false);
    } else {
        /* Renderizamos un espacio en texto */
        render_text(renderer, font, " ", text_color, &text_rect, false);
    }

    SDL_RenderPresent(renderer);
}


void render_end_game(SDL_Renderer* renderer, SDL_Texture* textures[OBJ_QTY], SDL_Rect* window_rectangle, button_t* main_menu_btn_ptr)
{
    SDL_RenderClear(renderer);
    /* Renderizamos el fondo del menu */
    SDL_RenderCopy(renderer, textures[menu_bck], NULL, window_rectangle);
    /* Renderizamos el botón para volver al menu */
    SDL_RenderCopy(renderer, textures[main_menu_btn_ptr->txt_enum], NULL, &(main_menu_btn_ptr->rect));
    SDL_RenderPresent(renderer);
}

void render_text(SDL_Renderer* renderer, TTF_Font* font, char* string, SDL_Color color,
        SDL_Rect* rect, bool center)
{
    SDL_Surface* text_sf = TTF_RenderText_Blended(font, string, color);
    if (!text_sf) {
        printf("No se pudo cargar texto a superficie. TTF_Error: %s\n", TTF_GetError());
    } else {
        if (center) rect->x = (2 * rect->x + rect->w - text_sf->w) / 2.0;
        rect->w = text_sf->w;
        rect->h = text_sf->h;
        SDL_Texture* text_txt = SDL_CreateTextureFromSurface(renderer, text_sf);
        SDL_RenderCopy(renderer, text_txt, NULL, rect);
        SDL_DestroyTexture(text_txt);
        text_txt = NULL;
    }
    SDL_FreeSurface(text_sf);
}
