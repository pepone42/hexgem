#ifndef H_FRAME_SKIP
#define H_FRAME_SKIP

extern char skip_next_frame;

void reset_frame_skip(void);
void init_frame_skip(void);
int frame_skip(void);


#endif

