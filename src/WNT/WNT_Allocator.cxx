////////////////////////////////////////////////////////////////////////////////
// WNT_Allocator                                                              //
//  Provides memory management and drawing operations for Windows 95.         //
//   World transformations and styled lines are managed by EHDC stuff.        //
//                                                                            //
// History: FEB-98 - creation (EUG)                                           //
//          MAR-98 - modification (DCB)                                       //
////////////////////////////////////////////////////////////////////////////////

#define PRO19042	//GG_251199 Avoid to crash with an insuffisant resources messag

#include "WNT_Allocator.hxx"

#include <math.h>
#include <limits>
#include <stdio.h>

#define ALLOCATOR ( ( PWNT_Allocator )myAllocator )

static XFORM xfmIdent = { ( FLOAT )0.8660,  ( FLOAT )0.5000,
                          ( FLOAT )-0.5000, ( FLOAT )0.8660,
                          ( FLOAT )0.0000,  ( FLOAT )0.0000
                        };

WNT_Allocator :: WNT_Allocator ( int anID, PW32_Allocator prev ) :
                  W32_Allocator ( anID, prev )
{
  myLinePen           = (HPEN)   GetStockObject ( NULL_PEN    );
  myLineWidth         = 1;
  myPolyBrush         = (HBRUSH) GetStockObject ( NULL_BRUSH  );
  myTextPen           = (HPEN)   GetStockObject ( NULL_PEN    );
  myTextBrush         = (HBRUSH) GetStockObject ( NULL_BRUSH  );
  myTextFont          = (HFONT)  GetStockObject ( SYSTEM_FONT );
  myMarkerPen         = (HPEN)   GetStockObject ( NULL_PEN    );
  myTextSlant         = EPS;

  myPO                = (HPEN)   NULL;
  myBO                = (HBRUSH) NULL;
  myFO                = (HFONT)  NULL;
  SetRectEmpty        ( &myURect );

  myMatrix.eM11 = myMatrix.eM22 = 1.0F;
  myMatrix.eM12 = myMatrix.eM21 = 0.0F;
  myMatrix.eDx  = myMatrix.eDy  = 0.0F;

  myFlags            &= ~W32F_WIN95;
}  // end constructor

WNT_Allocator :: ~WNT_Allocator ()
{
  DeletePen   ( myLinePen );
  DeleteBrush ( myPolyBrush );
  DeletePen   ( myTextPen );
  DeleteBrush ( myTextBrush );
  DeletePen   ( myMarkerPen );
}  // end destructor

void WNT_Allocator :: TransformPoint  (int& X, int& Y)
{
  X = int ( myScaleX * ( X - myPivot.x ) );
  Y = int ( myScaleY * ( Y - myPivot.y ) );
} // WNT_Allocator :: TransformPoint

void WNT_Allocator :: TransformPoint  (LPPOINT lpPoint)
{
  lpPoint->x = int ( myScaleX * ( lpPoint->x - myPivot.x ) );
  lpPoint->y = int ( myScaleY * ( lpPoint->y - myPivot.y ) );
} // WNT_Allocator :: TransformPoint

#define ADD_POINT_TO_RECT(aPoint) \
  { if ( aPoint.x < myURect.left   ) myURect.left   = aPoint.x - myLineWidth - 1; \
    if ( aPoint.x > myURect.right  ) myURect.right  = aPoint.x + myLineWidth + 1; \
    if ( aPoint.y < myURect.top    ) myURect.top    = aPoint.y - myLineWidth - 1; \
    if ( aPoint.y > myURect.bottom ) myURect.bottom = aPoint.y + myLineWidth + 1; \
  }

void WNT_Allocator :: Register ( int x, int y )
{
  static POINT pt;
  pt.x = x; pt.y = y;
  LPtoDP ( myHDC, &pt, 1 );
  ADD_POINT_TO_RECT (pt);
} // WNT_Allocator :: Register

void WNT_Allocator :: Register ( LPPOINT lpPts, int nPts )
{
  for ( int i = 0; i < nPts; i++ )
    Register ( lpPts[i].x, lpPts[i].y );
} // WNT_Allocator :: Register

void WNT_Allocator :: Register ( LPRECT lpRect )
{
  Register ( lpRect->left,  lpRect->top    );
  Register ( lpRect->left,  lpRect->bottom );
  Register ( lpRect->right, lpRect->top    );
  Register ( lpRect->right, lpRect->bottom );
} // WNT_Allocator :: Register

void WNT_Allocator :: RecalcMatrix (
                        XFORM& Matrix, double Angle,
                        double PivotX, double PivotY
                      )
{
  float CosA = ( float )cos ( Angle );
  float SinA = ( float )sin ( Angle );
  Matrix.eM11 =  float ( CosA );
  Matrix.eM12 =  float ( SinA );
  Matrix.eM21 = -float ( SinA );
  Matrix.eM22 =  float ( CosA );
  Matrix.eDx  =  float ( PivotX );
  Matrix.eDy  =  float ( PivotY );
}  // end  WNT_Allocator :: RecalcMatrix

void WNT_Allocator :: Play ( HDC hdc, PSIZE szClient )
{
  CopyMemory ( &mySize, szClient, sizeof ( SIZE ) );
  if (  !( myFlags & W32F_EMPTY )  ) {
    POINT pVOExt;
    SIZE  szWExt, szVExt;
    int   nMapMode, nGMode, nBkMode;

    myHDC          = hdc;
    nMapMode       = SetMapMode ( myHDC, MM_ISOTROPIC );
    SetRect          ( &myURect, mySize.cx, mySize.cy, 0, 0            );
    SetWindowExtEx   ( myHDC, szClient->cx, -szClient->cy,     &szWExt );
    SetViewportExtEx ( myHDC, szClient->cx,  szClient->cy,     &szVExt );
    SetViewportOrgEx ( myHDC, 0,             szClient->cy - 1, &pVOExt );
    nGMode         = SetGraphicsMode  ( myHDC, GM_ADVANCED );
    nBkMode        = SetBkMode        ( myHDC, TRANSPARENT );
    SelectPen        ( myHDC, myLinePen   );
    SelectBrush      ( myHDC, myPolyBrush );
    SetTextColor     ( myHDC, myTextColor );

    Xform ();
    for ( PW32_Block aBlock = myStart; aBlock != NULL; aBlock = aBlock -> next )
      for ( int i = 0; i < aBlock -> free; i += aBlock -> data[ i ] )
        if ( aBlock -> data[ i + 1 ] != __W32_DATA ) {
          W32_Note* pNote = ( W32_Note* )&( aBlock -> data[ i + 1 ] );
          pNote -> Play ();
        }  // end if

    SetMapMode       ( myHDC, nMapMode                   );
    SetWindowExtEx   ( myHDC, szWExt.cx, szWExt.cy, NULL );
    SetViewportExtEx ( myHDC, szVExt.cx, szVExt.cy, NULL );
    SetViewportOrgEx ( myHDC, pVOExt.x,  pVOExt.y,  NULL );
    SetBkMode        ( myHDC, nBkMode                    );
    SetGraphicsMode  ( myHDC, nGMode                     );
  }  // end if
}  // end WNT_Allocator :: Play

void WNT_Allocator :: URect ( LPRECT lpRect )
{
  CopyRect ( lpRect, &myURect );
  if ( lpRect -> top    < 0 )         lpRect -> top    = 0;
  if ( lpRect -> left   < 0 )         lpRect -> left   = 0;
  if ( lpRect -> right  > mySize.cx ) lpRect -> right  = mySize.cx;
  if ( lpRect -> bottom > mySize.cy ) lpRect -> bottom = mySize.cy;
}  // end WNT_Allocator :: URect

void WNT_Allocator :: Xform ( void )
{
  if ( myAngle  != 0.0 || myPivot.x != 0 || myPivot.y != 0 ||
       myMove.x != 0   || myMove.y  != 0 )
  {
    RecalcMatrix         ( myMatrix, myAngle,
                           myPivot.x + myMove.x,
                           myPivot.y + myMove.y );
    SetWorldTransform    ( myHDC, &myMatrix );
  } else {
    ModifyWorldTransform ( myHDC, NULL, MWT_IDENTITY );
  }
}  // end WNT_Allocator :: Xform

void WNT_Allocator :: Point ( int x, int y ) {

 new ( this ) WNT_PointNote ( x, y );

}  // end WNT_Allocator :: Point

void WNT_Allocator :: MarkerPoint ( int x, int y ) {

 new ( this ) WNT_MarkerPointNote ( x, y );

}  // end WNT_Allocator :: MarkerPoint

void WNT_Allocator :: Line ( int x, int y, int x1, int y1 ) {

 new ( this ) WNT_LineNote ( x, y, x1, y1 );

}  // end WNT_Allocator :: Line

void WNT_Allocator :: PolyEllipse ( int xc, int yc, int xr, int yr ) {

 new ( this ) WNT_PolyEllipseNote ( xc, yc, xr, yr );

}  // end WNT_Allocator :: PolyEllipse

void WNT_Allocator :: Ellipse ( int xc, int yc, int xr, int yr ) {

 new ( this ) WNT_EllipseNote ( xc, yc, xr, yr );

}  // end WNT_Allocator :: Ellipse

void WNT_Allocator :: Arc (
                       int xc, int yc, int xr, int yr,
                       double sa, double oa
                      ) {
                      
 new ( this ) WNT_ArcNote ( xc, yc, xr, yr, sa, oa );                      
                      
}  // end WNT_Allocator :: Arc

void WNT_Allocator :: PolyChord (
                       int xc, int yc, int xr, int yr,
                       double sa, double oa
                      ) {
                      
 new ( this ) WNT_PolyChordNote ( xc, yc, xr, yr, sa, oa );
                      
}  // end WNT_Allocator :: PolyChord

void WNT_Allocator :: Chord (
                       int xc, int yc, int xr, int yr,
                       double sa, double oa
                      ) {
                      
 new ( this ) WNT_PolyChordNote ( xc, yc, xr, yr, sa, oa );
                      
}  // end WNT_Allocator :: Chord

void WNT_Allocator :: PolySector (
                       int xc, int yc, int xr, int yr,
                       double sa, double oa
                      ) {
                      
 new ( this ) WNT_PolySectorNote ( xc, yc, xr, yr, sa, oa );
                      
}  // end WNT_Allocator :: PolySector

void WNT_Allocator :: Sector (
                       int xc, int yc, int xr, int yr,
                       double sa, double oa
                      ) {
                      
 new ( this ) WNT_SectorNote ( xc, yc, xr, yr, sa, oa );
                      
}  // end WNT_Allocator :: Sector

void WNT_Allocator :: PolyMarker ( int aMaxPoints ) {

 new ( this ) W32_PolyMarkerNote ( aMaxPoints );

}  // end WNT_Allocator :: PolyMarker

void WNT_Allocator :: PolyMarker1 (
                       int aMaxPoints, GetPointFunc fn,
                       int aStartPoint, void* fParam
                      ) {
                      
 new ( this ) WNT_PolyMarker1Note (
               aMaxPoints, fn, aStartPoint, fParam
              );
                      
}  // end WNT_Allocator :: PolyMarker1

void WNT_Allocator :: PolyMarker2 (
                       int aMaxPoints, GetPointFunc fn,
                       int aStartPoint, void* fParam
                      ) {
  
 new ( this ) WNT_PolyMarker2Note (
               aMaxPoints, fn, aStartPoint, fParam
              );
                      
}  // end WNT_Allocator :: PolyMarker1

W32_Note* WNT_Allocator :: Polygon ( int aMaxPoints ) {

 return new ( this ) WNT_PolygonNote ( aMaxPoints );

}  // end WNT_Allocator :: Polygon

W32_Note* WNT_Allocator :: Polyline ( int aMaxPoints ) {

 return new ( this ) WNT_PolylineNote ( aMaxPoints );

}  // end WNT_Allocator :: Polyline

void WNT_Allocator :: Image (
                       int x, int y, PW32_Bitmap pBmp, double aScale
                      ) {

 new ( this ) WNT_ImageNote ( x, y, pBmp, aScale );

}  // end WNT_Allocator :: Image

void WNT_Allocator :: Text (
                       int x, int y, double angle, void* text,
                       BOOL fWide, BOOL fOutlined
                      ) {

 new ( this ) WNT_TextNote ( x, y, angle, text, fWide, fOutlined );

}  // end WNT_Allocator :: Text

void WNT_Allocator :: Polytext (
                       int x, int y, double angle, double margin,
                       void* text, BOOL fWide, BOOL fOutlined
                      ) {
                      
 new ( this ) WNT_PolyTextNote ( x, y, angle, margin, text, fWide, fOutlined );
                      
}  // end WNT_Allocator :: Polytext

void WNT_Allocator :: BeginMarker (
                       int x, int y, int w, int h, double angle
                      ) {
                      
 new ( this ) WNT_BeginMarkerNote ( x, y, w, h, angle );                      
                      
}  // end WNT_Allocator :: BeginMarker

void WNT_Allocator :: EndMarker ( void ) {

 new ( this ) WNT_EndMarkerNote ();

}  // end WNT_Allocator :: EndMarker

void WNT_Allocator :: LineAttrib (
                       DWORD width, PLOGBRUSH plb,
                       DWORD nStyles, PDWORD pdwStyle
                      ) {
                      
 new ( this ) WNT_LineAttribNote ( width, plb, nStyles, pdwStyle );
                      
}  // end WNT_Allocator :: LineAttrib

void WNT_Allocator :: PolyAttrib (
                       PLOGBRUSH plb, BOOL fDrawEdge, int aFillMode
                      ) {

 new ( this ) WNT_PolyAttribNote ( plb, fDrawEdge, aFillMode );

}  // end WNT_Allocator :: PolyAttrib

void WNT_Allocator :: TextAttrib (
                       HFONT hFont, COLORREF color, double slant,
                       double hScale, double vScale,
                       BOOL fUnderlined, BOOL fFree, BOOL fIndex
                      ) {
 
 new ( this ) WNT_TextAttribNote (
               hFont, color, slant, hScale, vScale,
               fUnderlined, fFree, fIndex
              );

}  // end WNT_Allocator :: TextAttrib

void WNT_Allocator :: MarkerAttrib (
                       COLORREF color, DWORD dwWidth, BOOL fFill
                      ) {
                      
 new ( this ) WNT_MarkerAttribNote ( color, dwWidth, fFill );                      
                      
}  // end WNT_Allocator :: MarkerAttrib

void WNT_Allocator :: FunCall (
                       W32_FCall fCall, int sz, PW32_FCALLPARAM param
                      ) {

 new ( this ) W32_FCallNote ( fCall, sz, param );

}  // end WNT_Allocator :: FunCall

void WNT_Allocator :: GetExtent ( LPSIZE lpSZ )
{
  CopyMemory ( lpSZ, &mySize, sizeof ( SIZE ) );
}

int WNT_Allocator :: TextSize ( HDC hdc, char* str, PSIZE psz ) {

 int      gMode = GetGraphicsMode ( hdc );
 XFORM    xfm, xfms;
 POINT    pt;
 ABC      abcf;

 if ( gMode == GM_ADVANCED )
  GetWorldTransform ( hdc, &xfms );
 else
  SetGraphicsMode ( hdc, GM_ADVANCED );

 ModifyWorldTransform ( hdc, &xfmIdent, MWT_IDENTITY );
 xfm.eM11 = 1.0F;
 xfm.eM12 = 0.0F;
 xfm.eM21 = ( FLOAT )tan ( myTextSlant );
 xfm.eM22 = 1.0F;
 xfm.eDx  = 0.0F;
 xfm.eDy  = 0.0F;
 ModifyWorldTransform ( hdc, &xfm, MWT_LEFTMULTIPLY );
 xfm.eM11 = FLOAT ( myTextHScale * myScaleX );
 xfm.eM12 = 0.0F;
 xfm.eM21 = 0.0F;
 xfm.eM22 = FLOAT ( myTextVScale * myScaleY );
 xfm.eDx  = 0.0F;
 xfm.eDy  = 0.0F;
 ModifyWorldTransform ( hdc, &xfm, MWT_LEFTMULTIPLY );

 MoveToEx ( hdc, 0, 0, &pt );
 GetTextExtentPoint32A (
  hdc, str, lstrlenA ( str ), psz
 );
 GetCharABCWidthsA ( hdc, str[ 0 ], str[ 0 ], &abcf );
 MoveToEx ( hdc, pt.x, pt.y, NULL );

 if ( gMode == GM_ADVANCED )
   SetWorldTransform ( hdc, &xfms );
 else {
   psz->cx    = LONG ( psz->cx   * xfm.eM11 );
   psz->cy    = LONG ( psz->cy   * xfm.eM22 );
   abcf.abcA  = UINT ( abcf.abcA * xfm.eM11 );
   SetGraphicsMode ( hdc, gMode );
 }  // end else

 return abcf.abcA;
}  // end WNT_Allocator :: TextSize
  
int WNT_Allocator :: TextSize ( HDC hdc, wchar_t* str, PSIZE psz ) {

 int      gMode = GetGraphicsMode ( hdc );
 XFORM    xfm, xfms;
 POINT    pt;
 ABC      abcf;

 if ( gMode == GM_ADVANCED )
  GetWorldTransform ( hdc, &xfms );
 else
  SetGraphicsMode ( hdc, GM_ADVANCED );

 ModifyWorldTransform ( hdc, &xfmIdent, MWT_IDENTITY );
 xfm.eM11 = 1.0F;
 xfm.eM12 = 0.0F;
 xfm.eM21 = ( FLOAT )tan ( myTextSlant );
 xfm.eM22 = 1.0F;
 xfm.eDx  = 0.0F;
 xfm.eDy  = 0.0F;
 ModifyWorldTransform ( hdc, &xfm, MWT_LEFTMULTIPLY );
 xfm.eM11 = FLOAT ( myTextHScale * myScaleX );
 xfm.eM12 = 0.0F;
 xfm.eM21 = 0.0F;
 xfm.eM22 = FLOAT ( myTextVScale * myScaleY );
 xfm.eDx  = 0.0F;
 xfm.eDy  = 0.0F;
 ModifyWorldTransform ( hdc, &xfm, MWT_LEFTMULTIPLY );

 MoveToEx ( hdc, 0, 0, &pt );
 GetTextExtentPoint32W (
  hdc, str, lstrlenW ( str ), psz
 );
 GetCharABCWidthsW ( hdc, str[ 0 ], str[ 0 ], &abcf );
 MoveToEx ( hdc, pt.x, pt.y, NULL );

 if ( gMode == GM_ADVANCED )
   SetWorldTransform ( hdc, &xfms );
 else {
   psz->cx    = LONG ( psz->cx   * xfm.eM11 );
   psz->cy    = LONG ( psz->cy   * xfm.eM22 );
   abcf.abcA  = UINT ( abcf.abcA * xfm.eM11 );
   SetGraphicsMode ( hdc, gMode );
 }  // end else

 return abcf.abcA;
}  // end WNT_Allocator :: TextSize

////////////////////////////////////////////////////////////////////////////////
// W N T _ P o i n t N o t e                                                  //
////////////////////////////////////////////////////////////////////////////////
WNT_PointNote :: WNT_PointNote ( int x, int y ) :
                  W32_PointNote ( x, y )
{
}  // end constructor

void WNT_PointNote :: Play ( BOOL fDummy )
{
  Xform ();
  SetPixel  ( ALLOCATOR -> myHDC, myTX, myTY, ALLOCATOR -> myPointColor );
}  // end WNT_PointNote :: Play

void WNT_PointNote :: Xform ()
{
  myTX = myX;
  myTY = myY;
  ALLOCATOR -> TransformPoint ( myTX, myTY );
  ALLOCATOR -> Register       ( myTX, myTY );
}
////////////////////////////////////////////////////////////////////////////////
// W N T _ M a r k e r P o i n t N o t e                                      //
////////////////////////////////////////////////////////////////////////////////
WNT_MarkerPointNote :: WNT_MarkerPointNote ( int x, int y ) :
                        WNT_PointNote ( x, y )
{
}  // end constructor

void WNT_MarkerPointNote :: Play ( BOOL fDummy )
{
  Xform ();
  SetPixel  ( ALLOCATOR -> myHDC, myTX, myTY, ALLOCATOR -> myMarkerPointColor );
}  // end WNT_MarkerPointNote :: Play
////////////////////////////////////////////////////////////////////////////////
// W N T _ L i n e N o t e                                                    //
////////////////////////////////////////////////////////////////////////////////
WNT_LineNote :: WNT_LineNote ( int x, int y, int x1, int y1 ) :
                 WNT_PointNote ( x, y )
{
  myX2 = x1;
  myY2 = y1;
}  // end constructor

void WNT_LineNote :: Play ( BOOL fDummy )
{
  Xform ();
  MoveToEx                 ( ALLOCATOR -> myHDC, myTX , myTY, NULL    );
  LineTo                   ( ALLOCATOR -> myHDC, myTX2, myTY2         );
}  // end WNT_LineNote :: Play

void WNT_LineNote :: Xform ()
{
  WNT_PointNote :: Xform ();
  myTX2 = myX2;
  myTY2 = myY2;
  ALLOCATOR -> TransformPoint ( myTX2, myTY2 );
  ALLOCATOR -> Register       ( myTX2, myTY2 );
}
////////////////////////////////////////////////////////////////////////////////
// W N T _ P o l y E l l i p s e N o t e                                      //
////////////////////////////////////////////////////////////////////////////////
WNT_PolyEllipseNote :: WNT_PolyEllipseNote ( int xc, int yc, int xr, int yr ) :
                        WNT_PointNote ( xc, yc )
{
  myXr = xr;
  myYr = yr;
}  // end constructor

void WNT_PolyEllipseNote :: Play ( BOOL fDummy )
{
  Xform ();
  Ellipse ( ALLOCATOR -> myHDC, myTX - myTXr, myTY - myTYr,
                                myTX + myTXr, myTY + myTYr );
}  // end WNT_PolyEllipseNote :: Play

void WNT_PolyEllipseNote :: Xform ()
{
  WNT_PointNote :: Xform ();
  myTXr = int (myXr * ALLOCATOR -> myScaleX);
  myTYr = int (myYr * ALLOCATOR -> myScaleY);
  RECT r = { myTX - myTXr, myTY - myTYr, myTX + myTXr, myTY + myTYr };
  ALLOCATOR -> Register ( &r );
}
////////////////////////////////////////////////////////////////////////////////
// W N T _ E l l i p s e N o t e                                              //
////////////////////////////////////////////////////////////////////////////////
WNT_EllipseNote :: WNT_EllipseNote ( int xc, int yc, int xr, int yr ) :
                    WNT_PolyEllipseNote ( xc, yc, xr, yr )
{
}  // end constructor

void WNT_EllipseNote :: Play ( BOOL fDummy )
{
  Xform ();
  HBRUSH hob = SelectBrush ( ALLOCATOR -> myHDC, GetStockObject (NULL_BRUSH) );
  Ellipse     ( ALLOCATOR -> myHDC, myTX - myTXr, myTY - myTYr,
                                    myTX + myTXr, myTY + myTYr );
  SelectBrush ( ALLOCATOR -> myHDC, hob );
}  // end WNT_PolyEllipseNote :: Play
////////////////////////////////////////////////////////////////////////////////
// W N T _ A r c N o t e                                                      //
////////////////////////////////////////////////////////////////////////////////
WNT_ArcNote :: WNT_ArcNote (
                int xc, int yc, int xr, int yr, double sa, double oa
               ) : WNT_PolyEllipseNote ( xc, yc, xr, yr )
{
  int    Radius = max ( xr, yr );
  double sSinA  = sin ( sa + oa );
  double sCosA  = cos ( sa + oa );
  double eSinA  = sin ( sa );
  double eCosA  = cos ( sa );
  myDirect = ( sa + oa > 0. ) ? AD_COUNTERCLOCKWISE : AD_CLOCKWISE;

  mySX = int ( sCosA * 2 * Radius + xc );
  mySY = int ( sSinA * 2 * Radius + yc );
  myEX = int ( eCosA * 2 * Radius + xc );
  myEY = int ( eSinA * 2 * Radius + yc );
}  // end constructor

void WNT_ArcNote :: Play ( BOOL fDummy )
{
  Xform ();
  HDC hdc = ALLOCATOR -> myHDC;
  SetArcDirection ( ALLOCATOR -> myHDC, myDirect );
  Arc ( ALLOCATOR -> myHDC, myTX - myTXr, myTY - myTYr,
                            myTX + myTXr, myTY + myTYr,
                            myTSX, myTSY, myTEX, myTEY );
}  // end WNT_ArcNote :: Play

void WNT_ArcNote :: Xform ()
{
  WNT_PolyEllipseNote :: Xform ();
  myTSX = mySX;
  myTSY = mySY;
  ALLOCATOR -> TransformPoint ( myTSX, myTSY );
  myTEX = myEX;
  myTEY = myEY;
  ALLOCATOR -> TransformPoint ( myTEX, myTEY );
}
////////////////////////////////////////////////////////////////////////////////
// W N T _ P o l y C h o r d N o t e                                          //
////////////////////////////////////////////////////////////////////////////////
WNT_PolyChordNote :: WNT_PolyChordNote (
                      int xc, int yc, int xr, int yr, double sa, double oa
                     ) : WNT_ArcNote ( xc, yc, xr, yr, sa, oa )
{
}  // end constructor

void WNT_PolyChordNote :: Play ( BOOL fDummy )
{
  Xform ();
  Chord ( ALLOCATOR -> myHDC, myTX - myTXr, myTY - myTYr,
                              myTX + myTXr, myTY + myTYr,
                              myTEX, myTEY, myTSX, myTSY );
}  // end WNT_PolyChordNote :: Play
////////////////////////////////////////////////////////////////////////////////
// W N T _ C h o r d N o t e                                                  //
////////////////////////////////////////////////////////////////////////////////
WNT_ChordNote :: WNT_ChordNote (
                  int xc, int yc, int xr, int yr, double sa, double oa
                 ) : WNT_PolyChordNote ( xc, yc, xr, yr, sa, oa )
{
}  // end constructor

void WNT_ChordNote :: Play ( BOOL fDummy )
{
  Xform ();
  HBRUSH hob = SelectBrush ( ALLOCATOR -> myHDC, GetStockObject (NULL_BRUSH) );
  Chord       ( ALLOCATOR -> myHDC, myTX - myTXr, myTY - myTYr,
                                    myTX + myTXr, myTY + myTYr,
                                    myTEX, myTEY, myTSX, myTSY );
  SelectBrush ( ALLOCATOR -> myHDC, hob );
}  // end WNT_ChordNote :: Play
////////////////////////////////////////////////////////////////////////////////
// W N T _ P o l y S e c t o r N o t e                                        //
////////////////////////////////////////////////////////////////////////////////
WNT_PolySectorNote :: WNT_PolySectorNote (
                      int xc, int yc, int xr, int yr, double sa, double oa
                     ) : WNT_ArcNote ( xc, yc, xr, yr, sa, oa )
{
}  // end constructor

void WNT_PolySectorNote :: Play ( BOOL fDummy )
{
  Xform ();
  Pie ( ALLOCATOR -> myHDC, myTX - myTXr, myTY - myTYr,
                            myTX + myTXr, myTY + myTYr,
                            myTEX, myTEY, myTSX, myTSY );
}  // end WNT_PolySectorNote :: Play
////////////////////////////////////////////////////////////////////////////////
// W N T _ S e c t o r N o t e                                                //
////////////////////////////////////////////////////////////////////////////////
WNT_SectorNote :: WNT_SectorNote (
                  int xc, int yc, int xr, int yr, double sa, double oa
                 ) : WNT_PolySectorNote ( xc, yc, xr, yr, sa, oa )
{
}  // end constructor

void WNT_SectorNote :: Play ( BOOL fDummy )
{
  Xform ();
  HBRUSH hob = SelectBrush ( ALLOCATOR -> myHDC, GetStockObject (NULL_BRUSH) );
  Pie         ( ALLOCATOR -> myHDC, myTX - myTXr, myTY - myTYr,
                                    myTX + myTXr, myTY + myTYr,
                                    myTEX, myTEY, myTSX, myTSY );
  SelectBrush ( ALLOCATOR -> myHDC, hob );
}  // end WNT_SectorNote :: Play

////////////////////////////////////////////////////////////////////////////////
// D r a w P o l y l i n e                                                    //
////////////////////////////////////////////////////////////////////////////////
void DrawPolyline ( HDC hdc, LPPOINT lpPts, int nPts, BOOL fClose )
{
  BeginPath     ( hdc );
  Polyline      ( hdc, lpPts, nPts );
  if (fClose)
    CloseFigure ( hdc );
  EndPath       ( hdc );
  StrokePath    ( hdc );
}
////////////////////////////////////////////////////////////////////////////////
// D r a w P o l y g o n                                                      //
////////////////////////////////////////////////////////////////////////////////
void DrawPolygon  ( HDC hdc, LPPOINT lpPts, int nPts, UINT dwFlags )
{
  HPEN hpo = SelectPen ( hdc, GetStockObject ( NULL_PEN ) );
  Polygon   ( hdc, lpPts, nPts );
  SelectPen ( hdc, hpo );

  if ( dwFlags & POLYF_NOCLOSEDGE )
    DrawPolyline ( hdc, lpPts, nPts, FALSE );
  else if ( !( dwFlags & POLYF_NOEDGE ) )
    DrawPolyline ( hdc, lpPts, nPts, TRUE  );
}
////////////////////////////////////////////////////////////////////////////////
// W N T _ P o l y M a r k e r 1 N o t e                                      //
////////////////////////////////////////////////////////////////////////////////
WNT_PolyMarker1Note :: WNT_PolyMarker1Note (
                        int aMaxPoints, GetPointFunc fn, int aStartPoint,
                        void* fParam
                       ) : W32_PolyMarker1Note ( aMaxPoints, fn, aStartPoint, fParam)
{
}  // end constructor

void WNT_PolyMarker1Note :: Play ( BOOL fDummy )
{
  if ( ALLOCATOR -> myFlags & W32F_MFILL )
    DrawPolygon  ( ALLOCATOR -> myHDC, myPoints, mySetPoints, POLYF_NOCLOSEDGE );
  else
    DrawPolyline ( ALLOCATOR -> myHDC, myPoints, mySetPoints, FALSE );
}  // end WNT_PolyMarker1Note :: Play
////////////////////////////////////////////////////////////////////////////////
// W N T _ P o l y M a r k e r 2 N o t e                                      //
////////////////////////////////////////////////////////////////////////////////
WNT_PolyMarker2Note :: WNT_PolyMarker2Note (
                        int aMaxPoints, GetPointFunc fn, int aStartPoint,
                        void* fParam
                       ) : WNT_PolyMarker1Note (
                            aMaxPoints, fn, aStartPoint, fParam
                           )
{
}  // end constructor

void WNT_PolyMarker2Note :: Play ( BOOL fDummy )
{
  if ( ALLOCATOR -> myFlags & W32F_MFILL )
    DrawPolygon  ( ALLOCATOR -> myHDC, myPoints, mySetPoints - 1, 0 );
  else
    DrawPolyline ( ALLOCATOR -> myHDC, myPoints, mySetPoints - 1, TRUE );
}  // end WNT_PolyMarker2Note :: Play
////////////////////////////////////////////////////////////////////////////////
// W N T _ P o l y g o n N o t e                                              //
////////////////////////////////////////////////////////////////////////////////
WNT_PolygonNote :: WNT_PolygonNote ( int aMaxPoints ) :
                    W32_PolygonNote ( aMaxPoints )
{
  myTPoints = ( LPPOINT ) ALLOCATOR -> NewData (myMaxPoints * sizeof ( POINT ));
}  // end constructor

void WNT_PolygonNote :: Play ( BOOL fDummy )
{
  Xform ();
  DrawPolygon (
    ALLOCATOR -> myHDC, myTPoints, mySetPoints,
    ALLOCATOR -> myFlags & W32F_POUTL ? 0 : POLYF_NOEDGE
  );
}  // end WNT_PolygonNote :: Play

void WNT_PolygonNote :: Xform ()
{
  CopyMemory ( myTPoints, myPoints, mySetPoints*sizeof(POINT) );
  for ( int i = 0; i < mySetPoints; i++ )
    ALLOCATOR -> TransformPoint ( (myTPoints + i) );
  ALLOCATOR -> Register ( myTPoints, mySetPoints );
}
////////////////////////////////////////////////////////////////////////////////
// W N T _ P o l y l i n e N o t e                                            //
////////////////////////////////////////////////////////////////////////////////
WNT_PolylineNote :: WNT_PolylineNote ( int aMaxPoints ) :
                     WNT_PolygonNote ( aMaxPoints )
{
}  // end constructor

void WNT_PolylineNote :: Play ( BOOL fDummy )
{
  Xform ();
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
  DrawPolyline ( ALLOCATOR->myHDC, myTPoints, nPts, fClose );
}  // end WNT_PolylineNote :: Play
////////////////////////////////////////////////////////////////////////////////
// W N T _ I m a g e N o t e                                                  //
////////////////////////////////////////////////////////////////////////////////
WNT_ImageNote :: WNT_ImageNote (
                  int x, int y, PW32_Bitmap pBmp, double aScale
                 ) : WNT_PointNote ( x, y )
{
  myBitmap = pBmp;
  ++pBmp   -> nUsed;
  myScale  = aScale;
}  // end constructor

WNT_ImageNote :: ~WNT_ImageNote ()
{
  if ( myBitmap -> hBmp != NULL && --myBitmap -> nUsed == 0 ) {
    DeleteObject ( myBitmap -> hBmp );
    HeapFree (  GetProcessHeap (), 0, ( PVOID )myBitmap  );
  }  // end if
}  // end WNT_ImageNote :: ~WNT_ImageNote

void WNT_ImageNote :: Play ( BOOL fDummy )
{
  Xform ();
  int     xx, yy, w, h;
  HDC     hdcMem;
  BITMAP  bmp;
  HBITMAP hbo;

  GetObject (  myBitmap -> hBmp, sizeof ( BITMAP ), &bmp  );
  xx = myTX;
  yy = myTY;
  w  = bmp.bmWidth;
  h  = bmp.bmHeight;
  if ( myScale != 1.0 ) {
    w  = int (  w * myScale + 0.5 );
    h  = int (  h * myScale + 0.5 );
  }
  if ( ALLOCATOR -> myScaleX != 1.0 ) {
    xx = int ( xx * ALLOCATOR -> myScaleX + 0.5 );
    w  = int (  w * ALLOCATOR -> myScaleX + 0.5 );
  }  // end if
  if ( ALLOCATOR -> myScaleY != 1.0 ) {
    yy = int ( yy * ALLOCATOR -> myScaleY + 0.5 );
    h  = int (  h * ALLOCATOR -> myScaleY + 0.5 );
  }  // end if
  xx = xx - ( w >> 1 );
  yy = yy + ( h >> 1 );
  hdcMem = CreateCompatibleDC ( ALLOCATOR -> myHDC );
   hbo   = SelectBitmap ( hdcMem, myBitmap -> hBmp );
    SetStretchBltMode   ( hdcMem, COLORONCOLOR );
    StretchBlt (
      ALLOCATOR -> myHDC,    xx, yy,           w,           -h,
      hdcMem,                 0,  0, bmp.bmWidth, bmp.bmHeight,
      SRCCOPY
    );
    RECT r = { xx, yy, xx + w, yy - h };
    ALLOCATOR -> Register ( &r );
   SelectBitmap ( hdcMem, hbo );
  DeleteDC      ( hdcMem );
}  // end WNT_ImageNote :: Play
////////////////////////////////////////////////////////////////////////////////
// W N T _ T e x t N o t e                                                    //
////////////////////////////////////////////////////////////////////////////////
#define ROUNDL( d ) ( ( long )( ( d ) + ( ( d ) > 0 ? 0.5 : -0.5 ) ) )
WNT_TextNote :: WNT_TextNote (
                 int x, int y, double angle, void* text,
                 BOOL fWide, BOOL fOutlined
                ) : WNT_PointNote ( x, y )
{
  DWORD dwLen = fWide ? ( ( lstrlenW ( ( LPCWSTR )text ) + 1 ) << 1 )
                      : ( ( lstrlenA ( ( LPCSTR  )text ) + 1 ) << 0 );
  myText = ALLOCATOR -> NewData ( dwLen );
  CopyMemory ( myText, text, dwLen );
  myFlags = (  ( fWide     ? W32F_TUNICODE : 0 ) |
               ( fOutlined ? W32F_TOUTLINE : 0 )
            );
  ALLOCATOR -> RecalcMatrix ( RMatrix, angle );
}  // end constructor

void WNT_TextNote :: PaintText ()
{
  if (myFlags & W32F_TUNICODE)
    TextOutW ( ALLOCATOR -> myHDC, 0, myShift, (LPCWSTR)myText, lstrlenW ((LPCWSTR)myText) );
  else
    TextOutA ( ALLOCATOR -> myHDC, 0, myShift, (LPCSTR)myText,  lstrlenA ((LPCSTR)myText) );
}  // end WNT_TextNote :: PaintText 

void WNT_TextNote :: SetAttribs ()
{
  ALLOCATOR -> myPO = SelectPen   (ALLOCATOR -> myHDC, ALLOCATOR -> myTextPen  );
  ALLOCATOR -> myBO = SelectBrush (ALLOCATOR -> myHDC, ALLOCATOR -> myTextBrush);
  ALLOCATOR -> myFO = SelectFont  (ALLOCATOR -> myHDC, ALLOCATOR -> myTextFont );
}  // end WNT_TextNote :: SetAttribs

void WNT_TextNote :: RestoreAttribs ()
{
  SelectPen   ( ALLOCATOR -> myHDC, ALLOCATOR -> myPO );
  SelectBrush ( ALLOCATOR -> myHDC, ALLOCATOR -> myBO );
  SelectFont  ( ALLOCATOR -> myHDC, ALLOCATOR -> myFO );
  ALLOCATOR -> Xform ();
}  // end WNT_TextNote :: RestoreAttribs

void WNT_TextNote :: OutlineText ()
{
  BeginPath  ( ALLOCATOR -> myHDC );
  PaintText  ( );
  EndPath    ( ALLOCATOR -> myHDC );
  StrokePath ( ALLOCATOR -> myHDC );
}  // end WNT_TextNote :: OutlineText

void WNT_TextNote :: FillText ()
{
  PaintText ();
}  // end WNT_TextNote :: FillText

void WNT_TextNote :: Play ( BOOL fDummy )
{
  Xform ();
  SIZE              size;
  RECT              rect;
  HDC               hdc = ALLOCATOR -> myHDC;
  // Set new world transform and attribs to get text size and metrics
  SetAttribs ();
  if ( myFlags & W32F_TUNICODE )
    ALLOCATOR -> TextSize (  hdc, ( wchar_t* )myText, &size  );
  else
    ALLOCATOR -> TextSize (  hdc, ( char* )myText, &size  );
  ModifyWorldTransform  ( hdc, &RMatrix, MWT_LEFTMULTIPLY );
  // Register and draw the bounding rect
  rect.left    =  0;
  rect.top     =  0;
  rect.right   =  ROUNDL( ( FLOAT )size.cx * SMatrix.eM11 );
  rect.bottom  = -ROUNDL( ( FLOAT )size.cy * SMatrix.eM22 );
  if (ALLOCATOR -> myTextSlant >= 0.F)
    rect.right  +=  ROUNDL( ( FLOAT )rect.bottom * IMatrix.eM21 );
  else
    rect.left   +=  ROUNDL( ( FLOAT )rect.bottom * IMatrix.eM21 );
  ALLOCATOR -> Register ( &rect );
  // Draw ( fill or outline ) text
  myShift   = -size.cy;
  ModifyWorldTransform ( hdc, &IMatrix, MWT_LEFTMULTIPLY );
  ModifyWorldTransform ( hdc, &SMatrix, MWT_LEFTMULTIPLY );
  if ( myFlags & W32F_TOUTLINE || ALLOCATOR -> myFlags & W32F_MONO ) OutlineText ();
  else                                                               FillText    ();
  RestoreAttribs ();
}  // end WNT_TextNote :: Play

void WNT_TextNote :: Xform ( void )
{
  HDC hdc = ALLOCATOR -> myHDC;
  WNT_PointNote :: Xform ();
  RMatrix.eDx  =  float ( myTX ); 
  RMatrix.eDy  =  float ( myTY );
  SMatrix.eM11 =  float ( ALLOCATOR -> myTextHScale * ALLOCATOR -> myScaleX );
  SMatrix.eM12 =  0.0F;
  SMatrix.eM21 =  0.0F;
  SMatrix.eM22 = -float ( ALLOCATOR -> myTextVScale * ALLOCATOR -> myScaleY );
  SMatrix.eDx  =  0.0F;
  SMatrix.eDy  =  0.0F;
  IMatrix.eM11 =  1.0F;
  IMatrix.eM12 =  0.0F;
  IMatrix.eM21 =  float ( tan (ALLOCATOR -> myTextSlant) );
  IMatrix.eM22 =  1.0F;
  IMatrix.eDx  =  0.0F;
  IMatrix.eDy  =  0.0F;
}  // enf WNT_TextNote :: Xform
////////////////////////////////////////////////////////////////////////////////
// W N T _ P o l y T e x t N o t e                                            //
////////////////////////////////////////////////////////////////////////////////
WNT_PolyTextNote :: WNT_PolyTextNote (
                     int x, int y, double angle, double margin,
                     void* text, BOOL fWide, BOOL fOutlined
                    ) : WNT_TextNote ( x, y, angle, text, fWide, fOutlined ),
                        myMargin ( margin )
{
}  // end constructor

void WNT_PolyTextNote :: Play ( BOOL fDummy )
{
  Xform ();
  SIZE              size;
  RECT              rect;
  int               height;
  HDC               hdc = ALLOCATOR -> myHDC;
  // Set new world transform and attribs to get text size and metrics
  SetAttribs ();
  if ( myFlags & W32F_TUNICODE )
    ALLOCATOR -> TextSize (  hdc, ( wchar_t* )myText, &size  );
  else
    ALLOCATOR -> TextSize (  hdc, ( char* )myText, &size  );
  RestoreAttribs ();
  ModifyWorldTransform  ( hdc, &RMatrix, MWT_LEFTMULTIPLY );
  // Register and draw the bounding rect
  rect.left    =  0;
  rect.top     =  0;
  rect.right   =  ROUNDL( ( FLOAT )size.cx * SMatrix.eM11 );
  rect.bottom  = -ROUNDL( ( FLOAT )size.cy * SMatrix.eM22 );
  if (ALLOCATOR -> myTextSlant >= 0.F)
    rect.right  +=  ROUNDL( ( FLOAT )rect.bottom * IMatrix.eM21 );
  else
    rect.left   +=  ROUNDL( ( FLOAT )rect.bottom * IMatrix.eM21 );
  height = ROUNDL( (FLOAT)rect.bottom * (FLOAT)myMargin / 2.0F );
  InflateRect ( &rect, height, height );
  // Drawing round rectangle
  HPEN   hpo = ( (ALLOCATOR->myFlags & W32F_POUTL) ?
                    SelectPen ( hdc, ALLOCATOR -> myLinePen ):
                    SelectPen ( hdc, GetStockObject (NULL_PEN) ) );
  HBRUSH hbo = ( (ALLOCATOR->myFlags & W32F_NOFIL) ?
                    SelectBrush ( hdc, GetStockObject (NULL_BRUSH) ):
                    SelectBrush ( hdc, ALLOCATOR -> myPolyBrush ) );
  Rectangle    ( hdc, rect.left, rect.top, rect.right, rect.bottom );
  SelectBrush  ( hdc, hbo );
  SelectPen    ( hdc, hpo );
  ALLOCATOR -> Register ( &rect );
  // Draw ( fill or outline ) text
  myShift   = -size.cy;
  ModifyWorldTransform ( hdc, &IMatrix, MWT_LEFTMULTIPLY );
  ModifyWorldTransform ( hdc, &SMatrix, MWT_LEFTMULTIPLY );
  SetAttribs ();
  if (myFlags & W32F_TOUTLINE || ALLOCATOR -> myFlags & W32F_MONO) OutlineText ();
  else                                                             FillText    ();
  RestoreAttribs ();
}  // end WNT_PolyTextNote :: Play
////////////////////////////////////////////////////////////////////////////////
// W N T _ B e g i n M a r k e r N o t e                                      //
////////////////////////////////////////////////////////////////////////////////
WNT_BeginMarkerNote :: WNT_BeginMarkerNote (
                        int x, int y, int w, int h, double angle
                       ) : WNT_PointNote ( x, y ),
                           myAngle ( angle ), myWidth ( w ), myHeight ( h )
{
  myPrevAngle = ALLOCATOR -> myAngle;
  ALLOCATOR -> RecalcMatrix (myMatrix, myAngle - myPrevAngle, myX, myY);
}  // end constructor

void WNT_BeginMarkerNote :: Play ( BOOL fDummy )
{
  ALLOCATOR -> myPO = SelectPen (ALLOCATOR -> myHDC, ALLOCATOR -> myMarkerPen);
#ifdef PRO19042
  if ( ALLOCATOR -> myFlags & W32F_MFILL )
#else
  if ( ALLOCATOR -> myFlags | W32F_MFILL )
#endif
    ALLOCATOR -> myBO = SelectBrush (ALLOCATOR -> myHDC, ALLOCATOR -> myPolyBrush);
  else
    ALLOCATOR -> myBO = SelectBrush (ALLOCATOR -> myHDC, GetStockObject (NULL_BRUSH));
  Xform ();
}  // end WNT_BeginMarkerNote :: Play

void WNT_BeginMarkerNote :: Xform ()
{
  WNT_PointNote :: Xform ();
  if (myPrevAngle != ALLOCATOR -> myAngle) {
    myPrevAngle = ALLOCATOR -> myAngle;
    ALLOCATOR -> RecalcMatrix (myMatrix, myAngle - myPrevAngle, myX, myY);
  }  // end if
  myMatrix.eDx = float  ( myTX );
  myMatrix.eDy = float  ( myTY );
  ModifyWorldTransform  ( ALLOCATOR -> myHDC, &myMatrix, MWT_LEFTMULTIPLY );
  RECT r = { -myWidth / 2, -myHeight / 2, myWidth / 2, myHeight / 2 };
  ALLOCATOR -> Register ( &r );
}
////////////////////////////////////////////////////////////////////////////////
// W N T _ E n d M a r k e r N o t e                                          //
////////////////////////////////////////////////////////////////////////////////
WNT_EndMarkerNote :: WNT_EndMarkerNote ()
{
}

void WNT_EndMarkerNote :: Play ( BOOL fDummy )
{
  ALLOCATOR -> Xform ();
  SelectPen   ( ALLOCATOR -> myHDC, ALLOCATOR -> myPO );
  SelectBrush ( ALLOCATOR -> myHDC, ALLOCATOR -> myBO );
}  // end WNT_EndMarkerNote :: Play
////////////////////////////////////////////////////////////////////////////////
// W N T _ L i n e A t t r i b N o t e                                        //
////////////////////////////////////////////////////////////////////////////////
WNT_LineAttribNote :: WNT_LineAttribNote (
                       DWORD width, PLOGBRUSH plb,
                       DWORD nStyles, PDWORD pdwStyle
                      )
{
  myPointColor = plb -> lbColor;
  myWidth      = ((width == 0) ? 1 : width);

  if ((width == 0 || width == 1) && (nStyles == 0)) {
    myPen = CreatePen (PS_SOLID, width, plb->lbColor);
  } else {
    DWORD pStyle = ( nStyles > 0 ? PS_USERSTYLE : PS_SOLID );
    pStyle |= ( PS_ENDCAP_FLAT | PS_JOIN_MITER | PS_GEOMETRIC );
    myPen = ExtCreatePen ( pStyle, width, plb, nStyles, pdwStyle );
    if ( !myPen ) {
      char err[1024];
      DWORD errCode = GetLastError();
      printf ( " *** WNT_LineAttribNote : Can't create extended pen: %lu\n", errCode );
      wsprintf ( err, "Can't create extended pen: %lu", errCode );
      MessageBox ( NULL, err, "WNT_Allocator", MB_OK | MB_ICONEXCLAMATION );
      myPen = CreatePen ( PS_SOLID, 1, RGB(170,170,70) );
    }
  }
  Play  ( FALSE );
}  // end constructor

WNT_LineAttribNote :: ~WNT_LineAttribNote ( void )
{
#ifdef PRO19042
  if ( myPen ) {
#ifdef DEB
printf(" *** Destroy:WNT_LineAttribNote %x/%x\n",myPen,ALLOCATOR->myLinePen);
#endif
    DeletePen( myPen );
    myPen = NULL;
  }
#else
  if ( myPen && myPen != ALLOCATOR -> myLinePen )
    DeletePen( myPen );
#endif
}  // enf WNT_LineAttribNote :: ~WNT_LineAttribNote

void WNT_LineAttribNote :: Play ( BOOL fRealize )
{
  ALLOCATOR -> myLinePen    = myPen;
  ALLOCATOR -> myLineWidth  = myWidth;
  ALLOCATOR -> myPointColor = myPointColor;
  if ( fRealize )
    SelectPen ( ALLOCATOR -> myHDC, myPen );
}  // end WNT_LineAttribNote :: Play
////////////////////////////////////////////////////////////////////////////////
// W N T _ P o l y A t t r i b N o t e                                        //
////////////////////////////////////////////////////////////////////////////////
WNT_PolyAttribNote :: WNT_PolyAttribNote (
                       PLOGBRUSH plb, BOOL fDrawEdge, int aFillMode
                      )
{
  myfEdge    = fDrawEdge;
  myNoFill   = ( plb -> lbStyle == BS_NULL );
  myBrush    = CreateBrushIndirect ( plb );
  Play ( FALSE );
}  // end constructor

WNT_PolyAttribNote :: ~WNT_PolyAttribNote ( void )
{
#ifdef PRO19042
  if ( myBrush ) {
#ifdef DEB
printf(" *** Destroy:WNT_PolyAttribNote %x/%x\n",myBrush,ALLOCATOR->myPolyBrush);
#endif
    DeleteBrush( myBrush );
    myBrush = NULL;
  }
#else
  if ( myBrush && myBrush != ALLOCATOR -> myPolyBrush )
    DeleteBrush( myBrush );
#endif
}  // enf WNT_PolyAttribNote :: ~WNT_PolyAttribNote

void WNT_PolyAttribNote :: Play ( BOOL fRealize )
{
  if ( myfEdge )  ALLOCATOR -> myFlags |= W32F_POUTL;
  else            ALLOCATOR -> myFlags &= ~W32F_POUTL;
  if ( myNoFill ) ALLOCATOR -> myFlags |= W32F_NOFIL;
  else            ALLOCATOR -> myFlags &= ~W32F_NOFIL;
  ALLOCATOR -> myPolyBrush = myBrush;
  if ( fRealize ) {
    SelectPen   ( ALLOCATOR -> myHDC, ALLOCATOR -> myLinePen   );
    SelectBrush ( ALLOCATOR -> myHDC, ALLOCATOR -> myPolyBrush );
  }
}  // end WNT_PolyAttribNote :: Play
////////////////////////////////////////////////////////////////////////////////
// W N T _ T e x t A t t r i b N o t e                                        //
////////////////////////////////////////////////////////////////////////////////
WNT_TextAttribNote :: WNT_TextAttribNote (
                       HFONT hFont, COLORREF color, double slant,
                       double hScale, double vScale,
                       BOOL fUnderlined, BOOL fFree, BOOL fIndex
                      )
{
  myFont   = hFont;
  mySlant  = (slant == 0.0 ? EPS : slant);
  myHScale = vScale;
  myVScale = hScale;
  myFlags  = ( fUnderlined ? W32F_TFULINED : 0 ) |
             ( fIndex      ? W32F_TINDEX   : 0 ) |
             ( fFree       ? W32F_TFREE    : 0 );
  myColor  = color;
  myPen    = CreatePen        ( PS_SOLID, 0, color );
  myBrush  = CreateSolidBrush ( color );
  Play ( FALSE );
}  // end constructor

WNT_TextAttribNote :: ~WNT_TextAttribNote ()
{
#ifdef PRO19042
  if ( myFont && (myFlags & W32F_TFREE) ) {
#ifdef DEB
printf(" *** Destroy:WNT_TextAttribNote_FONT %x/%x\n",myFont,ALLOCATOR->myTextFont);
#endif
    DeleteFont  ( myFont  );
    myFont = NULL;
  }
  if ( myPen ) {
#ifdef DEB
printf(" *** Destroy:WNT_TextAttribNote_PEN %x/%x\n",myPen,ALLOCATOR->myTextPen);
#endif
    DeletePen   ( myPen   );
    myPen = NULL;
  }
  if ( myBrush ) {
#ifdef DEB
printf(" *** Destroy:WNT_TextAttribNote_BRUSH %x/%x\n",myBrush,ALLOCATOR->myTextBrush);
#endif
    DeleteBrush ( myBrush );
    myBrush = NULL;
  }
#else
  if ( myFlags & W32F_TFREE )                            DeleteFont  ( myFont  );
  if ( myPen   && myPen   != ALLOCATOR -> myTextPen   )  DeletePen   ( myPen   );
  if ( myBrush && myBrush != ALLOCATOR -> myTextBrush )  DeleteBrush ( myBrush );
#endif
}  // end destructor

void WNT_TextAttribNote :: Play ( BOOL fRealize )
{
  if ( ( ALLOCATOR -> myFlags & W32F_DFONT ) && !( myFlags & W32F_TINDEX ) ) {
    DeleteFont( ALLOCATOR -> myTextFont );
    ALLOCATOR -> myFlags &= ~W32F_DFONT;
  }  // end if
  ALLOCATOR -> myTextFont   = myFont;
  ALLOCATOR -> myTextSlant  = mySlant;
  ALLOCATOR -> myTextHScale = myHScale;
  ALLOCATOR -> myTextVScale = myVScale;
  ALLOCATOR -> myTextColor  = myColor;
  ALLOCATOR -> myTextPen    = myPen;
  ALLOCATOR -> myTextBrush  = myBrush;
  if (myFlags & W32F_TFULINED) ALLOCATOR -> myFlags |= W32F_TULIN;
  else                         ALLOCATOR -> myFlags &= ~W32F_TULIN;
  if ( fRealize && !( myFlags & W32F_TINDEX ) ) {
    SetTextColor ( ALLOCATOR -> myHDC, ALLOCATOR -> myTextColor );
    SelectPen    ( ALLOCATOR -> myHDC, ALLOCATOR -> myTextPen   );
    SelectBrush  ( ALLOCATOR -> myHDC, ALLOCATOR -> myTextBrush );
    SelectFont   ( ALLOCATOR -> myHDC, ALLOCATOR -> myTextFont  );
  }
}  // end WNT_TextAttribNote :: Play
////////////////////////////////////////////////////////////////////////////////
// W N T _ M a r k e r A t t r i b N o t e                                    //
////////////////////////////////////////////////////////////////////////////////
WNT_MarkerAttribNote :: WNT_MarkerAttribNote (
                         COLORREF color, DWORD dwWidth, BOOL fFill
                        )
{
  DWORD width = (dwWidth == 0) ? 1 : dwWidth;

  myFill             = fFill;
  myMarkerPointColor = color;
  myPen              = CreatePen ( PS_SOLID, width, color );
  Play ();
}  // end constructor

WNT_MarkerAttribNote :: ~WNT_MarkerAttribNote ( void )
{
#ifdef PRO19042
  if ( myPen ) {
#ifdef DEB
printf(" *** Destroy:WNT_MarkerAttribNote %x/%x\n",myPen,ALLOCATOR->myMarkerPen);
#endif
    DeletePen( myPen );
    myPen = NULL;
  }
#else
  if ( myPen && myPen != ALLOCATOR -> myMarkerPen ) DeletePen( myPen );
#endif
}  // enf WNT_MarkerAttribNote :: ~WNT_MarkerAttribNote

void WNT_MarkerAttribNote :: Play ( BOOL fRealize )
{
  if (myFill)  ALLOCATOR -> myFlags |= W32F_MFILL;
  else         ALLOCATOR -> myFlags &= ~W32F_MFILL;

  ALLOCATOR -> myMarkerPen           = myPen;
  ALLOCATOR -> myMarkerPointColor    = myMarkerPointColor;
}  // end WNT_MarkerAttribNote :: Play
