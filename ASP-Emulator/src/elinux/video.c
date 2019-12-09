#include "emumain.h"
#include "SDL/SDL.h"

#define YOFFSET 16
#define XOFFSET 16
unsigned short COLOR555TO565[65536] = {0};
extern UINT16 *gVideoBuff16;

void neo_init_color_table(void);
void neo_game_w_h_cb(int,int);
UINT32 poll_gamepad(void);

SDL_Surface *screen;
int gGameW = 320;
int gGameH = 240;

void init_screen(int w,int h){
	
	unsigned int color;
	if(SDL_Init(SDL_INIT_VIDEO)<0){
		fprintf(stderr,"SDL_Init error: %s\n",SDL_GetError());
		exit(1);
	}
	screen = SDL_SetVideoMode(w,h,16,SDL_SWSURFACE);
	if(screen == NULL){
		fprintf(stderr,"couldn't set %dx%d 16 bits color model, error is %s\n",w,h,SDL_GetError());
		exit(1);
	}
	
	atexit(SDL_Quit);
}
void update_screen(int skipframe)
{
	if(!skipframe){
		unsigned short *src = (unsigned short*)gVideoBuff16; 
		unsigned short *dst = (unsigned short*)screen->pixels;
		unsigned short lSrcPix = 0;
		int w,h;
		src += YOFFSET*NEOVIDEOBUFF_W+XOFFSET+(304 == gGameW ? 8 : 0);
		for(h=0;h<gGameH;h++) 
		{
			for(w=0;w<gGameW;w++)
			{
				lSrcPix = src[w];
				dst[w] = COLOR555TO565[lSrcPix];
			}
			dst+=gGameW;
			src+=NEOVIDEOBUFF_W;
		}

		SDL_Flip(screen);
	}
}
void neo_game_w_h_cb(int gamew,int gameh){
	gGameW = gamew;
	gGameH = gameh;
	
	neo_init_color_table();
	
	init_screen(gamew,gameh);

	printf("+++ gamew = %d gameh = %d +++\n",gGameW,gGameH);
}
void neo_init_color_table(void){

	unsigned short r,g,b;
	int i;
	//for(i=0;i<0x7FFF;i++){
	for(i=0;i<0xFFFF;i++){

		int color = i & 0x7FFF;

		b = (color >> 10) & 0x1F;
		g = (color >> 5) & 0x1F;
		r = color & 0x1F;

		g = (g << 1) + (g >> 4);

		COLOR555TO565[i] = (r<<11)|(g<<5)|b;
	}
}
UINT32 poll_gamepad(void){
	
	UINT32 lKeys = 0;

	//Event handler
	SDL_Event event;
	//Handle events on queue
	while( SDL_PollEvent( &event ) != 0 )
	{
		//User requests quit
		if( event.type == SDL_QUIT )
		{
			neogeo_emulator_exit();
		}
	}

	return lKeys;
}
