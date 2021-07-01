#ifndef UTILS
#define UTILS

#include "canvas.h"

#ifdef _WIN32
void fix_dir(void);
#endif

void save_canvas(int map[MAP_X][MAP_Y], int *posx, int *posy);

void load_canvas(int map[MAP_X][MAP_Y], int *posx, int *posy);

#endif
