// Copyright (c) 1996-1999 Matra Datavision
// Copyright (c) 1999-2012 OPEN CASCADE SAS
//
// The content of this file is subject to the Open CASCADE Technology Public
// License Version 6.5 (the "License"). You may not use the content of this file
// except in compliance with the License. Please obtain a copy of the License
// at http://www.opencascade.org and read it completely before using this file.
//
// The Initial Developer of the Original Code is Open CASCADE S.A.S., having its
// main offices at: 1, place des Freres Montgolfier, 78280 Guyancourt, France.
//
// The Original Code and all software distributed under the License is
// distributed on an "AS IS" basis, without warranty of any kind, and the
// Initial Developer hereby disclaims all such warranties, including without
// limitation, any warranties of merchantability, fitness for a particular
// purpose or non-infringement. Please see the License for the specific terms
// and conditions governing the rights and limitations under the License.

/***********************************************************************
     FONCTION :
     ----------
        Classe WNT_GraphicDevice.cxx :

     HISTORIQUE DES MODIFICATIONS   :
     --------------------------------
      03-01-95	: EUG   -> Creation.
      20-11-97  : FMN   -> Portage WIN95
      MAR-98    : EUG   -> Porting Win95
      JUN-98    : EUG   -> Modifications to provide access to this class
                           from CCL (second constructor + Init () method)
      SEP-98    : DCB   -> Avoid memory crash when color indices do not follow
                           each other or do not begin with '1'
      JAN-99    : EUG   -> Modifications to provide treating display window
                           as graphic device
************************************************************************/

/*----------------------------------------------------------------------*/
/*
 * Includes
 */

// include windows.h first to have all definitions available
#include <windows.h>

#include <WNT.h>
#include <WNT_GraphicDevice.ixx>
#include <Aspect_ColorMapEntry.hxx>
#include <Aspect_GraphicDriver.hxx>
#include <Aspect_Units.hxx>

#include <stdio.h>

/*----------------------------------------------------------------------*/
/*
 * Constantes
 */

#define PAL           ((PLOGPALETTE)myLogPal)
#define MAX_PAL_ERROR (3*256*256L)
#define DEFAULT_GAMMA 1.4

static int  __fastcall _createColorRamp ( HPALETTE* );
static BOOL s_SysPalInUse;

extern HWINSTA ( WINAPI *NTOpenWindowStation       ) ( LPTSTR, BOOL, DWORD        );
extern BOOL    ( WINAPI *NTSetProcessWindowStation ) ( HWINSTA                    );
extern HDESK   ( WINAPI *NTOpenDesktop             ) ( LPTSTR, DWORD, BOOL, DWORD );
extern BOOL    ( WINAPI *NTSetThreadDesktop        ) ( HDESK                      );
extern BOOL    ( WINAPI *NTCloseDesktop            ) ( HDESK                      );
extern BOOL    ( WINAPI *NTCloseWindowStation      ) ( HWINSTA                    );

extern OSVERSIONINFO WNT_osVer;

//=======================================================================
//function : WNT_GraphicDevice
//purpose  : 
//=======================================================================

WNT_GraphicDevice::WNT_GraphicDevice (
                      const Standard_Boolean aColorCube,
                      const Aspect_Handle    aDevContext
					 )
{

  Init ( aColorCube, aDevContext );

}  // end constructor ( 1 )

//=======================================================================
//function : WNT_GraphicDevice
//purpose  : 
//=======================================================================

WNT_GraphicDevice::WNT_GraphicDevice (
                      const Standard_Boolean aColorCube,
                      const Standard_Integer aDevContext
                     )
{
                     
  Init (  aColorCube, ( Aspect_Handle )aDevContext  );
                     
}  // end constructor ( 2 )

//=======================================================================
//function : Init
//purpose  : 
//=======================================================================

void WNT_GraphicDevice::Init (
                           const Standard_Boolean aColorCube,
                           const Aspect_Handle    aDevContext
                          )
{
                          
 HDC     hDC;
 BOOL    palDev;
 HWND    hwnd = NULL;
 HBITMAP hBmp = NULL;
 int     numRes, palSize, nPlanes, nBits;

 hDC  = ( aDevContext == NULL ) ? GetDC ( NULL ) :
          ( HDC )aDevContext, hwnd = WindowFromDC (  ( HDC )aDevContext  );

//  It is found that the error occurs in the place in the source code that is
// responsible for connecting to the desktop and window pointed by the environment
// variable CSF_DISPLAY.
// For the moment it is unclear at all what this code is needed for. Therefore it
// is removed from the sources in order to fix this bug. This allowed to run the
// code without problems on both win32 and win64 platforms.
// It is needed to carry out special investigations about this subject. Probably it
// can be used to connect to a virtual desktop in order to make snapshots on the
// Windows station on which no user is logged in.
//
// if ( aDevContext == 0 && WNT_osVer.dwPlatformId == VER_PLATFORM_WIN32_NT ) {

//  HWINSTA hWst = NULL;
//  HDESK   hDsk = NULL;
//  DWORD   dwLen, dwLenOrig;
//  LPTSTR  buff = NULL;
//  LPTSTR  sNam = NULL;
//  LPTSTR  dNam = NULL;
//  BOOL    fSts = FALSE;

//  dwLenOrig = GetEnvironmentVariable(  TEXT( "CSF_DISPLAY" ), NULL, 0  );

//  if ( dwLenOrig == 0 ) 
  
//   dwLen = sizeof (  TEXT( "WinSta0\\Default" )  );

//  else

//   dwLen = dwLenOrig + sizeof ( TCHAR );

//  buff = ( LPTSTR )HeapAlloc (
//                    GetProcessHeap (), HEAP_GENERATE_EXCEPTIONS | HEAP_ZERO_MEMORY, dwLen
//                   );

//  if ( dwLenOrig != 0 )

//   GetEnvironmentVariable (  TEXT( "CSF_DISPLAY" ), buff, dwLen  );

//  else

//   lstrcpy (  buff, TEXT( "WinSta0\\Default" )  );

//  dwLen = 0;

//  while (  buff[ dwLen ] != TEXT( '\\' ) && buff[ dwLen ] != TEXT( '\x00' )  ) ++dwLen;

//  sNam = ( LPTSTR )HeapAlloc (
//                    GetProcessHeap (), HEAP_GENERATE_EXCEPTIONS,
//                    dwLen * sizeof ( TCHAR ) + sizeof ( TCHAR )
//                   );

//  dwLenOrig = 0;

//  while ( dwLenOrig != dwLen ) {

//   sNam[ dwLenOrig ] = buff[ dwLenOrig ];
//   ++dwLenOrig;

//  }  // end while

//  sNam[ dwLenOrig ] = TEXT( '\x00' );

//  if (  buff[ dwLen ] == TEXT( '\x00' )  ) goto leave;

//  lstrcpy ( buff, &buff[ dwLen + 1 ] );

//  dNam = ( LPTSTR )HeapAlloc (
//                    GetProcessHeap (), HEAP_GENERATE_EXCEPTIONS,
//                    lstrlen ( buff ) * sizeof ( TCHAR ) + sizeof ( TCHAR )
//                   );

//  lstrcpy ( dNam, buff );

//  if (   (  hWst = ( *NTOpenWindowStation ) ( sNam, FALSE, MAXIMUM_ALLOWED )  ) == NULL ||
//         !( *NTSetProcessWindowStation ) ( hWst )
//  ) goto leave;

//  if (   (  hDsk = ( *NTOpenDesktop ) ( dNam, 0, FALSE, MAXIMUM_ALLOWED )  ) == NULL ||
//         !( *NTSetThreadDesktop ) ( hDsk )
//  )

//   if (  GetLastError () == ERROR_BUSY  ) {

//    if ( hDsk != NULL ) ( *NTCloseDesktop       ) ( hDsk );
//    if ( hWst != NULL ) ( *NTCloseWindowStation ) ( hWst );
     
//   } else goto leave;

//  fSts = TRUE;
//leave:
//  if ( sNam != NULL ) HeapFree (  GetProcessHeap (), 0, ( LPVOID )sNam  );
//  if ( dNam != NULL ) HeapFree (  GetProcessHeap (), 0, ( LPVOID )dNam  );
//  if ( buff != NULL ) HeapFree (  GetProcessHeap (), 0, ( LPVOID )buff  );

//  if ( !fSts ) {

//   if ( hDsk != NULL ) ( *NTCloseDesktop       ) ( hDsk );
//   if ( hWst != NULL ) ( *NTCloseWindowStation ) ( hWst );

//   Aspect_GraphicDeviceDefinitionError :: Raise ( "Access to station is denied" );

//  }  // end if

// }  // end if

  palDev    = (  GetDeviceCaps ( hDC, RASTERCAPS ) & RC_PALETTE  ) ? TRUE : FALSE;  
  numRes    = GetDeviceCaps ( hDC, NUMRESERVED );
  palSize   = ( palDev ) ? GetDeviceCaps ( hDC, SIZEPALETTE ) : 0;
  nPlanes   = GetDeviceCaps ( hDC, PLANES );
  nBits     = GetDeviceCaps ( hDC, BITSPIXEL );
  myWidth   = GetDeviceCaps ( hDC, HORZRES );
  myHeight  = GetDeviceCaps ( hDC, VERTRES );
  myMWidth  = ( float )GetDeviceCaps ( hDC, HORZSIZE ) / ( 1. MILLIMETER );
  myMHeight = ( float )GetDeviceCaps ( hDC, VERTSIZE ) / ( 1. MILLIMETER );

  if ( aDevContext != NULL && hwnd != NULL ) {

   RECT r;

   GetClientRect ( hwnd, &r );

   myMWidth  = myMWidth  * r.right  / myWidth;
   myMHeight = myMHeight * r.bottom / myHeight;
   myWidth   = r.right;
   myHeight  = r.bottom;

  } else {

   BITMAP bm;

   hBmp = ( HBITMAP )GetCurrentObject ( hDC, OBJ_BITMAP );

   if (   GetObject (  hBmp, sizeof ( BITMAP ), &bm  )   ) {

    myMWidth  = myMWidth  * bm.bmWidth  / myWidth;
    myMHeight = myMHeight * bm.bmHeight / myHeight;
// Tempory correction for Windows 98
    if ( WNT_osVer.dwPlatformId != VER_PLATFORM_WIN32_WINDOWS ) { 
// 
    myWidth   = bm.bmWidth;
    myHeight  = bm.bmHeight;

   } else hBmp = NULL;
// Tempory correction for Windows 98
  }
// 

  }  // end else

 if ( aDevContext == NULL ) ReleaseDC ( NULL, hDC );

 myNumColors = (  1 << ( nBits * nPlanes )  );

 if ( palDev ) myNumColors -= numRes;

 myFreeIndex     = 0;
 myOpenGLPalette = aColorCube;

 if ( palDev ) {

  if ( hwnd == NULL && hBmp == NULL ) {

   myLogPal = HeapAlloc (
               GetProcessHeap (), HEAP_ZERO_MEMORY,
               sizeof( LOGPALETTE ) +
               sizeof( PALETTEENTRY ) * myNumColors
              );

   if ( !myLogPal )

    Aspect_GraphicDeviceDefinitionError :: Raise ( "Out of memory" );

   PAL -> palVersion    = 0x300;
   PAL -> palNumEntries = 1;

   myPalette = CreatePalette ( PAL );

   if ( !myPalette ) {

    HeapFree (  GetProcessHeap (), 0, myLogPal  );
    Aspect_GraphicDeviceDefinitionError :: Raise ( "Unable to create user palette" );   

   }  // end if

   myFreeIndex = 0;

   if (  myOpenGLPalette && !_createColorRamp (  ( HPALETTE* )&myPalette  )  )

    Aspect_GraphicDeviceDefinitionError :: Raise ( "Colorcube creation failed" );

  } else {

   myPalette = GetCurrentObject ( hDC, OBJ_PAL );
   myLogPal  = NULL;

  }  // end else
  
 } else {  // not a palette device
 
  myPalette = 0;
  myLogPal  = NULL;
 
 }  // end else ( palDev . . . ) 

 myHighlightColor = RGB( 255, 255, 255 );
                                                    
}  // end WNT_GraphicDevice :: Init

void WNT_GraphicDevice :: Destroy () {

 if ( myPalette && myLogPal != NULL ) {

  DeletePalette ( myPalette );
  HeapFree (  GetProcessHeap (), 0, myLogPal  );

 }  // end if

}  // end WNT_GraphicDevice :: Destroy

WNT_ColorRef WNT_GraphicDevice :: SetColor (
                                   const Quantity_Color& aColor,
                                   const Standard_Boolean aHighlight
                                  ) {

 Standard_Real    r, g, b;
 Standard_Integer red, green, blue;

 aColor.Values ( r, g, b, Quantity_TOC_RGB );

 red   = ( Standard_Integer )( 255. * r );
 green = ( Standard_Integer )( 255. * g );
 blue  = ( Standard_Integer )( 255. * b );

 return SetColor ( red, green, blue, aHighlight );

}  // end WNT_GraphicDevice :: SetColor( 1 )

WNT_ColorRef WNT_GraphicDevice :: SetColor (
                                   const Standard_Integer aRed,
                                   const Standard_Integer aGreen,
                                   const Standard_Integer aBlue,
                                   const Standard_Boolean aHighlight
                                  ) {
                                  
 int          i;
 WNT_ColorRef retVal;
 BYTE         red, green, blue;

 red   = ( BYTE )aRed;
 green = ( BYTE )aGreen;
 blue  = ( BYTE )aBlue;

 if ( !myPalette )

  retVal = RGB( red, green, blue );

 else if ( myOpenGLPalette )
 
  retVal = PALETTEINDEX(
            GetNearestPaletteIndex (
             ( HPALETTE )myPalette, RGB( red, green, blue )
			)
		   );

 else {

  if ( myFreeIndex < myNumColors ) {

   for ( i = 0; i < myFreeIndex; ++i )  // avoid color duplication

    if ( PAL -> palPalEntry[ i ].peRed   == red   &&
         PAL -> palPalEntry[ i ].peGreen == green &&
         PAL -> palPalEntry[ i ].peBlue  == blue
    )

     break;

   if ( i == myFreeIndex ) {  // add new color entry

    PAL -> palPalEntry[ i ].peRed   = red;
    PAL -> palPalEntry[ i ].peGreen = green;
    PAL -> palPalEntry[ i ].peBlue  = blue;
    PAL -> palPalEntry[ i ].peFlags = 0;
    ++myFreeIndex;
    ResizePalette (  ( HPALETTE )myPalette, myFreeIndex );
    SetPaletteEntries (  ( HPALETTE )myPalette, i, 1, &PAL -> palPalEntry[ i ]  );

   }  // end if

   retVal = PALETTEINDEX( i );

  } else  // get closest color

   retVal = PALETTEINDEX(
             GetNearestPaletteIndex (
              ( HPALETTE )myPalette, RGB( red, green, blue )
			 )
		    );

 }  // end else ( !myPalette . . . )

 if ( aHighlight )

  myHighlightColor = retVal;

 return retVal;

}  // end WNT_GraphicDevice :: SetColor(2)

void WNT_GraphicDevice :: SetColor ( const WNT_Long& aPixel ) {

 if ( myPalette && !myOpenGLPalette && myFreeIndex < myNumColors ) {

  int  idx;
  BYTE red, green, blue;

  blue  = (BYTE) (aPixel & 0xFF);
  green = (BYTE) (( aPixel >>  8 ) & 0xFF);
  red   = (BYTE) (( aPixel >> 16 ) & 0xFF);

  idx = myFreeIndex;

  for ( int i = 2; i < myFreeIndex; ++i )  // avoid color duplication

   if ( PAL -> palPalEntry[ i ].peRed   == red   &&
        PAL -> palPalEntry[ i ].peGreen == green &&
        PAL -> palPalEntry[ i ].peBlue  == blue
   )

    return;

  PAL -> palPalEntry[ idx ].peRed   = red;
  PAL -> palPalEntry[ idx ].peGreen = green;
  PAL -> palPalEntry[ idx ].peBlue  = blue;
  PAL -> palPalEntry[ idx ].peFlags = 0;
  ++myFreeIndex;
  ResizePalette (  ( HPALETTE )myPalette, myFreeIndex );
  SetPaletteEntries (  ( HPALETTE )myPalette, idx, 1, &PAL -> palPalEntry[ idx ]  );

 }  // end if ( myPalette . . . )

}  // end WNT_GraphicDevice :: SetColor(3)

void WNT_GraphicDevice :: MapColors (
                           const Handle( Aspect_ColorMap )& aColorMap,
                                 Handle( WNT_HColorTable )& aColorTable
                          ) {

 Aspect_ColorMapEntry entry;
 Quantity_Color       color;
 COLORREF             dwColor;
 Standard_Real        r, g, b;
 int                  i, index;

 if (  myOpenGLPalette || !IsPaletteDevice ()  )  // readonly palette or no palette

  for ( i = 1; i <= aColorMap -> Size (); ++i ) {

   entry = aColorMap -> Entry ( i );
   color = entry.Color ();
   aColorTable -> SetValue (  entry.Index (), SetColor ( color )  );

  }  // end for

 else {  // writable palette

  for ( i = 1; i <= aColorMap -> Size (); ++i ) {
  
   entry   = aColorMap -> Entry ( i );
   color   = entry.Color ();
   dwColor = aColorTable -> Value ( i );
   index   = dwColor & 0x01000000 ? dwColor & 0x00FFFFFF : 0xFFFFFFFF;

   if ( index != 0xFFFFFFFF && index < myFreeIndex ) {
   
    color.Values ( r, g, b, Quantity_TOC_RGB );
   	PAL -> palPalEntry[ index ].peRed   = ( BYTE )( 255. * r );
	PAL -> palPalEntry[ index ].peGreen = ( BYTE )( 255. * g );
	PAL -> palPalEntry[ index ].peBlue  = ( BYTE )( 255. * b );
   
   } else
   
   	aColorTable -> SetValue (  i, SetColor ( color )  );
   
  }  // end for

  SetPaletteEntries (
   ( HPALETTE )myPalette, 0, myFreeIndex, &PAL -> palPalEntry[ 0 ]
  );

 }  // end else

}  // WNT_GraphicDevice :: MapColors

Handle( Aspect_GraphicDriver ) WNT_GraphicDevice :: GraphicDriver () const {

 Handle( Aspect_GraphicDriver ) dummy;

 return dummy;

}  // WNT_GraphicDevice :: GraphicDriver
//***//
//*** Function to create RGB color map for use with OpenGL.                 ***//
//*** For OpenGL RGB rendering we need to know red, green, & blue           ***//
//*** component bit sizes and positions. This program creates an RGB color  ***//
//*** cube with a default gamma of 1.4.                                     ***//
//*** Unfortunately, because the standard 20 colors in the system palette   ***//
//*** cannot be changed, if we select this palette into a display DC,       ***//
//*** we will not realize all of the logical palette. The function          ***//
//*** changes some of the entries in the logical palette to match enties in ***//
//*** the system palette using a least-squares calculation to find which    ***//
//*** entries to replace.                                                   ***//
//***//
static int __fastcall _createColorRamp ( HPALETTE* pal ) {

 int                   i, j;
 int                   sysPalSize, logPalSize;
 int                   red_max,  green_max,  blue_max,
                       red_mask, green_mask, blue_mask;
 int                   iPf;
 HDC                   hDC;
 PIXELFORMATDESCRIPTOR pfd;
 PPALETTEENTRY         sysPal, colorRamp;
 PBYTE                 gRed, gGreen, gBlue;
 BYTE                  inc;
 LONG                  error, min_error, error_index, delta;
 double                dv, gamma;
 char                  buff[ 32 ];

 ZeroMemory (  ( PVOID )&pfd, sizeof ( PIXELFORMATDESCRIPTOR )  );

 pfd.nSize      = sizeof ( PIXELFORMATDESCRIPTOR );
 pfd.nVersion   = 1;
 pfd.dwFlags    = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL |
                  PFD_DOUBLEBUFFER;
 pfd.iPixelType	= PFD_TYPE_RGBA;
 pfd.cColorBits = 24;
 pfd.cDepthBits = 32;
 pfd.iLayerType = PFD_MAIN_PLANE;

 hDC = GetDC ( NULL );

  iPf = ChoosePixelFormat ( hDC, &pfd );
  i = DescribePixelFormat (
       hDC, iPf, sizeof ( PIXELFORMATDESCRIPTOR ), &pfd
	  );

  if ( !i ) {

   ReleaseDC ( NULL, hDC );

   return 0;

  }  // end if

  sysPalSize = GetDeviceCaps ( hDC, NUMCOLORS );
  logPalSize = 1 << pfd.cColorBits;
  sysPal     = new PALETTEENTRY[ sysPalSize + logPalSize ];
  colorRamp  = sysPal + sysPalSize;
  GetSystemPaletteEntries ( hDC, 0, sysPalSize, sysPal );

 ReleaseDC ( NULL, hDC );

 red_max   = 1 << pfd.cRedBits;
 green_max = 1 << pfd.cGreenBits;
 blue_max  = 1 << pfd.cBlueBits;

 red_mask   = red_max   - 1;
 green_mask	= green_max - 1;
 blue_mask  = blue_max  - 1;

 gRed   = new BYTE[ red_max + green_max + blue_max ];
 gGreen = gRed + red_max;
 gBlue  = gGreen + green_max;

 inc = ( BYTE )(  255.0F / ( float )red_mask  );

 if (  GetEnvironmentVariable ( "CSF_GammaValue", buff, 32 )  ) {
 
  gamma = atof ( buff );

  if ( gamma == 0.0 )

   gamma = DEFAULT_GAMMA;
 
 } else

  gamma = DEFAULT_GAMMA;

 for ( i = 0; i < red_max; ++i ) {

  gRed[ i ] = ( i * inc ) & 0xFF;
  dv = ( 255. *  pow ( gRed[ i ] / 255., 1. / gamma ) ) + 0.5;
  gRed[ i ] = ( BYTE )dv;

 }  // end for
  
 inc = ( BYTE )(  255.0F / ( float )green_mask  );

 for ( i = 0; i < green_max; ++i ) {

  gGreen[ i ] = ( i * inc ) & 0xFF;
  dv = (  255. * pow ( gGreen[ i ] / 255., 1. / gamma )  ) + 0.5;
  gGreen[ i ] = ( BYTE )dv;

 }  // end for

 inc = ( BYTE )(  255.0F / ( float )blue_mask  );

 for ( i = 0; i < blue_max; ++i ) {

  gBlue[ i ] = ( i * inc ) & 0xFF;
  dv = (  255. * pow ( gBlue[ i ] / 255., 1. / gamma )  ) + 0.5;
  gBlue[ i ] = ( BYTE )dv;

 }  // end for

 for ( i = 0; i < logPalSize; ++i ) {
 
  colorRamp[ i ].peRed =
   gRed[   ( i >> pfd.cRedShift   ) & red_mask   ];
  colorRamp[ i ].peGreen =
   gGreen[ ( i >> pfd.cGreenShift ) & green_mask ];
  colorRamp[ i ].peBlue =
   gBlue[  ( i >> pfd.cBlueShift  ) & blue_mask  ];
  colorRamp[ i ].peFlags = 0;

 }  // end for

 if ( pfd.dwFlags & PFD_NEED_SYSTEM_PALETTE ) {
	 
  s_SysPalInUse = TRUE;

  for ( i = 1; i < logPalSize - 1; ++i ) colorRamp[ i ].peFlags = PC_NOCOLLAPSE;

 } else {

  for ( i = 0; i < sysPalSize; ++i )
 
   for ( j = 0; j < logPalSize; ++j ) 

    if (  sysPal[ i ].peRed   == colorRamp[ j ].peRed &&
          sysPal[ i ].peGreen == colorRamp[ j ].peRed &&
	      sysPal[ i ].peBlue  == colorRamp[ j ].peRed
    )

     sysPal[ i ].peFlags = colorRamp[ i ].peFlags = 1;

    else

     sysPal[ i ].peFlags = colorRamp[ i ].peFlags = 0;

  for ( i = 0; i < sysPalSize; ++i ) {
 
   if ( sysPal[ i ].peFlags ) continue;

   min_error = MAX_PAL_ERROR;

   for ( j = 0; j < logPalSize; ++j ) {
  
    if ( colorRamp[ j ].peFlags ) continue;
    
    delta = colorRamp[ j ].peRed - sysPal[ i ].peRed;
    error = delta * delta;
    delta = colorRamp[ j ].peGreen - sysPal[ i ].peGreen;
    error += delta * delta;
    delta = colorRamp[ j ].peBlue - sysPal[ i ].peBlue;
    error += delta * delta;

    if ( error < min_error ) {
   
   	 error_index = j;
	 min_error   = error;
   
    }  // end if

   }  // end for ( j . . . )

   colorRamp[ error_index ].peRed   = sysPal[ i ].peRed;
   colorRamp[ error_index ].peGreen = sysPal[ i ].peGreen;
   colorRamp[ error_index ].peBlue  = sysPal[ i ].peBlue;
   colorRamp[ error_index ].peFlags = PC_EXPLICIT;
 
  }  // end for ( i . . . )

 }  // end else

 ResizePalette ( *pal, logPalSize );
 SetPaletteEntries ( *pal, 0, logPalSize, colorRamp );
 
 delete [] gRed;
 delete [] sysPal;

 return 1;

}  // end createColorRamp

int WNT_SysPalInUse ( void ) {

 return s_SysPalInUse;

}  // end WNT_SysPalInUse
