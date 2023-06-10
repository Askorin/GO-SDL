#include "bit_ops.h"

void set_bit(unsigned int bitmap[12], int k)
{
    /* Para saber en qué indice del arreglo está la posición que nos pide, hacemos: */
    int idx = k / 32;
    /* Para saber saber cuál es la posición relativa del bit en ese entero del arreglo, hacemos: */
    int rel_pos = k % 32;
    /* Ahora seteamos el bit en la posición rel_pos del int bitmap[idx] a 1. */ 
    unsigned int num = bitmap[idx];
    num = num | (1 << rel_pos);
    bitmap[idx] = num;
}

int get_bit(unsigned int bitmap[12], int k)
{
    /* Para saber en qué indice del arreglo está la posición que nos pide, hacemos: */
    int idx = k / 32;
    /* Para saber saber cuál es la posición relativa del bit en ese entero del arreglo, hacemos: */
    int rel_pos = k % 32;
    /* Ahora conseguimos el bit en la posición rel_pos del int bitmap[idx] */
    unsigned int num = bitmap[idx];
    num = num >> rel_pos;
    num = num & 1;
    return num;
}


