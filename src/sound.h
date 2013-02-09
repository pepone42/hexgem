#ifndef H_SOUND_
#define H_SOUND_

typedef enum SOUNDID {
	SID_SWAP,
	SID_FALL,
	SID_WRONG,
	SID_COMBO1,
	SID_COMBO2,
	SID_COMBO3,
	SID_COMBO4,
	SID_GAMEOVER,

	SID_MUSIC1,
	SID_MUSIC2,
	SID_MUSIC3
}SOUNDID;

void init_sound(void);
void load_sound(void);
void play_music(SOUNDID id);
void play_sound(SOUNDID id);
void stop_music(void);
void fadout_music(int msec);

#endif