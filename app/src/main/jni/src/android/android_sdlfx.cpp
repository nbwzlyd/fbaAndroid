// Software blitter effects via SDL
#include "burner.h"
#include "vid_support.h"
#include "vid_softfx.h"

static int nInitedSubsytems = 0;
static int nGameWidth = 0, nGameHeight = 0;			// screen size
static int nSize;
static int nUseBlitter;
static int nUseSys;
static int nDirectAccess = 1;
static int nRotateGame = 0;
SDL_Surface* sdlFramebuf = NULL;
//SDL_Surface* sdlFramebufblend = NULL;

SDL_Window *window;
SDL_Renderer *renderer;
SDL_Texture *texture;
extern Uint8* SDL_expand_byte[9] ;

#define RGB_FROM_RGB565(Pixel, r, g, b)                                 \
    {                                                                   \
    r = SDL_expand_byte[3][((Pixel&0xF800)>>11)];                       \
    g = SDL_expand_byte[2][((Pixel&0x07E0)>>5)];                        \
    b = SDL_expand_byte[3][(Pixel&0x001F)];                             \
	}

static int BlitFXExit()
{
	VidSFreeVidImage();
	nRotateGame = 0;
	return 0;
}

static int BlitFXInit()
{
	if (nRotateGame & 1) {
		nVidImageWidth = nGameHeight;
		nVidImageHeight = nGameWidth;
	} else {
		nVidImageWidth = nGameWidth;
		nVidImageHeight = nGameHeight;
	}

	if (nUseBlitter >= 7 && nUseBlitter <= 9) {
		nVidImageDepth = 16;								// Use 565 format
	} else { 
		nVidImageDepth = sdlFramebuf->format->BitsPerPixel;// Use color depth of primary surface
	}
	nVidImageBPP = sdlFramebuf->format->BytesPerPixel;
	nBurnBpp = nVidImageBPP;								// Set Burn library Bytes per pixel
	printf("BlitFXInit : nVidImageDepth=%d,nVidImageWidth =%d,nVidImageHeight=%d,nBurnBpp= %d",
			nVidImageDepth,nVidImageWidth,nVidImageHeight,nBurnBpp);
	// Use our callback to get colors:
	SetBurnHighCol(nVidImageDepth);

	// Make the normal memory buffer
	if (VidSAllocVidImage()) {
		BlitFXExit();
		return 1;
	}

	return 0;
}

static int Exit()
{
	BlitFXExit();

	if (!(nInitedSubsytems & SDL_INIT_VIDEO)) {
		SDL_QuitSubSystem(SDL_INIT_VIDEO);
	}
	nInitedSubsytems = 0;
	SDL_Quit();
	return 0;
}


int len;
Uint16 *pPixels;
Uint16 gm_red;
Uint16 tmp;



static int Init()
{	
	nInitedSubsytems = SDL_WasInit(SDL_INIT_VIDEO);

	if (!(nInitedSubsytems & SDL_INIT_VIDEO)) {
		SDL_Init(SDL_INIT_EVERYTHING);
	
		if (SDL_CreateWindowAndRenderer(0, 0, 0, &window, &renderer) < 0) {
	  		printf("SDL_CreateWindowAndRenderer() failed: %s", SDL_GetError());
	  		return(2);
		}
	}

	nUseBlitter = 0;
	nGameWidth = nVidImageWidth; nGameHeight = nVidImageHeight;
	nRotateGame = 0;
	fbaprintf( "----vid_sdlfx.Init: nGame: %ix%i", nGameWidth, nGameHeight );

	if (bDrvOkay) {
		// Get the game screen size
		BurnDrvGetVisibleSize(&nGameWidth, &nGameHeight);

		fbaprintf( "vid_sdlfx.Init: nGame: %ix%i", nGameWidth, nGameHeight );

	    if (BurnDrvGetFlags() & BDF_ORIENTATION_VERTICAL) {
			if (nVidRotationAdjust & 1) {
				int n = nGameWidth;
				nGameWidth = nGameHeight;
				nGameHeight = n;
				nRotateGame |= (nVidRotationAdjust & 2);
			} else {
				nRotateGame |= 1;
			}
		}

		if (BurnDrvGetFlags() & BDF_ORIENTATION_FLIPPED) {
			nRotateGame ^= 2;
		}
	}

	nSize = VidSoftFXGetZoom(nUseBlitter);
	bVidScanlines = 0;								// !!!
			if ((sdlFramebuf = SDL_CreateRGBSurface(0, nGameWidth*nSize, nGameHeight*nSize, nVidDepth,
#if SDL_BYTEORDER == SDL_LIL_ENDIAN     /* OpenGL RGBA masks */
													 0x00000000,
													 0x00000000, 0x00000000, 0x00000000
#else
													 0x00000000,
													 0x00000000, 0x00000000, 0x00000000
#endif
							))==NULL)
					{
							
						fbaprintf(_T("*** Couldn't enter fullscreen mode.\n"));
						return 1;
			
					}
	len=sdlFramebuf->pitch*sdlFramebuf->h/2;
	pPixels=(Uint16 *)sdlFramebuf->pixels;
	texture=SDL_CreateTexture(renderer,sdlFramebuf->format->format,SDL_TEXTUREACCESS_STREAMING,nGameWidth*nSize,nGameHeight*nSize);
	//SDL_SetColorKey(sdlFramebuf,SDL_TRUE,0xff);
	SDL_RenderSetScale(renderer,0.5f,0.5f);
	fbaprintf("android_sdlfx.Init: SDL_CreateTexture( %i, %i )", nGameWidth * nSize, nGameHeight * nSize );

	fbaprintf("android_sdlfx.Init: sdlFramebuf PixelFormat = %s",
				SDL_GetPixelFormatName(
						SDL_MasksToPixelFormatEnum(
								sdlFramebuf->format->BitsPerPixel,
								sdlFramebuf->format->Rmask,
								sdlFramebuf->format->Gmask,
								sdlFramebuf->format->Bmask,
								sdlFramebuf->format->Amask) ) );

	fbaprintf("android_sdlfx.Init: ( %i, %u, %u, %u, %u )",
			sdlFramebuf->format->BitsPerPixel,
			sdlFramebuf->format->Rmask,
			sdlFramebuf->format->Gmask,
			sdlFramebuf->format->Bmask,
			sdlFramebuf->format->Amask );

	SDL_SetClipRect(sdlFramebuf, NULL);
	SDL_SetWindowSize(window,nGameWidth * nSize, nGameHeight * nSize);
	
	// Initialize the buffer surfaces
	BlitFXInit();

	if (VidSoftFXInit(nUseBlitter, nRotateGame)) {
		if (VidSoftFXInit(0, nRotateGame)) {
		Exit();
			return 1;
		}
	}

	printf( "vid_sdlfx.Init: nRotateGame=%i", nRotateGame );

	return 0;
}

static int vidScale(RECT* , int x, int y)
{
	SDL_RenderSetScale(renderer,x,y);
	return 0;
}
#define RGB565_FROM_RGB(Pixel, r, g, b)                                 \
{                                                                       \
    Pixel = ((r>>3)<<11)|((g>>2)<<5)|(b>>3);                            \
}

static int MemToSurf()
{
  	int i=0;
	/*for(;i<len;i++)
	{
		tmp =pPixels[i];
		gm_red= SDL_expand_byte[3][((pPixels[i]&0xF800)>>11)];  //RGB_FROM_RGB565(pPixels[i],gm_red,gm_green,gm_blue);
		RGB565_FROM_RGB(pPixels[i],gm_red,gm_red,gm_red);

	}*/
	
	SDL_UpdateTexture(texture,NULL,sdlFramebuf->pixels,sdlFramebuf->pitch);
	SDL_RenderCopy(renderer, texture, NULL, NULL);
	SDL_RenderPresent(renderer);
	return 0;
}

// Run one frame and render the screen
static int Frame(bool bRedraw)						// bRedraw = 0
{
	//if (pVidImage == NULL) {
	if( sdlFramebuf == NULL ) {
		printf("sdlFramebuf==NULL");
		return 1;
	}
	// fbaprintf("Frame");
	if (bDrvOkay) {
		if (bRedraw) {								// Redraw current frame
			if (BurnDrvRedraw()) {
				BurnDrvFrame();						// No redraw function provided, advance one frame
			}
		} else {
			BurnDrvFrame();							// Run one frame and draw the screen
		}
	}
	else
	{
		printf("bDrvOkay != TRUE");
		return 1;
	}

	MemToSurf();									// Copy the memory buffer to the directdraw buffer for later blitting
	return 0;
}

// Paint the BlitFX surface onto the primary surface
static int Paint(int bValidate)
{
	return 0;
}

static int GetSettings(InterfaceInfo* pInfo)
{
	TCHAR szString[MAX_PATH] = _T("");

	_sntprintf(szString, MAX_PATH, _T("Prescaling using %s (%i� zoom)"), VidSoftFXGetEffect(nUseBlitter), nSize);
	IntInfoAddStringModule(pInfo, szString);

	if (nRotateGame) {
		IntInfoAddStringModule(pInfo, _T("Using software rotation"));
	}

	return 0;
}

// The Video Output plugin:
struct VidOut VidOutSDLFX = { Init, Exit, Frame, Paint, vidScale, GetSettings, _T("SDL Software Effects video output") };
