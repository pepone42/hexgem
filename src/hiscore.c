#include <stdio.h>
#include "board.h"
#include "hiscore.h"



void inithiscore(void) {
	memset(highscore,0,sizeof(highscore));
}
void loadhiscore(char *file) {
	char name[32];
	int score,gt,rc;
	FILE *f=fopen(file,"r");
	if (f==NULL) return;
	do {
		if (fscanf(f,"%d %s %d",&gt,name,&score)==3) {
			printf("Add %d %s %d\n",gt,name,score);
			addhiscore(gt,name,score);
		}
	} while(feof(f)==0);
	fclose(f);
}
void savehiscore(char *file){
	int i,j;
	FILE *f=fopen(file,"w");
	if (f==NULL) return;
	for (i=0;i<GM_MAX;i++) {
		for (j=0;j<MAXHISCORE;j++) {
			if (highscore[i][j].score!=0)
				fprintf(f,"%d %s %d\n",i,highscore[i][j].name,highscore[i][j].score);
		}
	}
	fclose(f);
}
void addhiscore(int gametype,char *name,int score) {
	int i,j;
	for (i=0;i<MAXHISCORE;i++) {
		if (score>=highscore[gametype][i].score) {
			for (j=MAXHISCORE-1;j>i;--j) {
				highscore[gametype][j].score=highscore[gametype][j-1].score;
				sprintf(highscore[gametype][j].name,"%s",highscore[gametype][j-1].name);
			}
			highscore[gametype][i].score=score;
			sprintf(highscore[gametype][i].name,"%s",name);
			printf("Add score %d %s %d\n",gametype,highscore[gametype][i].name,highscore[gametype][i].score);
			break;
		}
	}
}
int ishiscore(int gametype,int score) {
	int i;
	for (i=0;i<MAXHISCORE;i++) {
		if (score>=highscore[gametype][i].score) {
			return 1;
		}
	}
	return 0;
}