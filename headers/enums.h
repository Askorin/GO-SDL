#ifndef ENUMS_H
#define ENUMS_H

/* 
 * Cualquier header que incluya enum debe tener esto, asi que lo pongo acá no más. Definimos un 
 * límite arbtirario para la cantidad de texturas/superficies a tener, además de una longitud
 * máxima para el path a un archivo bmp a ser cargado.
 */
#define OBJ_QTY 10
#define BMP_LEN 50


/* Enums para texturas y superficies, solo para que no hayan números mágicos dando vuelta. */
typedef enum {
    board = 0,
    white_pc,
    black_pc,
    menu_bck,
    play_btn,
    opt_btn,
    ldrbrd_btn,
} sf_and_txt_enum_t;

typedef enum {
    exit_st = 0,
    menu_st,
    game_st,
    opt_st,
}state_t;

#endif


