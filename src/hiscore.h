#ifndef H_HIGHSCORE_
#define H_HIGHSCORE_

#include "board.h"

typedef struct SCORE{
	char	name[32];
	int		score;
}SCORE;

#define MAXHISCORE 5

SCORE highscore[GM_MAX][MAXHISCORE];

void inithiscore(void);
void loadhiscore(char *file);
void savehiscore(char *file);
void addhiscore(int gametype,char *name,int score);
int ishiscore(int gametype,int score);

#endif