#ifndef GAME_SAVING_H 
#define GAME_SAVING_H
#include <stdio.h>
#include <stdbool.h>
#include "struct_objects.h"

bool create_save(game_stats_t*, int[19][19], int[19][19]);

game_stats_t load_save(char* name);

#endif
