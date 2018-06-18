import SDL_mixer;
import sound;

enum SOUNDID {
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
};

static Mix_Chunk* mc_swap,mc_fall,mc_wrong,
mc_combo1,mc_combo2,mc_combo3,mc_combo4,mc_gameover;
static Mix_Music* music1,music2,music3;

void init_sound() {
	/* Same setup as before */
	int audio_rate = 44100;
	Uint16 audio_format = AUDIO_S16; 
	int audio_channels = 2;
	int audio_buffers = 1024;
	if(Mix_OpenAudio(audio_rate, audio_format, audio_channels, audio_buffers)) {
		printf("Unable to open audio!\n");
		return;
	}
}
void load_sound() {
	mc_swap = Mix_LoadWAV("assets/swap.wav");
	mc_wrong = Mix_LoadWAV("assets/badmove.wav");
	mc_fall = Mix_LoadWAV("assets/fall.wav");
	mc_combo1 = Mix_LoadWAV("assets/combo1.wav");
	mc_combo2 = Mix_LoadWAV("assets/combo2.wav");
	mc_combo3 = Mix_LoadWAV("assets/combo3.wav");
	music1 = Mix_LoadMUS("assets/Grey_Sector_v0_85.mp3");
	music2 = Mix_LoadMUS("assets/Rusted_Soul.mp3");
	music3 = Mix_LoadMUS("assets/The_Time_You_Spend_v1_1.mp3");
	mc_gameover = Mix_LoadWAV("assets/gameover.wav");

}
void play_sound(SOUNDID id) {
	switch(id) {
	case SID_SWAP:
		Mix_PlayChannel(-1, mc_swap, 0);
		break;
	case SID_FALL:
		Mix_PlayChannel(-1, mc_fall, 0);
		break;
	case SID_WRONG:
		Mix_PlayChannel(-1, mc_wrong, 0);
		break;
	case SID_COMBO1:
		Mix_PlayChannel(-1, mc_combo1, 0);
		break;
	case SID_COMBO2:
		Mix_PlayChannel(-1, mc_combo2, 0);
		break;
	case SID_COMBO3:
		Mix_PlayChannel(-1, mc_combo3, 0);
		break;
	case SID_GAMEOVER:
		Mix_PlayChannel(-1, mc_gameover, 0);
		break;
		default:
		break;
	}
}
void play_music(SOUNDID id) {
	stop_music();
	switch(id) {
	case SID_MUSIC1:
		Mix_PlayMusic(music1, -1);
		break;
	case SID_MUSIC2:
		Mix_PlayMusic(music2, -1);
		break;
	case SID_MUSIC3:
		Mix_PlayMusic(music3, -1);
		break;
		default:
		break;
	}
}
void fadout_music(int msec) {
	Mix_FadeOutMusic(msec);
}
void stop_music() {
	Mix_RewindMusic();
	Mix_HaltMusic();
}
