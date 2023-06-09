#ifndef ENUMS_H
#define ENUMS_H

/* 
 * Cualquier header que incluya enum debe tener esto, asi que lo pongo acá no más. Definimos un 
 * límite arbtirario para la cantidad de texturas/superficies a tener, además de una longitud
 * máxima para el path a un archivo bmp a ser cargado.
 */
#define OBJ_QTY 40
#define BMP_LEN 50


/* Enums para texturas y superficies, solo para que no hayan números mágicos dando vuelta. */


/* 
 * Si se agregan más botones toggle, asegurarse de que el siguiente enum, textura, etc, sea el del
 * boton cuando es presionado, ya que el resto del código se basa en eso.
 */

typedef enum {
    white_pc                         = 0,
    black_pc                         = 1,
    menu_bck                         = 2,
    new_game_btn                     = 3,
    settings_btn                     = 4,
    start_btn                        = 5,
    rankings_btn                     = 6,
    exit_btn                         = 7,
    nine_by_nine_btn                 = 8,   
    nine_by_nine_btn_pressed         = 9,
    thirteen_by_thirteen_btn         = 10,  
    thirteen_by_thirteen_btn_pressed = 11,  
    nineteen_by_nineteen_btn         = 12,  
    nineteen_by_nineteen_btn_pressed = 13,
    grid                             = 14,
    black_panel                      = 15,
    white_panel                      = 16,
    pass_btn                         = 17,
    panel_mask                       = 18,
    resign_btn                       = 19,
    captures_text                    = 20,
    overlay_menu                     = 21,
    main_menu_text                   = 22,
    save_game_text                   = 23,
    exit_text                        = 24,
    save_name_field                  = 25,
    save_btn                         = 26,
    load_btn                         = 27,
    load_game_btn                    = 28,
    territory_text                   = 29,
    blk_pt_panel                     = 30,
    wht_pt_panel                     = 31,
    none_btn                         = 32,
    none_btn_pressed                 = 33,
    basic_btn                        = 34,
    basic_btn_pressed                = 35,
    end_enum                /* Este nos ayuda a iterar más facilmente */
} sf_and_txt_enum_t;

typedef enum {
    exit_st = 0,
    menu_st,
    game_set_st,
    game_st,
    end_game_st,
    save_game_st,
    load_game_st,
} state_t;

#endif


