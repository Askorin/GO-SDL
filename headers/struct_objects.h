#ifndef STRUCT_OBJECTS_H
#define STRUCT_OBJECTS_H
#include <stdio.h>

/* Objeto para uso en botones, guarda un rectangulo y el state al que debe llevar */
typedef struct{
    SDL_Rect rect;
    state_type st_event;
}button;

#endif
