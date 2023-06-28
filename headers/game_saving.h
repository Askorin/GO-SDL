#ifndef GAME_SAVING_H 
#define GAME_SAVING_H
#include <stdio.h>
#include <stdbool.h>
#include "struct_objects.h"

/* Crea un save en carpeta ./saves */
bool create_save(game_stats_t*, int[19][19], int[19][19], char*);

/* Carga un save */
bool load_save(game_stats_t*, int[19][19], int[19][19], char*);

void to_lower(char*, int);

#endif
