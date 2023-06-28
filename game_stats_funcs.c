#include "headers/struct_objects.h"

game_stats_t init_game_stats() {
    game_stats_t game_stats = {
        .player = 1,
        .pass = 0,
        .black_caps = 0,
        .white_caps = 0,
        .black_terr = 0,
        .white_terr = 0,
        .len = 9,
        .resign = 0,
        .komi = 6.5,
    };

    return game_stats;
}

void print_game_stats(game_stats_t game_stats) {
    printf("len: %dx%d\nplayer: %d\npass: %d\nwhite_caps: %d\nblack_caps: %d\nkomi: %.1f\n", game_stats.len, game_stats.len,
            game_stats.player, game_stats.pass, game_stats.black_caps, game_stats.white_caps, game_stats.komi);
}

