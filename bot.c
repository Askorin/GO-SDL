#include "headers/bot.h"
#include "headers/move_validation.h"
#include "headers/matrix_ops.h"
#include <time.h>
#include <SDL2/SDL.h>


bool process_bot_play(game_stats_t* game_stats_ptr, int prev_game_arr[19][19], int game_arr[19][19],
        unsigned int prev_play_ms, state_t* state_ptr)
{
    unsigned int current_ms = SDL_GetTicks64();
    bool success = false;

    /* Piezas mínimas para que el bot se rinda */
    const int pc_resign_condition = game_stats_ptr->len * game_stats_ptr->len * 0.2;
    /* Diferencia de puntos para que el bot se rinda */
    const float pt_resign_condition = game_stats_ptr->len * game_stats_ptr->len * (10 / 81.0); 

    /* Esperamos 1 segundo */
    if (current_ms - prev_play_ms >= 1000) {
        /* Esta seed es lo suficientemente buena para este propósito. */
        srand(time(NULL) + clock());
        int bot_player = game_stats_ptr->bot_player;
        int human_player = (bot_player % 2) + 1;
        /* En indice 0 negras, indice 1 blancas. */
        float total_points[2] = {get_black_total(game_stats_ptr), get_white_total(game_stats_ptr)};
        float point_diff = total_points[human_player - 1] - total_points[bot_player - 1];
        /* Cantidad total de piezas jugadas */
        int total_pieces = count_total_pieces(game_stats_ptr->len, game_arr);
        /* Si se cumplen las condiciones para que el bot se rinda */
        if (total_pieces >= pc_resign_condition && point_diff >= pt_resign_condition) {
            process_resign(game_stats_ptr, bot_player, state_ptr);
            success = true;
        } else if (is_play_available(game_stats_ptr, game_arr, prev_game_arr)) {
            int row = rand() % game_stats_ptr->len; /* Esto tiene un rango de [0, len] */
            int col = rand() % game_stats_ptr->len;
            success = process_move(game_stats_ptr, game_arr, row, col, prev_game_arr);
        } else {
            process_pass(game_stats_ptr, state_ptr);
            success = true;
        }
    }
    
    return success;
}

bool is_play_available(game_stats_t* game_stats_ptr, int game_arr[19][19],
        int prev_game_arr[19][19])
{
    int len = game_stats_ptr->len;
    /* Creamos dos matrices falsas sobre las cuales evaluar jugadas. */
    int dummy_game_arr[19][19] = {{0}};
    int dummy_prev_game_arr[19][19] = {{0}};
    copy_matrix(len, game_arr, dummy_game_arr);
    copy_matrix(len, prev_game_arr, dummy_prev_game_arr);
    for (int i = 0; i < len; ++i) {
        for (int j = 0; j < len; ++j) {
            if (game_arr[i][j] == 0) {
                if (process_move(game_stats_ptr, dummy_game_arr, i, j, dummy_prev_game_arr)) {
                    return true;
                }
            }
        }
    }
    return false;
}
    


    
