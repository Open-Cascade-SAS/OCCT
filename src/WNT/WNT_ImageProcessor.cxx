#define TEST	// EPV_160499
//		Avoid do retrieve properly an XWD image with
//		NT 4.0 Service pack 4

#define IMP080200	//GG
//		Enable to evaluate correctly the fileneme extension
//		according to the CSF_DefaultImageFormat symbol.

#define BUC60837	// GG 07/03/01
//		Enable to read a GIF image file even a if the file contains
//		wrong informations.

#define STRICT
#include <windows.h>
#include <windowsx.h>
#include <stdio.h>

#ifdef DrawText
# undef DrawText
#endif  /* DrawText */

#include <Aspect_XWD.hxx>

#include <WNT.h>
#include <Quantity_Color.hxx>
#include <WNT_GraphicDevice.hxx>
#include <WNT_TypeOfImage.hxx>
#include <Image_PixMap.hxx>

//***//
#define I_SUCCESS Standard_True
#define I_ERROR   Standard_False

#define PIXEL4   4
#define PIXEL8   8
#define PIXEL16 16
#define PIXEL24 24
#define PIXEL32 32

#define MAXCOLOR   256
#define PAD(a)     (   ( a ) % sizeof ( LONG ) ?                          \
                   sizeof ( LONG ) - (  ( a ) % sizeof ( LONG )  ) : 0  )
#ifdef _DEBUG
# define MALLOC(s) calloc (  ( s ), 1  )
# define FREE(p)   free   (  ( p )     )
#else
# define MALLOC(s)  HeapAlloc (  hHeap, HEAP_ZERO_MEMORY | HEAP_GENERATE_EXCEPTIONS, ( s )  )
# define FREE(p)    HeapFree  (  hHeap, 0, ( p )  )
#endif  /* _DEBUG */
#define WINNT35X() (WNT_osVer.dwPlatformId   == VER_PLATFORM_WIN32_NT && \
                    WNT_osVer.dwMajorVersion == 3 )
//***//
typedef struct {

		 BYTE pixel[ 3 ];

        } TRIPLET, *PTRIPLET;

class _init {

 public:

   _init ();
  ~_init ();

};  // end _init
//***//
OSVERSIONINFO WNT_osVer;

HWINSTA ( WINAPI *NTOpenWindowStation       ) ( LPTSTR, BOOL, DWORD        );
BOOL    ( WINAPI *NTSetProcessWindowStation ) ( HWINSTA                    );
HDESK   ( WINAPI *NTOpenDesktop             ) ( LPTSTR, DWORD, BOOL, DWORD );
BOOL    ( WINAPI *NTSetThreadDesktop        ) ( HDESK                      );
BOOL    ( WINAPI *NTCloseDesktop            ) ( HDESK                      );
BOOL    ( WINAPI *NTCloseWindowStation      ) ( HWINSTA                    );
//***//
static DWORD           dwFileSize;
static LPVOID          lpvFile;
static HDC             hDC;
#ifdef TEST
static BOOL            fWindow=FALSE;
#endif
static WNT_TypeOfImage imgType;
static HANDLE          hHeap;
static _init           init;
//***//
static HBITMAP loadXWD (  Handle( WNT_GraphicDevice )&  );
static HBITMAP loadBMP (  Handle( WNT_GraphicDevice )&  );
static HBITMAP loadGIF (  Handle( WNT_GraphicDevice )&  );
//***//
static void            __fastcall _swaplong   ( char*, unsigned );
static void            __fastcall _swapshort  ( char*, unsigned );
static int             __fastcall _getshift   ( unsigned long   );
static WNT_TypeOfImage __fastcall _image_type ( LPTSTR, BOOL    );
//***//
static void __fastcall _alloc_colors (
                        PVOID, int, int, int, LPRGBQUAD, Handle( WNT_GraphicDevice )&
                       );
//***//
int SaveBitmapToFile (
     Handle( WNT_GraphicDevice )& , HBITMAP , char*, int, int, int, int
	);
//***//
HBITMAP LoadImageFromFile (
         Handle( WNT_GraphicDevice )& gDev, char* fileName, HDC hDevCtx
	    ) {

 HANDLE   hFile, hFileMap = NULL;
 HBITMAP  retVal = NULL;
 DWORD    dwProtect, dwAccess;

 if ( WNT_osVer.dwPlatformId == VER_PLATFORM_WIN32_WINDOWS ) {

  dwProtect = PAGE_WRITECOPY;
  dwAccess  = GENERIC_READ | GENERIC_WRITE;

 } else {

  dwProtect = PAGE_READONLY;
  dwAccess  = GENERIC_READ;

 }  // end else

 hDC = ( hDevCtx != NULL ) ? hDevCtx : GetDC ( NULL );

#ifdef IMP080200	// Check extension
 WNT_TypeOfImage iType   = imgType;
 TCHAR           drv[ _MAX_DRIVE ];
 TCHAR           dir[ _MAX_DIR   ];
 TCHAR           fnm[ _MAX_FNAME ];
 TCHAR           ext[ _MAX_EXT   ];
 TCHAR           ifl[ _MAX_PATH  ];

 _splitpath ( fileName, drv, dir, fnm, ext );

 if (  ext[ 0 ] == TEXT( '\x00' )  ) {
   ext[ 0 ] = TEXT( '.'    );
   ext[ 1 ] = TEXT( '\x00' );
 }  // end if

 iType = _image_type ( &ext[ 1 ], FALSE );

 _makepath ( ifl, drv, dir, fnm, ext );
#endif

 __try {

  hFile = CreateFile (
#ifdef IMP080200
	ifl, dwAccess, FILE_SHARE_READ, NULL,
#else
	fileName, dwAccess, FILE_SHARE_READ, NULL,
#endif
           OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL
		  );

  if ( hFile == INVALID_HANDLE_VALUE ) __leave;

  dwFileSize = GetFileSize ( hFile, NULL );

  if ( dwFileSize == 0xFFFFFFFF ) __leave;

  hFileMap = CreateFileMapping (
              hFile, NULL, dwProtect, 0, dwFileSize, NULL
             );

  if ( hFileMap == NULL ) __leave;

  lpvFile = MapViewOfFile ( hFileMap, FILE_MAP_COPY, 0, 0, 0 );

  if ( lpvFile == NULL ) __leave;

  if (   memcmp (  ( const void* )lpvFile, ( const void* )"BM", 2  ) == 0   )

   retVal = loadBMP ( gDev );

  else if (   memcmp (  ( const void* )lpvFile, ( const void* )"GIF87a", 6  ) == 0 ||
              memcmp (  ( const void* )lpvFile, ( const void* )"GIF89a", 6  ) == 0
       )

   retVal = loadGIF ( gDev );

  else  // assume XWD file

   retVal = loadXWD ( gDev );

 }  // end __try

 __finally {

  if ( lpvFile  != NULL ) {

   UnmapViewOfFile ( lpvFile );
   lpvFile = NULL;

  }  // end if

  if ( hFileMap != NULL                 ) CloseHandle ( hFileMap );
  if ( hFile    != INVALID_HANDLE_VALUE ) CloseHandle ( hFile    );

 }  // end __finally

 if ( hDevCtx == NULL ) ReleaseDC ( NULL, hDC );

 return retVal;

}  // end LoadImageFromFile

int __WNT_API SaveWindowToFile (
     Handle( WNT_GraphicDevice )& gDev,
     HWND hWnd, char* fName, int x, int y, int w, int h
    ) {

 int      retVal = I_ERROR;
 HDC      hDCmem;
 HBITMAP  hBmp = NULL, hOldBmp;
 HPALETTE hOldPal;

 __try {

  hDC = GetDC ( hWnd );
#ifdef TEST
  fWindow = TRUE;
#endif

  if (  gDev -> IsPaletteDevice ()  ) {

   hOldPal = SelectPalette (  hDC, ( HPALETTE )(  gDev -> HPalette ()  ), FALSE  );
   RealizePalette ( hDC );

  }  // end if

  hDCmem = CreateCompatibleDC ( hDC );

  hBmp = CreateCompatibleBitmap ( hDC, w, h );

  if ( hBmp == NULL ) __leave;

  hOldBmp = SelectBitmap ( hDCmem, hBmp );

   BitBlt ( hDCmem, 0, 0, w, h, hDC, x, y, SRCCOPY );

  SelectBitmap ( hDCmem, hOldBmp );

  retVal = SaveBitmapToFile ( gDev, hBmp, fName, 0, 0, w, h );

 }  // end __try

 __finally {

#ifdef TEST
  fWindow = FALSE;
#endif

  if ( hBmp != NULL ) DeleteObject ( hBmp );

  if (  gDev -> IsPaletteDevice ()  )

   SelectPalette ( hDC, hOldPal, FALSE );

  DeleteDC ( hDCmem );
  ReleaseDC ( hWnd, hDC );

 }  // end __finally

 return retVal;

}  // end SaveWindowToFile

int DumpBitmapToFile (Handle(WNT_GraphicDevice)& , HDC ,
                      HBITMAP theHBitmap,
                      char* theFileName)
{
  // Copy data from HBITMAP
  BITMAP aBitmap;

  // Get informations about the bitmap
  GetObject (theHBitmap, sizeof(BITMAP), (LPSTR )&aBitmap);
  Standard_Integer aWidth  = aBitmap.bmWidth;
  Standard_Integer aHeight = aBitmap.bmHeight;

  // Setup image data
  BITMAPINFOHEADER aBitmapInfo;
  memset (&aBitmapInfo, 0, sizeof(BITMAPINFOHEADER));
  aBitmapInfo.biSize = sizeof(BITMAPINFOHEADER);
  aBitmapInfo.biWidth = aWidth;
  aBitmapInfo.biHeight = aHeight; // positive means bottom-up!
  aBitmapInfo.biPlanes = 1;
  aBitmapInfo.biBitCount = 32;
  aBitmapInfo.biCompression = BI_RGB;

  Standard_Integer aBytesPerLine = aWidth * 4;
  Standard_Byte* aDataPtr = new Standard_Byte[aBytesPerLine * aHeight];

  // Copy the pixels
  HDC aDC = GetDC (NULL);
  Standard_Boolean isSuccess
    = GetDIBits (aDC,             // handle to DC
                 theHBitmap,      // handle to bitmap
                 0,               // first scan line to set
                 aHeight,         // number of scan lines to copy
                 aDataPtr,        // array for bitmap bits
                 (LPBITMAPINFO )&aBitmapInfo, // bitmap data info
                 DIB_RGB_COLORS   // RGB
                 ) != 0;

  if (isSuccess)
  {
    Handle(Image_PixMap) anImagePixMap = new Image_PixMap (aDataPtr,
                                                           aWidth, aHeight,
                                                           aBytesPerLine,
                                                           aBitmapInfo.biBitCount,
                                                           Standard_False); // bottom-up!

    // Release dump memory here
    delete[] aDataPtr;

    // save the image
    anImagePixMap->Dump (theFileName);
  }
  else
  {
    // Release dump memory
    delete[] aDataPtr;
  }
  ReleaseDC (NULL, aDC);
  return isSuccess ? I_SUCCESS : I_ERROR;
}

//***//
int SaveBitmapToFile (Handle(WNT_GraphicDevice)& gDev,
                      HBITMAP hBmp, char* fName,
                      int x, int y, int w, int h)
{
  int      retVal  = I_ERROR;
  HBITMAP  hNewBmp = NULL;
  HPALETTE hOldPal;
  BOOL     newBmp  = FALSE, newDC = FALSE;

  __try {

  #ifdef TEST
    if (!fWindow)
  #else
    if (y != 0 || x != 0)
  #endif
    {
      HBITMAP hOldBmp;

      newDC = TRUE;
      hDC = GetDC (NULL);

      if (gDev->IsPaletteDevice())
      {
        hOldPal = SelectPalette (hDC, (HPALETTE)(gDev->HPalette()), FALSE);
        RealizePalette (hDC);
      }  // end if

      HDC hDCmemSrc = CreateCompatibleDC (hDC);
      HDC hDCmemDst = CreateCompatibleDC (hDC);

      hNewBmp = CreateCompatibleBitmap (hDC, w, h);
      if (hNewBmp != NULL)
      {
        hOldBmp = SelectBitmap (hDCmemDst, hNewBmp);
        SelectBitmap (hDCmemSrc, hBmp);
        BitBlt (hDCmemDst, 0, 0, w, h, hDCmemSrc, x, y, SRCCOPY);
        newBmp = TRUE;
        SelectBitmap (hDCmemDst, hOldBmp);
      }  // end if

      DeleteDC (hDCmemDst);
      DeleteDC (hDCmemSrc);

      if (gDev->IsPaletteDevice())
      {
        SelectPalette (hDC, hOldPal, FALSE);
      }
      if (hNewBmp == NULL) __leave;
    }
    else
    {
      hNewBmp = hBmp;
    }

    retVal = DumpBitmapToFile (gDev, NULL,
                               hNewBmp, fName);
  }  // end __try
  __finally {
    if (hNewBmp != NULL && newBmp) DeleteObject (hNewBmp);
    if (newDC) ReleaseDC (NULL, hDC);
  }  // end __finally
  return retVal;
}  // end SaveBitmapToFile
//***//
void SetOutputFormat ( WNT_TypeOfImage type ) {

 imgType = type;

}  // end SetOutputFormat
//***//
//**************** Routines to process XWD file ***************************//
//***//
static HBITMAP loadXWD (  Handle( WNT_GraphicDevice )& gDev  ) {

 UINT             i, j, k;
 UINT             red_shift, green_shift, blue_shift;
 HBITMAP          retVal = NULL;
 PBITMAPINFO      pBmi   = NULL;
 PBYTE            pbInit;
 UINT             nBytes, bitmapSize;
 WORD             bitCount;
 HPALETTE         hOldPal;
 LONG             lPixel;
 WORD             wPixel;
 BYTE             bPixel;
 BOOL             newMem = FALSE;
 PBYTE            ptrDIB, ptrXWD;
 UINT             incDIB, incXWD;
 XWDFileHeader*   xwdHdr;
 XColor*          xColor;
 Standard_Integer r, g, b;
 WORD             colors[ MAXCOLOR ];
 LPVOID           imageData;
 DWORD            dataSize;

 __try {

  xwdHdr = ( XWDFileHeader* )lpvFile;

  _swaplong (  ( char* )xwdHdr, sizeof ( XWDFileHeader )  );

  if ( xwdHdr -> file_version  != XWD_FILE_VERSION ||
       xwdHdr -> pixmap_format != ZPixmap          ||
       xwdHdr -> header_size   <  sizeof ( XWDFileHeader )
  ) __leave;

  xColor = ( XColor* )(  ( char* )lpvFile + xwdHdr -> header_size  );

  for ( i = 0; i < xwdHdr -> ncolors; ++i ) {

   _swaplong  (  ( char* )&xColor[ i ].pixel, sizeof ( unsigned long  )  );
   _swapshort (  ( char* )&xColor[ i ].red,   sizeof ( unsigned short ) * 3  );

  }  // end for

  imageData = ( LPVOID )( xColor + xwdHdr -> ncolors );
  imageData = ( LPVOID )(  ( char* )imageData + xwdHdr -> xoffset  );
  dataSize  = ( DWORD )(  ( char* )lpvFile + dwFileSize - ( char* )imageData  );

  nBytes = sizeof ( BITMAPINFO );

  switch ( xwdHdr -> visual_class ) {

   case StaticColor:
   case PseudoColor:

    if ( xwdHdr -> bits_per_pixel < 12 ) {

	 bitCount = ( xwdHdr -> bits_per_pixel <= 4 ) ? 4 : 8;
#ifdef TEST
	 nBytes  += sizeof ( RGBQUAD ) * ( 1 << bitCount );
#else
	 nBytes  += sizeof ( RGBQUAD ) * xwdHdr -> ncolors;
#endif
	} else if ( xwdHdr -> bits_per_pixel == 16 )

	 bitCount = 16;

	else

	 __leave;

    break;

   case TrueColor:
   case DirectColor:

     red_shift   = _getshift ( xwdHdr -> red_mask   );
     green_shift = _getshift ( xwdHdr -> green_mask );
     blue_shift  = _getshift ( xwdHdr -> blue_mask  );

   	 if ( xwdHdr -> bits_per_pixel < 24 )

	  bitCount = 16;

	 else if ( xwdHdr -> bits_per_pixel == 24 )

	  bitCount = 24;

	 else if ( xwdHdr -> bits_per_pixel == 32 )

	  bitCount = 32;

	 else

	  __leave;

    break;

  }  // end switch

  pBmi = ( PBITMAPINFO )MALLOC( nBytes );

  if ( pBmi == NULL ) __leave;

  pBmi -> bmiHeader.biSize          = sizeof ( BITMAPINFOHEADER );
  pBmi -> bmiHeader.biWidth	        = xwdHdr -> pixmap_width;
  pBmi -> bmiHeader.biHeight        = -( int )xwdHdr -> pixmap_height;
  pBmi -> bmiHeader.biPlanes        = 1;
  pBmi -> bmiHeader.biBitCount      = bitCount;
  pBmi -> bmiHeader.biCompression   = BI_RGB;

  ZeroMemory (  ( PVOID )&colors, sizeof ( colors )  );

  switch ( xwdHdr -> visual_class ) {

   case StaticColor:
   case PseudoColor:

    if ( bitCount != 16 ) {

     for ( i = 0; i < xwdHdr -> ncolors; ++i ) {

      pBmi -> bmiColors[ i ].rgbBlue  = ( xColor[ i ].blue  >> 8 );
      pBmi -> bmiColors[ i ].rgbGreen = ( xColor[ i ].green >> 8 );
      pBmi -> bmiColors[ i ].rgbRed   = ( xColor[ i ].red   >> 8 );

     }  // end for

     incXWD = xwdHdr -> bytes_per_line;
     incDIB = incXWD + PAD( incXWD );

     if ( xwdHdr -> bytes_per_line == incDIB )

	  pbInit = ( PBYTE )imageData;

	 else {

	  bitmapSize = xwdHdr -> pixmap_height * incDIB;

	  pbInit = ( PBYTE )MALLOC( bitmapSize );

	  if ( pbInit == NULL ) __leave;

	  newMem = TRUE;

	  ptrDIB = ( PBYTE )pbInit;
	  ptrXWD = ( PBYTE )imageData;

	  for ( i = 0; i < xwdHdr -> pixmap_height; ++i ) {

       CopyMemory ( ptrDIB, ptrXWD, incXWD );

	   ptrDIB += incDIB;
	   ptrXWD += incXWD;

	  }  // end for ( i . . . )

	 }  // end else

     ptrDIB = ( PBYTE )MALLOC( dataSize );

	 if ( ptrDIB == NULL ) __leave;

	 CopyMemory (  ( PVOID )ptrDIB, ( PVOID )pbInit, dataSize  );
	 FillMemory (  ( PVOID )colors, MAXCOLOR * sizeof ( WORD ), 0xFFFF );

	 for ( i = 0; i < dataSize; ++i ) {

	  if ( colors[  ptrDIB[ i ]  ] != 0xFFFF ) continue;

	  for ( j = 0; j < xwdHdr -> ncolors; ++j )

	   if ( xColor[ j ].pixel == pbInit[ i ] )

	    break;

	  for ( k = 0; k < dataSize; ++k )

	   if ( ptrDIB[ k ] == xColor[ j ].pixel )

	    pbInit[ k ] = j;

	  colors[  ptrDIB[ i ]  ] = j;

	 }  // end for

	  if (  gDev -> IsPaletteDevice ()  ) {

	   for ( i = 0; i < MAXCOLOR; ++i ) {

        if ( colors[ i ] == 0xFFFF ) continue;

	    b = ( Standard_Integer )( xColor[  colors[ i ]  ].blue  >> 8 );
	    g = ( Standard_Integer )( xColor[  colors[ i ]  ].green >> 8 );
	    r = ( Standard_Integer )( xColor[  colors[ i ]  ].red   >> 8 );

	    gDev -> SetColor ( r, g, b );

       }  // end for

	  }  // end if

	 FREE( ptrDIB );

    } else {  // bitCount == 16

     WORD red, green, blue;

	 incXWD = xwdHdr -> bytes_per_line;
	 incDIB = xwdHdr -> pixmap_width * sizeof ( WORD );
	 incDIB += PAD( incDIB );

     _swapshort (  ( char* )imageData, dataSize  );

     if ( xwdHdr -> bytes_per_line == incDIB )

	  pbInit = ( PBYTE )imageData;

	 else {

	  bitmapSize = xwdHdr -> pixmap_height * incDIB;

	  pbInit = ( PBYTE )MALLOC( bitmapSize );

	  if ( pbInit == NULL ) __leave;

	  newMem = TRUE;

	  ptrDIB = ( PBYTE )pbInit;
	  ptrXWD = ( PBYTE )imageData;

	  for ( i = 0; i < xwdHdr -> pixmap_height; ++i ) {

       CopyMemory ( ptrDIB, ptrXWD, incXWD );

	   ptrDIB += incDIB;
	   ptrXWD += incXWD;

	  }  // end for ( i . . . )

	 }  // end else

     ptrDIB = pbInit;
	 dataSize = xwdHdr -> pixmap_height * incDIB;

	 for ( i = 0; i < xwdHdr -> pixmap_height; ++i ) {

	  for ( j = 0; j < xwdHdr -> pixmap_width; ++j ) {

	   wPixel = (  ( PWORD )ptrDIB  )[ j ];
	   red   = ( xColor[ wPixel ].red   >> 11 ) << 10;
	   green = ( xColor[ wPixel ].green >> 11 ) <<  5;
	   blue  = ( xColor[ wPixel ].blue  >> 11 );
	   wPixel = red | green | blue;
	   (  ( PWORD )ptrDIB  )[ j ] = wPixel;

	  }  // end for ( j . . . )

	  ptrDIB += incDIB;

     }  // end for ( i . . . )

AllocColors_16:

	 if (  gDev -> IsPaletteDevice ()  )

	  _alloc_colors (
	   pbInit, xwdHdr -> pixmap_width, xwdHdr -> pixmap_height, PIXEL16, NULL, gDev
	  );

    }  // end else

   break;

   case TrueColor:
   case DirectColor:

    switch ( bitCount ) {

     case 16:

	  _swapshort (  ( char* )imageData, dataSize  );

	  incXWD = xwdHdr -> bytes_per_line;
	  incDIB = xwdHdr -> pixmap_width * sizeof ( WORD );
	  incDIB += PAD( incDIB );
	  bitmapSize = incDIB * xwdHdr -> pixmap_height;

	  pbInit = ( PBYTE )MALLOC( bitmapSize );

	  if ( pbInit == NULL ) __leave;

	  newMem = TRUE;

	  ptrDIB = ( PBYTE )pbInit;
	  ptrXWD = ( PBYTE )imageData;

      switch ( xwdHdr -> bits_per_pixel ) {

       case 8:

        for ( i = 0; i < xwdHdr -> pixmap_height; ++i ) {

         for ( j = 0; j < xwdHdr -> pixmap_width; ++j ) {

          bPixel = ptrXWD[ j ];
		  b = ( xColor[ ( bPixel & xwdHdr -> blue_mask  ) >> blue_shift  ].blue  ) >> 11;
		  g = ( xColor[ ( bPixel & xwdHdr -> green_mask ) >> green_shift ].green ) >> 11;
		  r = ( xColor[ ( bPixel & xwdHdr -> red_mask   ) >> red_shift   ].red   ) >> 11;
		  (  ( PWORD )ptrDIB  )[ j ] = ( WORD )(  b | ( g << 5 ) | ( r << 10 )  );

         }  // end for ( j . . . )

		 ptrDIB += incDIB;
		 ptrXWD += incXWD;

        }  // end for ( i . . . )

       break;

       case 16:

        for ( i = 0; i < xwdHdr -> pixmap_height; ++i ) {

         for ( j = 0; j < xwdHdr -> pixmap_width; ++j ) {

          wPixel = (  ( PWORD )ptrXWD  )[ j ];
		  b = ( xColor[ ( wPixel & xwdHdr -> blue_mask  ) >> blue_shift  ].blue  ) >> 11;
		  g = ( xColor[ ( wPixel & xwdHdr -> green_mask ) >> green_shift ].green ) >> 11;
		  r = ( xColor[ ( wPixel & xwdHdr -> red_mask   ) >> red_shift   ].red   ) >> 11;
		  (  ( PWORD )ptrDIB  )[ j ] = ( WORD )(  b | ( g << 5 ) | ( r << 10 )  );

         }  // end for ( j . . . )

		 ptrDIB += incDIB;
		 ptrXWD += incXWD;

        }  // end for ( i . . . )

	   break;

	   default:

	    __leave;

      }  // end switch ( xwdHdr -> bits_per_pixel . . . )

      goto AllocColors_16;   // not attractive but efficient

	 break;

     case 24:

      __leave;

     case 32:

      _swaplong (  ( char* )imageData, dataSize  );

	  pbInit = ( PBYTE )imageData;
	  bitmapSize = xwdHdr -> pixmap_height *
	               xwdHdr -> pixmap_width * sizeof ( LONG );

      if ( xwdHdr -> ncolors != 0 )

	   for (  i = j = 0; j < bitmapSize; i += sizeof ( LONG )  ) {

        lPixel = ( LONG )*(   ( PLONG )(  ( PBYTE )imageData + j  )   );

	    pbInit[ i ]     =
	     ( BYTE )( xColor[ ( lPixel & xwdHdr -> blue_mask  ) >> blue_shift  ].blue  );
	    pbInit[ i + 1 ] =
	     ( BYTE )( xColor[ ( lPixel & xwdHdr -> green_mask ) >> green_shift ].green );
	    pbInit[ i + 2 ] =
	     ( BYTE )( xColor[ ( lPixel & xwdHdr -> red_mask   ) >> red_shift   ].red   );
	    pbInit[ i + 3 ] = 0;

	    j += sizeof ( LONG );

	   }  // end for

	  else

	   for (  i = j = 0; j < bitmapSize; i += sizeof ( LONG )  ) {

        lPixel = ( LONG )*(   ( PLONG )(  ( PBYTE )imageData + j  )   );

	    pbInit[ i ]     =
	     ( BYTE )(  ( lPixel & xwdHdr -> blue_mask  ) >> blue_shift  );
	    pbInit[ i + 1 ] =
	     ( BYTE )(  ( lPixel & xwdHdr -> green_mask ) >> green_shift );
	    pbInit[ i + 2 ] =
	     ( BYTE )(  ( lPixel & xwdHdr -> red_mask   ) >> red_shift   );
	    pbInit[ i + 3 ] = 0;

	    j += sizeof ( LONG );

	   }  // end for

      if (  gDev -> IsPaletteDevice ()  )

	   _alloc_colors (
	    pbInit, xwdHdr -> pixmap_width, xwdHdr -> pixmap_height, PIXEL32, NULL, gDev
	   );

    }  // end switch ( bitCount . . . )

  }  // end switch ( xwdHdr -> visual_class . . . )

  if (  gDev -> IsPaletteDevice ()  ) {

   hOldPal = SelectPalette (  hDC, ( HPALETTE )(  gDev -> HPalette ()  ), FALSE  );

   if (  RealizePalette ( hDC )  )

    UpdateColors ( hDC );

  }  // end if

   retVal = CreateDIBitmap (
             hDC, &pBmi -> bmiHeader, CBM_INIT, pbInit, pBmi, DIB_RGB_COLORS
 	 	    );

  if (  gDev -> IsPaletteDevice ()  )

   SelectPalette ( hDC, hOldPal, FALSE );

 }  // end try

 __finally {

  if ( pbInit != NULL && newMem ) FREE( pbInit );
  if ( pBmi   != NULL           ) FREE( pBmi   );

 }  // end finally

 return retVal;

}  // end readXWD

static void __fastcall _swaplong ( char* bp, unsigned n ) {

 char  c;
 char* ep = bp + n;
 char* sp;

 while ( bp < ep ) {

  sp    = bp + 3;
  c     = *sp;
  *sp   = *bp;
  *bp++ = c;
  sp    = bp + 1;
  c     = *sp;
  *sp   = *bp;
  *bp++ = c;
  bp    += 2;

 }  // end while

}  // end _swaplong

static void __fastcall _swapshort ( char* bp, unsigned n ) {

 char  c;
 char* ep = bp + n;

 while ( bp < ep ) {

  c     = *bp;
  *bp++ = *( bp + 1 );
  *bp++ = c;

 }  // end while

}  // end _swapshort

static int __fastcall _getshift ( unsigned long mask ) {

 int retVal = 0;

 while (  !( mask & 1 )  ) {

  ++retVal;
  mask >>= 1;

 }  // end while

 return retVal;

}  // end _getshift

//**************** Routines to process BMP file ***************************//
//***//
static HBITMAP loadBMP (  Handle( WNT_GraphicDevice )& gDev  ) {

 HBITMAP           retVal = NULL;
 PBITMAPFILEHEADER pBmfh;
 PBITMAPINFOHEADER pBmih;
 LPRGBQUAD         pRGB;
 PBYTE             pData;
 HPALETTE          hOldPal;
 WORD              bitCount;
 UINT              nColors;
 DWORD             dwWidth, dwHeight;
 BOOL              os2Flag;

 __try {

  pBmfh = ( PBITMAPFILEHEADER )lpvFile;
  pBmih = ( PBITMAPINFOHEADER )(  ( PBYTE )lpvFile + sizeof ( BITMAPFILEHEADER )  );

  if (  pBmih -> biSize == sizeof ( BITMAPCOREHEADER )  ) {  // OS/2 bitmap

   PBITMAPCOREHEADER pBmch = ( PBITMAPCOREHEADER )pBmih;

   bitCount = pBmch -> bcBitCount;
   nColors = ( bitCount < 16 ) ? ( 1 << bitCount ) : 0;

   pBmih = ( PBITMAPINFOHEADER )MALLOC(
                                 sizeof ( BITMAPINFO ) +
						         sizeof ( RGBQUAD ) * nColors
					            );

   if ( pBmih == NULL ) __leave;

   pBmih -> biSize        = sizeof ( BITMAPINFOHEADER );
   pBmih -> biWidth       = pBmch -> bcWidth;
   pBmih -> biHeight      = pBmch -> bcHeight;
   pBmih -> biPlanes      = 1;
   pBmih -> biBitCount    = bitCount;
   pBmih -> biCompression = BI_RGB;

   for ( UINT i = 0; i < nColors; ++i ) {

   	(  ( PBITMAPINFO )pBmih  ) -> bmiColors[ i ].rgbRed =
   	 (  ( PBITMAPCOREINFO )pBmch  ) -> bmciColors[ i ].rgbtRed;
   	(  ( PBITMAPINFO )pBmih  ) -> bmiColors[ i ].rgbGreen =
   	 (  ( PBITMAPCOREINFO )pBmch  ) -> bmciColors[ i ].rgbtGreen;
   	(  ( PBITMAPINFO )pBmih  ) -> bmiColors[ i ].rgbBlue =
   	 (  ( PBITMAPCOREINFO )pBmch  ) -> bmciColors[ i ].rgbtBlue;

   }  // end for

   pRGB     = (  ( PBITMAPINFO )pBmih  ) -> bmiColors;
   os2Flag  = TRUE;

  } else {  // Windows DIB

   pRGB     = ( LPRGBQUAD )(  ( PBYTE )pBmih + pBmih -> biSize  );
   os2Flag  = FALSE;

  }  // end else

  bitCount = pBmih -> biBitCount;
  dwWidth  = pBmih -> biWidth;
  dwHeight = pBmih -> biHeight;
  pData    = ( PBYTE )(  ( PBYTE )lpvFile + pBmfh -> bfOffBits  );

  if (  gDev -> IsPaletteDevice ()  ) {

   _alloc_colors ( pData, dwWidth, dwHeight, bitCount, pRGB, gDev );

   hOldPal = SelectPalette (  hDC, ( HPALETTE )(  gDev -> HPalette ()  ), FALSE  );
   RealizePalette ( hDC );

  }  // end if

  retVal = CreateDIBitmap (
            hDC, pBmih, CBM_INIT, pData, ( PBITMAPINFO )pBmih, DIB_RGB_COLORS
 		   );

  if (  gDev -> IsPaletteDevice ()  )

   SelectPalette ( hDC, hOldPal, FALSE );

 }  // end __try

 __finally {

  if ( os2Flag && pBmih ) FREE( pBmih );

 }  // end __finally

  return retVal;

}  // end  loadBMP

//***//
//**************** Routines to process GIF file ***************************//
//***//
#define NEXT_BYTE      ( *ptr++ )
#define IMAGESEP       0x2C
#define INTERLACE_MASK 0x40
#define COLORMAP_MASK  0x80
#define COMMENT_BYTE   0x21

static UINT        x, y, pass, bytesPerLine, imgWidth, imgHeight;
static PBYTE       pData;
static PBITMAPINFO pBmi;
static BOOL        isInterlace;

static void __fastcall _add_pixel ( UINT );
//***//
static HBITMAP loadGIF (  Handle( WNT_GraphicDevice )& gDev  ) {

 int      i, nColors;
 HBITMAP  retVal = NULL;
 UINT     Bits, BitMask, CodeSize, ClearCode, EOFCode, FreeCode,
          InitCodeSize, MaxCode, ReadMask, FirstFree, OutCount, BitOffset,
          ByteOffset, Code, CurCode, OldCode, FinChar, InCode;
 PUINT    OutCode, Prefix, Suffix;
 BYTE     byte, byte1;
 PBYTE    rasterPtr, ptr1, ptr = (  ( PBYTE )lpvFile  ) + 10;
 BOOL     hasColormap;
 HPALETTE hOldPal;
 DWORD    dataSize;

#ifdef BUC60837
  pBmi = NULL;
  rasterPtr = pData = NULL;
  Suffix = Prefix = OutCode = NULL;
#endif

 __try {

#ifdef BUC60837
  OutCode = ( PUINT )MALLOC(  1026 * sizeof ( UINT )  );
#else
  pBmi      = NULL;
  rasterPtr = pData = NULL;
  OutCode = ( PUINT )MALLOC(  1025 * sizeof ( UINT )  );
#endif
  Prefix  = ( PUINT )MALLOC(  4096 * sizeof ( UINT )  );
  Suffix  = ( PUINT )MALLOC(  4096 * sizeof ( UINT )  );

  if ( OutCode == NULL || Prefix == NULL || Suffix == NULL ) __leave;

  byte = NEXT_BYTE;

  hasColormap = ( byte & COLORMAP_MASK ) ? TRUE : FALSE;
  Bits        = ( byte & 0x07 ) + 1;
  nColors     = 1 << Bits;
  BitMask     = nColors - 1;
  ++ptr;

  if ( NEXT_BYTE ) __leave;

  pBmi = ( PBITMAPINFO )MALLOC(
                         sizeof ( BITMAPINFO ) + sizeof ( RGBQUAD ) * 256
					    );

  if ( pBmi == NULL ) __leave;

  if ( hasColormap ) {

   for ( i = 0; i < nColors; ++i ) {

   	pBmi -> bmiColors[ i ].rgbRed   = NEXT_BYTE;
	pBmi -> bmiColors[ i ].rgbGreen = NEXT_BYTE;
	pBmi -> bmiColors[ i ].rgbBlue  = NEXT_BYTE;

   }  // end for

  }  // end if

  while ( *ptr == COMMENT_BYTE ) {

   ptr += 2;

   while ( *ptr ) ( ptr += *ptr )++;

   NEXT_BYTE;

  }  // end while

  if ( NEXT_BYTE != IMAGESEP ) __leave;

  rasterPtr = ( PBYTE )MALLOC( dwFileSize );

  if ( rasterPtr == NULL ) __leave;

  ptr += 4;

  byte      = NEXT_BYTE;
  imgWidth  = byte + 0x100 * NEXT_BYTE;
  byte      = NEXT_BYTE;
  imgHeight	= byte + 0x100 * NEXT_BYTE;

  isInterlace = ( NEXT_BYTE & INTERLACE_MASK ) ? TRUE : FALSE;

  CodeSize  = NEXT_BYTE;
  ClearCode = 1 << CodeSize;
  EOFCode   = ClearCode + 1;
  FreeCode  = FirstFree = EOFCode + 1;

  ++CodeSize;

  InitCodeSize = CodeSize;
  MaxCode      = 1 << CodeSize;
  ReadMask     = MaxCode - 1;

  ptr1 = rasterPtr;

  do {

   byte = byte1 = NEXT_BYTE;

   while ( byte != 0 ) {

    *ptr1++ = NEXT_BYTE;
	--byte;

   }  // end while

   if (  ( UINT )( ptr1 - rasterPtr ) > dwFileSize  ) __leave;  // corrupt file - unblock

  } while ( byte1 );

  bytesPerLine = imgWidth + PAD( imgWidth );
  dataSize     = bytesPerLine * imgHeight;

  pData = ( PBYTE )MALLOC( dataSize );

  if ( pData == NULL ) __leave;

  x = y = pass = OutCount = BitOffset = ByteOffset = 0;

  Code = rasterPtr[ ByteOffset ] + ( rasterPtr[ ByteOffset + 1 ] << 8 );

  if ( CodeSize >= 8 ) Code += ( rasterPtr[ ByteOffset + 2 ] << 16 );

  Code >>= ( BitOffset % 8 );
  BitOffset += CodeSize;
  Code      &= ReadMask;

  while ( Code != EOFCode ) {

   if ( Code == ClearCode ) {

   	CodeSize   = InitCodeSize;
	MaxCode    = 1 << CodeSize;
	ReadMask   = MaxCode - 1;
	FreeCode   = FirstFree;
	ByteOffset = BitOffset >> 3;
#ifdef BUC60837
      if( ByteOffset > (dwFileSize-3) ) break;
#endif
	Code       = rasterPtr[ ByteOffset ] + ( rasterPtr[ ByteOffset + 1 ] << 8 );

      if ( CodeSize >= 8 ) Code += ( rasterPtr[ ByteOffset + 2 ] << 16 );

	Code      >>= ( BitOffset % 8 );
	BitOffset +=  CodeSize;
	Code      &=  ReadMask;

	CurCode = OldCode = Code;
	FinChar = CurCode & BitMask;

	_add_pixel ( FinChar );

   } else {

   	CurCode = InCode = Code;

	if ( CurCode >= FreeCode ) {

	 CurCode = OldCode;
	 OutCode[ OutCount++ ] = FinChar;

	}  // end if

	while ( CurCode > BitMask ) {


	 if ( OutCount > 1024 )
#ifdef BUC60837
					break;
#else
					__leave;
#endif
	 OutCode[ OutCount++ ] = Suffix[ CurCode ];
	 CurCode               = Prefix[ CurCode ];

	}  // end while

	FinChar = CurCode & BitMask;
	OutCode[ OutCount++ ] = FinChar;

	for ( i = OutCount - 1; i >= 0; --i ) _add_pixel ( OutCode[ i ] );

	OutCount = 0;
	Prefix[ FreeCode ] = OldCode;
	Suffix[ FreeCode ] = FinChar;
	OldCode            = InCode;
	++FreeCode;

	if ( FreeCode >= MaxCode ) {

     if ( CodeSize < 12 ) {

      ++CodeSize;
	  MaxCode <<= 1;
	  ReadMask = ( 1 << CodeSize ) - 1;

     } // end if

	}  // end if

   }  // end else

   ByteOffset = BitOffset >> 3;
#ifdef BUC60837
      if( ByteOffset > (dwFileSize-3) ) break;
#endif
   Code = rasterPtr[ ByteOffset ] + ( rasterPtr[ ByteOffset + 1 ] << 8 );

   if ( CodeSize >= 8 ) Code += ( rasterPtr[ ByteOffset + 2 ] << 16 );

   Code      >>= ( BitOffset % 8 );
   BitOffset +=	 CodeSize;
   Code      &=  ReadMask;

  }  // end while

  pBmi -> bmiHeader.biSize        =  sizeof ( BITMAPINFOHEADER );
  pBmi -> bmiHeader.biWidth       =  imgWidth;
  pBmi -> bmiHeader.biHeight      = -( INT )imgHeight;
  pBmi -> bmiHeader.biPlanes      =  1;
  pBmi -> bmiHeader.biBitCount    =  8;
  pBmi -> bmiHeader.biCompression =  BI_RGB;

  if (  gDev -> IsPaletteDevice ()  ) {

   _alloc_colors (
    pData, imgWidth, imgHeight, PIXEL8, pBmi -> bmiColors, gDev
   );

   hOldPal = SelectPalette ( hDC, ( HPALETTE )(  gDev -> HPalette ()  ), FALSE );
   RealizePalette ( hDC );

  }  // end if

  retVal = CreateDIBitmap (
            hDC, ( PBITMAPINFOHEADER )pBmi, CBM_INIT, pData, pBmi, DIB_RGB_COLORS
		   );

  if (  gDev -> IsPaletteDevice ()  )

   SelectPalette ( hDC, hOldPal, FALSE );

 }  // end __try

 __finally {

  if ( pData     != NULL ) FREE( pData     );
  if ( rasterPtr != NULL ) FREE( rasterPtr );
  if ( pBmi      != NULL ) FREE( pBmi      );
  if ( Suffix    != NULL ) FREE( Suffix    );
  if ( Prefix    != NULL ) FREE( Prefix    );
  if ( OutCode   != NULL ) FREE( OutCode   );

 }  // end __finally

 return retVal;

}  // end loadGIF

typedef struct _screen_descr {

				char gifID[ 6 ];
				WORD scrnWidth;
				WORD scrnHeight;
				BYTE scrnFlag;

               } SCREEN_DESCR;

typedef struct _image_descr {

				WORD imgX;
				WORD imgY;
				WORD imgWidth;
				WORD imgHeight;
				BYTE imgFlag;

               } IMAGE_DESCR;

static void __fastcall _add_pixel ( UINT idx ) {

 if ( y < imgHeight )

  *( pData + y * bytesPerLine + x ) = ( BYTE )idx;

 if ( ++x == imgWidth ) {

  x = 0;

  if ( !isInterlace )

   ++y;

  else {

   switch ( pass ) {

   	case 0:

	 y += 8;

	 if ( y >= imgHeight ) ++pass, y = 4;

	break;

	case 1:

	 y += 8;

	 if ( y >= imgHeight ) ++pass, y = 2;

	break;

	case 2:

	 y += 4;

	 if ( y >= imgHeight ) ++pass, y = 1;

	break;

	case 3:

	 y += 2;

   }  // end switch

  }  // end else

 }  // end if

}  // end _add_pixel


//***//
//*************************************************************************//
//***//
static void __fastcall _alloc_colors (
                        PVOID data, int width, int height, int size, LPRGBQUAD colors,
                        Handle( WNT_GraphicDevice )& gDev
                       ) {

 int              i, j, k, bytes_per_line;
 Standard_Integer r, g, b;
 PBYTE            pLine;

 if ( height < 0 ) height = -height;

 switch ( size ) {

  case PIXEL4: {

   PBYTE pixels;
   int   l, idx, nColors;

   pixels = ( PBYTE )MALLOC( 0x10 );

   if ( pixels != NULL ) {

    pLine = ( PBYTE )data;

	bytes_per_line  = width >> 1;
	bytes_per_line += PAD( bytes_per_line );

	nColors = gDev -> NumColors ();

	for ( i = k = 0; i < height; ++i ) {

	 for ( j = l = 0; j < width; ++j ) {

	  idx = pLine[ l ] & 0x0F;

	  if ( !pixels[ idx ] ) {
retry_4:
	   r = ( Standard_Integer )( colors[ idx ].rgbRed   );
	   g = ( Standard_Integer )( colors[ idx ].rgbGreen );
	   b = ( Standard_Integer )( colors[ idx ].rgbBlue  );

	   gDev -> SetColor (  ( r << 16 ) | ( g << 8 ) | b  );

	   pixels[ idx ] = TRUE;

	   if ( ++k > nColors )

	    goto end_4;

	  } else {

	   idx = ( pLine[ l++ ] >> 4 ) & 0x0F;

	   if ( !pixels[ idx ] )

	    goto retry_4;

	  }  // end else

	 }  // end for ( j . . . )

	 pLine += bytes_per_line;

	}  // end for ( i . . . )
end_4:
   	FREE( pixels );

   }  // end if

  }  // PIXEL4

  break;

  case PIXEL8: {

   int   idx, nColors;
   PBYTE pixels;

   pixels = ( PBYTE )MALLOC( 0x100 );

   if ( pixels != NULL ) {

    pLine = ( PBYTE )data;

	bytes_per_line = width + PAD( width );

	nColors = gDev -> NumColors ();

	for ( i = k = 0; i < height; ++i ) {

	 for ( j = 0; j < width; ++j ) {

	  idx = pLine[ j ];

      if ( pixels[ idx ] ) continue;

	  r = ( Standard_Integer )( colors[ idx ].rgbRed   );
	  g = ( Standard_Integer )( colors[ idx ].rgbGreen );
	  b = ( Standard_Integer )( colors[ idx ].rgbBlue  );

	  gDev -> SetColor (  ( r << 16 ) | ( g << 8 ) | b  );

	  pixels[ idx ] = TRUE;

      if ( ++k > nColors ) goto end_8;

	 }  // end for ( j . . . )

	 pLine += bytes_per_line;

	}  // end for ( i . . . )
end_8:
   	FREE( pixels );

   }  // end if

  }  // PIXEL8

  break;

  case PIXEL16: {

   PWORD pixels, hPixels, pData;
   WORD  pix, hPix, mask = 0xFF, rmask = 0xFFFF, mult;
   DWORD dataSize;

   pixels  = ( PWORD )MALLOC(  0x100 * sizeof ( WORD )  );
   hPixels = ( PWORD )MALLOC(  0x100 * sizeof ( WORD )  );

   if ( pixels != NULL && hPixels != NULL ) {

    FillMemory (  ( PVOID )pixels,  0x100 * sizeof ( WORD ), ~0  );
    FillMemory (  ( PVOID )hPixels, 0x100 * sizeof ( WORD ), ~0  );

    pData = ( PWORD )data;

	bytes_per_line  = width * sizeof ( WORD );
	bytes_per_line += PAD( bytes_per_line );
	dataSize        = bytes_per_line * height;

	mult  = ( WORD )(  dataSize / sizeof ( WORD ) / RAND_MAX  );

	if ( mult == 0 ) {

	 mult  = 1;
	 rmask = ( WORD )(  dataSize / sizeof ( WORD ) - 1  );

	}  // end if

    for ( i = j = 0, k = gDev -> NumColors ();
          i < ( int )(  dataSize / sizeof ( WORD )  );
          ++i
    ) {

     pix  = pData[ (  rand () * mult  ) & rmask ];
	 hPix = (  ( pix >> 8 ) ^ pix  ) & mask;

	 if (  pixels[ hPix ] == ~0 || hPixels[ hPix ] != pix  ) {

	  gDev -> SetColor (
	           (  ( pix & 0x1F   ) << 3  ) |
	           (  ( pix & 0x3E0  ) << 6  ) |
	           (  ( pix & 0x7C00 ) << 9  )
	          );

	  if ( ++j > k ) break;

	   hPixels[ hPix ] = pixels[ hPix ] = pix;

	 }  // end if

    }  // end for

	FREE( hPixels );
	FREE( pixels  );

   }  // end if

  }  // PIXEL16

  break;

  case PIXEL24: {

   PLONG    pixels, hPixels;
   PTRIPLET	pData;
   LONG     pix, hPix, mask = 0x0FFF, rmask = 0xFFFFFFFF, mult;
   DWORD    dataSize;

   pixels  = ( PLONG )MALLOC(  0x1000 * sizeof ( LONG )  );
   hPixels = ( PLONG )MALLOC(  0x1000 * sizeof ( LONG )  );

   if ( pixels != NULL && hPixels != NULL ) {

    FillMemory (  ( PVOID )pixels,  0x1000 * sizeof ( LONG ), ~0  );
    FillMemory (  ( PVOID )hPixels, 0x1000 * sizeof ( LONG ), ~0  );

	pData = ( PTRIPLET )data;

	bytes_per_line  = width * sizeof ( TRIPLET );
	bytes_per_line += PAD( bytes_per_line );
	dataSize        = bytes_per_line * height;

	mult  = dataSize / sizeof ( TRIPLET ) / RAND_MAX;

	if ( mult == 0 ) {

	 mult  = 1;
	 rmask = dataSize / sizeof ( TRIPLET ) - 1;

	}  // end if

    for ( i = j = 0, k = gDev -> NumColors ();
          i < ( int )(  dataSize / sizeof ( TRIPLET )  );
          ++i
    ) {

	 pix = (  *( PLONG )( pData[ (  rand () * mult  ) & rmask ].pixel )  ) & 0x00FFFFFF;
	 hPix = (  ( pix >> 12 ) ^ pix  ) & mask;

	 if (  pixels[ hPix ] == ~0 || hPixels[ hPix ] != pix  ) {

	  gDev -> SetColor ( pix );

	  if ( ++j > k ) break;

	  hPixels[ hPix ] = pixels[ hPix ] = pix;

	 }  // end if

    }  // end for

	FREE( hPixels );
	FREE( pixels  );

   }  // end if

  }  // PIXEL24

  break;

  case PIXEL32: {

   PLONG pixels, hPixels, pData;
   LONG  pix, hPix, mask = 0x0FFF, rmask = 0xFFFFFFFF, mult;
   DWORD dataSize;

   pixels  = ( PLONG )MALLOC(  0x1000 * sizeof ( LONG )  );
   hPixels = ( PLONG )MALLOC(  0x1000 * sizeof ( LONG )  );

   if ( pixels != NULL && hPixels != NULL ) {

    FillMemory (  ( PVOID )pixels,  0x1000 * sizeof ( LONG ), ~0  );
    FillMemory (  ( PVOID )hPixels, 0x1000 * sizeof ( LONG ), ~0  );

	pData = ( PLONG )data;

	bytes_per_line  = width * sizeof ( LONG );
	bytes_per_line += PAD( bytes_per_line );
	dataSize        = bytes_per_line * height;

	mult  = dataSize / sizeof ( LONG ) / RAND_MAX;

	if ( mult == 0 ) {

	 mult  = 1;
	 rmask = dataSize / sizeof ( LONG ) - 1;

	}  // end if

    for ( i = j = 0, k = gDev -> NumColors ();
          i < ( int )(  dataSize / sizeof ( LONG )  );
          ++i
    ) {

	 pix = pData[ (  rand () * mult  ) & rmask ];
	 hPix = (  ( pix >> 12 ) ^ pix  ) & mask;

	 if (  pixels[ hPix ] == ~0 || hPixels[ hPix ] != pix  ) {

	  gDev -> SetColor ( pix );

	  if ( ++j > k ) break;

	  hPixels[ hPix ] = pixels[ hPix ] = pix;

	 }  // end if

    }  // end for

	FREE( hPixels );
	FREE( pixels  );

   }  // end if

  }  // PIXEL32

 }  // end switch

}  // end _alloc_colors

_init :: _init () {

 TCHAR cimgType[ 17 ];

 WNT_osVer.dwOSVersionInfoSize = sizeof ( OSVERSIONINFO );

 GetVersionEx ( &WNT_osVer );

 if ( hHeap == NULL ) {

  SYSTEM_INFO si;

  GetSystemInfo ( &si );

  hHeap = HeapCreate ( HEAP_GENERATE_EXCEPTIONS, si.dwPageSize, 0 );

 }  // end if

 if ( WNT_osVer.dwPlatformId == VER_PLATFORM_WIN32_NT ) {
#ifndef UNICODE
  LPSTR    lpNTproc[] = { "OpenWindowStationA", "SetProcessWindowStation",
                          "OpenDesktopA"      , "SetThreadDesktop"       ,
                          "CloseDesktop"      , "CloseWindowStation"
                        };
#else
  LPSTR    lpNTproc[] = { "OpenWindowStationW", "SetProcessWindowStation",
                          "OpenDesktopW"      , "SetThreadDesktop"       ,
                          "CloseDesktop"      , "CloseWindowStation"
                        };
#endif  // UNICODE
  FARPROC* fpNTproc[] = { ( FARPROC* )&NTOpenWindowStation, ( FARPROC* )&NTSetProcessWindowStation,
                          ( FARPROC* )&NTOpenDesktop,       ( FARPROC* )&NTSetThreadDesktop,
                          ( FARPROC* )&NTCloseDesktop,      ( FARPROC* )&NTCloseWindowStation
                        };
  HMODULE  hUser32    = GetModuleHandle (  TEXT( "USER32" )  );

  for (   int i = 0; i < (  sizeof ( lpNTproc ) / sizeof ( lpNTproc[ 0 ] )  ); ++i   )

   *fpNTproc[ i ] = GetProcAddress ( hUser32, lpNTproc[ i ] );

 }  // end if

 ZeroMemory (  cimgType, sizeof ( cimgType )  );

 if (   GetEnvironmentVariable (
         TEXT( "CSF_DefaultImageFormat" ), cimgType, sizeof ( imgType )
        ) == 0
 ) lstrcpy (  cimgType, TEXT( "GIF" )  );

 imgType = _image_type ( cimgType, TRUE );

}  // end constructor

_init :: ~_init () {

 if ( hHeap != NULL ) {

  HeapDestroy ( hHeap );
  hHeap = NULL;

 }  // end if

}  // end destructor

static WNT_TypeOfImage __fastcall _image_type ( LPTSTR ext, BOOL fInit ) {

 WNT_TypeOfImage retVal;

 if (   !lstrcmpi (  ext, TEXT( "BMP" )  )   )

  retVal = WNT_TOI_BMP;

 else if (   !lstrcmpi (  ext, TEXT( "XWD" )  )   )

  retVal = WNT_TOI_XWD;

 else if ( fInit )

  retVal = WNT_TOI_GIF;

 else {

  retVal = imgType;

  if (   lstrcmpi (  ext, TEXT( "GIF" )  )   )

   switch ( retVal ) {

    case WNT_TOI_BMP:

     lstrcpy (  ext, TEXT( "bmp" )  );

    break;

    case WNT_TOI_XWD:

     lstrcpy (  ext, TEXT( "xwd" )  );

    break;

    default:

     lstrcpy (  ext, TEXT( "gif" )  );

   }  // end switch

 }  // end else

 return retVal;

}  // end _image_type
