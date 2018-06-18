module SDL_mixer;

import SDL;

/* The internal format for an audio chunk */
struct Mix_Chunk {
	int allocated;
	Uint8 *abuf;
	Uint32 alen;
	Uint8 volume;		/* Per-sample volume, 0-128 */
}

struct Mix_Music {}