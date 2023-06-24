#include "headers/move_validation.h"
#include "headers/matrix_ops.h"
#include "headers/bit_ops.h"
#include "headers/player_input_processing.h"

bool process_move(int len, int game_arr[19][19], int player, int row, int col,
        int prev_game_arr[19][19])
{
    /* Flag de movimiento que cumple todas las condiciones para ser efectuado */
    bool success = false;

    /* Tablero de juego para analizar la jugada por separado, y hacer comparaciones. */
    int dummy_game_arr[19][19];
    
    copy_matrix(len, game_arr, dummy_game_arr);

    /* Hacemos al jugada en dummy */
    dummy_game_arr[row][col] = player;
         
    /* Regla de suicidio debe revisarse */
    if (check_suicide(len, game_arr, dummy_game_arr, row, col, player)) {

        if (check_ko(len, prev_game_arr, dummy_game_arr)) {
            /* Éxito, la jugada es válida. */
            success = true;
            /* La matriz de tablero previo se actualiza */
            copy_matrix(len, game_arr, prev_game_arr);
            /* Pasamos la jugada válida a la matriz de verdad */
            copy_matrix(len, dummy_game_arr, game_arr);
        }
    }
 

    return success;
}

bool check_suicide(int len, int game_arr[19][19], int dummy_game_arr[19][19], int row, int col,
        int player)
{
    /* Flag de suicidio válido, un suicidio solo es válido si hace capturas */
    bool is_valid = false;
   
    /* Conseguimos las libertades de la pieza jugada */
    int play_pc_libs = get_liberties(len, dummy_game_arr, row, col);

    /* Removemos las piezas muertas del dummy_game_arr */
    find_dead_pieces(len, dummy_game_arr);

    /* Revisamos si la jugada fue suicidio, es decir, si la pieza jugada no tiene libertades */
    if (!play_pc_libs) {

        /* 
         * Para analizar si el suicidio produjo captura, basta con contar las piezas en partida
         * antes y después 
         */
        int dummy_pcs[2] = {0}, game_pcs[2] = {0};
        count_pieces(len, dummy_game_arr, dummy_pcs);
        count_pieces(len, game_arr, game_pcs);

        /*
         * Si las piezas del jugador contrario han cambiado, hubo capturas. Entiendo que sería
         * más seguro chequear que hayan disminuido, pero el tema es que si por alguna razón crecen,
         * hay un problema mucho más grande en otra parte.
         */
       
        /* Para negras (1) queremos indice 1, 1 % 2 = 1. Para blancas (2) queremos indice 0. */
        if (game_pcs[player % 2] != dummy_pcs[player % 2]) {

            /* 
             * Ahora, resulta que queremos "resucitar" las piezas del jugador que hizo el suicidio
             * con capturas. Entonces, comparamos las dos matrices y reincorporamos las
             * incongruencias en piezas del jugador. Este es el método inocente, supongo, pero
             * no me da el tiempo para implementar algo más inteligente sin googlear.
             */
            correct_diff(len, player, game_arr, dummy_game_arr); 

            /* 
             * Ahora hacemos la jugada, esta no es una incongruencia que haya sido reparada, 
             * hay que hacerlo manualmente
             */
            dummy_game_arr[row][col] = player;

            is_valid = true;
        } 
    }
    /* En caso contrario, no fue suicidio, y podemos proseguir */
    else {
        is_valid = true;
    }

    return is_valid;

}

int get_liberties(int len, int game_arr[19][19], int row, int col)
{
    /* 
     * Creamos un bitmap array de libertades y coordenadas visitadas, además de un entero que
     * mantendrá cuenta de la cant de libertades de una pieza
     */
    unsigned int liberties[12] = {0}; 
    unsigned int visited[12] = {0};
    int cant_libertades = 0;
    /* Llamamos a chequear las libertades de la pieza */
    check_adj(len, game_arr, row, col, liberties, visited, &cant_libertades);
    return cant_libertades; 
}

void check_adj(int len, int game_arr[19][19], int row, int col, unsigned int liberties[12],
        unsigned int visited[12], int* cant_libertades)
{
    int directions[4][2] = {{-1, 0}, {1, 0}, {0, 1}, {0, -1}};
    int coords_en_num = coords_to_int(row, col, len);

    /* Está coordenada está oficialmente visitada. */
    set_bit(visited, coords_en_num);
    for (int i = 0; i < 4; ++i) {
        /* Chequeamos las coordenadas adyacantes */
        int adj_row = row + directions[i][0], adj_col = col + directions[i][1];

        /* Las transformamos en un entero, para poder encontrarlas en los arreglos de bitmaps */
        int adj_coords_en_num = coords_to_int(adj_row, adj_col, len);

        /* Revisamos que las coordenadas adyacentes estén dentro del rango del tablero. */
        if (adj_row >= 0 && adj_row < len && adj_col >= 0 && adj_col < len) {

            /*
             * Si la coordenada está vacía, es decir, no hay pieza puesta en ella, revisamos si es
             * candidata a ser añadida a libertades
             */
                                      

            /* Si puesto adyacente está vacio... */
            if (!game_arr[adj_row][adj_col]) {
                /*
                 * Si coordenada no está en libertades, la añadimos a libertades y sumamos al
                 * contador
                 */
                if (!get_bit(liberties, adj_coords_en_num)) {
                    *cant_libertades += 1;
                }
                set_bit(liberties, adj_coords_en_num);
                
            }
            /*
             * En caso de que la coordenada adyacente no está vacía, es del mismo color, y además
             * no ha sido visitada anteriormente, chequeamos sus libertades
             */
            else if (game_arr[adj_row][adj_col] == game_arr[row][col] &&
                    !get_bit(visited, adj_coords_en_num)) {
                check_adj(len, game_arr, adj_row, adj_col, liberties, visited, cant_libertades);
            }
        }
    }
}

int coords_to_int(int row, int col, int len) 
{
     return row * len + col; 
}

void find_dead_pieces(int len, int game_arr[19][19])
{
    int captured[len*len][2];
    int count = 0;

    for (int i = 0; i < len; ++i) {
        for (int j = 0; j < len; ++j) {
            if (game_arr[i][j] != 0) {
                /* 
                * Hay que tener cuidado de no eliminar la pieza inmediatamente, ya que si lo
                * hacemos, cuando se chequeen las libertades de las otras piezas, podrían
                * terminar vivas cuando en verdad estaban muertas.
                * Por eso, guardaermos las coordenadas de las piezas en un arreglo a recorrer
                * después.
                */
                if (!get_liberties(len, game_arr, i, j)) {
                    captured[count][0] = i;
                    captured[count][1] = j;
                    ++count;
                }
            }
        }
    }

    for (int i = 0; i < count; ++i) {
        int row = captured[i][0];
        int col = captured[i][1]; 
        game_arr[row][col] = 0;;
    }
}

bool check_ko(int len, int prev_game_arr[19][19], int dummy_game_arr[19][19])
{
    /* No creo que necesite explicación */
    bool success = true;
    if (matrices_are_equal(len, prev_game_arr, dummy_game_arr)) {
        success = false;
        printf("Ko!\n");
    }

    return success;
}

bool process_pass(game_stats_t* game_stats_ptr)
{
    int finish_game = false;
    if (game_stats_ptr->pass) {
        finish_game = true;
    } else {
        game_stats_ptr->pass = true;
    };
    return finish_game;
}

bool process_resign(game_stats_t* game_stats_ptr)
{
  return true;  
}



