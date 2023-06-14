#include "headers/move_validation.h"
#include "headers/matrix_ops.h"
#include "headers/bit_ops.h"
#include "headers/player_input_processing.h"

bool process_move(int len, int game_arr[19][19], SDL_MouseButtonEvent* mouse_event, int player,
        int prev_game_arr[19][19])
{
    int row, col;
    /* Flag de movimiento que cumple todas las condiciones para ser efectuado */
    bool success = false;

    /* Tablero de juego para analizar la jugada por separado, y hacer comparaciones. */
    int dummy_game_arr[19][19];
    
    copy_matrix(len, game_arr, dummy_game_arr);

    /* Chequeamos si la jugada se puede mapear a una cordenada de la matriz de tablero. */
    if (check_mdown(len, game_arr, mouse_event, player, &row, &col)) {
        /* 
        * Realizaremos el siguiente truco: 
        * Creamos una pieza "falsa" que será tres veces la pieza del jugador, es decir, si 
        * juega negro, la pieza tendrá un valor de 1*3 = 3, si juega blanco, de 2*3 = 6.
        * Luego, creamos las siguientes reglas:
        * 1- Si una pieza tiene adyacente a una pieza falsa que es tres veces su valor, esta
        *    cuenta como un 0, es decir, no le quita libertades, ni tampoco enlaza con ella.
        * 2- Si una pieza tiene adyacente a una pieza que no es cero, su triple, o de su mismo
        *    tipo, entonces cuenta como una pieza enemiga, es decir, le quita libertades.
        * Esto lo realizamos para casos como el siguiente: 
        *
        * 2 2 2
        * 1 1 1 2
        * 2 0 1 2
        * 1 1 1 2
        * 2 2 2 
        *
        * Ver que si las blancas juegan en el cero, queremos asegurarnos de que ni la pieza nueva
        * ni la que se enlaza a ella sean capturadas. Podemos aplicar el truco:
        *
        * 2 2 2
        * 1 1 1 2
        * 2 6 1 2
        * 1 1 1 2
        * 2 2 2 
        *
        * Efectivamente, para la pieza blanca que está rodeada, el 6 cuenta como una libertad, asi
        * que no será capturada, pero las negras sí.las negras sí.
        */
        dummy_game_arr[row][col] = player * 3;
         
        /* 
         * Hay que tener en cuenta de que las piezas falsas también pueden ser capturadas, asi que
         * revisamos si la jugada fue un suicidio.
         * Check_sucicide retornará true en caso de ser una jugada sin suicidio, o con suicidio 
         * válido, es decir con capturas. En caso de ser un sucidio inválido retorna false.
         */

        if (check_suicide(len, game_arr, dummy_game_arr, row, col, player)) {
                /* Retornamos la pieza falsa a su valor verdadero */
                dummy_game_arr[row][col] = player;

                /* Chequeamos si el movimiento cumple con la regla Ko */
            if (check_ko(len, prev_game_arr, dummy_game_arr)) {
                /* Éxito, la jugada es válida. */
                success = true;
                /* La matriz de tablero se actualiza */
                copy_matrix(len, game_arr, prev_game_arr);
                /* Pasamos la jugada válida a la matriz de verdad */
                copy_matrix(len, dummy_game_arr, game_arr);
            }
        }
    } 

    return success;
}

bool check_suicide(int len, int game_arr[19][19], int dummy_game_arr[19][19], int row, int col,
        int player)
{
    /* Flag de suicidio válido, un suicidio solo es válido si hace capturas */
    bool is_valid = false;
    /* 
     * Si una jugada es suicidio sin captura, entonces si dejaramos la jugada pasar, la matriz
     * posterior a remover las piezas muertas es necesariamente la misma. 
     */
    find_dead_pieces(len, dummy_game_arr);
    if (matrices_are_equal(len, dummy_game_arr, game_arr)) {
        //printf("Es un suicidio sin capturas.\n");
    }
    /* En caso de que haya sido una jugada sin suicidio, o suicidio pero con capturas */
    else {
        //printf("Es una jugada válida.\n");
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
            bool condicion_libertad = (game_arr[adj_row][adj_col] == 0)                        ||
                                      (game_arr[adj_row][adj_col] == (3 * game_arr[row][col])) ||
                                      (game_arr[adj_row][adj_col] * 3 == game_arr[row][col]);


            if (condicion_libertad) {
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





