#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include "SDL.h"
#include "board.h"
#include "screen.h"
#include "random.h"
#include "sound.h"


GEM *getgem(BOARD *b,int x,int y) {
	//printf("GETGEM %d,%d : ",x,y);
	if (x<0 || y<0 || x>=b->w || y>=b->h || (y==b->h-1 && (x&0x1)==0)) {
		//printf("OOB\n");
		return NULL;
	}
	//printf("OK\n");
	return &b->gem[x+y*b->w];
}
GEMCOL getgem_col(BOARD *b,int x,int y) {
	GEM *g=getgem(b,x,y);
	if (g) return g->col;
	return OUTOFBOUND;
}


GEMLIST *glist_append(GEMLIST *gl,GEM *g) {
	GEMLIST *glt=malloc(sizeof(GEMLIST));
	GEMLIST *l;
	glt->g=g;
	glt->next=NULL;
	if (gl==NULL) {
		return glt;
	}
	for (l=gl;l->next!=NULL;l=l->next);
		l->next=glt;
	return gl;
}
GEMLIST *glist_prepend(GEMLIST *gl,GEM *g) {
	GEMLIST *glt=malloc(sizeof(GEMLIST));
	glt->g=g;
	glt->next=gl;
	return glt;
}
GEMLIST *glist_insert(GEMLIST *gl,GEM *g) {
	GEMLIST *t=gl,*pt=NULL,*res=gl;
	GEMLIST *tmp=malloc(sizeof(GEMLIST));
	tmp->g=g;
	//printf("Insert\n");
	if (g==NULL) {
		printf("ASSERT ERROR : attempt to insert null GEM!\n");
		free(tmp);
		return gl;
	}
	for(t=gl;t && g->fall_y<t->g->fall_y;pt=t,t=t->next);
		if (t) {
		if (pt) { // insersion inside the list
			tmp->next=t;
			pt->next=tmp;
			tmp->g->fall_speed=pt->g->fall_speed;
		} else { // insersion at the begining of the list
			tmp->next=t;
			res=tmp;
		}
	} else {
		if (pt) { // insertion at the end of the list
			pt->next=tmp;
			tmp->next=NULL;
			tmp->g->fall_speed=pt->g->fall_speed;
		} else { // first element
			res=tmp;
			tmp->next=NULL;
		}
	}
	return res;
}
int glist_len(GEMLIST *gl) {
	int i=0;
	GEMLIST *l;
	if (gl==NULL) return 0;
	for (l=gl;l;l=l->next) i++;
		return i;
}
GEMLIST *glist_delgem(GEMLIST *gl,GEM *g) {
	GEMLIST *l,*t;
	if (g==gl->g) {
		free(gl->g);
		l=gl->next;
		free(gl);
		return l;
	}
	for (l=gl;l->next!=NULL;l=l->next) {
		if (l->next->g==g) {
			t=l->next;
			l->next=l->next->next;
			free(g);
			free(t);
		}
	}
}

void fallbuf_cleanup(BOARD *b,int column,int all) {
	GEMLIST *l,*p;
	l=b->fallbuf[column];
	p=NULL;
	while (l) {
		//printf("%p \n",l);
		if (all==1 || l->g->to_del!=0) {
			if (p==NULL) { /*  delete first */
			GEMLIST *t=l;
				//printf("Free head\n");
			l=l->next;
			free(t->g);
			free(t);
			b->fallbuf[column]=l;
			continue;
			} else { /* delete inside the list */
				//printf("Free inside\n");
			GEMLIST *t=l;
			p->next=l->next;
			l=l->next;
			free(t->g);
				free(t); // Issue here
				continue;
			}
		}
		p=l;
		l=l->next;
	}
}


GEM *gem_clone(GEM *g) {
	GEM *gc=malloc(sizeof(GEM));
	memcpy(gc,g,sizeof(GEM));
	return gc;
}

BOARD *create_board(int type,int w,int h,int maxcol) {
	BOARD *b;
	int i,j;

	b=malloc(sizeof(BOARD));
	if (b==NULL) return NULL;
	memset(b,0,sizeof(BOARD));
	b->gem=malloc(sizeof(GEM)*w*h);
	b->fallbuf=malloc(sizeof(GEMLIST*)*w);
	memset(b->fallbuf,0,sizeof(GEMLIST*)*w);
	b->gcount=malloc(sizeof(int)*MAXGEMCOL);
	memset(b->gcount,0,sizeof(int)*MAXGEMCOL);
	b->w=w;
	b->h=h;
	b->cursx=-1;
	b->cursy=-1;
	b->score=0;
	b->maxcol=maxcol;
	b->level=1;
	b->game_type=type;
	b->block_input=1;
	b->goal=3;
	b->anim_cnt=0;
	b->anim_state=INTRO;
	b->lucky=random_max(maxcol);
	b->timer=80*60;
	b->combo=1;
	return b;
}

void free_board(BOARD *b) {
	int i,j;
	for (i=0;i<b->w;i++) {
		fallbuf_cleanup(b,i,1);
	}
	free(b->gem);
	free(b->fallbuf);
	free(b->gcount);
}

int get_floor_level(BOARD *b,int column) {
	int j;
	for(j=0;j<(column&1?b->h:b->h-1);j++) {
		if (getgem_col(b,column,j)!=NOCOL /*&& getgem_col(b,i,j)!=OUTOFBOUND*/)
		break;
	}
	return j-1;
}

int check_col(BOARD *b,GEMCOL c,int x,int y) {
	int cc=getgem_col(b,x,y);
	if (cc==NOCOL || cc==OUTOFBOUND || c==OUTOFBOUND || c==NOCOL) return 0;
	if (cc==c) return 1;
	return 0;
}

int verify_gem_line(BOARD *b,GEMCOL c,int x,int y) {
	// int l1,l2,l3;
	// l1=1+check_col(b,c,x,y-1)+check_col(b,c,x,y-2)+check_col(b,c,x,y-1)+check_col(b,c,x,y-2);
	// l2=1+check_col(b,c,x-1,y)+check_col(b,c,x-2,y+1)+check_col(b,c,x+1,y-1)+check_col(b,c,x+2,y-1);
	// l3=1+check_col(b,c,x-1,y-1)+check_col(b,c,x-2,y-1)+check_col(b,c,x+1,y)+check_col(b,c,x+2,y+1);
	if (c==NOCOL || c==OUTOFBOUND) return 0;

	if (check_col(b,c,x,y-1)&check_col(b,c,x,y-2)) return 1;
	if (check_col(b,c,x,y-1)&check_col(b,c,x,y+1)) return 1;
	if (check_col(b,c,x,y+1)&check_col(b,c,x,y+2)) return 1;
	if ((x&1)==0) {
		if (check_col(b,c,x-1,y+1)&check_col(b,c,x-2,y+1)) return 1;
		if (check_col(b,c,x-1,y+1)&check_col(b,c,x+1,y)) return 1;
		if (check_col(b,c,x+1,y)&check_col(b,c,x+2,y-1)) return 1;

		if (check_col(b,c,x-1,y)&check_col(b,c,x-2,y-1)) return 1;
		if (check_col(b,c,x-1,y)&check_col(b,c,x+1,y+1)) return 1;
		if (check_col(b,c,x+1,y+1)&check_col(b,c,x+2,y+1)) return 1;
	} else {
		if (check_col(b,c,x-1,y)&check_col(b,c,x-2,y+1)) return 1;
		if (check_col(b,c,x-1,y)&check_col(b,c,x+1,y-1)) return 1;
		if (check_col(b,c,x+1,y-1)&check_col(b,c,x+2,y-1)) return 1;

		if (check_col(b,c,x-1,y-1)&check_col(b,c,x-2,y-1)) return 1;
		if (check_col(b,c,x-1,y-1)&check_col(b,c,x+1,y)) return 1;
		if (check_col(b,c,x+1,y)&check_col(b,c,x+2,y+1)) return 1;
	}
	return 0;
}

int verify_gem_tri(BOARD *b,GEMCOL c,int x,int y) {
	if (c==NOCOL || c==OUTOFBOUND) return 0;
	if (x&1) {
		if (check_col(b,c,x-1,y)&check_col(b,c,x-1,y-1)) return 1;
		if (check_col(b,c,x-1,y-1)&check_col(b,c,x,y-1)) return 1;
		if (check_col(b,c,x,y-1)&check_col(b,c,x+1,y-1)) return 1;
		if (check_col(b,c,x+1,y-1)&check_col(b,c,x+1,y)) return 1;
		if (check_col(b,c,x+1,y)&check_col(b,c,x,y+1)) return 1;
		if (check_col(b,c,x,y+1)&check_col(b,c,x-1,y)) return 1;
	} else {
		if (check_col(b,c,x-1,y+1)&check_col(b,c,x-1,y)) return 1;
		if (check_col(b,c,x-1,y)&check_col(b,c,x,y-1)) return 1;
		if (check_col(b,c,x,y-1)&check_col(b,c,x+1,y)) return 1;
		if (check_col(b,c,x+1,y)&check_col(b,c,x+1,y+1)) return 1;
		if (check_col(b,c,x+1,y+1)&check_col(b,c,x,y+1)) return 1;
		if (check_col(b,c,x,y+1)&check_col(b,c,x-1,y+1)) return 1;
	}
	return 0;
}

int verify_gem(BOARD *b,GEMCOL c,int x,int y) {
	int rc=0;
	if (b->game_type&GM_LINE)
		rc+= verify_gem_line(b,c,x,y);
	if (b->game_type&GM_TRI)
		rc+= verify_gem_tri(b,c,x,y);
	return rc;
}

void put_new_gem(BOARD *b,int x,int y) {
	GEMCOL c=NOCOL;
	GEM *g;
	unsigned int max=b->maxcol;
	//if (b->extracol==0) max--;

	if (x<0 || y<0 || x>=b->w || y>=b->h || (y==b->h-1 && (x&0x1)==0)) {
		c=OUTOFBOUND;
		//printf("put gem OOB %d,%d\n",x,y);
		return;
	}
	else {
		do { 
			c=random_max(max);
		} while (verify_gem(b,c,x,y)!=0);
		//printf("put gem %d %d,%d\n",c,x,y);
	}
	g=getgem(b,x,y);
	//printf("GEM=%p\n",g);
	if (g) {
		g->col=c;
		g->backup_col=NOCOL;
		g->anim_state=IDLE;
		g->anim_cnt=0;
		g->to_del=0;
	}
}

GEM *new_gem(BOARD *b) {
	GEMCOL c;
	GEM *g;
	int max=b->maxcol;
	//if (b->extracol==0) max--;
	c=random_max(max);
	g=malloc(sizeof(GEM));
	g->col=c;
	g->backup_col=NOCOL;
	g->anim_state=IDLE;
	g->anim_cnt=0;
	g->to_del=0;

	return g;
}

int valid_board(BOARD *b) {
	int i,j;
	int m=0;
	for (i=0;i<b->w;i++) {
		for(j=0;j<b->h;j++) {
			if (i&1) {
				m+=verify_gem(b,getgem_col(b,i-1,j-1),i,j); // LU
				m+=verify_gem(b,getgem_col(b,i,j-1),i,j); // U
				m+=verify_gem(b,getgem_col(b,i+1,j-1),i,j); // RU
				m+=verify_gem(b,getgem_col(b,i+1,j),i,j); // RD
				m+=verify_gem(b,getgem_col(b,i,j+1),i,j); // D
				m+=verify_gem(b,getgem_col(b,i-1,j),i,j); // LD
			} else {
				m+=verify_gem(b,getgem_col(b,i-1,j),i,j); // LU
				m+=verify_gem(b,getgem_col(b,i,j-1),i,j); // U
				m+=verify_gem(b,getgem_col(b,i+1,j),i,j); // RU
				m+=verify_gem(b,getgem_col(b,i+1,j+1),i,j); // RD
				m+=verify_gem(b,getgem_col(b,i,j+1),i,j); // D
				m+=verify_gem(b,getgem_col(b,i-1,j+1),i,j); // LD
			}
		}
	}
	return m;
}

void fill_board(BOARD *b) {
	int i,j;
	//b->extracol=(b->maxcol==MAXCOL?1:0);
	do {
		for (i=0;i<b->w;i++) {
			for(j=0;j<b->h;j++) {
				put_new_gem(b,i,j);
			}
		}
	} while (valid_board(b)==0);
}

int swap_gem(BOARD *b,int xs,int ys,int xd,int yd) {
	int col;
	GEM *gs,*gd;
	gs=getgem(b,xs,ys);
	gd=getgem(b,xd,yd);
	if (gs->col==NOCOL || gs->col==OUTOFBOUND || gs->to_del==1 || b->block_input==1)
		return 0;
	col=gd->col;
	gd->col=gs->col;
	gs->col=col;
	if   ( verify_gem(b,getgem_col(b,xd,yd),xd,yd)!=0 || verify_gem(b,getgem_col(b,xs,ys),xs,ys)!=0) {
		return 1;
	}

	col=gd->col;
	gd->col=gs->col;
	gs->col=col;    
	return 0;
}
#define MAXSWAP 5
static int swap_buf[MAXSWAP][4];
static int swap_index;

void begin_swap(BOARD *b,int xs,int ys,int xd,int yd) {
	GEM *gs,*gd;
	gs=getgem(b,xs,ys);
	gd=getgem(b,xd,yd);
	if (gs && gd && gs->col!=NOCOL && gd->col!=NOCOL) {
		gs->anim_state=SWAP;
		gd->anim_state=SWAP;
		gs->swap_cnt=gd->swap_cnt=8;
		gs->x=g2s_x(xs);
		gs->y=g2s_y(xs,ys);
		gd->x=g2s_x(xd);
		gd->y=g2s_y(xd,yd);
		gs->dx=(g2s_x(xd)-g2s_x(xs))/gs->swap_cnt;
		gs->dy=(g2s_y(xd,yd)-g2s_y(xs,ys))/gs->swap_cnt;
		gd->dx=-gs->dx;
		gd->dy=-gs->dy;
		gd->swap_id=gs->swap_id=swap_index;
		swap_buf[swap_index][0]=xs;
		swap_buf[swap_index][1]=ys;
		swap_buf[swap_index][2]=xd;
		swap_buf[swap_index][3]=yd;
		swap_index++;
		if (swap_index>=MAXSWAP) swap_index=0;
	}
}
void check_swap(BOARD *b,int index) {
	GEM *gs,*gd;
	int xs,ys,xd,yd;
	xs=swap_buf[index][0];
	ys=swap_buf[index][1];
	xd=swap_buf[index][2];
	yd=swap_buf[index][3];
	gs=getgem(b,xs,ys);
	gd=getgem(b,xd,yd);

	if (gs && gd &&swap_gem(b,xs,ys,xd,yd)==0) {
		gs->anim_state=SWAP_BACK;
		gd->anim_state=SWAP_BACK;
		gs->swap_cnt=gd->swap_cnt=8;
		// gs->x=g2s_x(xs);
		// gs->y=g2s_y(xs,ys);
		// gd->x=g2s_x(xd);
		// gd->y=g2s_y(xd,yd);
		// gs->dx=-(g2s_x(xd)-g2s_x(xs))/gs->swap_cnt;
		// gs->dy=-(g2s_y(xd,yd)-g2s_y(xs,ys))/gs->swap_cnt;
		// gd->dx=-gs->dx;
		// gd->dy=-gs->dy;
		play_sound(SID_WRONG);
	} else {
		gs->anim_state=IDLE;
		gd->anim_state=IDLE;
		reduce_board(b);
	}
}
void end_swap_back(BOARD *b,int index) {
	GEM *gs,*gd;
	int xs,ys,xd,yd;
	xs=swap_buf[index][0];
	ys=swap_buf[index][1];
	xd=swap_buf[index][2];
	yd=swap_buf[index][3];
	gs=getgem(b,xs,ys);
	gd=getgem(b,xd,yd);
	gs->anim_state=IDLE;
	gd->anim_state=IDLE;
}

#ifdef DEBUG
void waitevent(void) {
	SDL_Event e;
	while(1) {
		if (SDL_PollEvent(&e)) {
			if (e.type==SDL_KEYDOWN) return;
		}
	}
}
static void redrawboard(BOARD *b) {
	draw_board();
	draw_gems(b);
	SDL_SoftStretch(buffer,NULL,screen,NULL);
	SDL_UpdateRect(screen,0,0,screen->w,screen->h);
}
#endif
int check_board(BOARD *b) {
	int i,j,jj;
	int rc=0;
	GEM *g;
	int fall_y;
	for (i=0;i<b->w;i++) {
		for(j=0;j<(i&1?b->h:b->h-1);j++) {
			if (verify_gem(b,getgem_col(b,i,j),i,j)!=0) {
				g=getgem(b,i,j);
				if (g) g->to_del=1;
				rc++;
			}
		}
	}
	return rc;
}

int board_col_count(BOARD *b,int col) {
	int j;
	int count=0;
	GEMCOL c;
	for (j=0;j<(col&1?b->h:b->h-1);j++) {
		c=getgem_col(b,col,j);
		if (c!=NOCOL && c!=OUTOFBOUND)
			count++;
	}
	return count;
}

static void inc_timer(BOARD *b,int msec) {
	int max=(90-(b->level*10))*60;
	int mul;

	if ((b->game_type&GM_DIFFICULTY)==GM_EASY)
		msec*=4;
	if ((b->game_type&GM_DIFFICULTY)==GM_NORMAL)
		msec*=3;
	if ((b->game_type&GM_DIFFICULTY)==GM_HARD)
		msec*=2;
	if ((b->game_type&GM_DIFFICULTY)==GM_LEGEND)
		msec*=1;

	max=(max<20*60?20*60:max);
	b->timer+=msec;

	if (b->timer>max) {
		b->timer=max;
	}
}
static void reset_timer(BOARD *b) {
	// int min=20*60;
	// b->timer=(90-(b->level*10))*60;
	// if (b->timer<min) b->timer=min;
	if ((b->game_type&GM_DIFFICULTY)==GM_EASY)
		inc_timer(b,b->timer*3);
	if ((b->game_type&GM_DIFFICULTY)==GM_NORMAL)
		inc_timer(b,b->timer*2);
	if ((b->game_type&GM_DIFFICULTY)==GM_HARD)
		inc_timer(b,b->timer);
	if ((b->game_type&GM_DIFFICULTY)==GM_LEGEND)
		inc_timer(b,b->timer/2);

}
int reduce_board(BOARD *b) {
	int i,j,jj,y;
	int rc=0;
	GEM *g;
	int fall_y;
	int scores_helper[MAXGEMCOL][3];
	int count;

	for (i=0;i<MAXGEMCOL;i++) {
		scores_helper[i][0]=scores_helper[i][1]=scores_helper[i][2]=0;
	}

	for (i=0;i<b->w;i++) {
		count=0;
		if (i&1) {
			count=b->h-(board_col_count(b,i)+glist_len(b->fallbuf[i]));
			y=8;
		} else {
			y=24;
			count=b->h-1-(board_col_count(b,i)+glist_len(b->fallbuf[i]));
		}
		if (count!=0)
			printf("ASSERT ERROR : gem missing in col %d\n",i);
	}

	for (i=0;i<b->w;i++) {
		//reduced[i]=0;
		for(j=0;j<(i&1?b->h:b->h-1);j++) {
			if (verify_gem(b,getgem_col(b,i,j),i,j)!=0) {

				//printf("Clean %d,%d\n",i,j);
				g=getgem(b,i,j);
				if (g) {
					g->to_del=1;
					g->anim_state=OFF;
					g->off_cnt=0;
					g->backup_col=g->col;
					//reduced[i]++;
					rc++;
					// b->score+=(g->col==b->lucky?2:1)*b->level;
					// if (b->game_type!=BOTH)
					// 	b->timer+=10;
					b->gcount[g->col]++;
					scores_helper[g->col][0]+=g2s_x(i);
					scores_helper[g->col][1]+=g2s_y(i,j);
					scores_helper[g->col][2]++;
				} else
				printf("ASSERT ERROR : getgem return null %d,%d\n",i,j);

				for (jj=j-1;jj>=0;--jj) {
					g=getgem(b,i,jj);
					if(!g) printf("ASSERT ERROR : getgem return null %d,%d\n",i,jj);
					if (g && g->col!=NOCOL && g->col != OUTOFBOUND && g->to_del==0) {
						g->fall_y=g2s_y(i,jj);
						g->fall_speed=0;
						//g->anim_state=FALL;
						//printf("Append %d\n",g->col);
						b->fallbuf[i]=glist_insert(b->fallbuf[i],gem_clone(g));
						g->to_del=1;
					} 
				}
			}
		}
	}

	/* delete gems for good */
	for (i=0;i<b->w;i++) {
		for(j=0;j<(i&1?b->h:b->h-1);j++) {
			g=getgem(b,i,j);
			if (!g) printf("ASSERT ERROR : getgem return null %d,%d\n",i,j);
			if (g && g->to_del==1) {
				g->col=NOCOL;
				g->to_del=0;
			}
			
		}
	}
	/* Fill the fallbuf with the necesseray numbers of gems */
	for (i=0;i<b->w;i++) {
		count=0;
		if (i&1) {
			count=b->h-(board_col_count(b,i)+glist_len(b->fallbuf[i]));
			y=8;
		} else {
			y=24;
			count=b->h-1-(board_col_count(b,i)+glist_len(b->fallbuf[i]));
		}
		//printf("Col %d: insert %d gem into falllist (%d-%d+%d)\n",i,count,b->h,board_col_count(b,i),glist_len(b->fallbuf[i]));
		for(j=0;j<count;j++) {
			g=new_gem(b);
			if (g==NULL)
				printf("ASSERT ERROR : new_gem return null %d,%d\n",i,j);
			g->fall_y=y-32;
			g->fall_speed=1;
			//printf("Insert new %d %d %d\n",i,g->col,g->fall_y);
			b->fallbuf[i]=glist_insert(b->fallbuf[i],g);
			y-=32;
		}
	}

	/* Update scores */
	for (i=0;i<MAXGEMCOL;i++) {
		if (scores_helper[i][2]!=0) {
			b->scores[b->scores_index][0]=scores_helper[i][0]/scores_helper[i][2];
			b->scores[b->scores_index][1]=scores_helper[i][1]/scores_helper[i][2];
			if (scores_helper[i][2]<=3) {
				b->scores[b->scores_index][2]=scores_helper[i][2]*(i==b->lucky?2:1);
			} else {
				b->scores[b->scores_index][2]=3*(scores_helper[i][2]-2)*(i==b->lucky?2:1);
			}
			b->score+=b->scores[b->scores_index][2]*10*b->level*b->combo;
			// b->timer+=b->scores[b->scores_index][2];
			// if (b->timer>(90-(b->level*10))*60) {
			// 	b->timer=(90-(b->level*10))*60;
			// }
			inc_timer(b,b->scores[b->scores_index][2]*b->combo*3);
			b->scores[b->scores_index][3]=40;
			b->scores_index++;
			if (b->scores_index>=MAXGEMCOL)
				b->scores_index=0;
		}
	}
	if (rc>0) {
		b->chain++;
		if (b->chain==1)
			play_sound(SID_COMBO1);
		else if (b->chain==2)
			play_sound(SID_COMBO2);
		else 
			play_sound(SID_COMBO3);
		if (b->combo<10) b->combo++;
		b->combo_cnt=110-(b->combo*10);

	} else {
		b->chain=0;
	}
	check_goals(b);
	return rc;
}

int check_goals(BOARD *b) {
	int delay=0;
	int goal_ok=1;
	int i;

	/* We want all our combo, so if there is still some gems 
	   in the falling buf, then just wait, but we have to block the gameplay */
	for (i=0;i<b->h;i++) {
		if (glist_len(b->fallbuf[i])>0) 
			delay=1;
	}
/* check if all goal are achieved */
	for (i=0;i<b->maxcol;i++) {
		if (b->gcount[i]<b->goal) {
			goal_ok=0;
		}
	}
	if (goal_ok==1) {
		if (delay==0) {
//b->block_input=0;
			b->level++;
			b->goal++;
			b->combo=1;
			// b->timer=(90-(b->level*10))*60;
			// if (b->timer<20*60) b->timer=20*60;
			reset_timer(b);
			if (b->level%6==0) {
				if (b->maxcol==MAXGEMCOL)
					b->maxcol--;
				else
					b->maxcol++;
			}
			b->lucky=random_max(b->maxcol);
//fill_board(b);
			for (i=0;i<b->maxcol;i++) {
				b->gcount[i]=0;
			}
			b->anim_state=DELALL;
			b->anim_cnt=0;
			return 1;
		} else {
			b->block_input=1;
		}
	}
	return 0;
}


int could_this_be_the_end(BOARD *board) {
	if (!board->block_input)
		board->timer--;
	if (board->timer==0) {
		board->anim_state=GAMEOVER;
		board->anim_cnt=0;
		board->block_input=1;
		board->timer=-1;
		play_sound(SID_GAMEOVER);
		fadout_music(2000);
	}
	if (board->anim_state==GAMEOVER) {
			// Gameover;
			// TODO: free list, we may have a combo running

		board->anim_cnt++;
		if (board->anim_cnt>120) {
			return 1;
		}
	}
	return 0;
}
