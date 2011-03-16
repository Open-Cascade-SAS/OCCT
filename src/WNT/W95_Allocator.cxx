////////////////////////////////////////////////////////////////////////////////
// W95_Allocator                                                              //
//  Provides memory management and drawing operations for Windows 95.         //
//   World transformations and styled lines are managed by EHDC stuff.        //
//                                                                            //
// History: FEB-98 - creation (EUG)                                           //
//          MAR-98 - modification (DCB)                                       //
//                                                                            //
// Notes:   bitmap rotation code was created by MPO                           //
////////////////////////////////////////////////////////////////////////////////
#include "W95_Allocator.hxx"

#include <math.h>
#include <limits>

#define SGN( x )  (   ( x ) > 0 ? 1 : (  ( x ) < 0 ? -1 : 0 )   )
#define ALLOCATOR (   ( PW95_Allocator )myAllocator             )

#define MIN4( a, b, c, d ) min(   min(  ( a ), ( b )  ), min(  ( c ), ( d )  )   )
#define MAX4( a, b, c, d ) max(   max(                                       \
                                   (  ( a ) == INT_MAX ? INT_MIN : ( a )  ), \
                                   (  ( b ) == INT_MAX ? INT_MIN : ( b )  )  \
                                  ),                                         \
                                  max(                                       \
                                   (  ( c ) == INT_MAX ? INT_MIN : ( c )  ), \
                                   (  ( d ) == INT_MAX ? INT_MIN : ( d )  )  \
                                  )                                          \
                           )

static void WINAPI _InitXform      ( PXFORM, double, int, int );
static VOID CALLBACK _LineDDAProc  ( int, int, LPARAM         );	

W95_Allocator :: W95_Allocator ( int anID, PW32_Allocator prev ) :
                  W32_Allocator ( anID, prev ) {

 myLinePenWidth      = 0;
 myLineBrush.lbStyle = BS_SOLID;
 myLineBrush.lbColor = RGB( 0, 0, 0 );
 myLineBrush.lbHatch = 0;
 myLineStyleCount    = 0;
 myLineStyles        = NULL;
                        
 myPolyBrush.lbStyle = BS_NULL;

 myTextColor         = RGB( 0, 0, 0 );

 myMarkerBrush       = myPolyBrush;
 myMarkerWidth       = 0;
 myFlags            |= W32F_WIN95;
                                                      
}  // end constructor

W95_Allocator :: ~W95_Allocator () {

 if ( myLineStyles != NULL ) HeapFree (
                              GetProcessHeap (), 0, ( LPVOID )myLineStyles
                             );

}  // end destructor

void W95_Allocator :: Play ( HDC hdc, PSIZE szClient ) {

 if (  !( myFlags & W32F_EMPTY )  ) {

  myHDC.SetDC ( hdc, szClient );
  myHDC.ResetURect ();
  myHDC.SelectEPen (
         myLinePenWidth, &myLineBrush,
         myLineStyleCount, myLineStyles
        );
  myHDC.SelectEBrush ( &myPolyBrush, NULL );
  myHDC.SetTextAttrib (
         myTextColor, myTextFont, myTextSlant,
         myTextHScale, myTextVScale
        );
  myHDC.SetPolyFillMode ( myFillMode );

  Xform ();

  for ( PW32_Block aBlock = myStart; aBlock != NULL; aBlock = aBlock -> next )

   for ( int i = 0; i < aBlock -> free; i += aBlock -> data[ i ] )
	    
    if ( aBlock -> data[ i + 1 ] != __W32_DATA ) {                                         

     W32_Note* pNote = ( W32_Note* )&( aBlock -> data[ i + 1 ] );
     pNote -> Play ();

    }  // end if

 }  // end if

}  // end W95_Allocator :: Play

void W95_Allocator :: URect ( LPRECT lpRect ) {

 myHDC.GetURect ( lpRect );

}  // end W95_Allocator :: URect

void W95_Allocator :: Xform ( void ) {

 if ( myAngle  != 0.0 || myPivot.x != 0 || myPivot.y != 0 ||
      myMove.x != 0   || myMove.y  != 0 ||
      myScaleX != 1.0 || myScaleY  != 1.0
 ) {

  XFORM  xf;
  double sinVal, cosVal;

  sinVal = sin ( myAngle );
  cosVal = cos ( myAngle );

  xf.eM11 =                1.0F; xf.eM12 =                0.0F;
  xf.eM21 =                0.0F; xf.eM22 =                1.0F;
  xf.eDx  = ( FLOAT )-myPivot.x; xf.eDy  = ( FLOAT )-myPivot.y;
  myHDC.SetWorldTransform ( &xf );

  xf.eM11 = FLOAT (  cosVal * myScaleX ); xf.eM12 = FLOAT ( sinVal * myScaleX );
  xf.eM21 = FLOAT ( -sinVal * myScaleY ); xf.eM22 = FLOAT ( cosVal * myScaleY );
  xf.eDx  =                         0.0F; xf.eDy  =                        0.0F;
  myHDC.ModifyWorldTransform ( &xf, MWT_RIGHTMULTIPLY );

  xf.eM11 =                           1.0F; xf.eM12 =                           0.0F;
  xf.eM21 =                           0.0F; xf.eM22 =                           1.0F;
  xf.eDx  = FLOAT ( myPivot.x + myMove.x ); xf.eDy  = FLOAT ( myPivot.y + myMove.y );
  myHDC.ModifyWorldTransform ( &xf, MWT_RIGHTMULTIPLY );

  myFlags |= W32F_XFORM;

 } else {
 
  myHDC.ModifyWorldTransform ( NULL, MWT_IDENTITY );
  myFlags &= ~W32F_XFORM;

 }  // end else

}  // end W95_Allocator :: Xform

void W95_Allocator :: Point ( int x, int y ) {

 new ( this ) W32_PointNote ( x, y );

}  // end W95_Allocator :: Point

void W95_Allocator :: MarkerPoint ( int x, int y ) {

 new ( this ) W32_MarkerPointNote ( x, y );

}  // end W95_Allocator :: MarkerPoint

void W95_Allocator :: Line ( int x, int y, int x1, int y1 ) {

 new ( this ) W32_LineNote ( x, y, x1, y1 );

}  // end W95_Allocator :: Line

void W95_Allocator :: PolyEllipse ( int xc, int yc, int xr, int yr ) {

 new ( this ) W32_PolyEllipseNote ( xc, yc, xr, yr );

}  // end W95_Allocator :: PolyEllipse

void W95_Allocator :: Ellipse ( int xc, int yc, int xr, int yr ) {

 new ( this ) W32_EllipseNote ( xc, yc, xr, yr );

}  // end W95_Allocator :: Ellipse

void W95_Allocator :: Arc (
                       int xc, int yc, int xr, int yr,
                       double sa, double oa
                      ) {
                      
 new ( this ) W32_ArcNote ( xc, yc, xr, yr, sa, oa );                      
                      
}  // end W95_Allocator :: Arc

void W95_Allocator :: PolyChord (
                       int xc, int yc, int xr, int yr,
                       double sa, double oa
                      ) {
                      
 new ( this ) W32_PolyChordNote ( xc, yc, xr, yr, sa, oa );
                      
}  // end W95_Allocator :: PolyChord

void W95_Allocator :: Chord (
                       int xc, int yc, int xr, int yr,
                       double sa, double oa
                      ) {
                      
 new ( this ) W32_PolyChordNote ( xc, yc, xr, yr, sa, oa );
                      
}  // end W95_Allocator :: Chord

void W95_Allocator :: PolySector (
                       int xc, int yc, int xr, int yr,
                       double sa, double oa
                      ) {
                      
 new ( this ) W32_PolySectorNote ( xc, yc, xr, yr, sa, oa );
                      
}  // end W95_Allocator :: PolySector

void W95_Allocator :: Sector (
                       int xc, int yc, int xr, int yr,
                       double sa, double oa
                      ) {
                      
 new ( this ) W32_SectorNote ( xc, yc, xr, yr, sa, oa );
                      
}  // end W95_Allocator :: Sector

void W95_Allocator :: PolyMarker ( int aMaxPoints ) {

 new ( this ) W32_PolyMarkerNote ( aMaxPoints );

}  // end W95_Allocator :: PolyMarker

void W95_Allocator :: PolyMarker1 (
                       int aMaxPoints, GetPointFunc fn,
                       int aStartPoint, void* fParam
                      ) {
                      
 new ( this ) W32_PolyMarker1Note (
               aMaxPoints, fn, aStartPoint, fParam
              );                      
                      
}  // end W95_Allocator :: PolyMarker1

void W95_Allocator :: PolyMarker2 (
                       int aMaxPoints, GetPointFunc fn,
                       int aStartPoint, void* fParam
                      ) {
                      
 new ( this ) W32_PolyMarker2Note (
               aMaxPoints, fn, aStartPoint, fParam
              );                      
                      
}  // end W95_Allocator :: PolyMarker1

W32_Note* W95_Allocator :: Polygon ( int aMaxPoints ) {

 return new ( this ) W32_PolygonNote ( aMaxPoints );

}  // end W95_Allocator :: Polygon

W32_Note* W95_Allocator :: Polyline ( int aMaxPoints ) {

 return new ( this ) W32_PolylineNote ( aMaxPoints );

}  // end W95_Allocator :: Polyline

void W95_Allocator :: Image (
                       int x, int y, PW32_Bitmap pBmp, double aScale
                      ) {

 new ( this ) W32_ImageNote ( x, y, pBmp, aScale );

}  // end W95_Allocator :: Image

void W95_Allocator :: Text (
                       int x, int y, double angle, void* text,
                       BOOL fWide, BOOL fOutlined
                      ) {

 new ( this ) W32_TextNote ( x, y, angle, text, fWide, fOutlined );

}  // end W95_Allocator :: Text

void W95_Allocator :: Polytext (
                       int x, int y, double angle, double margin,
                       void* text, BOOL fWide, BOOL fOutlined
                      ) {
                      
 new ( this ) W32_PolyTextNote ( x, y, angle, margin, text, fWide, fOutlined );                      
                      
}  // end W95_Allocator :: Polytext

void W95_Allocator :: BeginMarker (
                       int x, int y, int w, int h, double angle
                      ) {
                      
 new ( this ) W32_BeginMarkerNote ( x, y, w, h, angle );                      
                      
}  // end W95_Allocator :: BeginMarker

void W95_Allocator :: EndMarker ( void ) {

 new ( this ) W32_EndMarkerNote ();

}  // end W95_Allocator :: EndMarker

void W95_Allocator :: LineAttrib (
                       DWORD width, PLOGBRUSH plb,
                       DWORD nStyles, PDWORD pdwStyle
                      ) {
                      
 new ( this ) W32_LineAttribNote ( width, plb, nStyles, pdwStyle );
                      
}  // end W95_Allocator :: LineAttrib

void W95_Allocator :: PolyAttrib (
                       PLOGBRUSH plb, BOOL fDrawEdge, int aFillMode
                      ) {

 new ( this ) W32_PolyAttribNote ( plb, fDrawEdge, aFillMode );

}  // end W95_Allocator :: PolyAttrib

void W95_Allocator :: TextAttrib (
                       HFONT hFont, COLORREF color, double slant,
                       double hScale, double vScale,
                       BOOL fUnderlined, BOOL fFree, BOOL fIndex
                      ) {
                      
 if ( myFlags & W32F_START )

  new ( this ) W32_TextAttribNote (
                hFont, color, slant, hScale, vScale,
                fUnderlined, fFree, fIndex
               );

 else {
 
  if ( myFlags & W32F_DFONT && !fIndex ) {
 
   DeleteFont( myTextFont );
   myFlags &= ~W32F_DFONT;
 
  }  // end if

  myTextColor   = color;
  myTextFont    = hFont;
  myTextSlant   = slant;
  myTextHScale  = vScale;
  myTextVScale  = hScale;
  myFlags      |= ( fUnderlined ? W32F_TULIN : 0 );
  
 }  // end else
                      
}  // end W95_Allocator :: TextAttrib

void W95_Allocator :: MarkerAttrib (
                       COLORREF color, DWORD dwWidth, BOOL fFill
                      ) {
                      
 new ( this ) W32_MarkerAttribNote ( color, dwWidth, fFill );                      
                      
}  // end W95_Allocator :: MarkerAttrib

void W95_Allocator :: FunCall (
                       W32_FCall fCall, int sz, PW32_FCALLPARAM param
                      ) {

 new ( this ) W32_FCallNote ( fCall, sz, param );

}  // end W95_Allocator :: FunCall

int W95_Allocator :: TextSize ( HDC hdc, char* str, PSIZE psz )
{
  ABC        abcf;
  GetTextExtentPoint32A ( hdc, str, lstrlenA ( str ), psz );
  GetCharABCWidthsA     ( hdc, str[ 0 ], str[ 0 ], &abcf  );
  psz->cx    = LONG ( psz->cx   * myTextHScale );
  psz->cy    = LONG ( psz->cy   * myTextVScale );
  abcf.abcA  = UINT ( abcf.abcA * myTextHScale );
  return abcf.abcA;
}  // end W95_Allocator :: TextSize
  
int W95_Allocator :: TextSize ( HDC hdc, wchar_t* str, PSIZE psz )
{
  ABC        abcf;
  GetTextExtentPoint32W ( hdc, str, lstrlenW ( str ), psz );
  GetCharABCWidthsW     ( hdc, str[ 0 ], str[ 0 ], &abcf  );
  psz->cx    = LONG ( psz->cx   * myTextHScale );
  psz->cy    = LONG ( psz->cy   * myTextVScale );
  abcf.abcA  = UINT ( abcf.abcA * myTextHScale );
  return abcf.abcA;
}  // end W95_Allocator :: TextSize
////////////////////////////////////////////////////////////////////////////////
// W 3 2 _ P o i n t N o t e                                                  //
////////////////////////////////////////////////////////////////////////////////
W32_PointNote :: W32_PointNote ( int x, int y ) {

 myX = x;
 myY = y;

}  // end constructor

void W32_PointNote :: Play ( BOOL fDummy ) {

 ALLOCATOR -> myHDC.SetPixel (  myX, myY, ALLOCATOR -> myPointColor  );

}  // end W32_PointNote :: Play
////////////////////////////////////////////////////////////////////////////////
// W 3 2 _ M a r k e r P o i n t N o t e                                      //
////////////////////////////////////////////////////////////////////////////////
W32_MarkerPointNote :: W32_MarkerPointNote ( int x, int y ) :
                        W32_PointNote ( x, y ) {

}  // end constructor

void W32_MarkerPointNote :: Play ( BOOL fDummy ) {

 ALLOCATOR -> myHDC.SetPixel (  myX, myY, ALLOCATOR -> myMarkerPointColor  );

}  // end W32_MarkerPointNote :: Play
////////////////////////////////////////////////////////////////////////////////
// W 3 2 _ L i n e N o t e                                                    //
////////////////////////////////////////////////////////////////////////////////
W32_LineNote :: W32_LineNote ( int x, int y, int x1, int y1 ) :
                 W32_PointNote ( x, y ) {

 myX2 = x1;
 myY2 = y1;

}  // end constructor

void W32_LineNote :: Play ( BOOL fDummy ) {

 EHDC* pe = &ALLOCATOR -> myHDC;

 pe -> MoveToEx ( myX,  myY, NULL );
 pe -> LineTo   ( myX2, myY2      );

}  // end W32_LineNote :: Play
////////////////////////////////////////////////////////////////////////////////
// W 3 2 _ P o l y E l l i p s e N o t e                                      //
////////////////////////////////////////////////////////////////////////////////
W32_PolyEllipseNote :: W32_PolyEllipseNote ( int xc, int yc, int xr, int yr ) :
                        W32_PointNote ( xc, yc ) {
                        
 myXr = xr;
 myYr = yr;                        
                        
}  // end constructor

void W32_PolyEllipseNote :: Play ( BOOL fDummy ) {

 ALLOCATOR -> myHDC.Polyarc ( myX, myY, myXr, myYr );

}  // end W32_PolyEllipseNote :: Play
////////////////////////////////////////////////////////////////////////////////
// W 3 2 _ E l l i p s e N o t e                                              //
////////////////////////////////////////////////////////////////////////////////
W32_EllipseNote :: W32_EllipseNote ( int xc, int yc, int xr, int yr ) :
                    W32_PolyEllipseNote ( xc, yc, xr, yr ) {
                        
}  // end constructor

void W32_EllipseNote :: Play ( BOOL fDummy ) {

 ALLOCATOR -> myHDC.Arc ( myX, myY, myXr, myYr );

}  // end W32_PolyEllipseNote :: Play
////////////////////////////////////////////////////////////////////////////////
// W 3 2 _ A r c N o t e                                                      //
////////////////////////////////////////////////////////////////////////////////
W32_ArcNote :: W32_ArcNote (
                int xc, int yc, int xr, int yr, double sa, double oa
               ) : W32_PolyEllipseNote ( xc, yc, xr, yr ) {

 mySa = sa;
 myOa = oa;

}  // end constructor

void W32_ArcNote :: Play ( BOOL fDummy ) {

 ALLOCATOR -> myHDC.Arc ( myX, myY, myXr, myYr, mySa, myOa );

}  // end W32_ArcNote :: Play
////////////////////////////////////////////////////////////////////////////////
// W 3 2 _ P o l y C h o r d N o t e                                          //
////////////////////////////////////////////////////////////////////////////////
W32_PolyChordNote :: W32_PolyChordNote (
                      int xc, int yc, int xr, int yr, double sa, double oa
                     ) : W32_ArcNote ( xc, yc, xr, yr, sa, oa ) {

}  // end constructor

void W32_PolyChordNote :: Play ( BOOL fDummy ) {

 ALLOCATOR -> myHDC.Polyarc ( myX, myY, myXr, myYr, mySa, myOa );

}  // end W32_PolyChordNote :: Play
////////////////////////////////////////////////////////////////////////////////
// W 3 2 _ C h o r d N o t e                                                  //
////////////////////////////////////////////////////////////////////////////////
W32_ChordNote :: W32_ChordNote (
                  int xc, int yc, int xr, int yr, double sa, double oa
                 ) : W32_PolyChordNote ( xc, yc, xr, yr, sa, oa ) {

}  // end constructor

void W32_ChordNote :: Play ( BOOL fDummy ) {

 ALLOCATOR -> myHDC.Arc ( myX, myY, myXr, myYr, mySa, myOa );

}  // end W32_ChordNote :: Play
////////////////////////////////////////////////////////////////////////////////
// W 3 2 _ P o l y S e c t o r N o t e                                        //
////////////////////////////////////////////////////////////////////////////////
W32_PolySectorNote :: W32_PolySectorNote (
                      int xc, int yc, int xr, int yr, double sa, double oa
                     ) : W32_ArcNote ( xc, yc, xr, yr, sa, oa ) {

}  // end constructor

void W32_PolySectorNote :: Play ( BOOL fDummy ) {

 ALLOCATOR -> myHDC.Polyarc ( myX, myY, myXr, myYr, mySa, myOa, FALSE );

}  // end W32_PolySectorNote :: Play
////////////////////////////////////////////////////////////////////////////////
// W 3 2 _ S e c t o r N o t e                                                //
////////////////////////////////////////////////////////////////////////////////
W32_SectorNote :: W32_SectorNote (
                  int xc, int yc, int xr, int yr, double sa, double oa
                 ) : W32_PolySectorNote ( xc, yc, xr, yr, sa, oa ) {

}  // end constructor

void W32_SectorNote :: Play ( BOOL fDummy ) {

 ALLOCATOR -> myHDC.Arc ( myX, myY, myXr, myYr, mySa, myOa, ARCF_PIE );

}  // end W32_SectorNote :: Play
////////////////////////////////////////////////////////////////////////////////
// W 3 2 _ P o l y M a r k e r N o t e                                        //
////////////////////////////////////////////////////////////////////////////////
W32_PolyMarkerNote :: W32_PolyMarkerNote ( int aMaxPoints ) {

 myMaxPoints = aMaxPoints;
 mySetPoints = 0;
 myPoints    = ( LPPOINT )myAllocator -> NewData (
                                          myMaxPoints * sizeof ( POINT )
                                         );

}  // end constructor

void W32_PolyMarkerNote :: Replace ( int idx, int x, int y ) {

 if ( idx >= myMaxPoints ) return;

 myPoints[ idx ].x = x;   
 myPoints[ idx ].y = y;

}  // end CPolyMarkerNote :: Replace
////////////////////////////////////////////////////////////////////////////////
// W 3 2 _ P o l y M a r k e r 1 N o t e                                      //
////////////////////////////////////////////////////////////////////////////////
W32_PolyMarker1Note :: W32_PolyMarker1Note (
                        int aMaxPoints, GetPointFunc fn, int aStartPoint,
                        void* fParam
                       ) : W32_PolyMarkerNote ( aMaxPoints ) {

 ( *fn ) ( aStartPoint, myMaxPoints, aStartPoint, myPoints, &mySetPoints, fParam );
                       
}  // end constructor

void W32_PolyMarker1Note :: Play ( BOOL fDummy ) {

 if ( ALLOCATOR -> myFlags & W32F_MFILL )

  ALLOCATOR -> myHDC.Polygon (
                      myPoints, mySetPoints,
                      POLYF_NOCLOSEDGE
                     );

 else
                
  ALLOCATOR -> myHDC.Polyline ( myPoints, mySetPoints );

}  // end W32_PolyMarker1Note :: Play
////////////////////////////////////////////////////////////////////////////////
// W 3 2 _ P o l y M a r k e r 2 N o t e                                      //
////////////////////////////////////////////////////////////////////////////////
W32_PolyMarker2Note :: W32_PolyMarker2Note (
                        int aMaxPoints, GetPointFunc fn, int aStartPoint,
                        void* fParam
                       ) : W32_PolyMarker1Note (
                            aMaxPoints, fn, aStartPoint, fParam
                           ) {

}  // end constructor

void W32_PolyMarker2Note :: Play ( BOOL fDummy ) {

 if ( ALLOCATOR -> myFlags & W32F_MFILL )

  ALLOCATOR -> myHDC.Polygon ( myPoints, mySetPoints - 1 );

 else
                
  ALLOCATOR -> myHDC.Polyline ( myPoints, mySetPoints - 1, TRUE );

}  // end W32_PolyMarker2Note :: Play
////////////////////////////////////////////////////////////////////////////////
// W 3 2 _ P o l y g o n N o t e                                              //
////////////////////////////////////////////////////////////////////////////////
W32_PolygonNote :: W32_PolygonNote ( int aMaxPoints ) :
                    W32_PolyMarkerNote ( aMaxPoints ) {
                                                            
}  // end constructor

void W32_PolygonNote :: Play ( BOOL fDummy ) {
  
 ALLOCATOR -> myHDC.Polygon (
                     myPoints, mySetPoints,
                     ALLOCATOR -> myFlags & W32F_POUTL ? 0 : POLYF_NOEDGE
                    );
 
}  // end W32_PolygonNote :: Play

void W32_PolygonNote :: Add ( int aX,int aY ) {

 if ( mySetPoints == myMaxPoints ) return;

 ( myPoints + mySetPoints ) -> x = aX;
 ( myPoints + mySetPoints ) -> y = aY;
 
 ++mySetPoints;

}  // end W32_CPolygonNote :: Add
////////////////////////////////////////////////////////////////////////////////
// W 3 2 _ P o l y l i n e N o t e                                            //
////////////////////////////////////////////////////////////////////////////////
W32_PolylineNote :: W32_PolylineNote ( int aMaxPoints ) :
                     W32_PolygonNote ( aMaxPoints ) {
                     
}  // end constructor

void W32_PolylineNote :: Play ( BOOL fDummy ) {

 DWORD nPts;
 BOOL  fClose;
 int   idx = mySetPoints - 1;

 if ( myPoints -> x == myPoints[ idx ].x &&
      myPoints -> y == myPoints[ idx ].y
 ) {
 
  nPts   = idx;
  fClose = TRUE;
 
 } else {
 
  nPts   = mySetPoints;
  fClose = FALSE;
 
 }  // end else

 ALLOCATOR -> myHDC.Polyline ( myPoints, nPts, fClose );

}  // end W32_PolylineNote :: Play
////////////////////////////////////////////////////////////////////////////////
// W 3 2 _ I m a g e N o t e                                                  //
////////////////////////////////////////////////////////////////////////////////
W32_ImageNote :: W32_ImageNote (
                  int x, int y, PW32_Bitmap pBmp, double aScale
                 ) : W32_PointNote ( x, y ) {

 myBitmap = pBmp;
 ++pBmp   -> nUsed;
 myScale  = aScale;

}  // end constructor

W32_ImageNote :: ~W32_ImageNote () {

 if ( myBitmap -> hBmp != NULL && --myBitmap -> nUsed == 0 ) {
 
  DeleteObject ( myBitmap -> hBmp );
  HeapFree (  GetProcessHeap (), 0, ( PVOID )myBitmap  );

 }  // end if

}  // end W32_ImageNote :: ~W32_ImageNote

void W32_ImageNote :: Play ( BOOL fDummy ) {

 int     xx, yy, w, h;
 HDC     hdc, hdcMem;
 BITMAP  bmp;
 HBITMAP hbo;
 SIZE    sz;
 POINT   up[ 4 ];
 BOOL    fScale = FALSE;

 ALLOCATOR -> myHDC.Extent ( &sz );
 GetObject (  myBitmap -> hBmp, sizeof ( BITMAP ), &bmp  );

 xx = myX;
 yy = myY;
 w  = bmp.bmWidth;
 h  = bmp.bmHeight;

 if ( myScale != 1.0 ) {
  
  w  = int (  w * myScale + 0.5 );
  h  = int (  h * myScale + 0.5 );
 
  fScale = TRUE;
 
 }
 
 if ( ALLOCATOR -> myScaleX != 1.0 ) {
  
  xx = int ( xx * ALLOCATOR -> myScaleX + 0.5 );
  w  = int (  w * ALLOCATOR -> myScaleX + 0.5 );

  fScale = TRUE;

 }  // end if

 if ( ALLOCATOR -> myScaleY != 1.0 ) {
 
  yy = int ( yy * ALLOCATOR -> myScaleY + 0.5 );
  h  = int (  h * ALLOCATOR -> myScaleY + 0.5 );

  fScale = TRUE;

 }  // end if

 xx = xx    -         ( w >> 1 )    + ALLOCATOR -> myPivot.x + ALLOCATOR -> myMove.x ;
 yy = sz.cy - (  yy + ( h >> 1 )  ) - ALLOCATOR -> myPivot.y - ALLOCATOR -> myMove.y;

 hdc = ALLOCATOR -> myHDC.Hdc ();
 hdcMem = CreateCompatibleDC ( hdc );
  hbo = SelectBitmap( hdcMem, myBitmap -> hBmp );

   if (  ALLOCATOR -> myAngle == 0.0  ) {
   
    StretchBlt (
     hdc,    xx, yy,           w,            h,
     hdcMem,  0,  0, bmp.bmWidth, bmp.bmHeight,
     SRCCOPY
    );

    up[ 0 ].x = xx;        up[ 0 ].y = yy;
    up[ 1 ].x = xx + w;    up[ 1 ].y = up[ 0 ].y;
    up[ 2 ].x = up[ 1 ].x; up[ 2 ].y = yy + h;
    up[ 3 ].x = up[ 0 ].x; up[ 3 ].y = up[ 2 ].y;

    ALLOCATOR -> myHDC.Register ( up, 4 );

   } else {
   
    int     i, j;
    int     x1      = xx;
    int     y1      = sz.cy - yy - h;
    int     x2      = x1 + w;
    int     y2      = y1;
    int     x3      = x2;
    int     y3      = y1 + h;
    int     x4      = x1;
    int     y4      = y3;
    int     xmin    = MIN4( x1, x2, x3, x4 );
    int     ymin    = MIN4( y1, y2, y3, y4 );
    double  DeltaX  = cos ( -ALLOCATOR -> myAngle );
    double  DeltaY  = sin ( -ALLOCATOR -> myAngle );
    int     X1      = int (  x1 * DeltaX + y1 * DeltaY ); 
    int     Y1      = int ( -x1 * DeltaY + y1 * DeltaX );
    int     X2      = int (  x2 * DeltaX + y2 * DeltaY );
    int     Y2      = int ( -x2 * DeltaY + y2 * DeltaX );
    int     X3      = int (  x3 * DeltaX + y3 * DeltaY );
    int     Y3      = int ( -x3 * DeltaY + y3 * DeltaX );
    int     X4      = int (  x4 * DeltaX + y4 * DeltaY );
    int     Y4      = int ( -x4 * DeltaY + y4 * DeltaX );
    int     YHeight = max(  abs ( Y1 - Y3 ), abs ( Y2 - Y4 )  ) + 1;
    int     Ymin    = MIN4( Y1, Y2, Y3, Y4 );
    PINT    line1, line2, line3, line4;
    PPOINT  lines;
    HBITMAP hbo2, hBmp;

    if ( fScale ) {

     HDC hdcMem2 = CreateCompatibleDC ( hdc );
      hBmp = CreateCompatibleBitmap ( hdc, w, h );

       hbo2 = SelectBitmap( hdcMem2, hBmp );
        StretchBlt (
         hdcMem2, 0, 0,           w,            h,
         hdcMem,  0, 0, bmp.bmWidth, bmp.bmHeight,
         SRCCOPY
        );
       SelectBitmap( hdcMem2, hbo2 );
     DeleteDC ( hdcMem2 );

     hbo2 = SelectBitmap( hdcMem,  hBmp );

    }  // end if

    lines = ( PPOINT )VirtualAlloc (
                       NULL, (   (  j = ( sizeof ( int ) * YHeight )  ) << 2   ) +
                             (      i = YHeight * sizeof ( POINT )             ),
                       MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE
                      );

     line1 = PINT (   (  ( PBYTE )lines  ) + i   );
     line2 = PINT (   (  ( PBYTE )line1  ) + j   );
     line3 = PINT (   (  ( PBYTE )line2  ) + j   );
     line4 = PINT (   (  ( PBYTE )line3  ) + j   );

     for ( i = 0; i < YHeight; ++i )
    
      line1[ i ] = line2[ i ] = line3[ i ] = line4[ i ] = INT_MAX;

     LineDDA (  X1, Y1 - Ymin, X2, Y2 - Ymin, &_LineDDAProc, ( LPARAM )line1  );
     LineDDA (  X2, Y2 - Ymin, X3, Y3 - Ymin, &_LineDDAProc, ( LPARAM )line2  );
     LineDDA (  X3, Y3 - Ymin, X4, Y4 - Ymin, &_LineDDAProc, ( LPARAM )line3  );
     LineDDA (  X4, Y4 - Ymin, X1, Y1 - Ymin, &_LineDDAProc, ( LPARAM )line4  );

     for ( i = 0; i < YHeight; ++i ) {

      lines[ i ].x = MIN4( line1[ i ], line2[ i ], line3[ i ], line4[ i ] );
  
      if ( lines[ i ].x < 0 ) lines[ i ].x = 0;

      lines[ i ].y = MAX4( line1[ i ], line2[ i ], line3[ i ], line4[ i ] );

     }  // end for

     for (  j = ( -Ymin < 0 ? 0 : -Ymin ); j < YHeight; ++j  ) {

      int    Yt = sz.cy - ( j + Ymin );
      int    Yy = j + Ymin;
      double XB =       lines[ j ].x * DeltaX - Yy * DeltaY - xmin;
      double YB = h - ( lines[ j ].x * DeltaY + Yy * DeltaX - ymin );

      up[ 0 ].x = lines[ j ].x; up[ 0 ].y = Yt;
      up[ 1 ].x = lines[ j ].y; up[ 1 ].y = Yt;

      ALLOCATOR -> myHDC.Register ( up, 2 );

      for ( i = lines[ j ].x; i <= lines[ j ].y; ++i ) {

       SetPixel (
        hdc, i, Yt, GetPixel (  hdcMem, ( int )XB, ( int )YB  )
       );

       XB += DeltaX; YB -= DeltaY;
     
      }  // end for ( i )

     }  // end for ( j )

    VirtualFree (  ( LPVOID )lines, 0, MEM_DECOMMIT | MEM_RELEASE  );

    if ( fScale ) {

     SelectBitmap( hdcMem, hbo2 );
     DeleteBitmap( hBmp );
    
    }  // end if

    ALLOCATOR -> Xform ();
   
   }  // end else

  SelectBitmap( hdcMem, hbo );
 DeleteDC ( hdcMem );

}  // end W32_ImageNote :: Play
////////////////////////////////////////////////////////////////////////////////
// W 3 2 _ T e x t N o t e                                                    //
////////////////////////////////////////////////////////////////////////////////
W32_TextNote :: W32_TextNote (
                 int x, int y, double angle, void* text,
                 BOOL fWide, BOOL fOutlined
                ) : W32_PointNote ( x, y ), myAngle ( angle ) {
             
 DWORD dwLen = fWide ? (    (   lstrlenW (  ( LPCWSTR )text  ) + 1   ) << 1    )
                     : (    (   lstrlenA (  ( LPCSTR  )text  ) + 1   ) << 0    );

 myText = ALLOCATOR -> NewData ( dwLen );
 CopyMemory ( myText, text, dwLen );

 myFlags = (  ( fWide     ? W32F_TUNICODE : 0 ) |
              ( fOutlined ? W32F_TOUTLINE : 0 )
           );
                
}  // end constructor

void W32_TextNote :: Play ( BOOL fDummy ) {

 if ( myFlags & W32F_TUNICODE )

  ALLOCATOR -> myHDC.ETextOut (
   myX, myY, ( wchar_t* )myText, myAngle,
   myFlags & W32F_TOUTLINE || ALLOCATOR -> myFlags & W32F_MONO
  );

 else

  ALLOCATOR -> myHDC.ETextOut (
   myX, myY, ( char* )myText, myAngle,
   myFlags & W32F_TOUTLINE || ALLOCATOR -> myFlags & W32F_MONO
  );

}  // end W32_TextNote :: Play
////////////////////////////////////////////////////////////////////////////////
// W 3 2 _ P o l y T e x t N o t e                                            //
////////////////////////////////////////////////////////////////////////////////
W32_PolyTextNote :: W32_PolyTextNote (
                     int x, int y, double angle, double margin,
                     void* text, BOOL fWide, BOOL fOutlined
                    ) : W32_TextNote ( x, y, angle, text, fWide, fOutlined ),
                        myMargin ( margin ) {

}  // end constructor

void W32_PolyTextNote :: Play ( BOOL fDummy ) {

 BOOL fNofill  = ( ALLOCATOR -> myFlags & W32F_NOFIL ? TRUE  : FALSE );
 BOOL fNoframe = ( ALLOCATOR -> myFlags & W32F_POUTL ? FALSE : TRUE  );

 if ( myFlags & W32F_TUNICODE )

  ALLOCATOR -> myHDC.PolyTextOut (
   myX, myY, ( wchar_t* )myText, myAngle,
   myMargin, myFlags & W32F_TOUTLINE || ALLOCATOR -> myFlags & W32F_MONO, fNofill,
   fNoframe
  );

 else

  ALLOCATOR -> myHDC.PolyTextOut (
   myX, myY, ( char* )myText, myAngle,
   myMargin, myFlags & W32F_TOUTLINE || ALLOCATOR -> myFlags & W32F_MONO, fNofill,
   fNoframe
  );

}  // end W32_PolyTextNote :: Play
////////////////////////////////////////////////////////////////////////////////
// W 3 2 _ B e g i n M a r k e r N o t e                                      //
////////////////////////////////////////////////////////////////////////////////
W32_BeginMarkerNote :: W32_BeginMarkerNote (
                        int x, int y, int w, int h, double angle
                       ) : W32_PointNote ( x, y ),
                           myAngle ( angle ), myWidth ( w ), myHeight ( h ) {                

}  // end constructor

void W32_BeginMarkerNote :: Play ( BOOL fDummy ) {

 XFORM    xfm;
 LOGBRUSH lb = { BS_SOLID, ALLOCATOR -> myMarkerPointColor, 0 };
 POINT    p  = { myX, myY };

 if ( ALLOCATOR -> myFlags & W32F_XFORM ) ALLOCATOR -> myHDC.Transform ( &p, 1 );

 _InitXform ( &xfm, myAngle, p.x, p.y );
 
 ALLOCATOR -> myHDC.SetWorldTransform ( &xfm );
 ALLOCATOR -> myHDC.SelectEPen (
                     ALLOCATOR -> myMarkerWidth, &lb
                    );
 ALLOCATOR -> myHDC.SelectEBrush (
                     &ALLOCATOR -> myMarkerBrush,
                     &ALLOCATOR -> myMarkerBrushOrig
                    );

}  // end W32_BeginMarkerNote :: Play
////////////////////////////////////////////////////////////////////////////////
// W 3 2 _ E n d M a r k e r N o t e                                          //
////////////////////////////////////////////////////////////////////////////////
W32_EndMarkerNote :: W32_EndMarkerNote () {}

void W32_EndMarkerNote :: Play ( BOOL fDummy ) {

 ALLOCATOR -> Xform ();
 ALLOCATOR -> myHDC.SelectEPen ( 0xFFFFFFFF, NULL );
 ALLOCATOR -> myHDC.SelectEBrush ( &ALLOCATOR -> myMarkerBrushOrig );

}  // end W32_EndMarkerNote :: Play

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
#define SET_ATTRIB \
  ( ((ALLOCATOR->myFlags & W32F_MONO) && (ALLOCATOR->myFlags & W32F_MINIT)) ||\
    !(ALLOCATOR->myFlags & W32F_MONO)\
  )
////////////////////////////////////////////////////////////////////////////////
// W 3 2 _ L i n e A t t r i b N o t e                                        //
////////////////////////////////////////////////////////////////////////////////
W32_LineAttribNote :: W32_LineAttribNote (
                       DWORD width, PLOGBRUSH plb,
                       DWORD nStyles, PDWORD pdwStyle
                      ) {
                   
 myPenWidth   = width;                         
 myLogBrush   = *plb;
 myStyleCount = nStyles;

 if ( myStyleCount != 0 ) {

  DWORD dwLen = nStyles * sizeof ( DWORD );

  myStyles = ( PDWORD )ALLOCATOR -> NewData ( dwLen );

  CopyMemory ( myStyles, pdwStyle, dwLen );

 }  // end if

 Play ( FALSE );
                      
}  // end constructor

void W32_LineAttribNote :: Play ( BOOL fRealize ) {

 ALLOCATOR -> myPointColor   = myLogBrush.lbColor;
 ALLOCATOR -> myLineBrush    = myLogBrush;
 ALLOCATOR -> myLinePenWidth = myPenWidth;

 if ( myStyleCount != 0 ) {

  DWORD dwLen = myStyleCount * sizeof ( DWORD );

  if ( ALLOCATOR -> myLineStyles != NULL )

   ALLOCATOR -> myLineStyles = ( PDWORD )HeapReAlloc (
                                          GetProcessHeap (),
                                          HEAP_GENERATE_EXCEPTIONS,
                                          ALLOCATOR -> myLineStyles,
                                          dwLen
                                         );

  else

   ALLOCATOR -> myLineStyles = ( PDWORD )HeapAlloc (
                                          GetProcessHeap (),
                                          HEAP_GENERATE_EXCEPTIONS,
                                          dwLen
                                         );

  CopyMemory ( ALLOCATOR -> myLineStyles, myStyles, dwLen );
  ALLOCATOR -> myLineStyleCount = myStyleCount;

 }  // end if

 if ( fRealize ) ALLOCATOR -> myHDC.SelectEPen (
                                     myPenWidth, &myLogBrush,
                                     myStyleCount, myStyles
                                    );

}  // end W32_LineAttribNote :: Play
////////////////////////////////////////////////////////////////////////////////
// W 3 2 _ P o l y A t t r i b N o t e                                        //
////////////////////////////////////////////////////////////////////////////////
W32_PolyAttribNote :: W32_PolyAttribNote (
                       PLOGBRUSH plb, BOOL fDrawEdge, int aFillMode
                      ) {

 myBrush     = *plb;
 myfEdge     =  fDrawEdge;
 myFillMode  =  aFillMode;

 Play ( FALSE );

}  // end constructor

void W32_PolyAttribNote :: Play ( BOOL fRealize ) {

 ALLOCATOR -> myFillMode  = myFillMode;
 if ( myfEdge )     ALLOCATOR -> myFlags |= W32F_POUTL;
 else               ALLOCATOR -> myFlags &= ~W32F_POUTL;
 if ( myBrush.lbStyle == BS_NULL )  ALLOCATOR -> myFlags |= W32F_NOFIL;
 else                               ALLOCATOR -> myFlags &= ~W32F_NOFIL;

 ALLOCATOR -> myPolyBrush = myBrush;
 if ( fRealize ) {
  ALLOCATOR -> myHDC.SelectEBrush    ( &myBrush   );
  ALLOCATOR -> myHDC.SetPolyFillMode ( myFillMode );
 }  // end if

}  // end W32_PolyAttribNote :: Play
////////////////////////////////////////////////////////////////////////////////
// W 3 2 _ T e x t A t t r i b N o t e                                        //
////////////////////////////////////////////////////////////////////////////////
W32_TextAttribNote :: W32_TextAttribNote (
                       HFONT hFont, COLORREF color, double slant,
                       double hScale, double vScale,
                       BOOL fUnderlined, BOOL fFree, BOOL fIndex
                      ) {
                      
 myColor     = color;
 myFont      = hFont;
 mySlant     = slant;
 myHScale    = vScale;
 myVScale    = hScale;
 myFlags     = ( fUnderlined ? W32F_TFULINED : 0 ) |
               ( fIndex      ? W32F_TINDEX   : 0 ) |
               ( fFree       ? W32F_TFREE    : 0 );

 Play ( FALSE );
                      
}  // end constructor

W32_TextAttribNote :: ~W32_TextAttribNote () {

 if ( myFlags & W32F_TFREE ) DeleteFont( myFont );

}  // end destructor

void W32_TextAttribNote :: Play ( BOOL fRealize ) {

 if (  ( ALLOCATOR -> myFlags & W32F_DFONT ) && !( myFlags & W32F_TINDEX )  ) {
  DeleteFont( ALLOCATOR -> myTextFont );
  ALLOCATOR -> myFlags &= ~W32F_DFONT;
 }  // end if

 ALLOCATOR -> myTextFont   = myFont;
 ALLOCATOR -> myTextSlant  = mySlant;
 ALLOCATOR -> myTextHScale = myHScale;
 ALLOCATOR -> myTextVScale = myVScale;
 if ( myFlags & W32F_TFULINED )  ALLOCATOR -> myFlags |= W32F_TULIN;
 else                            ALLOCATOR -> myFlags &= ~W32F_TULIN;

 ALLOCATOR -> myTextColor  = myColor;
 if (  fRealize && !( myFlags & W32F_TINDEX )  )
  ALLOCATOR -> myHDC.SetTextAttrib (
                      myColor, myFont, mySlant,
                      myHScale, myVScale
                     );

}  // end W32_TextAttribNote :: Play
////////////////////////////////////////////////////////////////////////////////
// W 3 2 _ M a r k e r A t t r i b N o t e                                    //
////////////////////////////////////////////////////////////////////////////////
W32_MarkerAttribNote :: W32_MarkerAttribNote (
                         COLORREF color, DWORD dwWidth, BOOL fFill
                        ) {
                        
 myLogBrush.lbStyle = fFill;
 myLogBrush.lbColor = color;
 myLogBrush.lbHatch = dwWidth;

 Play ();

}  // end constructor

void W32_MarkerAttribNote :: Play ( BOOL fDummy ) {

 ALLOCATOR -> myMarkerBrush.lbStyle = BS_SOLID;
 ALLOCATOR -> myMarkerBrush.lbColor = ALLOCATOR -> myPolyBrush.lbColor;
 ALLOCATOR -> myMarkerBrush.lbHatch = 0;
 ALLOCATOR -> myMarkerWidth         = myLogBrush.lbHatch;
 ALLOCATOR -> myMarkerPointColor    = myLogBrush.lbColor;
 
 if ( myLogBrush.lbStyle )
  ALLOCATOR -> myFlags |= W32F_MFILL;
 else
  ALLOCATOR -> myFlags &= ~W32F_MFILL;

}  // end W32_MarkerAttribNote :: Play
////////////////////////////////////////////////////////////////////////////////
// W 3 2 _ F C a l l N o t e                                                  //
////////////////////////////////////////////////////////////////////////////////
W32_FCallNote :: W32_FCallNote (
                  W32_FCall fCall, int sz, PW32_FCALLPARAM param
                 ) {

 myFunc = fCall;

 myParam = ( PW32_FCALLPARAM )ALLOCATOR -> NewData ( sz );

 CopyMemory ( myParam, param, sz );

}  // end constructor

void W32_FCallNote :: Play ( BOOL fDummy ) {

 ( *myFunc ) ( myParam );

}  // end W32_FCallNote :: Play
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
// M i s c e l l a n e o u s                                                 +//
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
static void WINAPI _InitXform ( PXFORM pxf, double angle, int x, int y ) {

 double cosVal, sinVal;

 cosVal = cos ( angle );
 sinVal = sin ( angle );

 pxf -> eM11 = ( FLOAT ) cosVal; pxf -> eM12 = ( FLOAT )sinVal;
 pxf -> eM21 = ( FLOAT )-sinVal; pxf -> eM22 = ( FLOAT )cosVal;
 pxf -> eDx  = ( FLOAT )      x; pxf -> eDy  = ( FLOAT )     y;

}  // end _InitXform

static VOID CALLBACK _LineDDAProc ( int x, int y, LPARAM lpData ) {

 (  ( PINT )lpData   )[ y ] = x;

}  // end LineDDAProc
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
