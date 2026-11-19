

// winrott.h


//husk at rette i winrott.c

extern int iGLOBAL_SCREENWIDTH;//bna val 800
extern int iGLOBAL_SCREENHEIGHT;//bna val 600
extern int iGLOBAL_SCREENBWIDE ;
extern int iG_SCREENWIDTH;// default screen width in bytes

extern int iGLOBAL_HEALTH_X;
extern int iGLOBAL_HEALTH_Y;
extern int iGLOBAL_AMMO_X;
extern int iGLOBAL_AMMO_Y;

extern int iGLOBAL_FOCALWIDTH;
extern double dGLOBAL_FPFOCALWIDTH;

void EnableScreenStretch(void);
void DisableScreenStretch(void);

#ifndef ROTT_IS
#ifdef C4ROTT
// c4: the port's sizes (Extended User Options' RESOLUTION, rt_menu.c):
// the original 320x200 and icculus.org's 640x480 and 800x600, which much
// of the code has numbers for, one by one, and Duke3D's between them
// (320x240, 400x300, 512x384, 640x400), which take numbers worked out
// from the size (winrott.c, rt_view.c, rt_draw.c, rt_game.c, ...)
#define ROTT_IS(w, h)  (iGLOBAL_SCREENWIDTH == (w) && iGLOBAL_SCREENHEIGHT == (h))
#define ROTT_OWN       (ROTT_IS(320, 200) || ROTT_IS(640, 480) || ROTT_IS(800, 600))
int rott_size_ok (int w, int h);
int rott_default_weaponscale (void);
#else
#define ROTT_IS(w, h)  (iGLOBAL_SCREENWIDTH == (w))
#endif
#endif

/*
double dYZANGLELIMIT;

#define FOCALWIDTH 160//160
#define FPFOCALWIDTH 160.0//160.0



#define MAXSCREENHEIGHT    480//600//     200*2
#define MAXSCREENWIDTH     640//800//     320*2
#define SCREENBWIDE        640*(96/320)//800*(96/320)//     96*2
#define MAXVIEWWIDTH       640//     320*2
#define SCREENWIDTH        640*(96/320)//800*(96/320)//     96*2              // default screen width in bytes


#define MAXSCREENHEIGHT    600
#define MAXSCREENWIDTH     800
#define SCREENBWIDE        800*(96/320)
#define MAXVIEWWIDTH       800
#define SCREENWIDTH        800*(96/320)// default screen width in bytes

*/
//#define VIEWWIDTH               MAXSCREENWIDTH//320*2             // size of view window
//#define VIEWHEIGHT              MAXSCREENHEIGHT//200*2
//#define MAXSCANLINES            MAXSCREENHEIGHT//200*2             // size of ylookup table







