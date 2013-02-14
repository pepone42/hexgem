#include "board.h"
#include "screen.h"
#include "fskip.h"
#include "hiscore.h"
#include "sound.h"
 /* Stolen from the mailing list */
       /* Creates a new mouse cursor from an XPM */

       /* XPM */
static const char *arrow[] = {
         /* width height num_colors chars_per_pixel */
	"    32    32        3            1",
         /* colors */
	"X c #000000",
	". c #ffffff",
	"  c None",
         /* pixels */
	"X                               ",
	"XX                              ",
	"X.X                             ",
	"X..X                            ",
	"X...X                           ",
	"X....X                          ",
	"X.....X                         ",
	"X......X                        ",
	"X.......X                       ",
	"X........X                      ",
	"X.....XXXXX                     ",
	"X..X..X                         ",
	"X.X X..X                        ",
	"XX  X..X                        ",
	"X    X..X                       ",
	"     X..X                       ",
	"      X..X                      ",
	"      X..X                      ",
	"       XX                       ",
	"                                ",
	"                                ",
	"                                ",
	"                                ",
	"                                ",
	"                                ",
	"                                ",
	"                                ",
	"                                ",
	"                                ",
	"                                ",
	"                                ",
	"                                ",
	"0,0"
};

static SDL_Cursor *init_system_cursor(const char *image[])
{
	int i, row, col;
	Uint8 data[4*32];
	Uint8 mask[4*32];
	int hot_x, hot_y;

	i = -1;
	for ( row=0; row<32; ++row ) {
		for ( col=0; col<32; ++col ) {
			if ( col % 8 ) {
				data[i] <<= 1;
				mask[i] <<= 1;
			} else {
				++i;
				data[i] = mask[i] = 0;
			}
			switch (image[4+row][col]) {
			case 'X':
				data[i] |= 0x01;
				//k[i] |= 0x01;
				break;
			case '.':
				mask[i] |= 0x01;
				break;
			case ' ':
				break;
			}
		}
	}
	sscanf(image[4+row], "%d,%d", &hot_x, &hot_y);
	return SDL_CreateCursor(data, mask, 32, 32, hot_x, hot_y);
}

void stoupper(char *s) {
	int i=0;
	while(s[i]!=0) {
		s[i]=toupper(s[i]);
		i++;
	}
}

typedef enum RETCODE {
	RT_VOID=0,
	RT_GAMEOVER,
	RT_CANCEL
}RETCODE;

SDL_Cursor *mouse_curs;

RETCODE run_game_mainloop(GAMETYPE gametype,int wide) {
	BOARD *board;
	SDL_Event event;
	int gameRunning = 1;
	int i,ox=0,oy=0;
	int omx,omy;

	if (wide) 
		board=create_board(gametype,9+4,7,MAXGEMCOL-1);
	else
		board=create_board(gametype,9,7,MAXGEMCOL-1);

	// TODO: the first run almost never give yellow, this is bad!
	fill_board(board);fill_board(board);
	play_music(SID_MUSIC1);
	while (1){
		while (SDL_PollEvent(&event)) {
			switch(event.type) {
			case SDL_QUIT: 
				free_board(board);
				return RT_CANCEL;
				break;
			case SDL_KEYDOWN:
				switch (event.key.keysym.sym) {
				case SDLK_q:
				case SDLK_ESCAPE:
					free_board(board);
					return RT_CANCEL;
					break;
// 				case SDLK_g:
// 					board->timer=1;
// 					break;
// #ifdef PANDORA
// 				case SDLK_c:
// 					if (mouse_curs==NULL)
// 						mouse_curs=init_system_cursor(arrow);
// 					SDL_SetCursor(mouse_curs);
// 					SDL_ShowCursor(SDL_ENABLE);
// 					break;
// #endif
// 				case SDLK_UP:
// 					if (board->cursy>0)
// 						board->cursy--;
// 					break;
				}
				break;
			case SDL_MOUSEBUTTONDOWN: 
				omx=omy=-1;
				screenspace2gemspace(board,event.button.x,event.button.y,&board->cursx,&board->cursy);
				if (board->cursx !=-1 && board->cursy!=-1) {
					omx=event.button.x;
					omy=event.button.y;
				}
				//printf("Click %d,%d -> %d %d\n",event.button.x,event.button.y,board->cursx,board->cursy);
				break;
			//case SDL_MOUSEBUTTONUP:
			case SDL_MOUSEMOTION:
				if (event.motion.state==SDL_PRESSED && omx!=-1 && omy!=-1) {

					screenspace2gemspace(board,event.motion.x,event.motion.y,&ox,&oy);
					if (ox!=board->cursx || oy!=board->cursy) {
						//printf("Motion %d,%d -> %d %d\n",event.motion.x,event.motion.y,ox,oy);
						if (ox!=-1 && oy!=-1 && board->block_input==0) {
							omx=omy=-1;
							play_sound(SID_SWAP);
							begin_swap(board,ox,oy,board->cursx,board->cursy);
						}
					}
				}
				break;
			}
		}

		draw_all(board);
		if (board->combo_cnt>0) {
			board->combo_cnt--;
			if (board->combo_cnt==0) {
				board->combo--;
				if (board->combo==0) board->combo=1;
				else board->combo_cnt=110-(board->combo*10);
				//board->combo=1;
			}
		}
		if (could_this_be_the_end(board)!=0) {
			/* TODO: Save score in hiscore here */
			if (ishiscore(gametype,board->score)) {
#ifdef PANDORA
				char *string=getenv("USER");
				stoupper(string);
#else
				char string[32];
				play_music(SID_MUSIC3);
				input_username(string,32);
				printf("String =%s\n",string);
#endif
				addhiscore(gametype,string,board->score);
			}
			free_board(board);
			return RT_GAMEOVER;
		}
#ifdef PANDORA
		SDL_BlitSurface(buffer,NULL,screen,NULL);
		SDL_Flip(screen);
#else
		SDL_SoftStretch(buffer,NULL,screen,NULL);
		SDL_UpdateRect(screen,0,0,screen->w,screen->h);
		frame_skip();//SDL_Delay(13);
#endif
	}

}

void intro_loop(void) {
	SDL_Event event;
	int counter=60*5;
	while (counter>0){
		while (SDL_PollEvent(&event)) {
			switch(event.type) {
			case SDL_KEYDOWN:
			case SDL_MOUSEBUTTONDOWN:
				return;
				break;
			}
		}
		draw_intro();
		flip_screen();
		counter--;
	}


}

void show_highscore(void) {
	SDL_Event event;
	int counter=60*5;
	int difficulty=0;
	while (difficulty<4){
		while (SDL_PollEvent(&event)) {
			switch(event.type) {
			case SDL_KEYDOWN:
			case SDL_MOUSEBUTTONDOWN:
				return;
				break;
			}
		}
		draw_scoreboard(difficulty|GM_LINE);
		flip_screen();
		counter--;
		if (counter==0) {
			difficulty++;
			counter=60*5;
		}
	}
}

static SDL_Rect butt_r={112,67,176,35};

int do_button(SDL_Rect *r,char *text,int mx,int my,int button) {
	draw_button(r,text);
	if (mx>r->x && mx<r->x+r->w && my>r->y && my<r->y+r->h && button!=0)
		return 1;
	return 0;
}

int main_menu(void) {
	SDL_Event event;
	int gametype=GM_LINE;
	int menulevel=2;
	int x,mx,my,b=0;
	int cnt=0;
	while (menulevel!=0){
		while (SDL_PollEvent(&event)) {
			switch(event.type) {
			case SDL_QUIT:
				return -1;
				break;
			case SDL_MOUSEBUTTONDOWN:
				mx=event.button.x;
				my=event.button.y;
				b=1;
				break;
			case SDL_MOUSEBUTTONUP:
				b=0;
				break;
			case SDL_KEYDOWN:
				switch (event.key.keysym.sym) {
				case SDLK_q:
				case SDLK_ESCAPE:
					return -1;
					break;
				}
				break;
			}
		}
		draw_main_menu();
		switch (menulevel) {
		case 2:
			//difficulty
			butt_r.y=67;
			if (do_button(&butt_r,"EASY",mx,my,b)) {
				menulevel=0;gametype|=GM_EASY;
			}
			butt_r.y+=40;
			if (do_button(&butt_r,"NORMAL",mx,my,b)) {
				menulevel=0;gametype|=GM_NORMAL;
			}
			butt_r.y+=40;
			if (do_button(&butt_r,"HARD",mx,my,b)) {
				menulevel=0;gametype|=GM_HARD;
			}
			butt_r.y+=40;
			if (do_button(&butt_r,"LEGEND!",mx,my,b)) {
				menulevel=0;gametype|=GM_LEGEND;
			}			
			break;
		case 1:
			// Pattern type
			//TODO
			break;
		}
		cnt++;
		if (cnt>60*20) {
			show_highscore();
			cnt=0;
		}
		flip_screen();
	}
	return gametype;
}

int main(int argc,char *argv[]) {
	BOARD *board=create_board(INLINE,9,7,MAXGEMCOL-1);
	int bstate=0;
	SDL_Event event;
	int gameRunning = 1;
	int i,ox=0,oy=0;
	int omx,omy;
	int rc,gt;
	init_rand(time(NULL));
	init_screen();
	load_assets();
	init_sound();
	load_sound();
	inithiscore();
	loadhiscore("sc.dat");

	mouse_curs=NULL;
#ifndef PANDORA
	init_frame_skip();
	reset_frame_skip();
#endif
	play_music(SID_MUSIC2);
	intro_loop();
	while ((gt=main_menu())!=-1) {
		printf("Gametype=%d\n",gt);
		rc=run_game_mainloop(gt,0);
		switch (rc) {
		case RT_CANCEL:
			break;
		case RT_GAMEOVER:
			break;
		}
		play_music(SID_MUSIC2);
	}
	savehiscore("sc.dat");

	Mix_CloseAudio();
	SDL_Quit();
	return 0;
}