#include "headers/move_validation.h"

bool process_move(int len, int game_arr[len][len], SDL_MouseButtonEvent* mouse_event, int player)
{
    int row, col;
    /* Flag de movimiento que cumple todas las condiciones para ser efectuado */
    bool success = false;

    /* Chequeamos si la jugada se puede mapear a una cordenada de la matriz de tablero. */
    if (check_mdown(len, game_arr, mouse_event, player, &row, &col)) {
        /* Ahora, chequear si la jugada es suicidio, aquí el camino se bifurca en 3:
         * 1 - La jugada es suicidio y no hace capturas, entonces no se procesa y se retorna false.
         * 2 - La jugada no es suicidio, entonces se procesa normalmente, se chequea Ko.
         * 3 - La jugada es suicidio y hace capturas, en este caso se debe guardar las coordenadas
         *     de la figura que se "suicidó", y flagearla para ser ignorada. Pero no parar ahí, 
         *     cualquier figura que haya estado enlazada a esta tampoco puede ser capturada.
         *     En ese sentido, quizás sería mejor poner una pieza "falsa" en ese punto, con tal
         *     de que se borren las libertades que queremos, y después poner las piezas de verdad.
         *     Se me ocurre poner un número de la misma paridad de la pieza que fue jugada. Es de
         *     cir, si se jugó una piedra negra (1), se pone un 3. Después modificar el código a 
         *     que si una piedra tiene como libertad a una pieza con número distinta a ella pero de
         *     de su misma paridad (excluimos al 0), entonces esa posición sigue siendo libertad,
         *     y tampoco se enlaza con ella. 
         */
        success = true;
        game_arr[row][col] = player;
        find_dead_pieces(len, game_arr);
       
    } 

    return success;
}

int get_liberties(int len, int game_arr[len][len], int row, int col)
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

void check_adj(int len, int game_arr[len][len], int row, int col, unsigned int liberties[12],
        unsigned int visited[12], int* cant_libertades)
{
    int directions[4][2] = {{-1, 0}, {1, 0}, {0, 1}, {0, -1}};
    int coords_en_num = coords_to_int(row, col, len);

    /* Está coordenara está oficialmente visitada. */
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
            if (game_arr[adj_row][adj_col] == 0) {
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

int coords_to_int(int row, int col, int len) {
     return row * len + col; 
 }

void find_dead_pieces(int len, int game_arr[len][len])
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


