#include "headers/struct_objects.h"

game_stats_t init_game_stats() {
    game_stats_t game_stats = {
        .player = 1,
        .pass = 0,
        .black_caps = 0,
        .white_caps = 0,
    };

    return game_stats;
}

void print_game_stats(game_stats_t game_stats) {
    printf("Jugador actual: %d\nPass: %d\nCapturas por blancas: %d\nCapturas por negras: %d\n",
            game_stats.player, game_stats.pass, game_stats.black_caps, game_stats.white_caps);
}

