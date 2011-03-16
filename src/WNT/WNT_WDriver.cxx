// File:        WNT_WDriver.cxx
// Created:     Mon Mar 17 17:28:42 1997
// Author:      EugenyPLOTNIKOV
// Modified:    MAR-98 (DCB) MFT
// DCB: SEP-98  Avoid memory crash when color indices do not follow
//              each other or do not begin with '1'
// Copyright:   Matra Datavision 1993

// include windows.h first to have all definitions available
#include <windows.h>

#define MFT // Study S3553

#define PRO17334	//GG_060199
//		SetXxxAttrib() must raise when index is wrong
//		Idem for DrawMarker()

#define PRO17381	//GG_080199
//			Protection : Do nothing when NULL string is found in TextSize

#define PRO19042        //GG_261199
//                      Don't clear buffer at begin draw .If not this clear
//                      the current buffer attribs and the resulting buffer is black

#include <WNT_WDriver.ixx>

#include <WNT_GraphicDevice.hxx>
#include <WNT_ImageManager.hxx>
#include <WNT_Image.hxx>

#include <Aspect_FontStyle.hxx>
#include <Aspect_FontMapEntry.hxx>
#include <Aspect_LineStyle.hxx>
#include <Aspect_TypeMapEntry.hxx>
#include <Aspect_TypeOfLine.hxx>
#include <Aspect_WidthMapEntry.hxx>
#include <Aspect_ColorMapEntry.hxx>
#include <Aspect_MarkMapEntry.hxx>

#include <TColStd_Array1OfBoolean.hxx>
#include <TColQuantity_Array1OfLength.hxx>
#include <Resource_Unicode.hxx>

#include <windowsx.h>

#include "W32_Allocator.hxx"

#define A    (  ( PW32_Allocator )myAllocator  )
#define SA   ( !( A -> myFlags & W32F_MONO ) ||                                     \
                (  ( A -> myFlags & W32F_MONO ) && ( A -> myFlags & W32F_MINIT )  ) \
             )

/*====================== MFT ====================*/
#ifdef MFT
#include <Aspect_Units.hxx>
#include "WNT_MFTDraw.hxx"
#endif
/*====================== MFT ====================*/

typedef struct _m_param {

                Aspect_MarkerStyle* s;
                int                 w;
                int                 h;

               } MPARAM, *PMPARAM;

typedef struct {

         float r;
         float g;
         float b;

        } PIXEL2D, *PPIXEL2D;

extern double  W32_TextFactor;
static PW32_Allocator __fastcall _FindAllocator ( Standard_Address, Standard_Integer );
static int            __fastcall _GetROP2       ( Aspect_TypeOfDrawMode              );
static char*          __fastcall _ConvertU2F    ( const TCollection_ExtendedString&  );

static void WINAPI _SetPoint ( int, int, int, LPPOINT, int*, void* );

WNT_WDriver :: WNT_WDriver ( const Handle( WNT_Window )& aWindow ) :
                Aspect_WindowDriver ( aWindow ) {

 WINDOW_DATA*     wd;
 Standard_Integer pXSize, pYSize;
 Quantity_Length  dXSize, dYSize;

 Handle( WNT_GraphicDevice ) gDev  = Handle ( WNT_GraphicDevice ) ::
                                      DownCast (  MyWindow -> GraphicDevice ()  );

 gDev -> DisplaySize ( pXSize, pYSize );
 gDev -> DisplaySize ( dXSize, dYSize );

 myWNTWindow   = Handle( WNT_Window ) :: DownCast ( MyWindow );
 myPixelToUnit = ( dXSize / pXSize + dYSize / pYSize ) / 2.0;
 myAllocators  = myAllocator = ( *W32_GetAllocator ) ( 0, NULL );
 MyDrawMode    = Aspect_TODM_REPLACE;

/*====================== MFT ====================*/
#ifdef MFT
 myTextManager = new WNT_TextManager (myPixelToUnit);
#endif
/*====================== MFT ====================*/

 wd = (WINDOW_DATA* )GetWindowLongPtr ((HWND )aWindow->HWindow(), GWLP_USERDATA);
 wd->WNT_WDriver_Ptr = (void* )this;

}  // end constructor

void WNT_WDriver :: Destroy () {

 do {

  myAllocator = (  ( PW32_Allocator )myAllocators  ) -> myNext;

  delete ( PW32_Allocator )myAllocators;

  myAllocators = ( PW32_Allocator )myAllocator;

 } while ( myAllocators != NULL );

}  // end WNT_WDriver :: Destroy

Standard_Address WNT_WDriver :: SelectBuffer (
                                 const Standard_Integer aRetainBuffer
                                ) const {

 PW32_Allocator a = _FindAllocator ( myAllocators, aRetainBuffer );

 if ( a != NULL ) *(  ( Standard_Address* )&myAllocator   ) = ( Standard_Address )a;

 return myAllocator;

}  // end WNT_WDriver :: SelectBuffer

void WNT_WDriver :: BeginDraw (
                     const Standard_Boolean aDoubleBuffer,
                     const Standard_Integer aRetainBuffer
                    ) {

 PW32_Allocator a = _FindAllocator ( myAllocators, aRetainBuffer );

 if ( a != NULL ) {

  if (  aDoubleBuffer ) {

   MyWindow -> SetDoubleBuffer ( aDoubleBuffer );
   a -> myFlags |= W32F_DBUFF;

  } else a -> myFlags &= ~W32F_DBUFF;

#ifdef PRO19042
  if( !aRetainBuffer )
#endif
  	a -> ClearBlocks ();
  a -> myFlags |=  W32F_START;
  a -> myFlags &= ~W32F_DVDRV;

  SelectBuffer ( aRetainBuffer );

 }  // end if

}  // end WNT_WDriver :: BeginDraw

void WNT_WDriver :: EndDraw ( const Standard_Boolean aSynchronize ) {

 (  ( PW32_Allocator )myAllocator  ) -> myFlags &= ~W32F_START;

 DrawBuffer (   (  ( PW32_Allocator )myAllocator  ) -> myID   );

}  // end WNT_WDriver :: EndDraw

Aspect_TypeOfResize WNT_WDriver :: ResizeSpace () {

 return myWNTWindow -> DoResize ();

}  // end WNT_WDriver :: ResizeSpace

Standard_Boolean WNT_WDriver :: SetHighlightColor (
                                 const Standard_Integer aColorIdx
                                ) const {

 Handle( WNT_GraphicDevice ) gDev = Handle( WNT_GraphicDevice ) ::
                                     DownCast (  MyWindow -> GraphicDevice ()  );

 gDev -> SetColor (
          ColorMap () -> Entry ( aColorIdx + 1 ).Color (), Standard_True
         );

 return Standard_True;

}  // end WNT_WDriver :: SetHighlightColor

void WNT_WDriver :: SetDrawMode ( const Aspect_TypeOfDrawMode aMode ) {

 MyDrawMode = aMode;

}  // end WNT_WDriver :: SetDrawMode

void WNT_WDriver :: SetLineAttrib (
                     const Standard_Integer ColorIndex,
                     const Standard_Integer TypeIndex,
                     const Standard_Integer WidthIndex
                    )
{
#ifdef PRO17334
  if( myColors.IsNull() ||
	ColorIndex < myColors->Lower() || ColorIndex > myColors->Upper() )
                	Aspect_DriverError::Raise ("Bad Color Index") ;
  if( myTypeIdxs.IsNull() ||
	TypeIndex < myTypeIdxs->Lower() || TypeIndex > myTypeIdxs->Upper() )
                	Aspect_DriverError::Raise ("Bad Type Index") ;
  if( myWidthIdxs.IsNull() ||
	WidthIndex < myWidthIdxs->Lower() || WidthIndex > myWidthIdxs->Upper() )
                	Aspect_DriverError::Raise ("Bad Width Index") ;

  Aspect_LineStyle    Entry = TypeMap () -> Entry ( myTypeIdxs -> Value(TypeIndex) ).Type ();
#else
  Standard_Integer theIndex = TypeIndex;
  if (TypeIndex < myTypeIdxs->Lower() || TypeIndex > myTypeIdxs->Upper())
    theIndex = myTypeIdxs->Lower();

  Aspect_LineStyle    Entry = TypeMap () -> Entry ( myTypeIdxs -> Value(theIndex) ).Type ();
#endif
  Standard_Integer    type  = ((Entry.Style() == Aspect_TOL_SOLID) ? 0 : PS_USERSTYLE);
  double              width = ( double )ProcessWidthIndex ( WidthIndex );
  Standard_Boolean    fUser = ( type & PS_USERSTYLE );
  LOGBRUSH            lb    = { BS_SOLID, ProcessColorIndex ( ColorIndex ), 0 };
  if ( fUser ) {
    Standard_Integer Lower   = Entry.Values ().Lower ();
    Standard_Integer Count   = Entry.Values ().Length ();
    PDWORD           dwStyle = ( PDWORD )HeapAlloc (
                                          GetProcessHeap (),
                                          HEAP_GENERATE_EXCEPTIONS,
                                          Count * sizeof ( DWORD )
                                         );
     for ( Standard_Integer i = 0; i < Count; ++i ) {
       dwStyle[ i ] = DWORD (  Entry.Values ().Value ( i + Lower ) / myPixelToUnit + 0.5  );
       if ( dwStyle[ i ] == 0.0 ) dwStyle[ i ] = 1;
     }  // end for
     A -> LineAttrib (  DWORD( width + 0.5 ), &lb, Count, dwStyle  );
    HeapFree (  GetProcessHeap (), 0, ( LPVOID )dwStyle  );
  } else A -> LineAttrib (  DWORD( width + 0.5 ), &lb  );
}  // end WNT_WDriver :: SetLineAttrib

void WNT_WDriver :: SetTextAttrib (
                     const Standard_Integer ColorIndex,
                     const Standard_Integer FontIndex
                    )
{
  SetTextAttrib ( ColorIndex, FontIndex, 0.0, 1.0, 0.0,Standard_False );
}  // end WNT_WDriver :: SetTextAttrib

void WNT_WDriver :: SetTextAttrib (
                     const Standard_Integer    ColorIndex,
                     const Standard_Integer    FontIndex,
                     const Quantity_PlaneAngle aSlant,
                     const Quantity_Factor     aHScale,
                     const Quantity_Factor     aWScale,
                     const Standard_Boolean    isUnderlined
                    )
{
#ifdef PRO17334
  if( myColors.IsNull() ||
	ColorIndex < myColors->Lower() || ColorIndex > myColors->Upper() )
                	Aspect_DriverError::Raise ("Bad Color Index") ;
  if( myFonts.IsNull() ||
	FontIndex < myFonts->Lower() || FontIndex > myFonts->Upper() )
                	Aspect_DriverError::Raise ("Bad Font Index") ;
#endif
   HFONT                      hFont;
   LOGFONT                    lf;
   BOOL                       fFree = FALSE;
   Handle( WNT_FontMapEntry ) fmEntry = myFonts -> Value ( FontIndex );
  /*====================== MFT ====================*/
#ifdef MFT
   if ( UseMFT() ) {
     //============== Set some font attributes =============
     Handle(MFT_FontManager) theFontManager =  MFT_Font(FontIndex);
     Standard_ShortReal      theTextSize    =  MFT_Size(FontIndex);
     theFontManager->SetFontAttribs (
       Abs(theTextSize)*(aWScale > 0.0 ? aWScale : aHScale)*A->myScaleX,
       Abs(theTextSize)*aHScale                            *A->myScaleY,
       aSlant, 0., ((theTextSize > 0.) ? Standard_False : Standard_True)
     );
     A -> TextAttrib (
           (HFONT)FontIndex, ProcessColorIndex ( ColorIndex ), aSlant,
           aHScale, ( aWScale > 0.0 ? aWScale : aHScale ),
           isUnderlined, Standard_False, Standard_True
          );
   } else {
#endif
  /*====================== MFT ====================*/
     GetObject (
      hFont = ( HFONT )fmEntry-> HFont (),
      sizeof ( LOGFONT ), &lf
     );

     if ( isUnderlined && !lf.lfUnderline ) {
      FONT_DATA fd = { TRUE };
      hFont = ( HFONT )fmEntry -> SetAttrib ( faUnderlined, &fd );
      fFree = TRUE;
     }  // end if

     A -> TextAttrib (
       hFont, ProcessColorIndex ( ColorIndex ), aSlant,
       aHScale / W32_TextFactor,
       ( aWScale > 0.0 ? aWScale : aHScale ) / W32_TextFactor,
       isUnderlined, fFree
     );
#if 0
     char* str = (isUnderlined ? "UNDERLINED" : "GOD DAMN  ");
     HDC   hdc = GetDC ( ( HWND )myWNTWindow -> HWindow () );
     HFONT hf  = SelectFont ( hdc, hFont );
     SetTextColor ( hdc, RGB (133, 211, 66) );
     SetBkColor   ( hdc, RGB (0, 0, 0) );
     Rectangle    ( hdc, 0, 0, 1220, 220 );
     SetBkMode    ( hdc, OPAQUE );
     TextOut      ( hdc, 0, 0, str, lstrlen(str) );
     SelectFont   ( hdc, hf );
     ReleaseDC    ( ( HWND )myWNTWindow -> HWindow (), hdc );
     MessageBox   ( ( HWND )myWNTWindow -> HWindow (), str, "Son of a BITCH", MB_OK );
#endif
  /*====================== MFT ====================*/
#ifdef MFT
   }
#endif
  /*====================== MFT ====================*/
}  // end WNT_WDriver :: SetTextAttrib

void WNT_WDriver :: SetPolyAttrib (
                     const Standard_Integer ColorIndex,
                     const Standard_Integer TileIndex,
                     const Standard_Boolean DrawEdgeFlag
                    )
{
#ifdef PRO17334
  if( myColors.IsNull() ||
	ColorIndex < myColors->Lower() || ColorIndex > myColors->Upper() )
                	Aspect_DriverError::Raise ("Bad Color Index") ;
#endif
  if ( !SA ) return;
  LOGBRUSH lb = { TileIndex < 0 ? BS_NULL : BS_SOLID, ProcessColorIndex ( ColorIndex ), 0 };
  A -> PolyAttrib ( &lb, DrawEdgeFlag );
}  // end WNT_WDriver :: SetPolyAttrib

void WNT_WDriver :: SetPolyAttrib (
                     const Standard_Integer ColorIndex,
                     const Standard_Integer TileIndex,
                     const Standard_Integer PolygonMode,
                     const Standard_Boolean DrawEdgeFlag
                    )
{
#ifdef PRO17334
  if( myColors.IsNull() ||
	ColorIndex < myColors->Lower() || ColorIndex > myColors->Upper() )
                	Aspect_DriverError::Raise ("Bad Color Index") ;
#endif
  if ( !SA ) return;
  LOGBRUSH lb = { BS_SOLID, ProcessColorIndex ( ColorIndex ), 0 };
  A -> PolyAttrib ( &lb, DrawEdgeFlag, PolygonMode );
}  // end WNT_WDriver :: SetPolyAttrib

void WNT_WDriver :: SetMarkerAttrib (
                     const Standard_Integer ColorIndex,
                     const Standard_Integer EdgeWidthIndex,
                     const Standard_Boolean FillMarker
                    )
{
#ifdef PRO17334
  if( myColors.IsNull() ||
	ColorIndex < myColors->Lower() || ColorIndex > myColors->Upper() )
                	Aspect_DriverError::Raise ("Bad Color Index") ;
  if( myWidthIdxs.IsNull() ||
	EdgeWidthIndex < myWidthIdxs->Lower() || EdgeWidthIndex > myWidthIdxs->Upper() )
                	Aspect_DriverError::Raise ("Bad EdgeWidth Index") ;
#endif
  if ( SA )
    A -> MarkerAttrib (
          ProcessColorIndex ( ColorIndex ),
          DWORD(  ProcessWidthIndex ( EdgeWidthIndex ) + 0.5  ),
          FillMarker
         );
}  // end WNT_WDriver :: SetMarkerAttrib

Standard_Boolean WNT_WDriver :: IsKnownImage (
                                 const Handle( Standard_Transient )& anImage
                                ) {

 return myWNTWindow -> myImages -> Index (
                                    anImage -> HashCode(
                                                IntegerLast ()
                                               )
                                   ) ? Standard_True : Standard_False;

}  // end WNT_WDriver :: IsKnownImage

Standard_Boolean WNT_WDriver :: SizeOfImageFile (
                                 const Standard_CString  anImageFile,
                                       Standard_Integer&      aWidth,
                                       Standard_Integer&     aHeight
                                ) const {
#ifndef BUG	//Sets the size to 0 when the image file is not found.
 aWidth = 0; aHeight = 0;
#endif

 Standard_Integer i = myWNTWindow -> myImages -> Load ( anImageFile );

 if ( i ) myWNTWindow -> myImages -> Dim ( i, aWidth, aHeight );

 return i ? Standard_True : Standard_False;

}  // end WNT_WDriver :: SizeOfImageFile

void WNT_WDriver :: ClearImage (  const Handle( Standard_Transient )& anImageId  ) {

 Standard_Integer i = myWNTWindow -> myImages -> Index (
                                                  anImageId -> HashCode (
                                                                IntegerLast ()
                                                               )
                                                 );

 if ( i ) myWNTWindow -> myImages -> Discard ( i );

}  // end WNT_WDriver :: ClearImage

void WNT_WDriver :: ClearImageFile ( const Standard_CString anImageFile ) {

 Standard_Integer i = myWNTWindow -> myImages -> Load ( anImageFile );

 if ( i )  myWNTWindow -> myImages -> Discard ( i );

}  // end WNT_WDriver :: ClearImageFile

void WNT_WDriver :: DrawImage (
                     const Handle( Standard_Transient )& anImageId,
                     const Standard_ShortReal                   aX,
                     const Standard_ShortReal                   aY
                    ) {

 Standard_Integer i = myWNTWindow -> myImages -> Index (
                                                  anImageId -> HashCode (
                                                                IntegerLast ()
                                                               )
                                                 );

 if ( i ) A -> Image (
                P( aX ), P( aY ),
                ( PW32_Bitmap )myWNTWindow -> myImages -> Image ( i ) -> Image ()
               );

}  // end WNT_WDriver :: DrawImage

void WNT_WDriver :: DrawImageFile (
                     const Standard_CString   anImageFile,
                     const Standard_ShortReal          aX,
                     const Standard_ShortReal          aY,
                     const Quantity_Factor         aScale
                    ) {

 int i = myWNTWindow -> myImages -> Load ( anImageFile );

 if ( i ) {

  PW32_Bitmap pBmp;

  if (aScale != 1.0) {

   pBmp = ( PW32_Bitmap )HeapAlloc (
                          GetProcessHeap (),
                          HEAP_GENERATE_EXCEPTIONS,
                          sizeof ( W32_Bitmap )
                         );
   pBmp -> hBmp  = ( HBITMAP )myWNTWindow -> myImages -> Scale ( i, aScale, aScale );
   pBmp -> nUsed = 0;

  } else

   pBmp = ( PW32_Bitmap )myWNTWindow -> myImages -> Image ( i ) -> Image ();

  A -> Image (  P( aX ), P( aY ), pBmp  );

 }  // end if

}  // end WNT_WDriver :: DrawImageFile

void WNT_WDriver :: FillAndDrawImage (
                     const Handle(Standard_Transient)& anImageId,
                     const Standard_ShortReal                 aX,
                     const Standard_ShortReal                 aY,
                     const Standard_Integer                Width,
                     const Standard_Integer                Height,
                     const Standard_Address      anArrayOfPixels
                    ) {

 HDC                         hdc, hdcMem;
 HBITMAP                     hbo;
 HPALETTE                    hpo = NULL;
 Standard_Integer            i, h;
 Quantity_Color              color;
 PPIXEL2D                    p = ( PPIXEL2D )anArrayOfPixels;
 Handle( WNT_GraphicDevice ) gDev = Handle( WNT_GraphicDevice ) ::
                                     DownCast ( MyWindow -> GraphicDevice ()  );

 hdc = GetDC (  ( HWND )myWNTWindow -> HWindow ()  );
  hdcMem = CreateCompatibleDC ( hdc );

   if (  gDev -> IsPaletteDevice ()  )

    hpo = SelectPalette (  hdcMem, ( HPALETTE )gDev -> HPalette (), FALSE  );

    i = myWNTWindow -> myImages -> Index (
                                    h = anImageId -> HashCode (  IntegerLast ()  )
                                   );

    if ( i == 0 ) i = myWNTWindow -> myImages -> Open ( hdc, Width, Height, h );

    if ( Width > 0 && Height > 0 ) {

     Standard_Integer iX, iY;
     Standard_Real    sX, sY;

     myWNTWindow -> myImages -> Dim ( i, iX, iY );

     if ( iX != Width || iY != Height ) {

      sX = Width  / ( Standard_Real )iX;
      sY = Height / ( Standard_Real )iY;

      myWNTWindow -> myImages -> Scale ( i, sX, sY, Standard_True );

     }  // end if

     hbo = SelectBitmap(
            hdcMem, ( HBITMAP )myWNTWindow -> myImages -> ImageHandle ( i )
           );

      for ( int i = 0; i < Height; ++i )

       for ( int j = 0; j < Width;  ++j, ++p ) {

        color.SetValues ( p -> r, p -> g, p -> b, Quantity_TOC_RGB );
        SetPixel (  hdcMem, j, i, gDev -> SetColor ( color )  );

       }  // end for

     SelectBitmap( hdcMem, hbo );

    }  // end if

   if ( hpo != NULL ) SelectPalette ( hdcMem, hpo, FALSE );

  DeleteDC ( hdcMem );
 ReleaseDC (  ( HWND )myWNTWindow -> HWindow (), hdc  );

 DrawImage ( anImageId, aX, aY );

}  // end WNT_WDriver :: FillAndDrawImage

void WNT_WDriver :: FillAndDrawImage (
                     const Handle( Standard_Transient )& anImageId,
                     const Standard_ShortReal                   aX,
                     const Standard_ShortReal                   aY,
                     const Standard_Integer          anIndexOfLine,
                     const Standard_Integer                  Width,
                     const Standard_Integer                 Height,
                     const Standard_Address        anArrayOfPixels
                    ) {

 HDC                         hdc, hdcMem;
 HBITMAP                     hbo;
 HPALETTE                    hpo = NULL;
 Standard_Integer            i, h;
 Quantity_Color              color;
 PPIXEL2D                    p = ( PPIXEL2D )anArrayOfPixels;
 Handle( WNT_GraphicDevice ) gDev = Handle( WNT_GraphicDevice ) ::
                                     DownCast ( MyWindow -> GraphicDevice ()  );

 hdc = GetDC (  ( HWND )myWNTWindow -> HWindow ()  );
  hdcMem = CreateCompatibleDC ( hdc );

   if (  gDev -> IsPaletteDevice ()  )

    hpo = SelectPalette (  hdcMem, ( HPALETTE )gDev -> HPalette (), FALSE  );

    i = myWNTWindow -> myImages -> Index (
                                    h = anImageId -> HashCode (  IntegerLast ()  )
                                   );

    if ( i == 0 ) i = myWNTWindow -> myImages -> Open ( hdc, Width, Height, h );

    if ( Width > 0 && Height > 0 ) {

     Standard_Integer iX, iY;
     Standard_Real    sX, sY;

     myWNTWindow -> myImages -> Dim ( i, iX, iY );

     if ( iX != Width || iY != Height ) {

      sX = Width  / ( Standard_Real )iX;
      sY = Height / ( Standard_Real )iY;

      myWNTWindow -> myImages -> Scale ( i, sX, sY, Standard_True );

     }  // end if

     hbo = SelectBitmap(
            hdcMem, ( HBITMAP )myWNTWindow -> myImages -> ImageHandle ( i )
           );

      for ( int j = 0; j < Width; ++j, ++p ) {

       color.SetValues ( p -> r, p -> g, p -> b, Quantity_TOC_RGB );
       SetPixel (  hdcMem, j, anIndexOfLine, gDev -> SetColor ( color )  );

      }  // end for

     SelectBitmap( hdcMem, hbo );

    }  // end if

   if ( hpo != NULL ) SelectPalette ( hdcMem, hpo, FALSE );

  DeleteDC ( hdcMem );
 ReleaseDC (  ( HWND )myWNTWindow -> HWindow (), hdc  );

 if ( anIndexOfLine == Height - 1 ) DrawImage ( anImageId, aX, aY );

}  // end WNT_WDriver :: FillAndDrawImage

void WNT_WDriver :: DrawPolyline (
                     const TShort_Array1OfShortReal& ListX,
                     const TShort_Array1OfShortReal& ListY
                    ) {

 Standard_Integer nPts = ListX.Length ();

 if (  nPts != ListY.Length ()  )

  Aspect_DriverError :: Raise ( "DrawPolyline: incorrect argument" );

 if ( nPts > 0 ) {

  Standard_Integer lX = ListX.Lower ();
  Standard_Integer lY = ListY.Lower ();

  if ( A -> myPrimitive != zzNone ) ClosePrimitive ();

  W32_PolygonNote* p = ( W32_PolygonNote* )(  A -> Polyline ( nPts )  );

  for ( Standard_Integer i = 0; i < nPts; ++i )

   p -> Add (
         P(  ListX.Value ( lX + i )  ),
         P(  ListY.Value ( lY + i )  )
        );

 }  // end if

}  // end WNT_WDriver :: DrawPolyline

void WNT_WDriver :: DrawPolygon (
                     const TShort_Array1OfShortReal& ListX,
                     const TShort_Array1OfShortReal& ListY
                    ) {

 Standard_Integer nPts = ListX.Length ();

 if (  nPts != ListY.Length ()  )

  Aspect_DriverError :: Raise ( "DrawPolygon: incorrect argument" );

 if ( nPts > 0 ) {

  Standard_Integer lX = ListX.Lower ();
  Standard_Integer lY = ListY.Lower ();

  if ( A -> myPrimitive != zzNone ) ClosePrimitive ();

  W32_PolygonNote* p = ( W32_PolygonNote* )(  A -> Polygon ( nPts )  );

  for ( Standard_Integer i = 0; i < nPts; ++i )

   p -> Add (
         P(  ListX.Value ( lX + i )  ),
         P(  ListY.Value ( lY + i )  )
        );

 }  // end if

}  // end WNT_WDriver :: DrawPolygon

void WNT_WDriver :: DrawSegment (
                     const Standard_ShortReal X1,
                     const Standard_ShortReal Y1,
                     const Standard_ShortReal X2,
                     const Standard_ShortReal Y2
                    ) {

 A -> Line (  P( X1 ), P( Y1 ), P( X2 ), P( Y2 )  );

}  // end WNT_WDriver :: DrawSegment


void WNT_WDriver :: DrawText (
                     const TCollection_ExtendedString& Text,
                     const Standard_ShortReal          Xpos,
                     const Standard_ShortReal          Ypos,
                     const Standard_ShortReal       anAngle,
                     const Aspect_TypeOfText          aType
                    )
{
/*====================== MFT ====================*/
#ifdef MFT
  if ( UseMFT() ) {
    MFT_DRAWTEXT   dt = {
      myAllocator, this, myPixelToUnit,
      Xpos, Ypos, anAngle, 0., (int)aType,
      FALSE, TRUE, Text.Length(), NULL
    };
    CopyMemory (dt.theText, Text.ToExtString(),
                Text.Length()*sizeof(short));
    A -> FunCall ( _Do_MFTDrawText_, sizeof(MFT_DRAWTEXT),
                   PW32_FCALLPARAM(&dt) );
  } else {
#endif
/*====================== MFT ====================*/
    char* str = _ConvertU2F ( Text );
    A -> Text (
           P( Xpos ), P( Ypos ),
           anAngle, str, FALSE,
           ( aType == Aspect_TOT_SOLID ) ? FALSE : TRUE
         );
    HeapFree (  GetProcessHeap (), 0, str  );
/*====================== MFT ====================*/
#ifdef MFT
  }
#endif
/*====================== MFT ====================*/
}  // end WNT_WDriver :: DrawText


void WNT_WDriver :: DrawText (
                     const Standard_CString      Text,
                     const Standard_ShortReal    Xpos,
                     const Standard_ShortReal    Ypos,
                     const Standard_ShortReal anAngle,
                     const Aspect_TypeOfText    aType
                    )
{
/*====================== MFT ====================*/
#ifdef MFT
  if ( UseMFT() ) {
    MFT_DRAWTEXT   dt = {
      myAllocator, this, myPixelToUnit,
      Xpos, Ypos, anAngle, 0., (int)aType,
      FALSE, FALSE, lstrlen(Text), NULL
    };
    CopyMemory (dt.theText, Text, lstrlen(Text));
    A -> FunCall ( _Do_MFTDrawText_, sizeof(MFT_DRAWTEXT),
                   PW32_FCALLPARAM(&dt) );
  } else
#endif
/*====================== MFT ====================*/
    A -> Text (
           P( Xpos ), P( Ypos ), anAngle, (Standard_Address)Text, FALSE,
           ( aType == Aspect_TOT_SOLID ) ? FALSE : TRUE
         );
}  // end WNT_WDriver :: DrawText

void WNT_WDriver :: DrawPolyText (
                     const TCollection_ExtendedString& aText,
                     const Standard_ShortReal           Xpos,
                     const Standard_ShortReal           Ypos,
                     const Quantity_Ratio             aMarge,
                     const Standard_ShortReal        anAngle,
                     const Aspect_TypeOfText           aType
                    )
{
/*====================== MFT ====================*/
#ifdef MFT
  if ( UseMFT() ) {
    MFT_DRAWTEXT   dt = {
      myAllocator, this, myPixelToUnit,
      Xpos, Ypos, anAngle, aMarge, (int)aType,
      TRUE, TRUE, aText.Length(), NULL
    };
    CopyMemory (dt.theText, aText.ToExtString(),
                aText.Length()*sizeof(short));
    A -> FunCall ( _Do_MFTDrawText_, sizeof(MFT_DRAWTEXT),
                   PW32_FCALLPARAM(&dt) );
  } else {
#endif
/*====================== MFT ====================*/
    char* str = _ConvertU2F ( aText );
    A -> Polytext (
           P( Xpos ), P( Ypos ), anAngle, aMarge, str, FALSE,
           ( aType == Aspect_TOT_SOLID ) ? FALSE : TRUE
         );
    HeapFree (  GetProcessHeap (), 0, str  );
/*====================== MFT ====================*/
#ifdef MFT
  }
#endif
/*====================== MFT ====================*/
}  // end WNT_WDriver :: DrawPolyText

void WNT_WDriver :: DrawPolyText (
                     const Standard_CString     aText,
                     const Standard_ShortReal    Xpos,
                     const Standard_ShortReal    Ypos,
                     const Quantity_Ratio      aMarge,
                     const Standard_ShortReal anAngle,
                     const Aspect_TypeOfText    aType
                    )
{
/*====================== MFT ====================*/
#ifdef MFT
  if ( UseMFT() ) {
    MFT_DRAWTEXT   dt = {
      myAllocator, this, myPixelToUnit,
      Xpos, Ypos, anAngle, aMarge, (int)aType,
      TRUE, FALSE, lstrlen(aText), NULL
    };
    CopyMemory (dt.theText, aText, lstrlen(aText));
    A -> FunCall ( _Do_MFTDrawText_, sizeof(MFT_DRAWTEXT),
                   PW32_FCALLPARAM(&dt) );
  } else
#endif
/*====================== MFT ====================*/
    A -> Polytext (
           P( Xpos ), P( Ypos ), anAngle, aMarge, (Standard_Address)aText, FALSE,
           ( aType == Aspect_TOT_SOLID ) ? FALSE : TRUE
         );

}  // end WNT_WDriver :: DrawPolyText

void WNT_WDriver :: DrawPoint (
                     const Standard_ShortReal X,
                     const Standard_ShortReal Y
                    )
{
  switch ( A -> myPrimitive ) {
    case zzPolygon:
      (  ( W32_PolygonNote* )( A -> myNote )  ) -> Add (  P( X ), P( Y )  );
      break;
    case zzPolyline:
      (  ( W32_PolylineNote* )( A -> myNote )  ) -> Add (  P( X ), P( Y )  );
      break;
   default:
      A -> Point (  P( X ), P( Y )  );
  }  // end switch
}  // end WNT_WDriver :: DrawPoint

void WNT_WDriver :: DrawMarker (
                     const Standard_Integer   aMarker,
                     const Standard_ShortReal    Xpos,
                     const Standard_ShortReal    Ypos,
                     const Standard_ShortReal   Width,
                     const Standard_ShortReal  Height,
                     const Standard_ShortReal   Angle
                    ) {

 if ( A -> myPrimitive != zzNone && A -> myPrimitive != zzMarker ) ClosePrimitive ();

 int aWidth  = P( Width  );
 int aHeight = P( Height );

 Standard_Integer theIndex = -1;
#ifdef PRO17334
 if( !myMarkerIdxs.IsNull() &&
	 (aMarker >= myMarkerIdxs->Lower() && aMarker <= myMarkerIdxs->Upper()) )
   theIndex = myMarkerIdxs->Value(aMarker);
 if( theIndex < 0 ) Aspect_DriverError::Raise ("Bad Marker Index") ;

 if ( aWidth && aHeight && (aMarker > 0) ) {
#else
 if (aMarker >= myMarkerIdxs->Lower() || aMarker <= myMarkerIdxs->Upper())
   theIndex = myMarkerIdxs->Value(aMarker);

 if ( aWidth && aHeight && (theIndex >= 0) ) {
#endif


  A -> BeginMarker (  P( Xpos ), P( Ypos ), aWidth, aHeight, Angle  );

  Aspect_MarkerStyle Style = MarkMap () -> Entry ( theIndex ).Style ();

  int MarkerSize = Style.Length ();
  int DrawCount  = 0;
  int PrevPoint  = 0;
  int i;
  MPARAM mp = { &Style, aWidth, aHeight };

  for ( i = 2; i <= MarkerSize; ++i ) {

   if (  Style.SValues ().Value ( i )  ) {

    if (  Style.SValues ().Value ( i - 1 )  )

     ++DrawCount;

    else {

     PrevPoint = i - 1;
     DrawCount = 2;

    }  // end else

   } else {

    if (  Style.SValues().Value ( i - 1 )  ) {

     if (   (  Style.XValues ().Value ( i ) == Style.XValues ().Value ( PrevPoint )  ) &&
            (  Style.YValues ().Value ( i ) == Style.YValues ().Value ( PrevPoint )  )
     )

      A -> PolyMarker2 ( DrawCount, &_SetPoint, PrevPoint, &mp );

     else

      A -> PolyMarker1 ( DrawCount, &_SetPoint, PrevPoint, &mp );

    }  // end if

    DrawCount = 0;

   }  // end else

  }  // end for

  --i;

  if ( DrawCount ) {

   if (   (  Style.XValues ().Value ( i ) == Style.XValues ().Value ( PrevPoint )  ) &&
          (  Style.YValues ().Value ( i ) == Style.YValues ().Value ( PrevPoint )  )
   )

    A -> PolyMarker2 ( DrawCount, &_SetPoint, PrevPoint, &mp );

   else

    A -> PolyMarker1 ( DrawCount, &_SetPoint, PrevPoint, &mp );

   }  // end if

  A -> EndMarker ();

 } else A -> MarkerPoint (  P( Xpos ), P( Ypos )  );

}  // end WNT_WDriver :: DrawMarker

Standard_Boolean WNT_WDriver :: DrawArc (
                                 const Standard_ShortReal X,
                                 const Standard_ShortReal Y,
                                 const Standard_ShortReal aXradius,
                                 const Standard_ShortReal aYradius,
                                 const Standard_ShortReal aStartAngle,
                                 const Standard_ShortReal anOpenAngle
                                ) {

 if ( A -> myPrimitive != zzNone && A -> myPrimitive != zzArc )

  ClosePrimitive ();

 if ( anOpenAngle >= 6.28318 )

  A -> Ellipse (  P( X ), P( Y ), P( aXradius ), P( aYradius )  );

 else

  A -> Arc (
        P( X ), P( Y ), P( aXradius ), P( aYradius ),
        aStartAngle, anOpenAngle
       );

 return Standard_True;

}  // end WNT_WDriver :: DrawArc

Standard_Boolean WNT_WDriver :: DrawPolyArc (
                                 const Standard_ShortReal X,
                                 const Standard_ShortReal Y,
                                 const Standard_ShortReal anXradius,
                                 const Standard_ShortReal anYradius,
                                 const Standard_ShortReal aStartAngle,
                                 const Standard_ShortReal anOpenAngle

                                ) {

 if ( A -> myPrimitive != zzNone && A -> myPrimitive != zzPolySector )

  ClosePrimitive ();

 if ( anOpenAngle >= 6.28318 )

  A -> PolyEllipse (  P( X ), P( Y ), P( anXradius ), P( anYradius )  );

 else

  A -> PolySector (
        P( X ), P( Y ), P( anXradius ), P( anYradius ),
        aStartAngle, anOpenAngle
       );

 return Standard_True;

}  // end WNT_WDriver :: DrawPolyArc

void WNT_WDriver :: BeginPolyline ( const Standard_Integer aNumber ) {

 A -> myPrimitive = zzPolyline;
 A -> myNote      = A -> Polyline ( aNumber );

}  // end WNT_WDriver :: BeginPolyline

void WNT_WDriver :: BeginPolygon ( const Standard_Integer aNumber ) {

 A -> myPrimitive = zzPolygon;
 A -> myNote      = A -> Polygon ( aNumber );

}  // end WNT_WDriver :: BeginPolygon

void WNT_WDriver :: BeginSegments () {

 A -> myPrimitive = zzLine;
 A -> myNote      = NULL;

}  // end WNT_WDriver :: BeginSegments

void WNT_WDriver :: BeginArcs () {

 A -> myPrimitive = zzArc;
 A -> myNote      = NULL;

}  // end WNT_WDriver :: BeginArcs

void WNT_WDriver :: BeginPolyArcs () {

 A -> myPrimitive = zzPolySector;
 A -> myNote      = NULL;

}  // end WNT_WDriver :: BeginPolyArcs

void WNT_WDriver :: BeginMarkers () {

 A -> myPrimitive = zzMarker;
 A -> myNote      = NULL;

}  // end WNT_WDriver :: BeginMarkers

void WNT_WDriver :: BeginPoints () {

 A -> myPrimitive = zzPoint;
 A -> myNote      = NULL;

}  // end WNT_WDriver :: BeginPoints

void WNT_WDriver :: ClosePrimitive () {

 A -> myPrimitive = zzNone;
 A -> myNote      = NULL;

}  // end WNT_WDriver :: ClosePrimitive

void WNT_WDriver :: InitializeColorMap (  const Handle( Aspect_ColorMap) & Colormap  )
{
  Handle( WNT_GraphicDevice ) gDev = Handle( WNT_GraphicDevice ) ::
                                       DownCast (  MyWindow -> GraphicDevice ()  );

  Aspect_ColorMapEntry entry;
  Standard_Integer i, minindex = IntegerLast(), maxindex = -minindex;
  for (i = 1; i <= Colormap->Size (); i++) {
    entry = Colormap->Entry (i);
    maxindex = Max(maxindex, entry.Index ());
    minindex = Min(minindex, entry.Index ());
  }

  if ( myColors.IsNull () || myColors -> Length () != Colormap -> Size () )
    myColors = new WNT_HColorTable ( minindex, maxindex, (WNT_ColorRef)0x00000000 );

  gDev -> MapColors ( Colormap, myColors );
}  // end WNT_WDriver :: InitializeColorMap

void WNT_WDriver :: InitializeTypeMap (  const Handle( Aspect_TypeMap )& Typemap  )
{
  Aspect_TypeMapEntry entry;
  Standard_Integer i, minindex = IntegerLast(), maxindex = -minindex;
  for (i = 1; i <= Typemap->Size (); i++) {
    entry = Typemap->Entry (i);
    maxindex = Max(maxindex, entry.Index ());
    minindex = Min(minindex, entry.Index ());
  }

  myTypeIdxs = new TColStd_HArray1OfInteger (minindex, maxindex, 1);
  for (i = 1; i <= Typemap->Size (); i++) {
    entry = Typemap->Entry (i);
    myTypeIdxs -> SetValue (entry.Index(), i);
  }
}  // end WNT_WDriver :: InitializeTypeMap

void WNT_WDriver :: InitializeWidthMap (  const Handle( Aspect_WidthMap )& Widthmap  )
{
  Aspect_WidthMapEntry entry;
  Standard_Integer i, minindex = IntegerLast(), maxindex = -minindex;
  for (i = 1; i <= Widthmap->Size (); i++) {
    entry = Widthmap->Entry (i);
    maxindex = Max(maxindex, entry.Index ());
    minindex = Min(minindex, entry.Index ());
  }

  myWidthIdxs = new TColStd_HArray1OfInteger (minindex, maxindex, 1);
  for (i = 1; i <= Widthmap->Size (); i++) {
    entry = Widthmap->Entry (i);
    myWidthIdxs -> SetValue (entry.Index(), i);
  }
}  // end WNT_WDriver :: InitializeWidthMap

void WNT_WDriver :: InitializeFontMap (  const Handle( Aspect_FontMap )& Fontmap  )
{
  Aspect_FontStyle           fontStyle;
  Handle( WNT_FontMapEntry ) fmEntry;
  Standard_Character         left[ 260 ], right[ 250 ];
  Standard_PCharacter        pSize;
  Standard_Integer           height, mapSize = Fontmap -> Size ();
  HDC                        hdc = GetDC (  ( HWND )myWNTWindow -> HWindow ()  );
  HFONT                      hfo = SelectFont(  hdc, GetStockObject ( DEFAULT_GUI_FONT )  );
  Standard_Integer           i;
/*====================== MFT ====================*/
#ifdef MFT
  Aspect_FontMapEntry     entry;
  Aspect_FontStyle        style;
  Quantity_Length         size;
  Quantity_PlaneAngle     slant;
  TCollection_AsciiString aname;
  if ( UseMFT() ) {
    Handle(MFT_FontManager) theFontManager;
    myMFTFonts  = new WNT_HListOfMFTFonts (0, mapSize, theFontManager);
    myMFTSizes  = new TShort_HArray1OfShortReal (0, mapSize, 0.F);
  }
#endif
/*====================== MFT ====================*/
  myFonts = new WNT_HFontTable ( 0, mapSize - 1 );
  for ( i = 0; i < mapSize; ++i ) {
/*====================== MFT ====================*/
#ifdef MFT
    if ( UseMFT() ) {
      entry  = Fontmap->Entry(i+1);
      style  = entry.Type ();
      size   = (Quantity_Length) TOMILLIMETER (style.Size());
      slant  = style.Slant ();
      if (style.CapsHeight())  size = -size;
      aname  = style.AliasName ();
      Standard_Boolean found = MFT_FontManager::IsKnown (aname.ToCString());
      if (!found) {
        cout << " WNT_WDriver::SetFontMap can't find the MFT font name '"
             << aname << "', using 'Defaultfont'" << endl << flush;
        aname = TCollection_AsciiString("Defaultfont");
        found = Standard_True;
      }
      if (found) {
        Handle(MFT_FontManager) theFontManager;
        Aspect_FontStyle        theStyle = MFT_FontManager::Font (aname.ToCString());
        found = Standard_False;
        for (int k = 0; k < mapSize; k++) {
          theFontManager = myMFTFonts -> Value (k);
          if (!theFontManager.IsNull()) {
            if (theStyle == theFontManager->Font()) {
              found = Standard_True;
              break;
            }
          }
        }
        if (!found)
          theFontManager = new MFT_FontManager (aname.ToCString());
        theFontManager->SetFontAttribs (
          Abs(size), Abs(size), slant, 0., Standard_Boolean(size < 0.)
        );
        myMFTFonts->SetValue (i, theFontManager);
        myMFTSizes->SetValue (i, (Standard_ShortReal)size);
      }
    }
    if ( UseMFT()  && !myMFTFonts->Value(i).IsNull())
      continue;
#endif
/*====================== MFT ====================*/
    fontStyle = Fontmap -> Entry ( i + 1 ).Type ();
    height    = Standard_Integer (  fontStyle.Size () / myPixelToUnit );
    lstrcpyA ( right,"-*-*-*-*-*-*-*-*-*-*-*-*-" );
    switch (  fontStyle.Style ()  ) {
      case Aspect_TOF_HELVETICA:
        lstrcatA ( right,"Arial" );
        break;
      case Aspect_TOF_DEFAULT:
      case Aspect_TOF_COURIER:
        lstrcatA ( right, "Courier New" );
        break;
      case Aspect_TOF_TIMES:
        lstrcatA ( right,"Times New Roman" );
        break;
      case Aspect_TOF_USERDEFINED:
        lstrcpyA (  left, fontStyle.Value ()  );
        if (   strchr (  fontStyle.Value (), '-'  )   ) {
          height = atol (  pSize = strtok ( left, "-" )  );
          if ( height == 0 )
            height = Standard_Integer (  atof ( pSize ) / myPixelToUnit  );
          if ( height == 0 )
            height = Standard_Integer (  fontStyle.Size () / myPixelToUnit  );
          pSize  = left + lstrlenA ( left );
          *pSize = '-';
          lstrcpyA ( right, pSize );
        } else lstrcatA ( right,left );
    }  // end switch
    height = Standard_Integer ( height * W32_TextFactor );
    ltoa ( height, left, 10 );
    lstrcatA ( left, right );
    fmEntry = new WNT_FontMapEntry ( left );
    if (  fontStyle.CapsHeight ()  ) {
      double     r;
      TEXTMETRIC tm;
      SelectFont(  hdc, fmEntry -> HFont ()  );
      GetTextMetrics ( hdc, &tm );
      r = tm.tmAscent * tm.tmHeight / ( tm.tmAscent - tm.tmInternalLeading );
      lstrcpy (  right, strchr ( left, '-' )  );
      ltoa (  LONG( r + 0.5 ), left, 10  );
      lstrcatA ( left, right );
      fmEntry = new WNT_FontMapEntry ( left );
    }  // end if
    myFonts -> SetValue ( i, fmEntry );
  }  // end for
  SelectFont( hdc, hfo );
  ReleaseDC (  ( HWND )myWNTWindow -> HWindow (), hdc  );
}  // end WNT_WDriver :: InitializeFontMap

void WNT_WDriver :: InitializeMarkMap (  const Handle( Aspect_MarkMap )& Markmap  )
{
  Aspect_MarkMapEntry entry;
  Standard_Integer i, minindex = IntegerLast(), maxindex = -minindex;
  for (i = 1; i <= Markmap->Size (); i++) {
    entry = Markmap->Entry (i);
    maxindex = Max(maxindex, entry.Index ());
    minindex = Min(minindex, entry.Index ());
  }

  myMarkerIdxs = new TColStd_HArray1OfInteger (minindex, maxindex, -1);
  for (i = 1; i <= Markmap->Size (); i++) {
    entry = Markmap->Entry (i);
    myMarkerIdxs -> SetValue (entry.Index(), i);
  }
}  // end WNT_WDriver :: InitializeMarkMap

Standard_Address WNT_WDriver :: InternalOpenBuffer (
                                 const Standard_Integer      aRetainBuffer,
                                 const Standard_Boolean      aMono,
                                 const Standard_ShortReal    aPivotX,
                                 const Standard_ShortReal    aPivotY,
                                 const Standard_Integer      aColorIndex,
                                 const Standard_Integer      aWidthIndex,
                                 const Standard_Integer      aTypeIndex,
                                 const Standard_Integer      aFontIndex,
                                 const Aspect_TypeOfDrawMode aDrawMode
                                ) {

 PW32_Allocator        aNew     = NULL;
 Aspect_TypeOfDrawMode DrawMode = Aspect_TODM_REPLACE;

 MyDrawMode = aDrawMode;
 aNew       = _FindAllocator ( myAllocators, aRetainBuffer );

 if ( aNew != NULL )

  ClearBuffer ( aRetainBuffer );

 else aNew = ( *W32_GetAllocator ) (
                 aRetainBuffer, ( PW32_Allocator )myAllocators
                );

 aNew -> myPivot.x = P( aPivotX );
 aNew -> myPivot.y = P( aPivotY );

 if ( aRetainBuffer ) {

  Standard_Address aSave = myAllocator;
  myAllocator = ( Standard_Address )aNew;

   A -> myFlags |= ( W32F_MONO | W32F_MINIT );
   SetLineAttrib   ( aColorIndex, aTypeIndex, aWidthIndex     );
   SetTextAttrib   ( aColorIndex, aFontIndex                  );
   SetPolyAttrib   ( aColorIndex, -1, Standard_True           );
   SetMarkerAttrib ( aColorIndex, aWidthIndex, Standard_False );
   A -> myFlags &= ~W32F_MINIT;

  myAllocator = aSave;

 } else A -> myFlags &= ~W32F_MONO;

 return aNew;

}  // end WNT_WDriver :: InternalOpenBuffer

Standard_Boolean WNT_WDriver :: OpenBuffer (
                                 const Standard_Integer      aRetainBuffer,
                                 const Standard_ShortReal    aPivotX,
                                 const Standard_ShortReal    aPivotY,
                                 const Standard_Integer      aWidthIndex,
                                 const Standard_Integer      aColorIndex,
                                 const Standard_Integer      aFontIndex,
                                 const Aspect_TypeOfDrawMode aDrawMode
                                ) {
 return ( Standard_Boolean )InternalOpenBuffer (
                             aRetainBuffer, Standard_True,
                             aPivotX, aPivotY, aColorIndex, aWidthIndex,
                             0, aFontIndex, aDrawMode
                            );

}  // end WNT_WDriver :: OpenBuffer

Standard_Boolean WNT_WDriver :: OpenColorBuffer (
                                 const Standard_Integer      aRetainBuffer,
                                 const Standard_ShortReal    aPivotX,
                                 const Standard_ShortReal    aPivotY,
                                 const Standard_Integer      aWidthIndex,
                                 const Standard_Integer      aColorIndex,
                                 const Standard_Integer      aFontIndex,
                                 const Aspect_TypeOfDrawMode aDrawMode
                                ) {

 return ( Standard_Boolean )InternalOpenBuffer (
                             aRetainBuffer, Standard_False,
                             aPivotX, aPivotY, aColorIndex, aWidthIndex,
                             0, aFontIndex, aDrawMode
                            );

}  // end WNT_WDriver :: OpenColorBuffer

void WNT_WDriver :: CloseBuffer ( const Standard_Integer aRetainBuffer ) const {

 if ( aRetainBuffer ) {

  PW32_Allocator a = _FindAllocator ( myAllocators, aRetainBuffer );

  if ( a != NULL ) {

   PW32_Allocator b = ( PW32_Allocator )myAllocators;

   while ( b -> myNext != a ) b = b -> myNext;

   if ( a -> myFlags & W32F_DRAWN ) EraseBuffer ( aRetainBuffer );

   b -> myNext = a -> myNext;

   delete a;

  }  // end if

  SelectBuffer ( 0 );

 }  // end if

}  // end WNT_WDriver :: CloseBuffer

void WNT_WDriver :: ClearBuffer ( const Standard_Integer aRetainBuffer ) const {

  PW32_Allocator a = _FindAllocator ( myAllocators, aRetainBuffer );

  if ( a != NULL ) {

   if ( a -> myFlags & W32F_DRAWN ) EraseBuffer ( aRetainBuffer );

   a -> ClearBlocks ();

  }  // end if

}  // end WNT_WDriver :: ClearBuffer

void WNT_WDriver :: DrawBuffer ( const Standard_Integer aRetainBuffer ) const {

 PW32_Allocator a = _FindAllocator ( myAllocators, aRetainBuffer );

 if ( a != NULL ) {

  RECT     r;
  SIZE     sz;
  HPALETTE hpo = NULL;
  HDC      hdc = GetDC (  ( HWND )myWNTWindow -> HWindow ()  );

   Handle( WNT_GraphicDevice ) gDev  = Handle ( WNT_GraphicDevice ) ::
                                        DownCast (  MyWindow -> GraphicDevice ()  );

   if ( gDev -> IsPaletteDevice ()  )

    hpo = SelectPalette (  hdc, ( HPALETTE )gDev -> HPalette (), FALSE  );

   GetClientRect (  ( HWND )myWNTWindow -> HWindow (), &r  );
   sz.cx = r.right;
   sz.cy = r.bottom;

   if ( a -> myFlags & W32F_DBUFF ) {
    HDC hdcMem = CreateCompatibleDC ( hdc );
     HBITMAP hbo = SelectBitmap(  hdcMem, ( HBITMAP )myWNTWindow -> HPixmap ()  );
      HPALETTE hpo;

       if ( gDev -> IsPaletteDevice ()  )

        hpo = SelectPalette (  hdcMem, ( HPALETTE )gDev -> HPalette (), FALSE  );

        SetROP2 (  hdcMem, _GetROP2 ( MyDrawMode )  );

        a -> Play ( hdcMem, &sz );

        BitBlt (
         hdc, r.left, r.top, r.right + 1, r.bottom + 1,
         hdcMem, r.left, r.top, SRCCOPY
        );

       if ( gDev -> IsPaletteDevice ()  ) SelectPalette ( hdcMem, hpo, FALSE );

     SelectBitmap( hdcMem, hbo );
    DeleteDC ( hdcMem );
   } else {
    SetROP2 (  hdc, _GetROP2 ( MyDrawMode )  );
    a -> Play ( hdc, &sz );
   }  // end else

  if ( hpo != NULL ) SelectPalette( hdc, hpo, FALSE );

  ReleaseDC (  ( HWND )myWNTWindow -> HWindow (), hdc );

  a -> myFlags |= W32F_DRAWN;

 }  // end if

}  // end WNT_WDriver :: DrawBuffer

void WNT_WDriver :: EraseBuffer ( const Standard_Integer aRetainBuffer ) const {

 PW32_Allocator a = _FindAllocator ( myAllocators, aRetainBuffer );

 if (  a != NULL && ( a -> myFlags & W32F_DRAWN )  ) {

  RECT r;

  a -> URect ( &r );

  if (  !IsRectEmpty ( &r )  ) {

   if ( a -> myID != 0 ) {

    if ( MyWindow -> BackingStore ()  )

     MyWindow -> RestoreArea (
                  ( r.left  + r.right ) / 2, ( r.top  + r.bottom ) / 2,
                  r.right - r.left + 2,      r.bottom - r.top + 2
                 );

    else if (   (  ( PW32_Allocator )myAllocators  ) -> myFlags & W32F_DRAWN  )

     DrawBuffer ( 0 );

    else
clear:
     MyWindow -> ClearArea (
                  ( r.left  + r.right ) / 2, ( r.top  + r.bottom ) / 2,
                  r.right - r.left + 2,          r.bottom - r.top + 2
                 );

   } else goto clear;

  }  // end if

  a -> myFlags &= ~W32F_DRAWN;

 }  // end if

}  // end WNT_WDriver :: EraseBuffer

void WNT_WDriver :: MoveBuffer (
                     const Standard_Integer   aRetainBuffer,
                     const Standard_ShortReal       aPivotX,
                     const Standard_ShortReal       aPivotY
                    ) const {

 PW32_Allocator a = _FindAllocator ( myAllocators, aRetainBuffer );

 if ( a != NULL ) {

  BOOL fDrawn = a -> myFlags & W32F_DRAWN;

  if ( fDrawn ) EraseBuffer ( aRetainBuffer );

  a -> myMove.x = P( aPivotX ) - a -> myPivot.x;
  a -> myMove.y = P( aPivotY ) - a -> myPivot.y;

  if ( fDrawn ) DrawBuffer ( aRetainBuffer );

 }  // end if

}  // end WNT_WDriver :: MoveBuffer

void WNT_WDriver :: ScaleBuffer (
                     const Standard_Integer aRetainBuffer,
                     const Quantity_Factor        aScaleX,
                     const Quantity_Factor        aScaleY
                    ) const {

 PW32_Allocator a = _FindAllocator ( myAllocators, aRetainBuffer );

 if ( a != NULL ) {

  BOOL fDrawn = a -> myFlags & W32F_DRAWN;

  if ( fDrawn ) EraseBuffer ( aRetainBuffer );

  a -> myScaleX = aScaleX;
  a -> myScaleY = aScaleY;

  if ( fDrawn ) DrawBuffer ( aRetainBuffer );

 }  // end if

}  // end WNT_WDriver :: ScaleBuffer

void WNT_WDriver :: RotateBuffer (
                     const Standard_Integer    aRetainBuffer,
                     const Quantity_PlaneAngle       anAngle
                    ) const {

 PW32_Allocator a = _FindAllocator ( myAllocators, aRetainBuffer );

 if ( a != NULL ) {

  BOOL fDrawn = a -> myFlags & W32F_DRAWN;

  if ( fDrawn ) EraseBuffer ( aRetainBuffer );

  a -> myAngle = anAngle;

  if ( fDrawn ) DrawBuffer ( aRetainBuffer );

 }  // end if

}  // end WNT_WDriver :: RotateBuffer

void WNT_WDriver :: WorkSpace ( Quantity_Length& Width, Quantity_Length& Heigth ) const {

 RECT r;

 GetClientRect (   ( HWND )(  myWNTWindow -> HWindow ()  ), &r   );

 Width  = U( r.right  );
 Heigth = U( r.bottom );

}  // end WNT_WDriver :: WorkSpace

Quantity_Length WNT_WDriver :: Convert ( const Standard_Integer PV ) const {

 return U( PV );

}  // end WNT_WDriver :: Convert

Standard_Integer WNT_WDriver :: Convert ( const Quantity_Length DV ) const {

 return P( DV );

}  // end WNT_WDriver :: Convert

void WNT_WDriver :: Convert (
                     const Standard_Integer PX,
                     const Standard_Integer PY,
                     Quantity_Length&       DX,
                     Quantity_Length&       DY
                    ) const {

 RECT r;
 GetClientRect (  ( HWND )myWNTWindow -> HWindow (), &r  );

 DX = U( PX            );
 DY = U( r.bottom - PY );

}  // end WNT_WDriver :: Convert

void WNT_WDriver :: Convert (
                     const Quantity_Length DX,
                     const Quantity_Length DY,
                     Standard_Integer&     PX,
                     Standard_Integer&     PY
                    ) const {

 RECT r;
 GetClientRect (  ( HWND )myWNTWindow -> HWindow (), &r  );

 PX = P( DX );
 PY = r.bottom - P( DY );

}  // end WNT_WDriver :: Convert

Standard_Integer WNT_WDriver :: ProcessColorIndex (
                                 const Standard_Integer ColorIndex
                                ) const {

 if ( !SA ) return A -> myPointColor;

 if (  myColors.IsNull ()  ) return ( Standard_Integer )RGB( 0, 0, 0 );

 Handle( WNT_GraphicDevice ) gDev = Handle( WNT_GraphicDevice ) ::
                                     DownCast (  MyWindow -> GraphicDevice ()  );

 if ( ColorIndex <= 0 ) {

   if ((A -> myFlags & W32F_MONO) && (A -> myFlags & W32F_MINIT)) {
    return RGB(255, 255, 255);
   } else {
    LOGBRUSH lb;
    GetObject (  myWNTWindow -> HBackground(), sizeof ( LOGBRUSH ), &lb  );
    return lb.lbColor;
   }

 } else

  switch ( MyDrawMode ) {

   default                   :
   case Aspect_TODM_REPLACE  :

    if (ColorIndex < myColors->Lower() || ColorIndex > myColors->Upper())
      return myColors -> Value ( myColors->Lower() );
    return myColors -> Value ( ColorIndex );

   case Aspect_TODM_ERASE    :

    return gDev -> SetColor (  MyWindow -> Background ().Color ()  );

   case Aspect_TODM_XORLIGHT :
   case Aspect_TODM_XOR      :

    return gDev -> HighlightColor ();

  }  // end switch

}  // end WNT_WDriver :: ProcessColorIndex

Quantity_Length WNT_WDriver :: ProcessWidthIndex (
                                const Standard_Integer WidthIndex
                               ) const
{
  Standard_Integer theIndex = WidthIndex;
  if (WidthIndex < myWidthIdxs->Lower() || WidthIndex > myWidthIdxs->Upper())
    theIndex = myWidthIdxs->Lower();

  Aspect_WidthMapEntry Entry = WidthMap () -> Entry ( myWidthIdxs -> Value(theIndex) );
  return (  Entry.Type () == Aspect_WOL_THIN  ) ?
         ( Quantity_Length )0.0 : Entry.Width () / myPixelToUnit;
}  // end WNT_WDriver :: ProcessWidthIndex

Standard_Integer WNT_WDriver :: ProcessTypeIndex (
                                 const Standard_Integer TypeIndex
                                ) const{

 return (  TypeMap () -> Entry ( TypeIndex + 1 ).Type ().Style () == Aspect_TOL_SOLID  ) ?
        0 : PS_USERSTYLE;

}  // end WNT_WDriver :: ProcessTypeIndex

Standard_Boolean WNT_WDriver :: BufferIsOpen (
                                 const Standard_Integer aRetainBuffer
                                ) const {

 return ( _FindAllocator ( myAllocators, aRetainBuffer) != NULL ?
           Standard_True : Standard_False
 );

}  // end WNT_WDriver :: BufferIsOpen

Standard_Boolean WNT_WDriver :: BufferIsEmpty (
                                 const Standard_Integer aRetainBuffer
                                ) const {

 PW32_Allocator a = _FindAllocator ( myAllocators, aRetainBuffer );

 if ( a != NULL )

  return ( a -> myFlags & W32F_EMPTY ) ? Standard_True : Standard_False;

 return Standard_True;

}  // end WNT_WDriver :: BufferIsEmpty

Standard_Boolean WNT_WDriver :: BufferIsDrawn (
                                 const Standard_Integer aRetainBuffer
                                ) const {

 PW32_Allocator a = _FindAllocator ( myAllocators, aRetainBuffer );

 if ( a != NULL )

  return ( a -> myFlags & W32F_DRAWN ) ? Standard_True : Standard_False;

 return Standard_False;

}  // end WNT_WDriver :: BufferIsDrawn

void WNT_WDriver :: AngleOfBuffer (
                     const Standard_Integer aRetainBuffer,
                     Quantity_PlaneAngle&         anAngle
                    ) const {

 PW32_Allocator a = _FindAllocator ( myAllocators, aRetainBuffer );

 if ( a != NULL ) anAngle = a -> myAngle;

}  // end WNT_WDriver :: AngleOfBuffer

void WNT_WDriver :: ScaleOfBuffer (
                     const Standard_Integer aRetainBuffer,
                     Quantity_Factor&             aScaleX,
                     Quantity_Factor&             aScaleY
                    ) const {

 PW32_Allocator a = _FindAllocator ( myAllocators, aRetainBuffer );

 if ( a != NULL ) {

  aScaleX = a -> myScaleX;
  aScaleY = a -> myScaleY;

 }  // end if

}  // end WNT_WDriver :: ScaleOfBuffer

void WNT_WDriver :: PositionOfBuffer (
                     const Standard_Integer aRetainBuffer,
                     Standard_ShortReal&          aPivotX,
                     Standard_ShortReal&          aPivotY
                    ) const {

 PW32_Allocator a = _FindAllocator ( myAllocators, aRetainBuffer );

 if ( a != NULL ) {

  aPivotX = Standard_ShortReal (  U( a -> myPivot.x + a -> myMove.x )  );
  aPivotY = Standard_ShortReal (  U( a -> myPivot.y + a -> myMove.y )  );

 }  // end if

}  // end WNT_WDriver :: PositionOfBuffer

void WNT_WDriver :: TextSize (
                     const TCollection_ExtendedString&  aText,
                     Standard_ShortReal&               aWidth,
                     Standard_ShortReal&              aHeight,
                     const Standard_Integer        aFontIndex
                    ) const {
#ifdef PRO17381
if( aText.Length() <= 0 ) {
  aWidth = aHeight = 0.;
  return;
}
#endif

 Standard_ShortReal xoffset, yoffset;
/*====================== MFT ====================*/
#ifdef MFT
 if ( UseMFT() )
   TextSize (aText, aWidth, aHeight, xoffset, yoffset, aFontIndex);
 else {
#endif
/*====================== MFT ====================*/

 char* str = _ConvertU2F ( aText );

  TextSize ( str, aWidth, aHeight, xoffset, yoffset, aFontIndex );

 HeapFree (  GetProcessHeap (), 0, str  );

/*====================== MFT ====================*/
#ifdef MFT
 }
#endif
/*====================== MFT ====================*/
}  // end WNT_WDriver :: TextSize

void WNT_WDriver :: TextSize (
                     const TCollection_ExtendedString&  aText,
                     Standard_ShortReal&               aWidth,
                     Standard_ShortReal&              aHeight,
                     Standard_ShortReal&            anXoffset,
                     Standard_ShortReal&            anYoffset,
                     const Standard_Integer        aFontIndex
                    ) const {
#ifdef PRO17381
if( aText.Length() <= 0 ) {
  aWidth = aHeight = anXoffset = anYoffset = 0.;
  return;
}
#endif

/*====================== MFT ====================*/
#ifdef MFT
 if ( UseMFT() ) {
   Standard_Integer aFont = (aFontIndex < 0 ?
     (int)A->myTextFont : aFontIndex);
   Handle(MFT_FontManager) theFontManager =
     ((WNT_WDriver *const)(this))->MFT_Font(aFont);
   Standard_Real theWidth = 0.,theAscent = 0.,
                 theLbearing = 0.,theDescent = 0.;
   if (aText.IsAscii()) {
     TCollection_AsciiString ascii (aText,'?');
     theFontManager->TextSize(ascii.ToCString(), theWidth,
                              theAscent, theLbearing, theDescent);
   } else if (theFontManager->IsComposite()) {
     theFontManager->TextSize(aText.ToExtString(), theWidth,
                              theAscent, theLbearing, theDescent);
   }
   theDescent = Max(
     theDescent, (A->myFlags & W32F_TULIN ?
                  theFontManager->UnderlinePosition() : 0.));
   aWidth    = Standard_ShortReal (theWidth              );
   aHeight   = Standard_ShortReal (theAscent + theDescent);
   anXoffset = Standard_ShortReal (theLbearing           );
   anYoffset = Standard_ShortReal (theDescent            );
 } else {
#endif
 char* str = ( char* )_ConvertU2F ( aText );

 HDC   hdc;
 HFONT hfo;
 SIZE  sz;
 int   xo;

 hdc = GetDC (  ( HWND )myWNTWindow -> HWindow ()  );
  hfo = SelectFont(
         hdc,
         aFontIndex < 0 ? A -> myTextFont : ( HFONT )myFonts -> Value ( aFontIndex ) -> HFont ()
        );

   xo      = A -> TextSize      ( hdc, str, &sz );
   aWidth  = Standard_ShortReal ( U( sz.cx ) );
   aHeight = Standard_ShortReal ( U( sz.cy ) );

   anXoffset = Standard_ShortReal ( U( xo ) );
   anYoffset = Standard_ShortReal ( 0.0     );

  SelectFont( hdc, hfo );
 ReleaseDC (  ( HWND )myWNTWindow -> HWindow (), hdc  );

 HeapFree (  GetProcessHeap (), 0, str  );

/*====================== MFT ====================*/
#ifdef MFT
 }
#endif
/*====================== MFT ====================*/
}  // end WNT_WDriver :: TextSize

void WNT_WDriver :: TextSize (
                     const Standard_CString      aText,
                     Standard_ShortReal&        aWidth,
                     Standard_ShortReal&       aHeight,
                     Standard_ShortReal&     anXoffset,
                     Standard_ShortReal&     anYoffset,
                     const Standard_Integer aFontIndex
                    ) const {
#ifdef PRO17381
if( !aText || (strlen(aText) <= 0) ) {
  aWidth = aHeight = anXoffset = anYoffset = 0.;
  return;
}
#endif

/*====================== MFT ====================*/
#ifdef MFT
 if ( UseMFT() ) {
   TCollection_ExtendedString extText (aText);
   TextSize (extText, aWidth, aHeight, anXoffset, anYoffset, aFontIndex);
 } else {
#endif
/*====================== MFT ====================*/
 HDC   hdc;
 HFONT hfo;
 SIZE  sz;
 int   xo;

 hdc = GetDC (  ( HWND )myWNTWindow -> HWindow ()  );
  hfo = SelectFont(
         hdc,
         aFontIndex < 0 ? A -> myTextFont : ( HFONT )myFonts -> Value ( aFontIndex ) -> HFont ()
        );

   xo      = A -> TextSize      ( hdc, (Standard_PCharacter)aText, &sz );
   aWidth  = Standard_ShortReal ( U( sz.cx ) );
   aHeight = Standard_ShortReal ( U( sz.cy ) );

   anXoffset = Standard_ShortReal ( U( xo ) );
   anYoffset = Standard_ShortReal ( 0.0     );

  SelectFont( hdc, hfo );
 ReleaseDC (  ( HWND )myWNTWindow -> HWindow (), hdc  );
/*====================== MFT ====================*/
#ifdef MFT
 }
#endif
/*====================== MFT ====================*/
}  // end WNT_WDriver :: TextSize

Standard_CString WNT_WDriver :: FontSize (
                                 Quantity_PlaneAngle&      aSlant,
                                 Standard_ShortReal&        aSize,
                                 Standard_ShortReal&      aBheight,
                                 const Standard_Integer aFontIndex
                                ) const {

/*====================== MFT ====================*/
#ifdef MFT
 if ( UseMFT() ) {
   Standard_CString fstring;
   Standard_Integer aFont = (aFontIndex < 0 ?
     (int)A->myTextFont : aFontIndex);
   Handle(MFT_FontManager) theFontManager =
     myMFTFonts->Value (aFont);
   Quantity_Length     theWidth, theHeight, theDescent;
   Quantity_PlaneAngle theSlant;
   Quantity_Factor     thePrecision;
   Standard_Boolean    theCapsHeight;
   fstring = theFontManager->FontAttribs (
     theWidth, theHeight, theDescent, theSlant, thePrecision, theCapsHeight
   );
   aSize    = (Standard_ShortReal)theHeight;
   aBheight = (Standard_ShortReal)theDescent;
   aSlant   =                     theSlant;
   return fstring;
 } else {
#endif
/*====================== MFT ====================*/
  if (aFontIndex < 0)
    aSize = Standard_ShortReal ( U(myFonts->Value(0)->myLogFont.lfHeight) );
  else
    aSize = Standard_ShortReal ( U(myFonts->Value(aFontIndex)->myLogFont.lfHeight) );
  aSize     = Standard_ShortReal ( aSize / W32_TextFactor );
  aSlant    = aFontIndex < 0 ? A -> myTextSlant : 0.0;
  aBheight  = 0.0F;

  return aFontIndex < 0 ? "" : myFonts->Value(aFontIndex)->myLogFont.lfFaceName;
/*====================== MFT ====================*/
#ifdef MFT
 }
#endif
/*====================== MFT ====================*/
}  // end WNT_WDriver :: FontSize

void WNT_WDriver :: ColorBoundIndexs (
                     Standard_Integer& aMinIndex,
                     Standard_Integer& aMaxIndex
                    ) const {

 if (  myColors.IsNull ()  ) {

  aMinIndex =
  aMaxIndex = 0;

 } else {

  aMinIndex = 1;
  aMaxIndex = myColors -> Length ();

 }  // end else

}  // end WNT_WDriver :: ColorBoundIndexs

Standard_Integer WNT_WDriver :: LocalColorIndex (
                                 const Standard_Integer anIndex
                                ) const {

 return (  anIndex < 1 || anIndex > myColors -> Length ()  ) ? -1 : anIndex;

}  // end WNT_WDriver :: LocalColorIndex

void WNT_WDriver :: FontBoundIndexs (
                     Standard_Integer& aMinIndex,
                     Standard_Integer& aMaxIndex
                    ) const {

 if (  myFonts.IsNull ()  ) {

  aMinIndex =
  aMaxIndex = 0;

 } else {

  aMinIndex = 1;
  aMaxIndex = myFonts -> Length ();

 }  // end else

}  // end WNT_WDriver :: FontBoundIndexs

Standard_Integer WNT_WDriver :: LocalFontIndex (
                                 const Standard_Integer anIndex
                                ) const {

 return (  anIndex < 1 || anIndex > myFonts -> Length ()  ) ? -1 : anIndex;

}  // end WNT_WDriver :: LocalFontIndex

void WNT_WDriver :: TypeBoundIndexs (
                     Standard_Integer& aMinIndex,
                     Standard_Integer& aMaxIndex
                    ) const {

 if (  TypeMap ().IsNull ()  ) {

  aMinIndex =
  aMaxIndex = 0;

 } else {

  aMinIndex = 1;
  aMaxIndex = TypeMap () -> Size ();

 }  // end else

}  // end WNT_WDriver :: TypeBoundIndexs

Standard_Integer WNT_WDriver :: LocalTypeIndex (
                                 const Standard_Integer anIndex
                                ) const {

 return (  anIndex < 1 || anIndex > TypeMap () -> Size ()  ) ? -1 : anIndex;

}  // end WNT_WDriver :: LocalTypeIndex

void WNT_WDriver :: WidthBoundIndexs (
                     Standard_Integer& aMinIndex,
                     Standard_Integer& aMaxIndex
                    ) const {

 if (  WidthMap ().IsNull ()  ) {

  aMinIndex =
  aMaxIndex = 0;

 } else {

  aMinIndex = 1;
  aMaxIndex = WidthMap () -> Size ();

 }  // end else

}  // end WNT_WDriver :: WidthBoundIndexs

Standard_Integer WNT_WDriver :: LocalWidthIndex (
                                 const Standard_Integer anIndex
                                ) const {

 return (  anIndex < 1 || anIndex > WidthMap () -> Size ()  ) ? -1 : anIndex;

}  // end WNT_WDriver :: LocalWidthIndex

void WNT_WDriver :: MarkBoundIndexs (
                     Standard_Integer& aMinIndex,
                     Standard_Integer& aMaxIndex
                    ) const {

 if (  MarkMap ().IsNull ()  ) {

  aMinIndex =
  aMaxIndex = 0;

 } else {

  aMinIndex = 1;
  aMaxIndex = MarkMap () -> Size ();

 }  // end else

}  // end WNT_WDriver :: MarkBoundIndexs

Standard_Integer WNT_WDriver :: LocalMarkIndex (
                                 const Standard_Integer anIndex
                                ) const {

 return (  anIndex < 1 || anIndex > MarkMap () -> Size ()  ) ? -1 : anIndex;

}  // end WNT_WDriver :: LocalMarkIndex

const Handle(WNT_TextManager)& WNT_WDriver :: TextManager ()
{
  return (myTextManager);
} // end WNT_WDriver :: TextManager

const Handle(MFT_FontManager)& WNT_WDriver :: MFT_Font
                               (const Standard_Integer anIndex)
{
  return myMFTFonts->Value(anIndex);
} // end WNT_WDriver :: MFT_FontList

Standard_ShortReal WNT_WDriver :: MFT_Size
                               (const Standard_Integer anIndex)
{
  return myMFTSizes->Value(anIndex);
} // end WNT_WDriver :: MFT_SizeList

static PW32_Allocator __fastcall _FindAllocator ( Standard_Address p, Standard_Integer id ) {

 while (   p != NULL && (  ( PW32_Allocator )p  ) -> myID != id   )

  p = (  ( PW32_Allocator )p  ) -> myNext;

 return ( PW32_Allocator )p;

}  // end _FindAllocator

static int __fastcall _GetROP2 ( Aspect_TypeOfDrawMode tdm ) {

 int retVal;

 switch ( tdm ) {

  case Aspect_TODM_ERASE:
  case Aspect_TODM_REPLACE:
  default:

   retVal = R2_COPYPEN;

  break;

  case Aspect_TODM_XOR:
  case Aspect_TODM_XORLIGHT:

   retVal = R2_XORPEN;

  break;

 }  // end switch

 return retVal;

}  // end _GetROP2

static void WINAPI _SetPoint ( int s, int m, int set, LPPOINT p, int* n, void* param ) {

 PMPARAM pmp = ( PMPARAM )param;

 *n = 0;

 for (  int i = 0; i < m; ++i, ++( *n ), ++s  ) {

  p[ i ].x = LONG (  pmp -> s -> XValues ().Value ( s ) * pmp -> w / 2.0 + 0.5  );
  p[ i ].y = LONG (  pmp -> s -> YValues ().Value ( s ) * pmp -> h / 2.0 + 0.5  );

 }  // end for

}  // end _SetPoint

static char* __fastcall _ConvertU2F ( const TCollection_ExtendedString& str ) {

 char*            retVal;
 Standard_Integer szBuff = (  str.Length () << 2  );

 retVal = ( char* )HeapAlloc (
                    GetProcessHeap (), HEAP_GENERATE_EXCEPTIONS,
                    szBuff
                   );

 while (  !Resource_Unicode :: ConvertUnicodeToFormat ( str, retVal, szBuff )  ) {

  szBuff <<= 1;
  retVal = ( char* )HeapReAlloc (
                     GetProcessHeap (), HEAP_GENERATE_EXCEPTIONS,
                     retVal, szBuff
                    );

 }  // end while

 return retVal;

}  // end _ConvertU2F
