#ifndef ENUMS_H
#define ENUMS_H

/* 
 * Cualquier header que incluya enum debe tener esto, asi que lo pongo acá no más. Definimos un 
 * límite arbtirario para la cantidad de texturas/superficies a tener, además de una longitud
 * máxima para el path a un archivo bmp a ser cargado.
 */
#define OBJ_QTY 20
#define BMP_LEN 50


/* Enums para texturas y superficies, solo para que no hayan números mágicos dando vuelta. */


/* 
 * Si se agregan más botones toggle, asegurarse de que el siguiente enum, textura, etc, sea el del
 * boton cuando es presionado, ya que el resto del código se basa en eso.
 */

typedef enum {
    board                            = 0,
    white_pc                         = 1,
    black_pc                         = 2,
    menu_bck                         = 3,
    new_game_btn                     = 4,
    settings_btn                     = 5,
    start_btn                        = 6,
    rankings_btn                     = 7,
    exit_btn                         = 8,
    nine_by_nine_btn                 = 9,   
    nine_by_nine_btn_pressed         = 10,
    thirteen_by_thirteen_btn         = 11,  
    thirteen_by_thirteen_btn_pressed = 12,  
    nineteen_by_nineteen_btn         = 13,  
    nineteen_by_nineteen_btn_pressed = 14,
    grid                             = 15,
    end_enum                /* Este nos ayuda a iterar más facilmente */
} sf_and_txt_enum_t;

typedef enum {
    exit_st = 0,
    menu_st,
    game_set_st,
    game_st,
    rankings_st,
    settings_st,
} state_t;

#endif


