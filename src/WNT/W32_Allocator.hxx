// Created by: PLOTNIKOV Eugeny & CHABROVSKY Dmitry
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


#ifndef __W32_ALLOCATOR_HXX
# define __W32_ALLOCATOR_HXX

# ifndef _WINDOWS_
#  ifndef STRICT
#   define STRICT
#  endif  /* STRICT */
#  ifndef WIN32_LEAN_AND_MEAN
#   define WIN32_LEAN_AND_MEAN
#   ifdef NOGDI
#     undef NOGDI /* we need GDI definitions here... */
#   endif
#   include <windows.h>
#  endif  /* WIN32_LEAN_AND_MEAN */
# endif  /* _WINDOWS */

# define __W32_DATA -1

# define EPS 0.005F

# define W32F_TOUTLINE 0x00000001  // text flags
# define W32F_TUNICODE 0x00000002
# define W32F_TINDEX   0x00000004
# define W32F_TFREE    0x00000008
# define W32F_TFULINED 0x00000010

# define W32F_EMPTY 0x00000001
# define W32F_DRAWN 0x00000002
# define W32F_XFORM 0x00000004
# define W32F_MONO  0x00000008
# define W32F_DBUFF 0x00000010
# define W32F_START 0x00000020
# define W32F_WIN95 0x00000040
# define W32F_DVDRV 0x00000080
# define W32F_TULIN 0x00000100
# define W32F_MINIT 0x00000200
# define W32F_NOFIL 0x00000400
# define W32F_MFILL 0x80000000
# define W32F_MOUTL 0x40000000
# define W32F_POUTL 0x20000000
# define W32F_DFONT 0x10000000

#define OCC5415 /* avoid warning C4291 in MS VC++ 6.0 */

typedef enum {

   zzNone,       zzPoint,         zzLine,     zzPolyRect,       zzRect,      zzPolyEllipse,
   zzRoundRect,  zzPolyRoundRect, zzArc,      zzChord,          zzPolyChord, zzSector,
   zzPolySector, zzPolygon,       zzPolyline, zzLinkedPolyline, zzBezier,    zzText,
   zzPolyText,   zzMarker  

} W32_Primitive;

struct W32_Allocator;
typedef W32_Allocator* PW32_Allocator;

typedef struct _W32_Block {  // memory block

         _W32_Block* next;
         int         size;
         int         free;
         int         data[ 1 ];

} W32_Block, *PW32_Block;

typedef struct _W32_bitmap {

                int     nUsed;
                HBITMAP hBmp;

               } W32_Bitmap, *PW32_Bitmap;

typedef struct _W32_FCallParam {

                void* pAllocator;
                void* pDriver;

               } W32_FCALLPARAM, *PW32_FCALLPARAM;

typedef void ( WINAPI *GetPointFunc ) ( int, int, int, LPPOINT, int*, void* );

typedef void ( __cdecl *W32_FCall ) ( PW32_FCALLPARAM );

struct W32_Note {  // base class to represent graphic object

 W32_Note () {}
 virtual ~W32_Note () {}

 void* operator new ( size_t, W32_Allocator* );

 virtual void Play    ( BOOL = TRUE ) = 0;
 virtual void Xform   ( void ) {}
 virtual void Release ( void ) {}

 PW32_Allocator myAllocator;

#ifdef OCC5415
 void operator delete (void*, W32_Allocator*) {}
private: // to protect against possible accidental usage
 void operator delete (void* p) {}
#endif
};
///
///////////////////////// POINTS ////////////////////////////
///
struct W32_PointNote : public W32_Note {

 W32_PointNote ( int, int );

 virtual void Play ( BOOL = TRUE );

 int myX, myY;     

};

struct WNT_PointNote : public W32_PointNote {

 WNT_PointNote ( int, int );

 virtual void Play  ( BOOL = TRUE );
 virtual void Xform ( void );

 int myTX, myTY;     

};

struct W32_MarkerPointNote : public W32_PointNote {

 W32_MarkerPointNote ( int, int );

 virtual void Play ( BOOL = TRUE );

};

struct WNT_MarkerPointNote : public WNT_PointNote {

 WNT_MarkerPointNote ( int, int );

 virtual void Play ( BOOL = TRUE );

};
///
///////////////////////// LINES //////////////////////////////
///
struct W32_LineNote : public W32_PointNote {

 W32_LineNote ( int, int, int, int );

 virtual void Play ( BOOL = TRUE );

 int myX2, myY2;

};

struct WNT_LineNote : public WNT_PointNote {

 WNT_LineNote ( int, int, int, int );

 virtual void Play  ( BOOL = TRUE );
 virtual void Xform ( void );

 int myX2, myY2, myTX2, myTY2;

};
///
////////////////////// POLY ELLIPSES /////////////////////////
///
struct W32_PolyEllipseNote : public W32_PointNote {

 W32_PolyEllipseNote ( int, int, int, int );

 virtual void Play ( BOOL = TRUE );

 int myXr, myYr;

};

struct WNT_PolyEllipseNote : public WNT_PointNote {

 WNT_PolyEllipseNote ( int, int, int, int );

 virtual void Play  ( BOOL = TRUE );
 virtual void Xform ( void );

 int myXr, myYr, myTXr, myTYr;

};
///
///////////////////////// ELLIPSES ///////////////////////////
///
struct W32_EllipseNote : public W32_PolyEllipseNote {

 W32_EllipseNote ( int, int, int, int );

 virtual void Play ( BOOL = TRUE );

};

struct WNT_EllipseNote : public WNT_PolyEllipseNote {

 WNT_EllipseNote ( int, int, int, int );

 virtual void Play ( BOOL = TRUE );

};
///
/////////////////////////// ARCS /////////////////////////////
///
struct W32_ArcNote : public W32_PolyEllipseNote {

 W32_ArcNote ( int, int, int, int, double, double );

 virtual void Play ( BOOL = TRUE );

 double mySa, myOa;
      
};

struct WNT_ArcNote : public WNT_PolyEllipseNote {

 WNT_ArcNote ( int, int, int, int, double, double );

 virtual void Play  ( BOOL = TRUE );
 virtual void Xform ( void );

 int mySX, mySY, myTSX, myTSY,
     myEX, myEY, myTEX, myTEY,
     myDirect;
};
///
/////////////////////// POLY CHORDS //////////////////////////
///
struct W32_PolyChordNote : public W32_ArcNote {

 W32_PolyChordNote ( int, int, int, int, double, double );

 virtual void Play ( BOOL = TRUE );
 
};

struct WNT_PolyChordNote : public WNT_ArcNote {

 WNT_PolyChordNote ( int, int, int, int, double, double );

 virtual void Play ( BOOL = TRUE );
 
};
///
////////////////////////// CHORDS ////////////////////////////
///
struct W32_ChordNote : public W32_PolyChordNote {

 W32_ChordNote ( int, int, int, int, double, double );

 virtual void Play ( BOOL = TRUE );

};

struct WNT_ChordNote : public WNT_PolyChordNote {

 WNT_ChordNote ( int, int, int, int, double, double );

 virtual void Play ( BOOL = TRUE );

};
///
////////////////////// POLY SECTORS //////////////////////////
///
struct W32_PolySectorNote : public W32_ArcNote {

 W32_PolySectorNote ( int, int, int, int, double, double );

 virtual void Play ( BOOL = TRUE );

};

struct WNT_PolySectorNote : public WNT_ArcNote {

 WNT_PolySectorNote ( int, int, int, int, double, double );

 virtual void Play ( BOOL = TRUE );

};
///
/////////////////////// SECTORS //////////////////////////////
///
struct W32_SectorNote : public W32_PolySectorNote {

 W32_SectorNote ( int, int, int, int, double, double );

 virtual void Play ( BOOL = TRUE );

};

struct WNT_SectorNote : public WNT_PolySectorNote {

 WNT_SectorNote ( int, int, int, int, double, double );

 virtual void Play ( BOOL = TRUE );

};
///
/////////////////////// MARKERS //////////////////////////////
///
struct W32_PolyMarkerNote : public W32_Note {

 W32_PolyMarkerNote ( int );

 void Replace ( int, int, int );

 virtual void Play ( BOOL = TRUE ) {}

 int     myMaxPoints, mySetPoints;
 LPPOINT myPoints;
  
};

struct W32_PolyMarker1Note : public W32_PolyMarkerNote {

 W32_PolyMarker1Note ( int, GetPointFunc, int, void* );

 virtual void Play ( BOOL = TRUE );

};

struct WNT_PolyMarker1Note : public W32_PolyMarker1Note {

 WNT_PolyMarker1Note ( int, GetPointFunc, int, void* );

 virtual void Play ( BOOL = TRUE );

};

struct W32_PolyMarker2Note : public W32_PolyMarker1Note {

 W32_PolyMarker2Note ( int, GetPointFunc, int, void* );

 virtual void Play ( BOOL = TRUE );

};

struct WNT_PolyMarker2Note : public WNT_PolyMarker1Note {

 WNT_PolyMarker2Note ( int, GetPointFunc, int, void* );

 virtual void Play ( BOOL = TRUE );

};
///
/////////////////////// POLYGONS /////////////////////////////
///
struct W32_PolygonNote : public W32_PolyMarkerNote {

 W32_PolygonNote ( int );

 void Add ( int, int );

 virtual void Play ( BOOL = TRUE );

};

struct WNT_PolygonNote : public W32_PolygonNote {

 WNT_PolygonNote ( int );

 virtual void Play  ( BOOL = TRUE );
 virtual void Xform ( void );

 LPPOINT myTPoints;

};
///
/////////////////////// POLYLINES ////////////////////////////
///
struct W32_PolylineNote : public W32_PolygonNote {

 W32_PolylineNote ( int );

 virtual void Play ( BOOL = TRUE );

};

struct WNT_PolylineNote : public WNT_PolygonNote {

 WNT_PolylineNote ( int );

 virtual void Play ( BOOL = TRUE );

};
///
///////////////////////// IMAGES /////////////////////////////
///
struct W32_ImageNote : public W32_PointNote {

 W32_ImageNote ( int, int, PW32_Bitmap, double = 1.0 );
 virtual ~W32_ImageNote ();

 virtual void Play ( BOOL = TRUE );

 PW32_Bitmap myBitmap;
 double      myScale;

};

struct WNT_ImageNote : public WNT_PointNote {

 WNT_ImageNote ( int, int, PW32_Bitmap, double = 1.0 );
 virtual ~WNT_ImageNote ();

 virtual void Play  ( BOOL = TRUE );

 PW32_Bitmap myBitmap;
 double      myScale;

};
///
///////////////////////// TEXTS //////////////////////////////
///
struct W32_TextNote : public W32_PointNote {

 W32_TextNote ( int, int, double, void*, BOOL = FALSE, BOOL = FALSE );

 virtual void Play ( BOOL = TRUE );

 void*  myText;
 double myAngle;
 DWORD  myFlags;

};

struct WNT_TextNote : public WNT_PointNote {

 WNT_TextNote ( int, int, double, void*, BOOL = FALSE, BOOL = FALSE );

 virtual void Play  ( BOOL = TRUE );
 virtual void Xform ( void );
 void         SetAttribs     ();
 void         RestoreAttribs ();
 void         PaintText      ();
 void         OutlineText    ();
 void         FillText       ();

 void*  myText;
 double mySlant;
 DWORD  myFlags;
 int    myShift;
 XFORM  RMatrix,
        IMatrix,
		    SMatrix;

};
///
/////////////////////// POLYTEXTS ////////////////////////////
///
struct W32_PolyTextNote : public W32_TextNote {

 W32_PolyTextNote ( int, int, double, double, void*, BOOL = FALSE, BOOL = FALSE );

 virtual void Play ( BOOL = TRUE );

 double myMargin;

};

struct WNT_PolyTextNote : public WNT_TextNote {

 WNT_PolyTextNote ( int, int, double, double, void*, BOOL = FALSE, BOOL = FALSE );

 virtual void Play ( BOOL = TRUE );

 double myMargin;

};
///
////////////////// BEGIN/END MARKERS /////////////////////////
///
struct W32_BeginMarkerNote : public W32_PointNote {

 W32_BeginMarkerNote ( int, int, int, int, double );

 virtual void Play ( BOOL = TRUE );

 int    myWidth, myHeight;
 double myAngle;

};

struct WNT_BeginMarkerNote : public WNT_PointNote {

 WNT_BeginMarkerNote ( int, int, int, int, double );

 virtual void Play  ( BOOL = TRUE );
 virtual void Xform ( void );

 int    myWidth, myHeight;
 double myAngle, myPrevAngle;
 XFORM  myMatrix;

};

struct W32_EndMarkerNote : public W32_Note {

 W32_EndMarkerNote ();

 virtual void Play ( BOOL = TRUE );

};

struct WNT_EndMarkerNote : public W32_Note {

 WNT_EndMarkerNote ();

 virtual void Play ( BOOL = TRUE );

};
///
//////////////////// A T T R I B U T E S /////////////////////
///
struct W32_LineAttribNote : public W32_Note {

 W32_LineAttribNote ( DWORD, PLOGBRUSH, DWORD = 0, PDWORD = NULL );

 virtual void Play ( BOOL = TRUE );

 LOGBRUSH myLogBrush;
 DWORD    myPenWidth;
 DWORD    myStyleCount;
 PDWORD   myStyles;

};

struct WNT_LineAttribNote : public W32_Note {

 WNT_LineAttribNote  ( DWORD, PLOGBRUSH, DWORD = 0, PDWORD = NULL );
 virtual ~WNT_LineAttribNote ();

 virtual void Play    ( BOOL = TRUE );

 HPEN     myPen;
 DWORD    myWidth;
 COLORREF myPointColor;

};

struct W32_PolyAttribNote : public W32_Note {

 W32_PolyAttribNote ( PLOGBRUSH, BOOL, int = ALTERNATE );

 virtual void Play ( BOOL = TRUE );

 LOGBRUSH myBrush;
 BOOL     myfEdge;
 int      myFillMode;

};

struct WNT_PolyAttribNote : public W32_Note {

 WNT_PolyAttribNote  ( PLOGBRUSH, BOOL, int = ALTERNATE );
 virtual ~WNT_PolyAttribNote ();

 virtual void Play    ( BOOL = TRUE );

 HBRUSH   myBrush;
 BOOL     myfEdge;
 int      myFillMode;
 BOOL     myNoFill;

};

struct W32_TextAttribNote : public W32_Note {

 W32_TextAttribNote (
  HFONT, COLORREF, double = 0.0, double = 1.0, double = 1.0,
  BOOL = FALSE, BOOL = FALSE, BOOL = FALSE
 );
 virtual ~W32_TextAttribNote ();

 virtual void Play ( BOOL = TRUE );

 COLORREF myColor;
 HFONT    myFont;
 DWORD    myFlags;
 double   mySlant, 
          myHScale,
          myVScale;

};

struct WNT_TextAttribNote : public W32_Note {

 WNT_TextAttribNote (
  HFONT, COLORREF, double = 0.0, double = 1.0, double = 1.0,
  BOOL = FALSE, BOOL = FALSE, BOOL = FALSE
 );
 virtual ~WNT_TextAttribNote ();

 virtual void Play    ( BOOL = TRUE );

 COLORREF myColor;
 HPEN     myPen;
 HBRUSH   myBrush;
 HFONT    myFont;
 BOOL     myfFree;
 DWORD    myFlags;
 double   mySlant, 
          myHScale,
          myVScale;

};

struct W32_MarkerAttribNote : public W32_Note {

 W32_MarkerAttribNote ( COLORREF, DWORD, BOOL );

 virtual void Play ( BOOL = TRUE );

 LOGBRUSH myLogBrush;

};

struct WNT_MarkerAttribNote : public W32_Note {

 WNT_MarkerAttribNote ( COLORREF, DWORD, BOOL );
 virtual ~WNT_MarkerAttribNote ();

 virtual void Play    ( BOOL = TRUE );

 HPEN     myPen;
 COLORREF myMarkerPointColor;
 BOOL     myFill;

};
///
//////////////////// F U N C T I O N  C A L L ////////////////
///
struct W32_FCallNote : public W32_Note {

 W32_FCallNote ( W32_FCall, int, PW32_FCALLPARAM );

 virtual void Play ( BOOL = TRUE );

 W32_FCall       myFunc;
 PW32_FCALLPARAM myParam;

};
///
//////////////////// A L L O C A T O R ///////////////////////
///
struct W32_Allocator {

 W32_Allocator ( int, PW32_Allocator = NULL );
 virtual ~W32_Allocator ();
 
 PW32_Block MakeBlock ( int );
    // allocates memory block
 virtual PW32_Block KillBlock ( PW32_Block );
    // deallocates memory block
 void KillBlocks ( PW32_Block );
    // deallocates memory blocks starting from specified one
 void ClearBlocks ( void );
    // resets allocator
 void ReleaseBlock ( PW32_Block );
    // releases graphics primitives in the specified block
 BOOL FreeSpace ( PW32_Block, int );
    // checks for free space in the specified block
 PW32_Block ReserveData ( unsigned int );
    // checks for free space in the current block
 PW32_Block ReserveFind ( unsigned int );
    // searches for free space
 void* ReserveBlock ( PW32_Block, int, BOOL = FALSE );
    // reserves data in the specified block
 void* NewClass ( unsigned int );
    // gets space for a new class ( W32_Note )
 void* NewData ( unsigned int, BOOL = FALSE );
    // gets space for a new data
 virtual void Xform ( void ) = 0;
    // sets world transformation in the device context

 virtual void Play ( HDC, PSIZE ) = 0;
    // plays notes
 virtual void URect ( LPRECT ) = 0;
    // gets updated rectangle

 virtual void Point ( int, int ) = 0;
    // stores a point
 virtual void MarkerPoint ( int, int ) = 0;
    // stores a marker point
 virtual void Line ( int, int, int, int ) = 0;
    // stores a line segment
 virtual void PolyEllipse ( int, int, int, int ) = 0;
    // stores a polyellipse
 virtual void Ellipse ( int, int, int, int ) = 0;
    // stores an ellipse
 virtual void Arc ( int, int, int, int, double, double ) = 0;
    // stores an arc
 virtual void PolyChord ( int, int, int, int, double, double ) = 0;
    // stores a polychord
 virtual void Chord ( int, int, int, int, double, double ) = 0;
    // stores a chord
 virtual void PolySector ( int, int, int, int, double, double ) = 0;
    // stores a polysector
 virtual void Sector ( int, int, int, int, double, double ) = 0;
    // stores a sector
 virtual void PolyMarker ( int ) = 0;
    // stores a polymarker
 virtual void PolyMarker1 ( int, GetPointFunc, int, void* ) = 0;
    // stores a polymarker1
 virtual void PolyMarker2 ( int, GetPointFunc, int, void* ) = 0;
    // stores a polymarker2
 virtual W32_Note* Polygon ( int ) = 0;
    // stores a polygon
 virtual W32_Note* Polyline ( int ) = 0;
    // stores a polyline
 virtual void Image ( int, int, PW32_Bitmap, double = 1.0 ) = 0;
    // stores an image
 virtual void Text ( int, int, double, void*, BOOL = FALSE, BOOL = FALSE ) = 0;
    // stores a text
 virtual void Polytext ( int, int, double, double, void*, BOOL = FALSE, BOOL = FALSE ) = 0;
    // stores a polytext
 virtual void BeginMarker ( int, int, int, int, double ) = 0;
    // stores the begin of marker
 virtual void EndMarker ( void ) = 0;
    // stores end of the marker
 virtual void LineAttrib ( DWORD, PLOGBRUSH, DWORD = 0, PDWORD = NULL ) = 0;
    // stores line attributes
 virtual void PolyAttrib ( PLOGBRUSH, BOOL, int = ALTERNATE ) = 0;
    // stores polygon attributes
 virtual void TextAttrib (
               HFONT, COLORREF, double = 0.0, double = 1.0, double = 1.0,
               BOOL = FALSE, BOOL = FALSE, BOOL = FALSE
              ) = 0;
    //stores text attributes
 virtual void MarkerAttrib ( COLORREF, DWORD, BOOL ) = 0;
    // stores marker attributes
 virtual void FunCall ( W32_FCall, int, PW32_FCALLPARAM ) = 0;
    // stores function call

 virtual int  TextSize ( HDC, char*,    PSIZE ) = 0;
 virtual int  TextSize ( HDC, wchar_t*, PSIZE ) = 0;
    //obtains dimensions of the text string

 int            myID;
 PW32_Allocator myNext;
 PW32_Block     myStart, myEnd;
 W32_Primitive  myPrimitive;
 W32_Note*      myNote;
 int            myFillMode;
 DWORD          myFlags;
 POINT          myPivot;
 POINT          myMove;
 double         myScaleX, myScaleY;
 double         myAngle;
 double         myTextVScale;
 double         myTextHScale;
 double         myTextSlant;
 HFONT          myTextFont;
 COLORREF       myPointColor,
                myMarkerPointColor;

};

extern PW32_Allocator ( *W32_GetAllocator ) ( int, PW32_Allocator );

#define P(v) LONG(  ( v ) / myPixelToUnit + 0.5  )
#define U(v) (  ( v ) * myPixelToUnit  )

#endif  // __W32_ALLOCATOR_HXX
