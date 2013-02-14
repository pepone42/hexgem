//#include "SDL.h"
#include "screen.h"
#include "board.h"
#include "sound.h"
#include "hiscore.h"

static SDL_Surface *gem_surface,*little_gem_surface;
static SDL_Surface *board_surface;
static SDL_Surface *board_top_surface;
static SDL_Surface *font_big,*font_small;
static SDL_Surface *button,*gamemode_button,*titlescreen,*dragonbox;


// static struct gem_asset_t {
// 	GEMCOL col;
// 	char *name;
// } gem_asset[] = {
// 	{GREEN,"assets/gem_green.bmp"},
// 	{BLUE,"assets/gem_blue.bmp"},
// 	{RED,"assets/gem_red.bmp"},
// 	{PURPLE,"assets/gem_purple.bmp"},
// 	{YELLOW,"assets/gem_yellow.bmp"},
// };



void SDL_putchar(SDL_Surface *font,SDL_Surface * dest, int x, int y, unsigned char c)
{
	static SDL_Rect font_rect, dest_rect;
	int indice = c - 32;

	if (c < 32 || c > 109)
		return;

	font_rect.x = indice *  (font->w/77);
	font_rect.y = 0;
	font_rect.w =  font->w/77;
	font_rect.h =  font->h;
	dest_rect.x = x;
	dest_rect.y = y;
	dest_rect.w =  font->w/77;
	dest_rect.h =  font->h;

	SDL_BlitSurface(font, &font_rect, dest, &dest_rect);

}

void SDL_textout(SDL_Surface *font,SDL_Surface * dest, int x, int y, const char *string)
{
	int i;int xx=x;
	for (i = 0; i < strlen(string); i++) {
		if (string[i]=='\n') {xx=x;y+=font->h;continue;}
		SDL_putchar(font,dest, xx , y, string[i]);
		xx+=(font->w/77);
	}
}

void SDL_printf(SDL_Surface *font,SDL_Surface * dest, int x, int y, char *fmt, ...) {
	char buf[512];
	va_list pvar;
	va_start(pvar, fmt);

	vsnprintf(buf, 511, fmt, pvar);
	SDL_textout(font,dest,x,y,buf);
}

#define LEFT 1
#define RIGHT 2
#define BACKSPACE 3
#define DEL 4

int SDL_getchar(void)
{
	SDL_Event event;
	SDL_WaitEvent(&event);
    //while(SDL_PollEvent(&event)){}
	switch (event.type) {
	case SDL_KEYDOWN:
		switch(event.key.keysym.sym) {
		case SDLK_RETURN:
			return -1;
		case SDLK_LEFT:
			return LEFT;
		case SDLK_RIGHT:
			return RIGHT;
		case SDLK_DELETE:
			return DEL;
		case SDLK_BACKSPACE:
			return BACKSPACE;
			default:
			break;
		}
		//printf("Press %d\n",event.key.keysym.unicode);
		if ((event.key.keysym.unicode & 0xFF80)==0 ) {
			int k=event.key.keysym.unicode;
			printf("Press %d\n",k);
			if ( k>=65+32) {
				k-=32;
			}
			return k;
		}
		break;
	}
	return 0;
}
void SDL_textinput(SDL_Surface *font,SDL_Surface * dest,int x,int y,char *string,int size)
{
	int sx;
	int a;
	int s=0;
	int i;
	int pos=0;
	SDL_Surface *save;
	SDL_Rect save_rect={x,y,size*font->w/77,font->h};
	SDL_Rect curs_rect={0,y+font->h,font->w/77,2};
	
	save=SDL_CreateRGBSurface(SDL_SWSURFACE,size*font->w/77,font->h,16, 0xF800, 0x7E0,0x1F, 0);
	SDL_BlitSurface(dest,&save_rect,save,NULL);
	memset(string,0,size+1);

	sx=x;
	SDL_EnableUNICODE(1);
	while((a=SDL_getchar())!=-1) {
		if (a==LEFT && pos>0) pos--;
		if (a==RIGHT && pos<s) pos++;
		if (a==BACKSPACE && pos>0) {
			for(i=pos-1;i<s;i++)
				string[i]=string[i+1];
			s--;
			pos--;
		}
		if (a==DEL && pos<s) {
			for(i=pos;i<s;i++)
				string[i]=string[i+1];
			s--;
		}
		if (a>32  && s<size ) {
			for(i=s;i>pos;i--)
				string[i]=string[i-1];
			string[pos]=(char)a;

			s++;
			pos++;
		}
		SDL_BlitSurface(save,NULL,dest,&save_rect);
        //SDL_BlitSurface(save,NULL,buffer,&clear_rect);
		SDL_textout(font,dest,sx,y,string);
  //       /* cursor */
  //       curs_rect.x=x+pos*font->w/77;
		// SDL_FillRect(dest,&curs_rect,0xFFFF);
#ifdef PANDORA
		SDL_BlitSurface(buffer,NULL,screen,NULL);
		SDL_Flip(screen);
#else
		SDL_SoftStretch(buffer,NULL,screen,NULL);
		SDL_UpdateRect(screen,0,0,screen->w,screen->h);
		//frame_skip();//SDL_Delay(13);
#endif

	}
	SDL_EnableUNICODE(0);
	SDL_FreeSurface(save);
}

void input_username(char *string,int size) {
	SDL_printf(font_big,buffer,130,130,"YOU GOT A HISCORE!!");
	SDL_printf(font_big,buffer,130,150,"   ENTER YOUR NAME:");
	SDL_printf(font_big,buffer,130,180,"-------------------");
	#ifdef PANDORA
	SDL_BlitSurface(buffer,NULL,screen,NULL);
	SDL_Flip(screen);
#else
	SDL_SoftStretch(buffer,NULL,screen,NULL);
	SDL_UpdateRect(screen,0,0,screen->w,screen->h);
		//frame_skip();//SDL_Delay(13);
#endif
	SDL_textinput(font_big,buffer,130,170,string,size);
}


/* blit a surface, zooming it if necessary, but don't zoom the border */
void SDL_BlitZoomBorder(SDL_Surface *src,SDL_Rect *sr,int b,
	SDL_Surface *dst,SDL_Rect *dr) {
	SDL_Rect trect;
	int sw,sh,sx,sy;
	if (dr==NULL) return;
	if (sr==NULL) {
		sw=src->w;sh=src->h;
		sx=0;sy=0;
	} else {
		sw=sr->w;sh=sr->h;
		sx=sr->x;sy=sr->y;
	}
	trect.w=b;trect.h=b;
	trect.x=sx;trect.y=sy;
	SDL_BlitSurface(src,&trect,dst,dr);
	trect.x=sw-b;trect.y=sy;
	SDL_BlitSurface(src,&trect,dst,dr);
	trect.x=sw-b;trect.y=sh-b;
	SDL_BlitSurface(src,&trect,dst,dr);
	trect.x=sx;trect.y=sh-b;
	SDL_BlitSurface(src,&trect,dst,dr);

	trect.w=sw-b*2;trect.h=sh-b*2;
	trect.x=sx+b;trect.y=sy+b;
	dr->x+=b;dr->y+=b;
	dr->w=dr->w-b*2;dr->h=dr->h-b*2;
	SDL_SoftStretch(src,&trect,dst,&dr);

}

void init_screen(void) {
	int rc;
	rc=SDL_Init(SDL_INIT_VIDEO|SDL_INIT_AUDIO);
	if(rc!=0) {
		printf("Error while SDL_Init\n");
		exit(1);
	}
#ifdef PANDORA
	screen=SDL_SetVideoMode(400,240,16,SDL_HWSURFACE|SDL_DOUBLEBUF|SDL_FULLSCREEN);
#else
	screen=SDL_SetVideoMode(400,240,16,SDL_SWSURFACE);
#endif
	buffer=SDL_CreateRGBSurface(SDL_SWSURFACE,400,240,16,0xF800, 0x7E0, 0x1F, 0);
}

SDL_Surface *load_bmp(char *filename,int colorkey) {
	SDL_Surface *temp,*res;

	temp=SDL_LoadBMP(filename);
	if (colorkey) {
		SDL_SetColorKey(temp,SDL_SRCCOLORKEY,SDL_MapRGB(temp->format, 255, 0, 255));
	}
	res=SDL_DisplayFormat(temp);
	SDL_FreeSurface(temp);

	return res;
}

void load_assets(void) {
	gem_surface=load_bmp("assets/gems.bmp",1);
	little_gem_surface=load_bmp("assets/little_gem.bmp",1);
	board_surface=load_bmp("assets/board169.bmp",0);
	board_top_surface=load_bmp("assets/board_top.bmp",1);
	font_big=load_bmp("assets/font_big.bmp",1);
	font_small=load_bmp("assets/font_small.bmp",1);
	button=load_bmp("assets/button.bmp",1);
	gamemode_button=load_bmp("assets/gamemode.bmp",1);
	titlescreen=load_bmp("assets/titlescreen.bmp",0);
	dragonbox=load_bmp("assets/dragonbox.bmp",0);
}

void draw_board(void) {
	SDL_BlitSurface(board_surface,NULL,buffer,NULL);
}
void draw_board_top(void) {
	SDL_Rect dst={80,0,board_top_surface->w,board_top_surface->h};
	SDL_BlitSurface(board_top_surface,NULL,buffer,&dst);
}


void test_draw_gems(int col,int x,int y) {
	static int cnt;int nb_frame=3;
	SDL_Rect src={0,col*32,32,32},dst={x,y,32,32};


	if (cnt==60+9*nb_frame) cnt=0;
	if (cnt>60) {
		src.x=32*((cnt-60)/nb_frame);
	}
	SDL_BlitSurface(gem_surface,&src,buffer,&dst);
	SDL_BlitSurface(buffer,NULL,screen,NULL);
	if (col==0) cnt++;
}

static void draw_gem_idle(GEM *g,int x,int y) {
	int nb_frame=5;
	SDL_Rect src={0,(g->col)*32,32,32},dst={x,y,32,32};
	if (g->col==NOCOL || g->col==OUTOFBOUND) return;
	// if (g->to_del==1) {
	// 	src.y=(g->col+9)*32;
	// 	src.x=32*4;
	// 	SDL_BlitSurface(gem_surface,&src,buffer,&dst);
	// 	return;
	// }

	if (g->anim_cnt==0 && (rand()&0xFFF)==0x1) g->anim_cnt=1;

	if (g->anim_cnt>0) {
		src.x=32*(g->anim_cnt/nb_frame);
		g->anim_cnt++;
	}
	SDL_BlitSurface(gem_surface,&src,buffer,&dst);
	if (g->anim_cnt>=(9)*nb_frame) 
		g->anim_cnt=0;

}

static void draw_gem_off(GEM *g,int x,int y) {
	int nb_frame=2;
	SDL_Rect src={0,(g->backup_col+9)*32,32,32},dst={x,y,32,32};

	g->off_cnt++;	
	src.x=32*(g->off_cnt/nb_frame);



	if (g->off_cnt>=(9)*nb_frame) {
		// stay on last frame
		src.x=32*8;
		dst.x+=(g->off_cnt-(9*nb_frame))/2;
		dst.y-=(g->off_cnt-(9*nb_frame))/2;
		if (g->off_cnt>=(13)*nb_frame) {
			g->off_cnt=0;
			g->anim_state=IDLE;
			g->backup_col=NOCOL;
		}
		if (g->off_cnt&0X1) SDL_BlitSurface(gem_surface,&src,buffer,&dst);
	} else
	SDL_BlitSurface(gem_surface,&src,buffer,&dst);

}
static void draw_gem_alloff(GEM *g,int x,int y,int cnt) {
	int nb_frame=2;
	SDL_Rect src={0,(g->col+9)*32,32,32},dst={x,y,32,32};

	src.x=32*(cnt/nb_frame);
	if (cnt>=(9)*nb_frame) {
		// stay on last frame
		src.x=32*8;
		dst.x+=(cnt-(9*nb_frame))/2;
		dst.y-=(cnt-(9*nb_frame))/2;
		if (cnt&0X1) SDL_BlitSurface(gem_surface,&src,buffer,&dst);
	} else
	SDL_BlitSurface(gem_surface,&src,buffer,&dst);

}

static void draw_gem(BOARD *b,GEM *g,int x,int y) {
	if (g->anim_state==IDLE || g->anim_state==FALL) {
		if (b->timer<10*60 && b->timer>0) {
			if (random_max(b->timer/20+1)<2) {
				x+=(random_max(3)-2);
				y+=(random_max(3)-2);
			}
		}
		draw_gem_idle(g,x,y);
	}
	if (g->anim_state==SWAP) {
		g->swap_cnt--;//SDL_Delay(100);
		if (g->swap_cnt==0) {
			check_swap(b,g->swap_id);
		}
		g->x+=g->dx;
		g->y+=g->dy;
		draw_gem_idle(g,g->x,g->y);
	}
	if (g->anim_state==SWAP_BACK) {
		g->swap_cnt--;//SDL_Delay(100);
		if (g->swap_cnt==0) {
			end_swap_back(b,g->swap_id);
		}
		g->x-=g->dx;
		g->y-=g->dy;
		draw_gem_idle(g,g->x,g->y);
	}
	if (g->backup_col!=NOCOL)
		draw_gem_off(g,x,y);
}
static void draw_curs(int x,int y) {
	SDL_Rect src={0,8*32,32,32},dst={x,y,32,32};
	
	SDL_BlitSurface(gem_surface,&src,buffer,&dst);
}

#define MAXSPEED 21
int fspeed[MAXSPEED]={0,0,0,0,0,0,0,0,0,0,0,0,1,1,1,1,2,2,3,5,8};
//int fspeed[MAXSPEED]={0,0,0,0,0,0,0,0,0,0,0,0,0,1,1,1,1,1,1,1,1};
void draw_gems(BOARD *b) {
	int i,j;
	GEMLIST *l,*p;
	GEM *g;
	int floor_y;
	int need_reduce=0;



	/*  Falling gem */
	for (i=b->w-1;i>=0;--i) {
		/* where is the floor? */
		j=get_floor_level(b,i);
		if (i&1) floor_y=8+j*32; else floor_y=24+j*32;

		/* draw the gem */
		for(l=b->fallbuf[i];l;l=l->next) {
			if (j==-1) printf("ASSERT ERROR: col %d floor is on top, and fall list not empty\n!",i);


			
			l->g->fall_y+=fspeed[l->g->fall_speed];
			if (l->g->fall_speed<MAXSPEED-1) l->g->fall_speed++;
			if (l->g->fall_y>floor_y) {
				/* The eagle is landing */
				g=getgem(b,i,j);
				if (g) {
					need_reduce=1;
					g->col=l->g->col;
					g->anim_state=IDLE;
					g->anim_cnt=0;
					g->to_del=0;


				} else 
				printf("ASSERT ERROR: get(%d,%d) return null\n",i,j);

				j=get_floor_level(b,i);
				if (i&1) floor_y=8+j*32; else floor_y=24+j*32;
				l->g->to_del=1;
			}
			if (l->g->to_del!=1) {
				if ((i&1)==1) 
					draw_gem(b,l->g,80+8+i*24,l->g->fall_y);
				else //if (j!=b->h-1)
					draw_gem(b,l->g,80+8+i*24,l->g->fall_y);
			}
		}
		/* delete */
		fallbuf_cleanup(b,i,0);
	}


	/* Draw gem */
	for (i=b->w-1;i>=0;--i) {
		for (j=0;j<(i&1?b->h:b->h-1);j++) {
			//printf("Draw %d,%d\n",i,j);
			if ((i&1)==1) 
				draw_gem(b,&b->gem[i+j*b->w],80+8+i*24,8+j*32);
			else //if (j!=b->h-1)
				draw_gem(b,&b->gem[i+j*b->w],80+8+i*24,24+j*32);

		}
	}

	/* Draw curs */
	if (b->cursx!=-1 && b->cursy!=-1) {
		if ((b->cursx&1)==1) 
			draw_curs(80+8+b->cursx*24,8+b->cursy*32);
		else if (b->cursy!=b->h-1)
			draw_curs(80+8+b->cursx*24,24+b->cursy*32);
	}

	/* Draw scores */
	for (i=0;i<MAXGEMCOL*2;i++) {
		if (b->scores[i][3]>0) {
			SDL_printf(font_big,buffer,b->scores[i][0]+8,b->scores[i][1]+8,"%d",b->scores[i][2]*10);
			b->scores[i][3]--;
		}
	}

	if (need_reduce) {
		reduce_board(b);
		play_sound(SID_FALL);
	}

}

void print_score(BOARD *b) {
	int i;
	SDL_Rect src={0,0,14,12},dst={346,28,14,12};
	for (i=0;i<b->maxcol;i++) {
		src.y=i*13;
		if (b->gcount[i]>=b->goal)
			src.x=15;
		else
			src.x=0;
		SDL_BlitSurface(little_gem_surface,&src,buffer,&dst);
		SDL_printf(font_small,buffer,362,30+i*13,"%*d",4,b->gcount[i]);
		dst.y+=13;
	}
	SDL_printf(font_small,buffer,355,210,"SCORE");
	SDL_printf(font_big,buffer,315,219,"%*d",10,b->score);

	SDL_printf(font_small,buffer,320,3,"LEVEL  %d",b->level);
	SDL_printf(font_small,buffer,320,12,"TARGET %d",b->goal);

	SDL_printf(font_small,buffer,346,142,"LUCKY");	
	dst.x=360;dst.y=152;
	src.x=0;src.y=b->lucky*13;
	SDL_BlitSurface(little_gem_surface,&src,buffer,&dst);

	if (b->timer>0) 
		SDL_printf(font_small,buffer,188,2,"%03d",b->timer/60);
	else
		SDL_printf(font_small,buffer,188,2,"XXX");

	if (b->combo>1) {
		SDL_printf(font_small,buffer,338,172,"CHAIN %d",b->combo);
	}
}

void draw_all (BOARD *b) {
	int i,j,y;
	SDL_Rect src={0,0,14,12},dst={346,28,14,12};
	draw_board();
	if (b->anim_state==LEVELUP) {
		SDL_printf(font_big,buffer,160,100,"NICE! GG!");
		//SDL_printf(font_small,buffer,160,100,"BONUS %d",);
		b->anim_cnt+=1;
		if (b->anim_cnt>120) {
			b->anim_state=INTRO;
			b->anim_cnt=0;
		}
	} else if (b->anim_state==INTRO) {
		SDL_printf(font_big,buffer,160,100,"GET READY!");
		SDL_printf(font_small,buffer,192,120,"GEM X2");
		dst.x=224;dst.y=130;
		src.x=0;src.y=b->lucky*13;
		SDL_BlitSurface(little_gem_surface,&src,buffer,&dst);

		b->anim_cnt+=1;
		if (b->anim_cnt>120) {
			b->anim_state=CREATE;
			b->anim_cnt=0;
		}
	} else if (b->anim_state==CREATE) {
		/* Draw gem */
		y=b->anim_cnt-240;
		for (i=b->w-1;i>=0;--i) {
			for (j=0;j<(i&1?b->h:b->h-1);j++) {
				//printf("Draw %d,%d\n",i,j);
				if ((i&1)==1) 
					draw_gem(b,&b->gem[i+j*b->w],80+8+i*24,y+8+j*32);
				else //if (j!=b->h-1)
					draw_gem(b,&b->gem[i+j*b->w],80+8+i*24,y+24+j*32);

			}
		}
		b->anim_cnt+=8;
		if (b->anim_cnt>240) {
			b->anim_state=IDLE;
			b->anim_cnt=0;
			b->block_input=0;
		}
	} else if (b->anim_state==DELALL) {
		for (i=b->w-1;i>=0;--i) {
			for (j=0;j<(i&1?b->h:b->h-1);j++) {
				//printf("Draw %d,%d\n",i,j);
				if ((i&1)==1) 
					draw_gem_alloff(&b->gem[i+j*b->w],80+8+i*24,8+j*32,b->anim_cnt);
				else //if (j!=b->h-1)
					draw_gem_alloff(&b->gem[i+j*b->w],80+8+i*24,24+j*32,b->anim_cnt);
			}
		}
		b->anim_cnt++;
		if (b->anim_cnt>=13) {
			b->anim_state=LEVELUP;
			b->anim_cnt=0;
			fill_board(b);
		}
	} else { //IDLE
		draw_gems(b);
	}
	draw_board_top();
	print_score(b);

	if (b->anim_state==GAMEOVER) {
		SDL_printf(font_big,buffer,160,100,"GAME OVER");
	}
}

void draw_scoreboard(int gametype) {
	char *diff_text[4]={"EASY","NORMAL","HARD","LEGEND"};
	int d=gametype&GM_DIFFICULTY;
	int i;
	SDL_BlitSurface(titlescreen,NULL,buffer,NULL);

	SDL_printf(font_big,buffer,30,60,"HIGHSCORE %s",diff_text[d]);
	for (i=0;i<MAXHISCORE;i++) {
		SDL_printf(font_big,buffer,30,80+i*20,"%s",highscore[gametype][i].name);
		SDL_printf(font_big,buffer,280,80+i*20,"%*d",10,highscore[gametype][i].score);
	}

}

/* converte screen space coord to gem coord and put the into gx,gy 
	put -1,-1 if input is not valid */

void screenspace2gemspace(BOARD *b,int sx,int sy,int *gx,int *gy) {
	int lb=80+12;
	int rb=80+226;
	int ub1=8;
	int ub0=24;
	int x,y;

	if (sx<lb || sx>rb) {
		*gx=-1; *gy=-1; return;
	}
	x=(sx-lb)/24;
	if ((x&1)==0) {
		if (sy<ub0 || sy>(ub0+32*6) ) {
			*gx=-1; *gy=-1; return;
		}
		y=(sy-ub0)/32;
	} else {
		if (sy<ub1 || sy>(ub1+32*7) ) {
			*gx=-1; *gy=-1; return;
		}
		y=(sy-ub1)/32;
	}
	*gx=x;*gy=y;
}

int g2s_x(int x) {
	return 80+8+x*24;
}
int g2s_y(int x,int y) {
	if (x&0x1)
		return 8+y*32;
	else
		return 24+y*32;
}

static int bandeau_x=640;
static char *bandeau="MADE FOR THE DRAGONBOX COMPETITION 2013 ... CODE AND GFX BY PEPONE ... MUSIC BY FOXSYNERGY ... FONT BY SPICYPIXEL ...";

int draw_intro(void) {
	SDL_BlitSurface(dragonbox,NULL,buffer,NULL);
}
int draw_main_menu(void) {
	SDL_BlitSurface(titlescreen,NULL,buffer,NULL);
}
// void draw_scoreboard(int d) {
// 	int i;
// 	SDL_BlitSurface(titlescreen,NULL,buffer,NULL);	
// 	switch(d) {
// 		case 
// 	}
// }
// int draw_high_score(int gamemode) {

// }
void draw_button(SDL_Rect *r,char *text) {
	int l=strlen(text)*8;
	SDL_BlitSurface(button,NULL,buffer,r);
	SDL_printf(font_big,buffer,200-l/2,r->y+(r->h-16)/2,text);
}
void flip_screen(void) {
#ifdef PANDORA
	SDL_BlitSurface(buffer,NULL,screen,NULL);
	SDL_Flip(screen);
#else
	SDL_SoftStretch(buffer,NULL,screen,NULL);
	SDL_UpdateRect(screen,0,0,screen->w,screen->h);
		frame_skip();//SDL_Delay(13);
#endif
	}