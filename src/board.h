#ifndef H_BOARD_
#define H_BOARD_

#include <stdint.h>



typedef enum GEMCOL {
	OUTOFBOUND=-2,
	NOCOL=-1,
	YELLOW=0,
	PURPLE,
	BLUE,
	RED,
	GREEN,
	BROWN,
	CYAN,
	DEEP_PURPLE,
	MAXGEMCOL
}GEMCOL;

typedef enum GEMANIMSTATE {
	IDLE=0,
	OFF,
	FALL,
	CREATE,
	DELALL,
	SWAP,
	SWAP_BACK,
	INTRO,
	LEVELUP,
	GAMEOVER
}GEMANIMSTATE;

typedef enum GAMETYPE {  
	INLINE=0,
	INTRIANGLE,
	BOTH
}GAMETYPE;

typedef struct GEM
{
	uint32_t sig;
	GEMCOL col;
	GEMCOL backup_col;
	GEMANIMSTATE anim_state;

	// idle state
	int anim_cnt;
	int off_cnt;

	//fall state
	int to_del;
	int fall_y;
	int fall_speed;

	//swap state
	int x;
	int y;
	int dx; 
	int dy;
	int swap_cnt;
	int swap_id;
}GEM;

typedef struct GEMLIST
{
	GEM *g;
	struct GEMLIST *next;
}GEMLIST;

typedef struct BOARD
{
	int w;
	int h;
	GEM *gem;
	GEMLIST **fallbuf;
	int maxcol;
	int cursx;
	int cursy;
	int score;
	int *gcount;
	int goal;
	int level;
	int game_type;
	int block_input;
	int anim_state;
	int anim_cnt;
	GEMCOL lucky;
	int timer;
	int itmer_max;
	int scores[MAXGEMCOL*2][4];
	int scores_index;
	int combo;
	int combo_cnt;
	int chain;
}BOARD;

#define GM_EXIT		-1

#define GM_EASY		0
#define GM_NORMAL	1
#define GM_HARD		2
#define GM_LEGEND	3

#define GM_DIFFICULTY 3

#define GM_LINE		4
#define GM_TRI		8
#define GM_BOTH		12

#define GM_PATERN	12

#define GM_WIDE		16

#define GM_MAX		17

GEMLIST *glist_append(GEMLIST *gl,GEM *g);
GEMLIST *glist_prepend(GEMLIST *gl,GEM *g);
int glist_len(GEMLIST *gl);
GEMLIST *glist_delgem(GEMLIST *gl,GEM *g);


GEM *getgem(BOARD *b,int x,int y);
GEMCOL getgem_col(BOARD *b,int x,int y);
void fill_board(BOARD *b);
int valid_board(BOARD *b);
void put_new_gem(BOARD *b,int x,int y);
int verify_gem(BOARD *b,GEMCOL c,int x,int y);
int check_col(BOARD *b,GEMCOL c,int x,int y);
int swap_gem(BOARD *b,int xs,int ys,int xd,int yd);
int check_board(BOARD *b);
int reduce_board(BOARD *b);
int get_floor_level(BOARD *b,int column);
BOARD *create_board(int type,int w,int h,int maxcol);
void fallbuf_cleanup(BOARD *b,int column,int all);
int could_this_be_the_end(BOARD *board);
void free_board(BOARD *b);

#endif