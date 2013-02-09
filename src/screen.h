#ifndef H_SCREEN_
#define H_SCREEN_
#include "board.h"
#include "SDL.h"

SDL_Surface *screen;
SDL_Surface *buffer;

void init_screen(void);
void load_assets(void);
void screenspace2gemspace(BOARD *b,int sx,int sy,int *gx,int *gy);
int g2s_x(int x);
int g2s_y(int x,int y);
void flip_screen(void);
int draw_intro(void);
int draw_main_menu(void);
int draw_high_score(int gamemode);
void draw_button(SDL_Rect *r,char *text);
#endif