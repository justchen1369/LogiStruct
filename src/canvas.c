#include <math.h>
#include <allegro5/allegro_primitives.h>
#include <allegro5/allegro_ttf.h>
#include "canvas.h"

bool m_range(ALLEGRO_MOUSE_STATE mouse, int xmin, int xmax, int ymin, int ymax) {
    if(mouse.x >= xmin && mouse.x <= xmax && mouse.y >= ymin && mouse.y <= ymax) {
        return true;
    }
    return false;
}

int r_lim(int min, int val, int max) {
    if (val < min) {return min;}
    else if (val > max) {return max;}
    else {return val;}
} 

void draw_map(bool grid, int map[96][50], ALLEGRO_FONT *font) {
    ALLEGRO_COLOR colormap[18] = {
        bgcolor, mediumgrey, red, black, redblack, black, redblack, green,
        blue, green, blue, lightgrey, white, black, redblack, gold
    };
    int xs[192];
    int ys[192];
    int saved = 0;

    al_draw_filled_rectangle(0, 1000, 1920, 1080, nearblack);

    for(int i = 0; i < 96; i++) {
        for(int j = 0; j < 50; j++) {
            if(grid) {
                al_draw_line(i*20, j*20, i*20+20, j*20, nearblack, 1);
                al_draw_line(i*20, j*20, i*20, j*20+20, nearblack, 1);
            }
            
            if(map[i][j] != empty) {
                al_draw_filled_rectangle(i*20, j*20, i*20+20, j*20+20, colormap[map[i][j]]);
                if(map[i][j] == and || map[i][j] == not) {
                    xs[saved] = i, ys[saved] = j, saved++;
                }
            }
        }
    }

    for(int i = 0; i < saved; i++) {
        if(map[xs[i]][ys[i]] == and) {
            al_draw_text(font, white, xs[i]*20+10, ys[i]*20+9 - al_get_font_line_height(font) / 2, ALLEGRO_ALIGN_CENTRE, "AND");
        } else if(map[xs[i]][ys[i]] == not) {
            al_draw_text(font, white, xs[i]*20+10, ys[i]*20+9 - al_get_font_line_height(font) / 2, ALLEGRO_ALIGN_CENTRE, "NOT");
        }
    }
}

void place_chip(int x, int y, comp chip, int map[96][50]) {
    if(chip == and) {
        x = r_lim(3, x, 92);
        y = r_lim(2, y, 47);
        for(int i = x - 4; i < x + 5; i++) {
            for(int j = y - 3; j < y + 4; j++) {
                if (map[r_lim(0, i, 95)][r_lim(0, j, 49)] > hiwire) {
                    goto trip;
                }
            }
        }
        for(int i = x - 2; i < x + 3; i++) {
            for(int j = y - 2; j < y + 3; j++) {
                map[i][j] = aboard;
            }
        }
        map[x][y] = and;
        map[x - 3][y + 1] = map[x - 3][y - 1] = lopinin;
        map[x + 3][y] = lopinout;
    } else if(chip == not) {
        x = r_lim(3, x, 92);
        y = r_lim(1, y, 48);
        for(int i = x - 4; i < x + 5; i++) {
            for(int j = y - 2; j < y + 3; j++) {
                if(map[r_lim(0, i, 95)][r_lim(0, j, 49)] > hiwire) {
                    goto trip;
                }
            }
        }
        for(int i = x - 2; i < x + 3; i++) {
            for(int j = y - 1; j < y + 2; j++) {
                map[i][j] = nboard;
            }
        }
        map[x][y] = not;
        map[x - 3][y] = lopinin;
        map[x + 3][y] = lopinout;
    } else if(chip == loflip) {
        x = r_lim(1, x, 94);
        y = r_lim(1, y, 48);
        for(int i = x - 2; i < x + 3; i++) {
            for(int j = y - 2; j < y + 3; j++) {
                if(map[r_lim(0, i, 95)][r_lim(0, j, 49)] > hiwire) {
                    goto trip;
                }
            }
        }
        for(int i = x - 1; i < x + 2; i++) {
            for(int j = y - 1; j < y + 2; j++) {
                map[i][j] = loflip;
            }
        }
    } else if(chip == lolight) {
        x = r_lim(1, x, 94);
        y = r_lim(1, y, 48);
        for(int i = x - 2; i < x + 3; i++) {
            for(int j = y - 2; j < y + 3; j++) {
                if(map[r_lim(0, i, 95)][r_lim(0, j, 49)] > hiwire) {
                    goto trip;
                }
            }
        }
        for(int i = x - 1; i < x + 2; i++) {
            for(int j = y - 1; j < y + 2; j++) {
                map[i][j] = lolight;
            }
        }
    } else if(chip == cross) {
        x = r_lim(2, x, 93);
        y = r_lim(2, y, 47);
        for(int i = x - 1; i < x + 2; i++) {
            for(int j = y - 1; j < y + 2; j++) {
                if(map[r_lim(0, i, 95)][r_lim(0, j, 49)] > hiwire) {
                    goto trip;
                }
            }
        }
        map[x][y] = cross;
    }
    trip:
        return;
}

void remove_chip(int map[96][50], int x, int y) {
    x = r_lim(0, x, 95);
    y = r_lim(0, y, 49);
    if(map[x][y] > hipinout) {
        map[x][y] = empty;
        remove_chip(map, x+1, y);
        remove_chip(map, x-1, y);
        remove_chip(map, x, y+1);
        remove_chip(map, x, y-1);
    } else if(map[x][y] > hiwire && map[x][y] < aboard) {
        map[x][y] = empty;
    }
}

int flip_switch(int map[96][50], int x, int y, int mode) {
    if(map[x][y] == loflip && (mode == 0 || mode == 1)) {
        map[x][y] = hiflip;
        flip_switch(map, x+1, y, 1);
        flip_switch(map, x-1, y, 1);
        flip_switch(map, x, y+1, 1);
        flip_switch(map, x, y-1, 1);
    } else if(map[x][y] == hiflip && (mode == 0 || mode == 2)) {
        map[x][y] = loflip;
        flip_switch(map, x+1, y, 2);
        flip_switch(map, x-1, y, 2);
        flip_switch(map, x, y+1, 2);
        flip_switch(map, x, y-1, 2);
    }
    if(!mode) {
        return 20;
    }
    return 0;
}

void lock_coords(int *lock, int *lx, int *ly, ALLEGRO_MOUSE_STATE state) {
    *lock = 2;
    *lx = state.x / 20; 
    *ly = state.y / 20;
}

void lock_handler(int *lock, int lx, int ly, int *x, int *y, int *dirx, int *diry) {
    if(*lock == 2) {
        *dirx = abs(lx - *x);
        *diry = abs(ly - *y);
        *lock -= *dirx;
        *lock -= *diry;
    }
    if(*dirx == 1) {
        *y = ly;
    } else if(*diry == 1) {
        *x = lx;
    }
}

void click_handler(int map[96][50], ALLEGRO_MOUSE_STATE state, int x, int y, int select, int *wait) {
    if(*wait == 0 && select == 0 && (map[x][y] == loflip || map[x][y] == hiflip) && state.buttons & 1) {*wait = flip_switch(map, x, y, 0);}
    else if(map[x][y] > hipinout && state.buttons & 2) {remove_chip(map, x, y);}
    else if(select == 1 && map[x][y] < lopinin && state.buttons & 1) {place_chip(x, y, and, map);}
    else if(select == 2 && map[x][y] < lopinin && state.buttons & 1) {place_chip(x, y, not, map);}
    else if(select == 3 && map[x][y] < lopinin && state.buttons & 1) {place_chip(x, y, loflip, map);}
    else if(select == 4 && map[x][y] < lopinin && state.buttons & 1) {place_chip(x, y, lolight, map);}
    else if(select == 5 && map[x][y] < lopinin && state.buttons & 1) {place_chip(x, y, cross, map);}
    else if(state.buttons & 2 && map[x][y] < lopinin) {map[x][y] = empty;}
    else if(state.buttons & 1 && map[x][y] == empty) {map[x][y] = lowire;}
}