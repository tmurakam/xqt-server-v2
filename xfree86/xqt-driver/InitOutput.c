#include	"xqt.h"

#include <stdarg.h>

unsigned long	g_ulServerGeneration;
int g_iScreenPrivateIndex;
int g_iGCPrivateIndex;
int g_iPixmapPrivateIndex;
int g_iWindowPrivateIndex;

Bool g_fRootless = FALSE;
Bool g_fMultiWindow = FALSE;
Bool g_fFullScreen = TRUE;

static PixmapFormatRec g_PixmapFormats[] = {
	/* depth - bitsPerPixel - scanlinePad */
	{1,	1,	BITMAP_SCANLINE_PAD },
	{4,	8,	BITMAP_SCANLINE_PAD },
	{8,	8,	BITMAP_SCANLINE_PAD },
	{15,	16,	BITMAP_SCANLINE_PAD },
	{16,	16,	BITMAP_SCANLINE_PAD },
	{24,	32,	BITMAP_SCANLINE_PAD },
	{32,	32,	BITMAP_SCANLINE_PAD },
};

#define	NUMFORMATS	(sizeof(g_PixmapFormats) / sizeof(g_PixmapFormats[0]))

void
InitOutput(ScreenInfo *screenInfo, int argc, char **argv)
{
	int i;

	monitorResolution = 75;	// DPI ###

	/* Setup global screen info parameters */
	screenInfo->imageByteOrder = IMAGE_BYTE_ORDER;
	screenInfo->bitmapScanlinePad = BITMAP_SCANLINE_PAD;
	screenInfo->bitmapScanlineUnit = BITMAP_SCANLINE_UNIT;
	screenInfo->bitmapBitOrder = BITMAP_BIT_ORDER;
	screenInfo->numPixmapFormats = NUMFORMATS;


	/* Describe how we want common pixmap formats padded */
	for (i = 0; i < NUMFORMATS; i++)
	{
		screenInfo->formats[i] = g_PixmapFormats[i];
	}

	/* supports only one screen */
	if (AddScreen(qtScreenInit, argc, argv) == -1) {
		FatalError("InitOutput - Couldn't add screen");
	}
}

/*
 * コマンドライン処理
 */
int	qtArgc = 1;
char	*qtArgv[64];

int
ddxProcessArgument(int argc, char *argv[], int i)
{
	/*
	 * Qt のコマンドライン引数を読み飛ばす
	 */
	const struct {
		const char *optname;
		int num;
	} qtopts[] = {
		{"-qws", 1},
		{"-qcop", 2},
		{"-display", 2},
		{"-geometry", 2},
		{"-name", 2},
		{"-title", 2},
		{"-preload", 1},
		{"-preload-show", 1}
	};
	int j, k;

	/* check options */
	for (j = 0; j < sizeof(qtopts)/sizeof(qtopts[0]); j++) {
		if (strcmp(argv[i], qtopts[j].optname) == 0) {
			/* save options */
			for (k = 0; k < qtopts[j].num; k++) {
				qtArgv[qtArgc++] = argv[i+k];
			}

			return qtopts[j].num;
		}
	}

	/* -rootless */
	if (strcmp(argv[i], "-rootless") == 0) {
		g_fRootless = TRUE;
		return 1;
	}

	/* -disable-fullscreen */
	if (strcmp(argv[i], "-disable-fullscreen") == 0) {
		g_fFullScreen = FALSE;
		return 1;
	}

	return 0;
}

void
OsVendorFatalError(void)
{
	qtShowFatalError("Fatal error");
}

static void osVendorVError(const char *arg, va_list args)
{
	char errmsg[256];
	vsnprintf(errmsg, sizeof(errmsg), arg, args);

	fprintf(stderr, errmsg);
}

void
OsVendorInit(void)
{
	OsVendorVErrorFProc = osVendorVError;

	/* get argv[0] */
	qtArgv[0] = argvGlobal[0];

#if 0
	for (i = 0; i < qtArgc; i++) {
		fprintf(stderr, "argv[%d] = '%s'\n", i, qtArgv[i]);
	}
#endif

	/* startup Qt application */
	qtStartAp(qtArgc, qtArgv);
}

void
ddxUseMsg(void)
{
  ErrorF ("-disable-fullscreen    Disable full screen mode.\n");
  ErrorF ("-rootless\n"
	  "\tEXPERIMENTAL: Run the server in pseudo-rootless mode.\n");
}

void
ddxGiveUp()
{
	AbortDDX();
}

void
AbortDDX(void)
{
	exit(1);	/* ### */
}

#ifdef DDXTIME /* from ServerOSDefines */
CARD32
GetTimeInMillis()
{
    struct timeval  tp;

    X_GETTIMEOFDAY(&tp);
    return(tp.tv_sec * 1000) + (tp.tv_usec / 1000);
}
#endif
