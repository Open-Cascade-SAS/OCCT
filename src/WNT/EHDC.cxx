// Copyright (c) 1998-1999 Matra Datavision
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

// Modifications: PLOTNIKOV Eugeny at July 1998 (BUC60286)

#include "EHDC.hxx"

#include <math.h>
#include <limits>

#define ROUNDL( d ) ( LONG )(  ( d ) + 0.5  )
#define Y( y )      (  mySize.cy - ( y )  )

typedef struct _fpoint {

                FLOAT x;
                FLOAT y;

               } FPOINT, *PFPOINT;

typedef struct _ddap {

                EHDC*  _this;
                PPOINT pp;

               } DDAP, *PDDAP;

static void WINAPI _XFORMultiply ( PXFORM,  PXFORM, PXFORM );
static void WINAPI _XFORMApply   ( PPOINT,  int, PXFORM    );
static void WINAPI _XFORMApplyf  ( PFPOINT, int, PXFORM    );

static void WINAPI _RenderPath ( HDC, LPPOINT, PBYTE, int );
static int  WINAPI _TextPath   ( HDC, int, int, void*, LPPOINT, PBYTE, int, PSIZE, BOOL );

EHDC :: EHDC ( HDC hdc, PSIZE szClient ) {

 _Init ();

 SetDC ( hdc, szClient );

}  // end EHDC constructor ( 1 )

EHDC :: EHDC () {

 _Init ();

}  // end EHDC constructor ( 2 )

EHDC :: ~EHDC () {

 HANDLE hHeap = GetProcessHeap ();

 if ( myHDC != NULL ) {
 
  SelectPen(    myHDC, GetStockObject ( NULL_PEN   )  );
  SelectBrush(  myHDC, GetStockObject ( NULL_BRUSH )  );
 
 }  // end if

 if ( myTextBrush != NULL ) DeleteBrush( myTextBrush );
 if ( myTextPen   != NULL ) DeletePen(   myTextPen   );

 if ( myTextPath != NULL ) HeapFree (
                            hHeap, 0,
                            ( LPVOID )myTextPath
                           );
 if (  myPStyle != NULL && ( myFlags & EHDCF_SFREE )  )
  
  HeapFree (  hHeap, 0, ( LPVOID )myPStyle  );

 if ( myPBrush != NULL ) DeleteBrush( myPBrush ); 
 if ( myPPen   != NULL ) DeletePen(     myPPen );
 
 if ( myFlags & EHDCF_PPUSH ) {
 
  if ( myPPenPush   != NULL ) DeletePen( myPPenPush );
  if ( myPBrushPush != NULL ) DeleteBrush( myPBrushPush );
 
 }  // end if

 HeapFree (  hHeap, 0, ( LPVOID )myDDA1  );
 HeapFree (  hHeap, 0, ( LPVOID )myDDA2  );

}  // end EHDC destructor

void EHDC :: SetDC ( HDC hdc, PSIZE szClient ) {

 myHDC        = hdc;
 myTextHScale =
 myTextVScale = 1.0;
 myTextSlant  = 0.0;
 mySize       = *szClient;

 SelectPen( myHDC, myPPen );
 SelectBrush( myHDC, myPBrush );

 MoveToEx   ( 0, 0, NULL );
 ResetURect ();

}  // end EHDC :: SetDC

HBRUSH EHDC :: SelectEPen (
                DWORD           dwWidth,
                CONST LOGBRUSH* lplb,
                DWORD           dwStyleCount,
                DWORD*          lpStyle,
                BOOL            fNoAlloc
               ) {

 DWORD dwLen = dwStyleCount * sizeof ( DWORD );
        
 if ( dwStyleCount ) {
 
  if ( !fNoAlloc ) {

   if ( myPStyle == NULL ) {

    myPStyle = ( PDWORD )HeapAlloc (
                          GetProcessHeap (),
                          HEAP_GENERATE_EXCEPTIONS,
                          dwLen
                         );
setPBS:
    myPBS = dwStyleCount;
   
   } else if ( myPBS < dwStyleCount ) {

    myPStyle = ( PDWORD )HeapReAlloc (
                          GetProcessHeap (),
                          HEAP_GENERATE_EXCEPTIONS,
                          myPStyle, dwLen
                         );

    goto setPBS;

   }  // end if

   CopyMemory ( myPStyle, lpStyle, dwLen );
   myFlags |= EHDCF_SFREE;

  } else if (  myPStyle != NULL && ( myFlags & EHDCF_SFREE )  ) {
  
   HeapFree (  GetProcessHeap (), 0, ( LPVOID )myPStyle  );
   myPStyle = lpStyle;

  }  // end if

 }  // end if

 myPIndex = 0;
 myPWidth = dwWidth;
 myPNS    = dwStyleCount;
 
 if ( myPBrush != NULL ) {
 
  LOGBRUSH lb;
 
  GetObject (  myPBrush, sizeof ( LOGBRUSH ), &lb  );

  if ( lb.lbStyle != lplb -> lbStyle ||
       lb.lbColor != lplb -> lbColor ||
       lb.lbHatch != lplb -> lbHatch
  ) {

   SelectBrush(  myHDC, GetStockObject ( NULL_BRUSH )  );
   DeleteBrush( myPBrush );
newBrush: 
   myPBrush  = CreateBrushIndirect ( lplb );

  }  // end if

  if ( dwWidth > 1 ) {

   myFlags  |= ( EHDCF_EPEN | EHDCF_PDRAW | EHDCF_CJOIN );
   myFlags  &= ~EHDCF_JOIN;

  } else myFlags &= ~EHDCF_EPEN;

 } else goto newBrush;

 if ( myPPen != NULL ) {
 
  LOGPEN lp;

  GetObject (  myPPen, sizeof ( LOGPEN ), &lp  );
 
  if ( lp.lopnColor   != lplb -> lbColor ||
       lp.lopnWidth.x != 0
  ) {
  
   SelectPen(  myHDC, GetStockObject ( NULL_PEN )  );
   DeletePen( myPPen );
newPen:
   myPPen = CreatePen ( PS_SOLID, 0, lplb -> lbColor );

  }  // end if

 } else goto newPen;

 SelectPen( myHDC, myPPen );
 
 return SelectBrush( myHDC, myPBrush );

}  // end EHDC :: SelectEPen

void EHDC :: SelectEPen (
              DWORD           dwWidth,
              CONST LOGBRUSH* lplb
             ) {
               
 if (  dwWidth == 0xFFFFFFFF && ( myFlags & EHDCF_PPUSH )  ) {
 
  SelectPen( myHDC, myPPenPush );
  SelectBrush( myHDC, myPBrushPush );
  DeletePen( myPPen );
  DeleteBrush( myPBrush );

  myFlags  &= ~EHDCF_PPUSH;
  myPPen    = myPPenPush;
  myPBrush  = myPBrushPush;
  myPWidth  = myPWPush;
  myFlags  |= ( myfXpenPush ? EHDCF_EPEN : 0 );
 
 } else if (  !( myFlags & EHDCF_PPUSH )  ){
 
  myFlags     |= EHDCF_PPUSH;
  myfXpenPush  = ( myFlags & EHDCF_EPEN );
  myPPenPush   = myPPen;
  myPBrushPush = myPBrush;
  myPWPush     = myPWidth;

  myPWidth = dwWidth;
  SelectBrush(  myHDC, myPBrush = CreateBrushIndirect ( lplb )                );
  SelectPen(    myHDC, myPPen   = CreatePen ( PS_SOLID, 0, lplb -> lbColor )  );

  if ( dwWidth > 1 ) {

   myFlags  |= ( EHDCF_EPEN | EHDCF_PDRAW | EHDCF_CJOIN );
   myFlags  &= ~EHDCF_JOIN;

  } else myFlags &= ~EHDCF_EPEN;

 }  // end if
               
}  // end EHDC :: SelectEPen

HPEN EHDC :: SelectEPen ( HPEN hPen ) {

 HPEN retVal = NULL;

 if ( myHDC != NULL ) retVal = SelectPen( myHDC, hPen );

 myFlags &= ~EHDCF_EPEN;
 
 return hPen;

}  // end EHDC :: SelectEPen

void EHDC :: SelectEBrush ( PLOGBRUSH lplb, PLOGBRUSH lplbo ) {

 if ( lplb == NULL ) {

  DeleteBrush( myBrush );
  myBrush = NULL;

 } else if ( myBrush != NULL ) {
 
  LOGBRUSH lb;
 
  GetObject (  myBrush, sizeof ( LOGBRUSH ), &lb  );

  if ( lplbo != NULL ) *lplbo = lb;

  if ( lb.lbStyle != lplb -> lbStyle ||
       lb.lbColor != lplb -> lbColor ||
       lb.lbHatch != lplb -> lbHatch
  ) {
  
   DeleteBrush( myBrush );
newBrush: 
   myBrush  = CreateBrushIndirect ( lplb );

  }  // end if

 } else goto newBrush; 

}  // end EHDC :: SelectEBrush

void EHDC :: SetMiter ( BOOL fMiter ) {

 if ( fMiter )

  myFlags &= ~EHDCF_JBEVL;

 else

  myFlags |= EHDCF_JBEVL;

}  // end EHDC :: SetMiter

void EHDC :: MoveToEx ( int x, int y, LPPOINT lpPoint ) {

 if ( lpPoint != NULL ) *lpPoint = myStart;

 myStart.x = x;
 myStart.y = y;

 if ( myFlags & EHDCF_XFORM ) _XFORMApply ( &myStart, 1, &myXform );

 myStart.y = Y( myStart.y );
 Register ( &myStart, 1 );

 ::MoveToEx ( myHDC, myStart.x, myStart.y, NULL );

 myClose  = myStart;
 myPIndex = 0;
 myFlags &= ~EHDCF_JOIN;
 myFlags |= ( EHDCF_PDRAW | EHDCF_DDAF );
 myPIndex = 0;
 myPRlen  = 0;

}  // end EHDC :: MoveToEx

BOOL EHDC :: LineTo ( int x, int y ) {

 POINT p = { x, y };

 if ( myFlags & EHDCF_XFORM ) _XFORMApply ( &p, 1, &myXform );

 p.y = Y( p.y );

 if ( myFlags & EHDCF_EPEN ) {

  if ( myPNS == 0 )
  
   _DrawTo ( &p );

  else if (  !_DrawToEx ( &p )  ) return FALSE;

  if ( myFlags & EHDCF_CJOIN ) {
  
   myFlags &= ~EHDCF_CJOIN;
   myCJoin  = p;
  
  }  // end if

  if ( myFlags & EHDCF_JOIN )
  
   _Join ();

  else

   myJoin = p;
   
  if (  myFlags & EHDCF_PDRAW && !( myFlags & EHDCF_NJOIN )  )
  
   myFlags |= EHDCF_JOIN;

  else

   myFlags &= ~( EHDCF_JOIN | EHDCF_NJOIN );
 
 } else if ( myPNS == 0 ) {
 
  Register ( &p, 1 );

  ::LineTo ( myHDC, p.x, p.y );

  myStart = p;

 } else _LineTo ( &p );

 return TRUE;

}  // end EHDC :: LineTo

void EHDC :: ClosePath ( void ) {

 BOOL  ltf;
 POINT pa = myStart;
 POINT pb = myClose;
 DWORD dw = ( myFlags & EHDCF_XFORM );

 myFlags &= ~EHDCF_XFORM;

 if ( myStart.x != myClose.x ||
      myStart.y != myClose.y
 ) {
 
  ltf = LineTo (  myClose.x, Y( myClose.y )  );

  myFlags |= dw;

  if (  ltf && ( myFlags & EHDCF_PDRAW ) && ( myFlags & EHDCF_EPEN )  ) {

   myA     = pa;
   myB     = pb;
   myStart = myCJoin;

   _Join ();

  }  // end if

 } else myFlags |= dw;

 myFlags  &= ~EHDCF_JOIN;
 myPIndex  = 0;
 myFlags  |= ( EHDCF_PDRAW | EHDCF_CJOIN );
 myPRlen   = 0;

}  // end EHDC :: ClosePath

void EHDC :: Polyline ( CONST POINT* lpPts, int nPts, BOOL fClose ) {

 DWORD dwPIndex = myPIndex;
 DWORD dwPRlen  = myPRlen;
 DWORD dwFlags  = myFlags;
 POINT pA       = myA;
 POINT pB       = myB;
 POINT pClose   = myClose;
 POINT pJoin    = myJoin;
 POINT pCJoin   = myCJoin;
 POINT pStart;

 myFlags |= EHDCF_CJOIN;

 MoveToEx ( lpPts -> x, lpPts -> y, &pStart );

 for ( int i = 1; i < nPts; ++i )

  LineTo ( lpPts[ i ].x, lpPts[ i ].y );

 if ( fClose ) ClosePath ();

 myFlags  = dwFlags;
 myA      = pA;
 myB      = pB;
 myClose  = pClose;
 myJoin   = pJoin;
 myCJoin  = pCJoin;
 myStart  = pStart;
 myPRlen  = dwPRlen;
 myPIndex = dwPIndex;

}  // end EHDC :: Polyline

void EHDC :: Polygon ( CONST POINT* lpPts, int nPts, DWORD dwFlags ) {

 POINT  p;
 HPEN   hpo = SelectPen(  myHDC, GetStockObject ( NULL_PEN )  );
 HBRUSH hbo = SelectBrush(  myHDC, myBrush ? myBrush : GetStockObject ( NULL_BRUSH )  );

 BeginPath ( myHDC );
  
  p.x = lpPts -> x;
  p.y = lpPts -> y;

  if ( myFlags & EHDCF_XFORM ) _XFORMApply ( &p, 1, &myXform );

  ::MoveToEx (  myHDC, p.x, Y( p.y ), NULL  );

  for ( int i = 1; i < nPts; ++i ) {
  
   p.x = lpPts[ i ].x;
   p.y = lpPts[ i ].y;

   if ( myFlags & EHDCF_XFORM ) _XFORMApply ( &p, 1, &myXform );
   
   ::LineTo (  myHDC, p.x, Y( p.y )  );
  
  }  // end for

  CloseFigure ( myHDC );

 EndPath ( myHDC );

 FillPath ( myHDC );

 SelectBrush( myHDC, hbo );
 SelectPen( myHDC, hpo );

 if ( dwFlags & POLYF_NOCLOSEDGE )
 
  Polyline ( lpPts, nPts, FALSE );

 else if (  !( dwFlags & POLYF_NOEDGE )  )

  Polyline ( lpPts, nPts, TRUE );

}  // end EHDC :: Polygon

void EHDC :: Arc (
              int x, int y, int xr, int yr, double sa, double oa, DWORD dwAFlags
             ) {

 int    i = 0, j, n;
 double delta;
 FPOINT fp[ 362 ];
 BOOL   fXform = ( myFlags & EHDCF_XFORM );
 DWORD  dwPIndex = myPIndex;
 DWORD  dwPRlen  = myPRlen;
 DWORD  dwFlags  = myFlags;
 POINT  pA       = myA;
 POINT  pB       = myB;
 POINT  pClose   = myClose;
 POINT  pJoin    = myJoin;
 POINT  pCJoin   = myCJoin;
 POINT  pStart   = myStart;

 myFlags &= ~EHDCF_XFORM;

 if ( oa > 6.28318 )

  oa = 6.28318;

 else if ( oa < -6.28318 )

  oa = -6.28318;

 n = abs (  ROUNDL( oa * 180.0 / 3.14159 )  );
 delta = oa / n;

 fp[ i   ].x = ( FLOAT )x;
 fp[ i++ ].y = ( FLOAT )y;
 n += 2;

 for ( ; i < n; ++i, sa += delta ) {
 
  fp[ i ].x = FLOAT(  x + xr * cos ( sa )  );
  fp[ i ].y = FLOAT(  y + yr * sin ( sa )  );

 }  // end for

 if ( fXform ) _XFORMApplyf ( fp, n, &myXform );

 i = 0;

 if ( dwAFlags & ARCF_PIE ) {
 
  myFlags &= ~EHDCF_JBEVL;
  MoveToEx (  ROUNDL( fp[ 0 ].x ), ROUNDL( fp[ 0 ].y ), NULL  );
  LineTo   (  ROUNDL( fp[ 1 ].x ), ROUNDL( fp[ 1 ].y )  );
  
  ++i;

  if ( i >= n ) goto end;

  LineTo (  ROUNDL( fp[ i ].x ), ROUNDL( fp[ i ].y )  );

  ++i;

 } else {
 
  MoveToEx (  ROUNDL( fp[ 1 ].x ), ROUNDL( fp[ 1 ].y ), NULL  );
  ++i;

 }  // end else

 myFlags |= EHDCF_JBEVL;
 
 for ( j = i - 1; i < n; ++i ) {
 
  x = ROUNDL( fp[ i ].x );
  y = ROUNDL( fp[ i ].y );

  if (  x == ROUNDL( fp[ j ].x ) && y == ROUNDL( fp[ j ].y )  ) continue;

  LineTo ( x, y );
  j = i;

 }  // end for
end:
 if (  dwAFlags & ( ARCF_PIE | ARCF_CHORD ) || fabs ( oa ) == 6.28318  ) {
 
  myFlags &= ~EHDCF_JBEVL;
  ClosePath (); 
 
 }  // end if

 myFlags  = dwFlags;
 myA      = pA;
 myB      = pB;
 myClose  = pClose;
 myJoin   = pJoin;
 myCJoin  = pCJoin;
 myStart  = pStart;
 myPRlen  = dwPRlen;
 myPIndex = dwPIndex;

}  // end EHDC :: Arc

void EHDC :: Polyarc (
              int x, int y, int xr, int yr, double sa, double oa, BOOL fChord
             ) {

 int       i = 0, n;
 double    ssa = sa, delta;
 FPOINT    fp[ 362 ];
 BOOL      fXform = ( myFlags & EHDCF_XFORM );

 if ( oa > 6.28318 )

  oa = 6.28318;

 else if ( oa < -6.28318 )

  oa = -6.28318;

 n = abs (  ROUNDL( oa * 180.0 / 3.14159 )  );
 delta = oa / n;

 fp[ i   ].x = ( FLOAT )x;
 fp[ i++ ].y = ( FLOAT )y;
 n += 2;

 for ( ; i < n; ++i, sa += delta ) {
 
  fp[ i ].x = FLOAT(  x + xr * cos ( sa )  );
  fp[ i ].y = FLOAT(  y + yr * sin ( sa )  );

 }  // end for

 if ( fXform ) _XFORMApplyf ( fp, n, &myXform );

 i = 0;

 HPEN hpo = SelectPen(  myHDC, GetStockObject ( NULL_PEN )  );
 HBRUSH hbo = SelectBrush(  myHDC, myBrush ? myBrush : GetStockObject ( NULL_BRUSH )  );

  BeginPath ( myHDC );

   if ( !fChord ) {
 
    ::MoveToEx (
       myHDC,
       ROUNDL( fp[ 0 ].x ), Y(  ROUNDL( fp[ 0 ].y )  ), NULL
      );
    ::LineTo (
       myHDC,
       ROUNDL( fp[ 1 ].x ), Y(  ROUNDL( fp[ 1 ].y )  )
      );
  
    ++i;

    if ( i >= n ) goto end;

    ::LineTo (
       myHDC,
       ROUNDL( fp[ i ].x ), Y(  ROUNDL( fp[ i ].y )  )
      );

   } else {
 
    ::MoveToEx (
       myHDC,
       ROUNDL( fp[ 1 ].x ), Y(  ROUNDL( fp[ 1 ].y )  ), NULL
      );
    ++i;

   }  // end else

   for ( ; i < n; ++i ) ::LineTo (
                           myHDC,
                           ROUNDL( fp[ i ].x ), Y(  ROUNDL( fp[ i ].y )  )
                          );
end:
   CloseFigure ( myHDC ); 

  EndPath ( myHDC );
 
  FillPath ( myHDC );

 SelectBrush( myHDC, hbo );
 SelectPen( myHDC, hpo );

 Arc ( x, y, xr, yr, ssa, oa, fChord ? ARCF_CHORD : ARCF_PIE );

}  // end EHDC :: Polyarc

void EHDC :: SetPixel ( int x, int y, COLORREF c ) {

 POINT p = { x, y };

 if ( myFlags & EHDCF_XFORM ) _XFORMApply ( &p, 1, &myXform );

 p.y = Y( p.y );

 ::SetPixel ( myHDC, p.x, p.y, c );

}  // end EHDC :: SetPixel

void EHDC :: SetTextColor ( COLORREF c ) {

 LOGPEN lp;

 GetObject (  myTextPen, sizeof ( LOGPEN ), &lp  );

 if ( lp.lopnColor != c ) {

  LOGBRUSH lb = { BS_SOLID, c, 0 };
 
  if ( myTextBrush != NULL ) DeleteBrush( myTextBrush );
  if ( myTextPen   != NULL ) DeletePen( myTextPen     );

  myTextPen   = CreatePen ( PS_SOLID, 0, c );
  myTextBrush = CreateBrushIndirect ( &lb );
 
 }  // end if

}  // end EHDC :: SetTextColor

void EHDC :: SetTextAttrib ( COLORREF c, HFONT f, double slant, double hs, double vs ) {

 SetTextColor ( c );

 myTextFont   = f;
 myTextSlant  = slant;
 myTextHScale = hs;
 myTextVScale = vs;

}  // end EHDC :: SetTextAttrib

void EHDC :: ETextOut ( int x, int y, char* str, double angle, BOOL fOutline ) {

 _ETextOut ( x, y, str, angle, fOutline );

}  // end EHDC :: TextOut

void EHDC :: ETextOut ( int x, int y, wchar_t* str, double angle, BOOL fOutline ) {

 _ETextOut ( x, y, str, angle, fOutline, TRUE );

}  // end EHDC :: TextOut

void EHDC :: PolyTextOut (
      int x, int y, char* str, double angle,
      double margin, BOOL fOutline, BOOL fNofill, BOOL fNoframe
     ) {

 _PolyTextOut ( x, y, str, angle, margin, fOutline, FALSE, fNofill, fNoframe );

}  // end EHDC :: PolyTextOut

void EHDC :: PolyTextOut (
      int x, int y, wchar_t* str, double angle, double margin,
      BOOL fOutline, BOOL fNofill, BOOL fNoframe
     ) {

 _PolyTextOut ( x, y, str, angle, margin, fOutline, TRUE, fNofill, fNoframe );

}  // end EHDC :: PolyTextOut

void EHDC :: SetWorldTransform ( XFORM* xf ) {

 if ( xf == NULL )

  myFlags &= ~EHDCF_XFORM;

 else {

  myXform = *xf;
  myFlags |= EHDCF_XFORM;

 }  // end else

}  // end EHDC :: SetWorldTransform

void EHDC :: ModifyWorldTransform ( XFORM* xf, DWORD iMode ) {

 XFORM  rxf;
 PXFORM pxfLeft;
 PXFORM pxfRight;

 if ( iMode == MWT_RIGHTMULTIPLY ) {
 
  pxfLeft  = &myXform;
  pxfRight = xf;
 
 } else if ( iMode == MWT_LEFTMULTIPLY ) {
 
  pxfLeft  = xf;
  pxfRight = &myXform;
  
 } else if ( iMode == MWT_IDENTITY ) {
 
  myXform.eM11 = 1.0F; myXform.eM12 = 0.0F;
  myXform.eM21 = 0.0F; myXform.eM22 = 1.0F;
  myXform.eDx  = 0.0F; myXform.eDy  = 0.0F;

  myFlags &= ~EHDCF_XFORM;
 
  return;
 
 } else return;

 _XFORMultiply ( &rxf, pxfLeft, pxfRight );

 myXform  = rxf;
 myFlags |= EHDCF_XFORM;

}  // end EHDC :: ModifyWorldTransform

void EHDC :: Transform ( LPPOINT p, int n ) {

 _XFORMApply ( p, n, &myXform );

}  // end TransformXY

void EHDC :: ResetURect ( void ) {

 myURect.left   = LONG_MAX;
 myURect.top    = LONG_MAX;
 myURect.right  = LONG_MIN;
 myURect.bottom = LONG_MIN;

}  // end EHDC :: ResetURect

void EHDC :: GetURect ( LPRECT lpRect ) {

 *lpRect = myURect;

 if ( lpRect -> top    < 0         ) lpRect -> top    = 0;
 if ( lpRect -> left   < 0         ) lpRect -> left   = 0;
 if ( lpRect -> bottom > mySize.cy ) lpRect -> bottom = mySize.cy;
 if ( lpRect -> right  > mySize.cx ) lpRect -> right  = mySize.cx;

}  // end EHDC :: GetURect

void EHDC :: _Init ( void ) {

 myTextPen    = ( HPEN   )GetStockObject ( WHITE_PEN   );
 myTextBrush  = ( HBRUSH )GetStockObject ( WHITE_BRUSH );
 myTextFont   = NULL;
 myTextPath   = NULL;
 myPPen       = myTextPen;
 myPBrush     = 
 myBrush      = myTextBrush;
 myPStyle     = NULL;
 myTextBS     =
 myPBS        =
 myPNS        = 0;
 myFlags      = 0;
 myTextSlant  = 0.0;
 myTextHScale =
 myTextVScale = 1.0;
 myNDDA       = 1024;
 myDDA1       = ( PPOINT )HeapAlloc (
                           GetProcessHeap (), HEAP_GENERATE_EXCEPTIONS,
                           sizeof ( POINT ) * myNDDA
                          );
 myDDA2       = ( PPOINT )HeapAlloc (
                           GetProcessHeap (), HEAP_GENERATE_EXCEPTIONS,
                           sizeof ( POINT ) * myNDDA
                          );
 myIDDA       = 0;

 ModifyWorldTransform ( NULL, MWT_IDENTITY );
 myFlags &= ~EHDCF_XFORM;

}  // end EHDC :: _Init

void EHDC :: _DrawTo ( PPOINT p ) {

 POINT  ip[ 4 ];
 double dx, dy, dxhw, dyhw, factor;

 dx = ( double )(  p -> x - myStart.x  );
 dy = ( double )(  p -> y - myStart.y  );

 if ( dx == 0.0 && dy == 0.0 ) return;

 factor = myPWidth / 2.0 / sqrt ( dx * dx + dy * dy );
 dxhw   = dy * factor;
 dyhw   = dx * factor;

 ip[ 0 ].x = ROUNDL( myStart.x + dxhw );
 ip[ 0 ].y = ROUNDL( myStart.y - dyhw );

 ip[ 1 ].x = ROUNDL( p -> x + dxhw );
 ip[ 1 ].y = ROUNDL( p -> y - dyhw );

 ip[ 2 ].x = ROUNDL( p -> x - dxhw );
 ip[ 2 ].y = ROUNDL( p -> y + dyhw );

 ip[ 3 ].x = ROUNDL( myStart.x - dxhw );
 ip[ 3 ].y = ROUNDL( myStart.y + dyhw );

 Register ( ip, 4 );

 ::Polygon ( myHDC, ip, 4 );

 myA     = myB;
 myB     = myStart;
 myStart = *p;

}  // end EHDC :: _DrawTo

void EHDC :: _Join ( void ) {

 int   np;
 POINT p[ 4 ];
 double r, ba, bc, dxba, dyba, dxbc, dybc,
        s, sa, fc,   xd,   yd,   xe,   ye,
               xf,   yf,   m1,   m2,   n1,
               n2,  rm1,  rm2,  rn1,  rn2;
              
 r    = myPWidth / 2.0;
 dxba = ( double )(  myA.x     - myB.x  ); dyba = ( FLOAT )(  myA.y     - myB.y  );
 dxbc = ( double )(  myStart.x - myB.x  ); dybc = ( FLOAT )(  myStart.y - myB.y  );
 ba   = sqrt ( dxba * dxba + dyba * dyba );
 bc   = sqrt ( dxbc * dxbc + dybc * dybc );

 if ( ba == 0.0 || bc == 0.0 ) return;

 n1 = -dxba / ba; n2 = dyba / ba;
 m1 = -dxbc / bc; m2 = dybc / bc;
 s  = m1 * n2 - n1 * m2;

 if (   (  sa = fabs ( s )  ) < 0.001   ) return;

 fc   = r / sa;
 xd   = myB.x + fc * ( m1 + n1 );
 yd   = myB.y - fc * ( m2 + n2 );
 rm1  = r * m1; rm2 = r * m2;
 rn1  = r * n1; rn2 = r * n2;

 if ( s > 0.0 ) {
 
  xe = myB.x - rm2; ye = myB.y - rm1;
  xf = myB.x + rn2; yf = myB.y + rn1;
 
 } else if ( s < 0.0 ) {
 
  xe = myB.x + rm2; ye = myB.y + rm1;
  xf = myB.x - rn2; yf = myB.y - rn1;
 
 } else return;

 p[ 0 ] = myB;
 p[ 1 ].x = ROUNDL( xf ); p[ 1 ].y = ROUNDL( yf );
 
 if (  !( myFlags & EHDCF_JBEVL )  ) {
 
  p[ 2 ].x = ROUNDL( xd ); p[ 2 ].y = ROUNDL( yd );
  p[ 3 ].x = ROUNDL( xe ); p[ 3 ].y = ROUNDL( ye );
  np = 4;

 } else {
 
  p[ 2 ].x = ROUNDL( xe ); p[ 2 ].y = ROUNDL( ye );
  np = 3;

 }  // end else

 Register ( p, np );
#if 0
  ::SetPixel (  myHDC, p[ 0 ].x, p[ 0 ].y, RGB( 255, 255, 255 )  );
  ::SetPixel (  myHDC, p[ 1 ].x, p[ 1 ].y, RGB( 255, 255, 255 )  );
  ::SetPixel (  myHDC, p[ 2 ].x, p[ 2 ].y, RGB( 255, 255, 255 )  );
  ::SetPixel (  myHDC, p[ 3 ].x, p[ 3 ].y, RGB( 255, 255, 255 )  );
#endif
 HPEN hpo = SelectPen(  myHDC, GetStockObject ( NULL_PEN )  );
  ::Polygon ( myHDC, p, np );
 SelectPen( myHDC, hpo );

}  // end EHDC :: _Join

BOOL EHDC :: _DrawToEx ( PPOINT p ) {

 DWORD  i = 0, j, k;
 DDAP   ddap = { this };
 POINT  pp[ 4 ];
 double dx, dy, dxhw, dyhw, factor;

 dx = ( double )( p -> x - myStart.x );
 dy = ( double )( p -> y - myStart.y );

 if ( dx == 0.0 && dy == 0.0 ) return FALSE;

 factor = myPWidth / 2.0 / sqrt ( dx * dx + dy * dy );
 dxhw   = dy * factor;
 dyhw   = dx * factor;

 pp[ 0 ].x = ROUNDL( myStart.x + dxhw );
 pp[ 0 ].y = ROUNDL( myStart.y - dyhw );

 pp[ 1 ].x = ROUNDL( myStart.x - dxhw );
 pp[ 1 ].y = ROUNDL( myStart.y + dyhw );

 pp[ 2 ].x = ROUNDL( p -> x - dxhw );
 pp[ 2 ].y = ROUNDL( p -> y + dyhw );

 pp[ 3 ].x = ROUNDL( p -> x + dxhw );
 pp[ 3 ].y = ROUNDL( p -> y - dyhw );

 myIDDA = 0;
 myFlags &= ~EHDCF_DDAF;
 ddap.pp = myDDA1;
 LineDDA (  pp[ 0 ].x, pp[ 0 ].y, pp[ 3 ].x, pp[ 3 ].y, &___auxDDAF, ( LPARAM )&ddap  );
 k = myIDDA;
 myIDDA = 0;
 myFlags &= ~EHDCF_DDAF;
 ddap.pp = myDDA2;
 LineDDA (  pp[ 1 ].x, pp[ 1 ].y, pp[ 2 ].x, pp[ 2 ].y, &___auxDDAF, ( LPARAM )&ddap  );
 myIDDA = min( k, myIDDA );
 HPEN hpo = SelectPen(  myHDC, GetStockObject ( NULL_PEN )  );

 while ( TRUE ) {
 
  if ( myPRlen == 0 ) {
 
   myPRlen = myPStyle[ myPIndex++ ];

   if ( myPIndex == myPNS ) myPIndex = 0;
 
  }  // end if
 
  j = i;

  while ( myPRlen != 0 && j < myIDDA ) ++j, --myPRlen;

  if ( j == myIDDA ) {
  
   myDDA1[ j ] = pp[ 3 ];
   myDDA2[ j ] = pp[ 2 ];

  }  // end if

  if ( myFlags & EHDCF_PDRAW ) {

   POINT p[ 4 ];

   p[ 0 ] = myDDA1[ i ];
   p[ 1 ] = myDDA1[ j ];
   p[ 2 ] = myDDA2[ j ];
   p[ 3 ] = myDDA2[ i ];

   Register ( p, 4 );
   ::Polygon ( myHDC, p, 4 );
  
  }  // end if
 
  if ( myPRlen == 0 )
  
   myFlags ^= EHDCF_PDRAW;

  else break;

  i = j;

  if ( j >= myIDDA ) break;

 }  // end while

 SelectPen( myHDC, hpo );

 if ( myPRlen == 0 ) myFlags |= EHDCF_NJOIN;

 myA = myB;
 myB = myStart;
 myStart = *p;

 return TRUE;

}  // end EHDC :: _DrawToEx

void EHDC :: _LineTo ( PPOINT p ) {

 DWORD  i = 0;
 DDAP   ddap = { this };

 Register ( p, 1 );

 myIDDA = 0;
 ddap.pp = myDDA1;
 LineDDA (  myStart.x, myStart.y, p -> x, p -> y, &___auxDDAF, ( LPARAM )&ddap  );

 while ( TRUE ) {

  if ( myPRlen == 0 ) {
 
   myPRlen = myPStyle[ myPIndex++ ];

   if ( myPIndex == myPNS ) myPIndex = 0;
 
  }  // end if

  if ( myFlags & EHDCF_PDRAW ) ::MoveToEx ( myHDC, myDDA1[ i ].x, myDDA1[ i ].y, NULL );

  while ( myPRlen != 0 && i < myIDDA ) ++i, --myPRlen;

  if ( i == myIDDA ) myDDA1[ i ] = *p;

  if ( myFlags & EHDCF_PDRAW )
 
   ::LineTo ( myHDC, myDDA1[ i ].x, myDDA1[ i ].y );

  if ( myPRlen == 0 )
  
   myFlags ^= EHDCF_PDRAW;

  else break;

 }  // end while

 myStart = *p;

}  // end EHDC :: _LineTo

void EHDC :: _ETextOut ( int x, int y, void* str, double angle, BOOL fOutline, BOOL fWide ) {
#if 1
 int     i;
 DWORD   dwLen;
 HPEN    hpo;
 HBRUSH  hbo;
 HFONT   hfo;
 XFORM   lXfm, rXfm, xfm;
 PXFORM  pXfm;
 SIZE    sz;
 double  tanVal;
 double  sinVal;
 double  cosVal;
 LPPOINT lp;

 angle  = -angle;
 tanVal = tan ( myTextSlant ) * myTextVScale;
 sinVal = sin ( angle       );
 cosVal = cos ( angle       );

 if (  !( myFlags & EHDCF_TPATH )  ) {

  hfo = SelectFont( myHDC, myTextFont );

   i = myTextNP = _TextPath (
                   myHDC, 0, 0, str, NULL, NULL, 0, &sz, fWide
                  );

  SelectFont( myHDC, hfo );

  if ( i <= 0 ) return;

  if ( myTextBS < myTextNP ) {

   dwLen = sizeof ( POINT ) * myTextNP + sizeof ( BYTE ) * myTextNP;
 
   if ( myTextPath == NULL )

    myTextPath = ( LPPOINT )HeapAlloc (
                             GetProcessHeap (),
                             HEAP_GENERATE_EXCEPTIONS, dwLen
                            );

   else

    myTextPath = ( LPPOINT )HeapReAlloc (
                             GetProcessHeap (),
                             HEAP_GENERATE_EXCEPTIONS,
                             myTextPath, dwLen
                            );

   myTextBS = myTextNP;

  }  // end if

  myTextType = ( PBYTE )( myTextPath + myTextNP );

  _TextPath ( myHDC, 0, 0, str, myTextPath, myTextType, myTextNP, NULL, fWide );

  lp = myTextPath;

  while ( i-- ) {
   
   lp -> y -= sz.cy;
   ++lp;

  }  // end while

 }  // end if

 lXfm.eM11 = ( FLOAT )myTextHScale; lXfm.eM21 = ( FLOAT )-tanVal;
 lXfm.eM12 = 0.0F;                  lXfm.eM22 = ( FLOAT )myTextVScale;
 lXfm.eDx  = 0.0F;
 lXfm.eDy  = 0.0F;

 rXfm.eM11 = ( FLOAT ) cosVal; rXfm.eM12 = ( FLOAT )sinVal;
 rXfm.eM21 = ( FLOAT )-sinVal; rXfm.eM22 = ( FLOAT )cosVal;
 rXfm.eDx = 0.0F;
 rXfm.eDy = 0.0F;
 _XFORMultiply ( &xfm, &lXfm, &rXfm );

 rXfm.eM11 = 1.0F; rXfm.eM21 =  0.0F;
 rXfm.eM12 = 0.0F; rXfm.eM22 = -1.0F;
 rXfm.eDx  = ( FLOAT )x;
 rXfm.eDy  = ( FLOAT )y;
 _XFORMultiply ( &lXfm,  &xfm, &rXfm   );

 if ( myFlags & EHDCF_XFORM ) {
 
  _XFORMultiply ( &xfm, &lXfm, &myXform );
  pXfm = &xfm;

 } else pXfm = &lXfm;

 _XFORMApply ( myTextPath, myTextNP, pXfm );

 i  = myTextNP;
 lp = myTextPath;

 while ( i-- ) lp -> y = Y( lp -> y ), ++lp;
  
 hpo = SelectPen( myHDC, myTextPen );

 if ( !fOutline ) hbo = SelectBrush( myHDC, myTextBrush );

  if (  !( myFlags & EHDCF_TREG )  ) Register ( myTextPath, myTextNP );

  BeginPath ( myHDC );
   _RenderPath ( myHDC, myTextPath, myTextType, myTextNP );
  EndPath ( myHDC );

  if ( !fOutline ) {
 
   FillPath ( myHDC );
   SelectBrush( myHDC, hbo );
 
  } else StrokePath ( myHDC );

 SelectPen( myHDC, hpo );
#else
 int     i;
 DWORD   dwLen;
 HPEN    hpo;
 HBRUSH  hbo;
 HFONT   hfo;
 XFORM   lXfm, rXfm, xfm;
 PXFORM  pXfm;
 SIZE    sz;
 double  tanVal;
 double  sinVal;
 double  cosVal;
 LPPOINT lp;
 BOOL    fNew = FALSE;

 angle  = -angle;
 tanVal = tan ( myTextSlant );
 sinVal = sin ( angle       );
 cosVal = cos ( angle       );

 if (  !( myFlags & EHDCF_TPATH )  ) {

  hfo = SelectFont( myHDC, myTextFont );

   if ( myTextVScale != 1.0 || myTextHScale != 1.0 ) {
   
    LOGFONT    lf;
    TEXTMETRIC tm;

    GetObject (  myTextFont, sizeof ( LOGFONT ), &lf  );
    GetTextMetrics ( myHDC, &tm );

    lf.lfHeight = ROUNDL( lf.lfHeight * myTextVScale );
    lf.lfWidth  = ROUNDL( lf.lfWidth  * myTextHScale );

    lf.lfQuality = DRAFT_QUALITY;

    if ( lf.lfWidth == 0 )

     lf.lfWidth = ROUNDL( tm.tmAveCharWidth * myTextHScale );

    SelectFont( myHDC, hfo );

    hfo  = SelectFont(  myHDC, CreateFontIndirect ( &lf )  );
    fNew = TRUE;
   
   }  // end if

   i = myTextNP = _TextPath (
                   myHDC, 0, 0, str, NULL, NULL, 0, &sz, fWide
                  );

  hfo = SelectFont( myHDC, hfo );

  if ( fNew ) DeleteFont( hfo );

  if ( i <= 0 ) return;

  if ( myTextBS < myTextNP ) {

   dwLen = sizeof ( POINT ) * myTextNP + sizeof ( BYTE ) * myTextNP;
 
   if ( myTextPath == NULL )

    myTextPath = ( LPPOINT )HeapAlloc (
                             GetProcessHeap (),
                             HEAP_GENERATE_EXCEPTIONS, dwLen
                            );

   else

    myTextPath = ( LPPOINT )HeapReAlloc (
                             GetProcessHeap (),
                             HEAP_GENERATE_EXCEPTIONS,
                             myTextPath, dwLen
                            );

   myTextBS = myTextNP;

  }  // end if

  myTextType = ( PBYTE )( myTextPath + myTextNP );

  _TextPath ( myHDC, 0, 0, str, myTextPath, myTextType, myTextNP, NULL, fWide );

  lp = myTextPath;

  while ( i-- ) {
   
   lp -> y -= sz.cy;
   ++lp;

  }  // end while

 }  // end if

 lXfm.eM11 = 1.0F; lXfm.eM21 = ( FLOAT )-tanVal;
 lXfm.eM12 = 0.0F; lXfm.eM22 = 1.0F;
 lXfm.eDx  = 0.0F;
 lXfm.eDy  = 0.0F;

 rXfm.eM11 = ( FLOAT ) cosVal; rXfm.eM12 = ( FLOAT )sinVal;
 rXfm.eM21 = ( FLOAT )-sinVal; rXfm.eM22 = ( FLOAT )cosVal;
 rXfm.eDx = 0.0F;
 rXfm.eDy = 0.0F;
 _XFORMultiply ( &xfm, &lXfm, &rXfm );

 rXfm.eM11 = 1.0F; rXfm.eM21 =  0.0F;
 rXfm.eM12 = 0.0F; rXfm.eM22 = -1.0F;
 rXfm.eDx  = ( FLOAT )x;
 rXfm.eDy  = ( FLOAT )y;
 _XFORMultiply ( &lXfm,  &xfm, &rXfm   );

 if ( myFlags & EHDCF_XFORM ) {
 
  _XFORMultiply ( &xfm, &lXfm, &myXform );
  pXfm = &xfm;

 } else pXfm = &lXfm;

 _XFORMApply ( myTextPath, myTextNP, pXfm );

 i  = myTextNP;
 lp = myTextPath;

 while ( i-- ) lp -> y = Y( lp -> y ), ++lp;
  
 hpo = SelectPen( myHDC, myTextPen );

 if ( !fOutline ) hbo = SelectBrush( myHDC, myTextBrush );

  if (  !( myFlags & EHDCF_TREG )  ) Register ( myTextPath, myTextNP );

  BeginPath ( myHDC );
   _RenderPath ( myHDC, myTextPath, myTextType, myTextNP );
  EndPath ( myHDC );

  if ( !fOutline ) {
 
   FillPath ( myHDC );
   SelectBrush( myHDC, hbo );
 
  } else StrokePath ( myHDC );

 SelectPen( myHDC, hpo );
#endif
}  // end EHDC :: _ETextOut

void EHDC :: _PolyTextOut (
      int x, int y, void* str, double angle, double margin,
      BOOL fOutline, BOOL fWide, BOOL fNofill, BOOL fNoframe
     ) {

 POINT      p[ 4 ];     
 XFORM      lXfm, xfm;
 PXFORM     pXfm;
 SIZE       sz;
 HFONT      hfo;
 DWORD      dwFlags;
 TEXTMETRIC tm;
 double     sinVal = sin ( angle );
 double     cosVal = cos ( angle );
 double     tanVal = -tan ( myTextSlant );
 double     height;

 hfo = SelectFont( myHDC, myTextFont );

  if ( !fWide )
 
   GetTextExtentPoint32A (
    myHDC, ( char* )str, lstrlenA (  ( char* )str  ), &sz
   );
 
  else
 
   GetTextExtentPoint32W (
    myHDC, ( wchar_t* )str, lstrlenW (  ( wchar_t* )str  ), &sz
   );
 
 GetTextMetrics ( myHDC, &tm );
 SelectFont( myHDC, hfo );

 sz.cy -= tm.tmInternalLeading;

 p[ 0 ].x = 0;
 p[ 0 ].y = ROUNDL( sz.cy * myTextVScale );
 p[ 1 ].x = ROUNDL( sz.cx * myTextHScale );
 p[ 1 ].y = p[ 0 ].y;
 p[ 2 ].x = p[ 1 ].x;
 p[ 2 ].y = 0;
 p[ 3 ].x = 0;
 p[ 3 ].y = 0;

 tanVal *= p[ 1 ].y;

 if ( myTextSlant < 0.0F ) {
 
  p[ 0 ].x -= ( LONG )tanVal;
  p[ 3 ].x -= ( LONG )tanVal;
  
 } else {
 
  p[ 1 ].x -= ( LONG )tanVal;
  p[ 2 ].x -= ( LONG )tanVal;
 
 }  /* end else */

 height = ROUNDL( p[ 0 ].y * margin / 2.0 );

 p[ 0 ].x = ROUNDL( p[ 0 ].x - height );
 p[ 0 ].y = ROUNDL( p[ 0 ].y + height );
 p[ 1 ].x = ROUNDL( p[ 1 ].x + height );
 p[ 1 ].y = ROUNDL( p[ 1 ].y + height );
 p[ 2 ].x = ROUNDL( p[ 2 ].x + height );
 p[ 2 ].y = ROUNDL( p[ 2 ].y - height );
 p[ 3 ].x = ROUNDL( p[ 3 ].x - height );
 p[ 3 ].y = ROUNDL( p[ 3 ].y - height );

 lXfm.eM11 = ( FLOAT ) cosVal; lXfm.eM12 = ( FLOAT )sinVal;
 lXfm.eM21 = ( FLOAT )-sinVal; lXfm.eM22 = ( FLOAT )cosVal;
 lXfm.eDx  = ( FLOAT )x;
 lXfm.eDy  = ( FLOAT )y;

 if ( myFlags & EHDCF_XFORM ) {
 
  _XFORMultiply ( &xfm, &lXfm, &myXform );
  pXfm = &xfm;

 } else pXfm = &lXfm;

 _XFORMApply ( p, 4, pXfm );
 
 dwFlags = ( myFlags & EHDCF_XFORM );
 myFlags &= ~EHDCF_XFORM;

 if ( fNofill )
 
  Polyline ( p, 4, TRUE );

 else

  Polygon ( p, 4, fNoframe ? POLYF_NOEDGE : 0 );

 myFlags |= dwFlags;

 _ETextOut ( x, y, str, angle, fOutline, fWide );
     
}  // end EHDC :: _PolyTextOut

void EHDC :: Register ( LPPOINT lpPts, int nPts ) {

 while ( nPts-- ) {

  if ( lpPts -> x < myURect.left )

   myURect.left = lpPts -> x - 1;

  else if ( lpPts -> x > myURect.right )

   myURect.right = lpPts -> x + 2;

  if ( lpPts -> y < myURect.top )

   myURect.top = lpPts -> y - 1;

  else if ( lpPts -> y > myURect.bottom )

   myURect.bottom = lpPts -> y + 2;
  
  ++lpPts;

 }  // end while

}  // end EHDC :: _Register
//+++//
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
//+++//
static void WINAPI _XFORMultiply ( PXFORM res, PXFORM left, PXFORM right ) {

 res -> eM11 = left -> eM11 * right -> eM11 + left -> eM12 * right -> eM21;
 res -> eM12 = left -> eM11 * right -> eM12 + left -> eM12 * right -> eM22;
 res -> eDx  = left -> eDx  * right -> eM11 + left -> eDy  * right -> eM21 + right -> eDx;

 res -> eM21 = left -> eM21 * right -> eM11 + left -> eM22 * right -> eM21;
 res -> eM22 = left -> eM21 * right -> eM12 + left -> eM22 * right -> eM22;
 res -> eDy  = left -> eDx  * right -> eM12 + left -> eDy  * right -> eM22 + right -> eDy;

}  /* end _XFORMultiply */

static void WINAPI _XFORMApply ( PPOINT pp, int n, PXFORM xf ) {

 int    i;
 double dx, dy;

 for ( i = 0; i < n; ++i ) {

  dx = pp[ i ].x * xf -> eM11 + pp[ i ].y * xf -> eM21 + xf -> eDx;
  dy = pp[ i ].x * xf -> eM12 + pp[ i ].y * xf -> eM22 + xf -> eDy;

  pp[ i ].x = ROUNDL( dx );
  pp[ i ].y = ROUNDL( dy );

 }  /* end for */

}  /* end _XFORMApply */

static void WINAPI _XFORMApplyf ( PFPOINT pp, int n, PXFORM xf ) {

 int    i;
 double dx, dy;

 for ( i = 0; i < n; ++i ) {

  dx = pp[ i ].x * xf -> eM11 + pp[ i ].y * xf -> eM21 + xf -> eDx;
  dy = pp[ i ].x * xf -> eM12 + pp[ i ].y * xf -> eM22 + xf -> eDy;

  pp[ i ].x = ( FLOAT )dx;
  pp[ i ].y = ( FLOAT )dy;

 }  /* end for */

}  /* end _XFORMApply */

static void WINAPI _RenderPath ( HDC hdc, LPPOINT lpPts, PBYTE lpType, int nPts ) {

 LPPOINT pp;

 for ( int i = 0; i < nPts; ++i ) {

  switch ( lpType[ i ] ) {

   case PT_MOVETO:

    MoveToEx ( hdc, lpPts[ i ].x, lpPts[ i ].y, NULL );
    pp = &lpPts[ i ];
         
   continue;
      
   case PT_LINETO:
   case PT_LINETO | PT_CLOSEFIGURE:

    LineTo ( hdc, lpPts[ i ].x, lpPts[ i ].y );
    goto testClose;
         
   case PT_BEZIERTO:
   case PT_BEZIERTO | PT_CLOSEFIGURE:

    PolyBezierTo ( hdc, &lpPts[ i ], 3 );
		 
    i += 2;
testClose:
    if ( lpType[ i ] & PT_CLOSEFIGURE ) LineTo ( hdc, pp -> x, pp -> y );
         
  }  /* end switch */

 }  /* end for */

}  /* end _RenderPath */

static int WINAPI _TextPath (
                   HDC hdc, int x, int y, void* str, LPPOINT lpPts, PBYTE lpType,
                   int nPts, PSIZE pSz, BOOL fWide
                  ) {

 int retVal;

 if ( lpPts == NULL ) {

  int bmo = SetBkMode ( hdc, TRANSPARENT );

   BeginPath ( hdc );

    if ( !fWide )

     TextOutA (   hdc,  x, y, ( char* )str, lstrlenA (  ( char* )str  )   );

    else

     TextOutW (   hdc,  x, y, ( wchar_t* )str, lstrlenW (  ( wchar_t* )str  )   );

   EndPath ( hdc );

  SetBkMode ( hdc, bmo );

 }  /* end if */

 retVal = GetPath ( hdc, lpPts, lpType, nPts );

 if ( pSz != NULL )

  if ( !fWide )

   GetTextExtentPoint32A (   hdc, ( char* )str, lstrlenA (  ( char* )str  ), pSz   );

  else

   GetTextExtentPoint32W (   hdc, ( wchar_t* )str, lstrlenW (  ( wchar_t* )str  ), pSz   );

 return retVal;

}  /* end _TextPath */

VOID CALLBACK ___auxDDAF ( int x, int y, LPARAM lpParam ) {

 PDDAP p = ( PDDAP )lpParam;

 p -> pp[ p -> _this -> myIDDA   ].x = x;
 p -> pp[ p -> _this -> myIDDA++ ].y = y;

 if ( p -> _this -> myIDDA >= p -> _this -> myNDDA ) {
 
  p -> _this -> myDDA1 = ( PPOINT )HeapReAlloc (
                               GetProcessHeap (), HEAP_GENERATE_EXCEPTIONS,
                               p -> _this -> myDDA1, p -> _this -> myNDDA <<= 1
                              );
  p -> _this -> myDDA2 = ( PPOINT )HeapReAlloc (
                                    GetProcessHeap (), HEAP_GENERATE_EXCEPTIONS,
                                    p -> _this -> myDDA2, p -> _this -> myNDDA <<= 1
                                   );

 }  // end if

}  // end ___auxDDAF
//+++//
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
