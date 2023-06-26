#include "headers/game_saving.h"


bool create_save(game_stats_t* game_stats_ptr, int prev_game_arr[19][19], int game_arr[19][19])
{
    FILE* save_file = fopen("save.bin", "wb");
    /* Guardamos game_stats */
    fwrite(&(game_stats_ptr->pass), sizeof(game_stats_ptr->pass), 1, save_file);
    fwrite(&(game_stats_ptr->player), sizeof(game_stats_ptr->player), 1, save_file);
    fwrite(&(game_stats_ptr->black_caps), sizeof(game_stats_ptr->black_caps), 1, save_file);
    fwrite(&(game_stats_ptr->white_caps), sizeof(game_stats_ptr->white_caps), 1, save_file);
    fwrite(&(game_stats_ptr->len), sizeof(game_stats_ptr->len), 1, save_file);
    fwrite(&(game_stats_ptr->komi), sizeof(game_stats_ptr->komi), 1, save_file);

    /* Guardamos el prev_game_arr y game_arr en ese orden */
    fwrite(prev_game_arr, sizeof(prev_game_arr[0][0]), 19*19, save_file);
    fwrite(game_arr, sizeof(game_arr[0][0]), 19*19, save_file);

    fclose(save_file);
    return true;
}

game_stats_t load_save(char* name)
{
    game_stats_t game_stats_o;
    FILE* save_file = fopen(name, "rb");
    fread(&(game_stats_o.pass), sizeof(game_stats_o.pass), 1, save_file);
    fread(&(game_stats_o.player), sizeof(game_stats_o.player), 1, save_file);
    fread(&(game_stats_o.black_caps), sizeof(game_stats_o.black_caps), 1, save_file);
    fread(&(game_stats_o.white_caps), sizeof(game_stats_o.white_caps), 1, save_file);
    fread(&(game_stats_o.len), sizeof(game_stats_o.len), 1, save_file);
    fread(&(game_stats_o.komi), sizeof(game_stats_o.komi), 1, save_file);
    fclose(save_file);
    return game_stats_o;
}

