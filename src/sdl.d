module SDL;

alias ubyte Uint8;
alias ushort Uint16;
alias uint Uint32;
alias short Sint16;


/* The structure passed to the low level blit functions */
struct SDL_BlitInfo{
	Uint8 *s_pixels;
	int s_width;
	int s_height;
	int s_skip;
	Uint8 *d_pixels;
	int d_width;
	int d_height;
	int d_skip;
	void *aux_data;
	SDL_PixelFormat *src;
	Uint8 *table;
	SDL_PixelFormat *dst;
};

/* The type definition for the low level blit functions */
alias  extern(C) void  function(SDL_BlitInfo *info) SDL_loblit;

alias extern(C) int function(SDL_Surface *src, SDL_Rect *srcrect,
			SDL_Surface *dst, SDL_Rect *dstrect) SDL_blit;

/* This is the private info structure for software accelerated blits */
struct private_swaccel {
	SDL_loblit blit;
	void *aux_data;
}

struct private_hwaccel;
//struct private_swaccel;

/* Blit mapping definition */
struct SDL_BlitMap {
	SDL_Surface *dst;
	int identity;
	Uint8 *table;
	SDL_blit hw_blit;
	SDL_blit sw_blit;
	private_hwaccel *hw_data;
	private_swaccel *sw_data;

	/* the version count matches the destination; mismatch indicates
	   an invalid mapping */
        uint format_version;
}


struct SDL_Rect {
	Sint16 x, y;
	Uint16 w, h;
};

struct SDL_Color {
	Uint8 r;
	Uint8 g;
	Uint8 b;
	Uint8 unused;
}
alias SDL_Color SDL_Colour ;

struct SDL_Palette {
	int       ncolors;
	SDL_Color *colors;
}

struct SDL_PixelFormat {
	SDL_Palette *palette;
	Uint8  BitsPerPixel;
	Uint8  BytesPerPixel;
	Uint8  Rloss;
	Uint8  Gloss;
	Uint8  Bloss;
	Uint8  Aloss;
	Uint8  Rshift;
	Uint8  Gshift;
	Uint8  Bshift;
	Uint8  Ashift;
	Uint32 Rmask;
	Uint32 Gmask;
	Uint32 Bmask;
	Uint32 Amask;

	/** RGB color key information */
	Uint32 colorkey;
	/** Alpha value information (per-surface alpha) */
	Uint8  alpha;
}

struct private_hwdata {}

struct SDL_Surface {
	Uint32 flags;				/**< Read-only */
	SDL_PixelFormat *format;		/**< Read-only */
	int w, h;				/**< Read-only */
	Uint16 pitch;				/**< Read-only */
	void *pixels;				/**< Read-write */
	int offset;				/**< Private */

	/** Hardware-specific surface info */
	private_hwdata *hwdata;

	/** clipping information */
	SDL_Rect clip_rect;			/**< Read-only */
	Uint32 unused1;				/**< for binary compatibility */

	/** Allow recursive locks */
	Uint32 locked;				/**< Private */

	/** info for fast blit mapping to other surfaces */
	SDL_BlitMap *map;		/**< Private */

	/** format version, bumped at every change to invalidate blit maps */
	uint format_version;		/**< Private */

	/** Reference count -- used when freeing surface */
	int refcount;				/**< Read-mostly */
}

struct SDL_Cursor {}

extern(C) {
}