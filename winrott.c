/* Routines from winrott needed for the highres support for the SDL port */
#include <stdlib.h>
#include <string.h>
#include "WinRott.h"
#include "modexlib.h"

//typedef unsigned char byte;

#ifdef C4ROTT
// c4: the original 320x200, a fifth of the work of 640x480; a config can
// still ask for more (ScreenWidth, ScreenHeight in config.rot)
int iGLOBAL_SCREENWIDTH  = 320;
int iGLOBAL_SCREENHEIGHT = 200;
#else
int iGLOBAL_SCREENWIDTH  = 640;//bna val 800
int iGLOBAL_SCREENHEIGHT = 480;//bna val 600
#endif
int iGLOBAL_SCREENBWIDE ;
int iG_SCREENWIDTH;// default screen width in bytes

int iGLOBAL_HEALTH_X;
int iGLOBAL_HEALTH_Y;
int iGLOBAL_AMMO_X;
int iGLOBAL_AMMO_Y;

int iGLOBAL_FOCALWIDTH;
double dGLOBAL_FPFOCALWIDTH;

double dTopYZANGLELIMIT;

int iG_X_center;
int iG_Y_center;

boolean iG_aimCross = 0;

extern int  viewheight;
extern int  viewwidth;

//----------------------------------------------------------------------
#define FINEANGLES                        2048

#ifdef C4ROTT
// c4: the port's sizes, as Duke3D's video modes (games/duke3d's
// c4/duke-display.c): the RESOLUTION parameter, a config's ScreenWidth and
// ScreenHeight and the menu (rt_menu.c) take these and no others
static int rott_sizes[][2] = {
	{320, 200}, {320, 240}, {400, 300}, {512, 384},
	{640, 400}, {640, 480}, {800, 600}
};
int rott_size_ok (int w, int h)
{
	int i;
	for (i = 0; i < 7; i++)
		if (rott_sizes[i][0] == w && rott_sizes[i][1] == h)
			return 1;
	return 0;
}

// The weapon's height when a config has none (G_weaponscale, rt_view.c):
// the original's 168, 640x480's 299 and 800x600's 376, and between them
// as 320x200's on a 16:10 screen and 640x480's on a 4:3 one
int rott_default_weaponscale (void)
{
	int w = iGLOBAL_SCREENWIDTH, h = iGLOBAL_SCREENHEIGHT;
	if (w == 320) return 168;
	if (w == 640 && h == 480) return 299;
	if (w == 800) return 376;
	if (h * 8 == w * 5) return 168 * w / 320;
	return 299 * w / 640;
}
#endif
void SetRottScreenRes (int Width, int Height) 
{
	
	iGLOBAL_SCREENWIDTH = Width;
	iGLOBAL_SCREENHEIGHT = Height;


    iGLOBAL_SCREENBWIDE = iGLOBAL_SCREENWIDTH*(96/320);
	iG_SCREENWIDTH = iGLOBAL_SCREENWIDTH*(96/320);;// default screen width in bytes

	if (iGLOBAL_SCREENWIDTH == 320) {
		iGLOBAL_FOCALWIDTH = 160;
		dGLOBAL_FPFOCALWIDTH = 160.0; 
		iGLOBAL_HEALTH_X = 20;
		iGLOBAL_HEALTH_Y = 185;
		iGLOBAL_AMMO_X = 300;
		iGLOBAL_AMMO_Y = 184;

		dTopYZANGLELIMIT = (44*FINEANGLES/360);;
	}
	if (iGLOBAL_SCREENWIDTH == 640) {
		iGLOBAL_FOCALWIDTH = 180;
		dGLOBAL_FPFOCALWIDTH = 180.0 ;
		iGLOBAL_HEALTH_X = 40;//20*2;
		iGLOBAL_HEALTH_Y = 466;//(185*2)+16;
		iGLOBAL_AMMO_X = 600;//300*2;
		iGLOBAL_AMMO_Y = 464;//480-16;

		dTopYZANGLELIMIT = (42*FINEANGLES/360);;
	}
	if (iGLOBAL_SCREENWIDTH == 800) {
		iGLOBAL_FOCALWIDTH = 200;
		dGLOBAL_FPFOCALWIDTH = 200.0 ;
		iGLOBAL_HEALTH_X = 40;//20*2;
		iGLOBAL_HEALTH_Y = 585;//(185/200)*600;
		iGLOBAL_AMMO_X = 750;//(300/320)*800;
		iGLOBAL_AMMO_Y = 584;//600-16;
	
		dTopYZANGLELIMIT = (90*FINEANGLES/360);;
	}
#ifdef C4ROTT
	// c4: the sizes between (WinRott.h). The view's height for the 4:3
	// display: 160 at 320x200 and 640x400 (each 16:10 on it), 192 at the
	// square-pixelled ones (640x480 and 800x600 have 180 and 200 of
	// their own). The bars' numbers at the bottom, as 640x480's.
	if (!ROTT_OWN) {
		iGLOBAL_FOCALWIDTH = (Height * 256) / Width;
		dGLOBAL_FPFOCALWIDTH = iGLOBAL_FOCALWIDTH;
		iGLOBAL_HEALTH_X = (Width > 320) ? 40 : 20;
		iGLOBAL_HEALTH_Y = Height - 15;
		iGLOBAL_AMMO_X = (Width > 320) ? Width - 40 : 300;
		iGLOBAL_AMMO_Y = Height - 16;
		dTopYZANGLELIMIT = (42*FINEANGLES/360);
	}
#endif

	//dYZANGLELIMIT = (12*FINEANGLES/360);
	//#define YZANGLELIMIT  (12*FINEANGLES/360)//bna--(30*FINEANGLES/360)

	//#define TopYZANGLELIMIT  (44*FINEANGLES/360)//bna added

//	GetCurrentDirectory(sizeof(ApogeePath),ApogeePath);// curent directory name

}

//----------------------------------------------------------------------
//luckey for me that I am not programmin a 386 or the next
//4 function would never have worked. bna++
extern int     viewsize;
void MoveScreenUpLeft()
{
	int startX,startY,startoffset;
    byte  *Ycnt,*b;
//   SetTextMode (  );
    b=(byte *)bufferofs;
	b += (((iGLOBAL_SCREENHEIGHT-viewheight)/2)*iGLOBAL_SCREENWIDTH)+((iGLOBAL_SCREENWIDTH-viewwidth)/2);
	if (viewsize == 8) {b += 8*iGLOBAL_SCREENWIDTH;}
	startX = 3; //take 3 pixels to the right
	startY = 3; //take 3 lines down
	startoffset = (startY*iGLOBAL_SCREENWIDTH)+startX;

	for (Ycnt=b;Ycnt<b+((viewheight-startY)*iGLOBAL_SCREENWIDTH);Ycnt+=iGLOBAL_SCREENWIDTH){
		memcpy(Ycnt,Ycnt+startoffset, viewwidth-startX);
	}
}
//----------------------------------------------------------------------
void MoveScreenDownLeft()
{
	int startX,startY,startoffset;
    byte  *Ycnt,*b;
//   SetTextMode (  );
    b=(byte *)bufferofs;
	b += (((iGLOBAL_SCREENHEIGHT-viewheight)/2)*iGLOBAL_SCREENWIDTH)+((iGLOBAL_SCREENWIDTH-viewwidth)/2);
	if (viewsize == 8) {b += 8*iGLOBAL_SCREENWIDTH;}
	startX = 3; //take 3 pixels to the right
	startY = 3; //take 3 lines down
	startoffset = (startY*iGLOBAL_SCREENWIDTH);//+startX;

	//Ycnt starts in botton of screen and copys lines upwards
	for (Ycnt=b+((viewheight-startY-1)*iGLOBAL_SCREENWIDTH);Ycnt>b;Ycnt-=iGLOBAL_SCREENWIDTH){
		memcpy(Ycnt+startoffset,Ycnt+startX,viewwidth-startX);
	}
}
//----------------------------------------------------------------------
void MoveScreenUpRight()
{
	int startX,startY,startoffset;
    byte  *Ycnt,*b;
//   SetTextMode (  );
    b=(byte *)bufferofs;

	b += (((iGLOBAL_SCREENHEIGHT-viewheight)/2)*iGLOBAL_SCREENWIDTH)+((iGLOBAL_SCREENWIDTH-viewwidth)/2);
	if (viewsize == 8) {b += 8*iGLOBAL_SCREENWIDTH;}
	startX = 3; //take 3 pixels to the right
	startY = 3; //take 3 lines down
	startoffset = (startY*iGLOBAL_SCREENWIDTH);//+startX;

	for (Ycnt=b;Ycnt<b+((viewheight-startY)*iGLOBAL_SCREENWIDTH);Ycnt+=iGLOBAL_SCREENWIDTH){
		memcpy(Ycnt+startX,Ycnt+startoffset, viewwidth-startX);
	}
}
//----------------------------------------------------------------------
void MoveScreenDownRight()
{
	int startX,startY,startoffset;
    byte  *Ycnt,*b;
//   SetTextMode (  );
    b=(byte *)bufferofs;

	b += (((iGLOBAL_SCREENHEIGHT-viewheight)/2)*iGLOBAL_SCREENWIDTH)+((iGLOBAL_SCREENWIDTH-viewwidth)/2);
	if (viewsize == 8) {b += 8*iGLOBAL_SCREENWIDTH;}
	startX = 3; //take 3 pixels to the right
	startY = 3; //take 3 lines down
	startoffset = (startY*iGLOBAL_SCREENWIDTH)+startX;

	//Ycnt starts in botton of screen and copys lines upwards
	for (Ycnt=b+((viewheight-startY-1)*iGLOBAL_SCREENWIDTH);Ycnt>b;Ycnt-=iGLOBAL_SCREENWIDTH){
		memcpy(Ycnt+startoffset,Ycnt,viewwidth-startX);
	}
}


