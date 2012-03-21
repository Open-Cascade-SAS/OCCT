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


#ifndef __WNT_ALLOCATOR_HXX
# define __WNT_ALLOCATOR_HXX

# include "EHDC.hxx"
# include "W32_Allocator.hxx"

struct WNT_Allocator : public W32_Allocator {

 WNT_Allocator ( int, PW32_Allocator = NULL );
 virtual ~WNT_Allocator ();

 virtual void Xform ( void );

 virtual void Play  ( HDC, PSIZE );
 virtual void URect ( LPRECT     );

 virtual void Point        ( int, int                                                    );
 virtual void MarkerPoint  ( int, int                                                    );
 virtual void Line         ( int, int, int, int                                          );
 virtual void PolyEllipse  ( int, int, int, int                                          );
 virtual void Ellipse      ( int, int, int, int                                          );
 virtual void Arc          ( int, int, int, int, double, double                          );
 virtual void PolyChord    ( int, int, int, int, double, double                          );
 virtual void Chord        ( int, int, int, int, double, double                          );
 virtual void PolySector   ( int, int, int, int, double, double                          );
 virtual void Sector       ( int, int, int, int, double, double                          );
 virtual void PolyMarker   ( int                                                         );
 virtual void PolyMarker1  ( int, GetPointFunc, int, void*                               );
 virtual void PolyMarker2  ( int, GetPointFunc, int, void*                               );
 virtual W32_Note* Polygon ( int                                                         );
 virtual W32_Note* Polyline( int                                                         );
 virtual void Image        ( int, int, PW32_Bitmap, double = 1.0                         );
 virtual void Text         ( int, int, double, void*, BOOL = FALSE, BOOL = FALSE         );
 virtual void Polytext     ( int, int, double, double, void*, BOOL = FALSE, BOOL = FALSE );
 virtual void BeginMarker  ( int, int, int, int, double                                  );
 virtual void EndMarker    ( void                                                        );
 virtual void LineAttrib   ( DWORD, PLOGBRUSH, DWORD = 0, PDWORD = NULL                  );
 virtual void PolyAttrib   ( PLOGBRUSH, BOOL, int = ALTERNATE                            );
 virtual void TextAttrib   (
               HFONT, COLORREF, double = 0.0, double = 1.0, double = 1.0,
               BOOL = FALSE, BOOL = FALSE, BOOL = FALSE
              );
 virtual void MarkerAttrib ( COLORREF, DWORD, BOOL                                       );
 virtual void FunCall      ( W32_FCall, int, PW32_FCALLPARAM                             );

 virtual int  TextSize ( HDC, char*,    PSIZE );
 virtual int  TextSize ( HDC, wchar_t*, PSIZE );

 void      GetExtent       ( LPSIZE lpSZ );
 void      TransformPoint  ( int&, int&  );
 void      TransformPoint  ( LPPOINT     );
 void      RecalcMatrix    ( XFORM&, double = 0.0,
                             double = 0.0, double = 0.0
                           );
 void      Register        ( int, int     );
 void      Register        ( LPPOINT, int );
 void      Register        ( LPRECT       );

 SIZE      mySize;              // HDC's extent
 HDC       myHDC;               // HDC to draw
 RECT      myURect;             // Update RECT
 XFORM     myMatrix;            // World transformation matrix

 HPEN      myLinePen;           // Line attributes
 DWORD     myLineWidth;
 HBRUSH    myPolyBrush;         // Poly attributes
 HPEN      myTextPen;           // Text attributes
 HBRUSH    myTextBrush;
 COLORREF  myTextColor;
 HPEN      myMarkerPen;         // Marker attributes

 HPEN      myPO;                // Used to remember old
 HBRUSH    myBO;                // pen, brush and font
 HFONT     myFO;

};

typedef WNT_Allocator* PWNT_Allocator;

#endif  // __WNT_ALLOCATOR_HXX
