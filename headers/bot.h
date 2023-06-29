#ifndef BOT_H
#define BOT_H
#include <stdio.h>
#include "struct_objects.h"
#include "enums.h"

/* 
 * Efectua la jugada del bot. La implementación actual tiene paremetros internos bajo los cuales
 * decide si rendise o no. El bot solo pasará el turno si ve que no puede hacer más jugadas.
 */
bool process_bot_play(game_stats_t*, int[19][19], int[19][19], unsigned int, state_t*);

/* Revisa si existen jugadas posibles. */
bool is_play_available(game_stats_t*, int[19][19], int[19][19]);

#endif

