#include "headers/bot.h"
#include "headers/move_validation.h"
#include <time.h>
#include <SDL2/SDL.h>


bool process_bot_play(game_stats_t* game_stats_ptr, int prev_game_arr[19][19], int game_arr[19][19], unsigned int prev_play_ms)
{
    unsigned int current_ms = SDL_GetTicks64();
    bool success = false;
    /* Esperamos 1 segundo */
    if (current_ms - prev_play_ms >= 1000) {
        srand(time(NULL));
        int row = rand() % game_stats_ptr->len; /* Esto tiene un rango de [0, len] */
        int col = rand() % game_stats_ptr->len;
        success = process_move(game_stats_ptr, game_arr, row, col, prev_game_arr);
    }
    return success;
}
