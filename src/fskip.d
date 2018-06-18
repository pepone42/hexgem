/*  hexgem, a hexagonal puzzle game
 *  Copyright (C) 2013 Peponas Mathieu
 */
module fskip;

import core.stdc.stdint;
import core.stdc.time;
import core.stdc.math;

import SDL;
import hexgem;
import fskip;


// #ifndef uclock_t
// #define uclock_t Uint32
// #endif

alias uint uclock_t ;

enum TICKS_PER_SEC = 1000000UL;
static uclock_t F;

enum MAX_FRAMESKIP = 10;

byte skip_next_frame = 0;

// #if defined(HAVE_GETTIMEOFDAY) && !defined(WII)
// static int CPU_FPS = 60;
// static struct timeval init_tv = { 0, 0 };
// #else
/* Looks like SDL_GetTicks is not as precise... */
static int CPU_FPS=61;
static Uint32 init_tv=0;
// #endif
uclock_t bench;

// #if defined(HAVE_GETTIMEOFDAY) && !defined(WII)
// uclock_t get_ticks(void) {
// 	struct timeval tv;

// 	gettimeofday(&tv, 0);
// 	if (init_tv.tv_sec == 0)
// 		init_tv = tv;
// 	return (tv.tv_sec - init_tv.tv_sec) * TICKS_PER_SEC + tv.tv_usec
// 			- init_tv.tv_usec;

// }
// #else
Uint32 get_ticks()
{
	Uint32 tv;
	if (init_tv==0)
	init_tv=SDL_GetTicks();
	return (SDL_GetTicks()-init_tv)*1000;
}
// #endif

void reset_frame_skip() {
// #if defined(HAVE_GETTIMEOFDAY) && !defined(WII)
// 	init_tv.tv_usec = 0;
// 	init_tv.tv_sec = 0;
// #else
	init_tv=0;
//#endif
	skip_next_frame = 0;

	F = cast(uclock_t) (cast(double) TICKS_PER_SEC / CPU_FPS);
}

static uclock_t target;
static int nbFrame = 0;
static uclock_t sec = 0;

void init_frame_skip() {
		target = get_ticks();
		nbFrame = 0;
		sec = 0;
}

int frame_skip() {
	static int f2skip;
	static uclock_t rfd;


	static uint nbFrame_moy = 0;
	static int skpFrm = 0;


	target += F;
	if (f2skip > 0) {
		f2skip--;
		skpFrm++;
		return 1;
	} else
		skpFrm = 0;
//	printf("%d %d\n",conf.autoframeskip,conf.show_fps);

	rfd = get_ticks();

	if (1/*conf.autoframeskip*/) {
		if (rfd < target && f2skip == 0)
			while (get_ticks() < target) {
				version (Windows) {
				// if (conf.sleep_idle) {
				 	usleep(5);
				// }
				}
			}
		// else {
		// 	f2skip = (rfd - target) / (double) F;
		// 	if (f2skip > MAX_FRAMESKIP) {
		// 		f2skip = MAX_FRAMESKIP;
		// 		reset_frame_skip();
		// 	}
			// printf("Skip %d frame(s) %lu %lu\n",f2skip,target,rfd);
		//}
	}

	nbFrame++;
	nbFrame_moy++;

//if (conf.show_fps) {
		if (get_ticks() - sec >= TICKS_PER_SEC) {
			//printf("%d\n",nbFrame);

			nbFrame = 0;
			sec = get_ticks();
		}
//	}

	return 0;
}