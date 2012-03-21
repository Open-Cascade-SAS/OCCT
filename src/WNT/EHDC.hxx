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

/*****************************************************************************/
/*                                                                           */
/* EXTENDED HDC                                                              */
/*                                                                           */
/*  Abstract: extends existing WIN32's HDC ( styled lines drawing etc. )     */
/*  Note    : use it only in Windows 95 since Windows NT provides these      */
/*            functionalities                                                */
/*                                                                           */
/*  History : JAN-1998 EUG ( creation )                                      */
/*                                                                           */
/*****************************************************************************/
#ifndef __EHDC_HXX
# define __EHDC_HXX

# ifndef __cplusplus
#  error "C++ compiler required for EHDC stuff"
# endif  /* __cplusplus */

# ifndef _WINDOWS_
#  ifndef STRICT
#   define STRICT
#  endif  /* STRICT */
#  ifndef WIN32_LEAN_AND_MEAN
#   define WIN32_LEAN_AND_MEAN
#   include <windows.h>
#   include <windowsx.h>
#  endif  /* WIN32_LEAN_AND_MEAN */
# endif  /* _WINDOWS_ */

# define ARCF_PIE   0x00000001
# define ARCF_CHORD 0x00000002

# define POLYF_NOEDGE     0x00000001
# define POLYF_NOCLOSEDGE 0x00000002

# define EHDCF_XFORM  0x00000001
# define EHDCF_EPEN   0x00000002
# define EHDCF_JOIN   0x00000004
# define EHDCF_PDRAW  0x00000008
# define EHDCF_CJOIN  0x00000010
# define EHDCF_NJOIN  0x00000020
# define EHDCF_JBEVL  0x00000040
# define EHDCF_SFREE  0x80000000
# define EHDCF_TPATH  0x40000000
# define EHDCF_TREG   0x20000000
# define EHDCF_PPUSH  0x10000000
# define EHDCF_BPUSH  0x08000000
# define EHDCF_DDAF   0x04000000

class EHDC {  /* defined extended HDC */

 public:

  EHDC  ( HDC, PSIZE );  /* constructor      */
  EHDC  ();              /*  yet another one */
  ~EHDC ();              /* destructor       */

          /*****************/
          /* DC MANAGEMENT */
          /*****************/

  void   SetDC ( HDC, PSIZE );
     /* sets WIN32's DC for graphics operations */
  HDC    Hdc ( void ) { return myHDC; }
     /* returns WIN32's HDC */
  HBRUSH SelectEPen   ( DWORD, CONST LOGBRUSH*, DWORD, DWORD*, BOOL = FALSE );
     /* selects pen for line drawing */
  void   SelectEPen   ( DWORD, CONST LOGBRUSH*                              );
     /* selects pen for line drawing ( old one is saved ) */
  HPEN   SelectEPen   ( HPEN                                                );
     /* selects pen for line drawing */
  void   SelectEBrush ( PLOGBRUSH, PLOGBRUSH = NULL                         );
     /* selects brush for polygon filling */
  int    SetPolyFillMode ( int aMode ) { return ::SetPolyFillMode ( myHDC, aMode ); }
    /* sets polygon fill mode */

  BOOL   Miter ( void ) { return !( myFlags & EHDCF_JBEVL ); }
    /*  checks for line join style */
  void   SetMiter ( BOOL );
    /*  sets line join style */

  void   Extent ( PSIZE psz ) { *psz = mySize; }
    /* Retrieves viewport extents */

            /*******************/
            /* DRAWING METHODS */
            /*******************/

  void MoveToEx  ( int, int, LPPOINT );
      /* moves current point and begins a new path */
  BOOL LineTo    ( int, int          );
      /* draws a segment from current position and */
      /*  updates this position                    */
  void ClosePath ( void              );
      /* closes current path */
  void Polyline  ( CONST POINT*, int, BOOL = FALSE );
      /* draws a polyline ( path and current position */
      /*  are unchanged )                             */
  void Polygon   ( CONST POINT*, int, DWORD = 0 );
      /* draws a filled polygon ( path and current position */
      /*  are unchanged )                                   */
      /* Note: specify n - 1 points in parameters           */
  void Arc       ( int, int, int, int, double = 0.0, double = 6.283185, DWORD = 0   );
      /* Draws an arc ( path and current position */
      /*  are unchanged )                         */
  void Polyarc   ( int, int, int, int, double = 0.0, double = 6.283185, BOOL = TRUE );
      /* draws a polyarc ( path and current position */
      /*  are unchanged )                            */
  void SetPixel  ( int, int, COLORREF );
      /* draws single pixel */

              /****************/
              /* TEXT METHODS */
              /****************/

  void SetTextColor  ( COLORREF );
  void SetTextAttrib ( COLORREF, HFONT, double = 0.0, double = 1.0, double = 1.0    );
  void ETextOut      ( int, int, char*,    double = 0.0, BOOL = FALSE               );
  void ETextOut      ( int, int, wchar_t*, double = 0.0, BOOL = FALSE               );
  void PolyTextOut (
        int, int, char*,    double = 0.0, double = 0.1, BOOL = FALSE, BOOL = FALSE,
        BOOL = FALSE
       );
  void PolyTextOut (
        int, int, wchar_t*, double = 0.0, double = 0.1, BOOL = FALSE, BOOL = FALSE,
        BOOL = FALSE
       );

         /**************************/
         /* TRANSFORMATION METHODS */
         /**************************/

  void SetWorldTransform    ( XFORM*        );
  void ModifyWorldTransform ( XFORM*, DWORD );
  void GetWorldTransform    ( XFORM* xf     ) { *xf = myXform; }
  void Transform            ( LPPOINT, int  );

         /**************************/
         /* UPDATED REGION METHODS */
         /**************************/

  void ResetURect ( void          );
  void SetURect   ( LPRECT lpRect ) { myURect = *lpRect; }
  void GetURect   ( LPRECT        );
  void Register   ( LPPOINT, int  );

 protected:

  void _Init     ( void         );
  void _DrawTo   ( PPOINT       );
  void _Join     ( void         );
  BOOL _DrawToEx ( PPOINT       );
  void _LineTo   ( PPOINT       );
  void _ETextOut    ( int, int, void*, double = 0.0, BOOL = FALSE, BOOL = FALSE );
  void _PolyTextOut (
        int, int, void*, double = 0.0, double = 0.1,
        BOOL = FALSE, BOOL = FALSE, BOOL = FALSE, BOOL = FALSE
       );

  HDC    myHDC;
  POINT  myStart;
  POINT  myClose;
  POINT  myA, myB;
  POINT  myJoin;
  POINT  myCJoin;
  XFORM  myXform;
  DWORD  myFlags;
  PPOINT myTextPath;
  PBYTE  myTextType;
  DWORD  myTextNP;
  DWORD  myTextBS;
  HPEN   myTextPen;
  HBRUSH myTextBrush;
  HFONT  myTextFont;
  double myTextSlant;
  double myTextHScale;
  double myTextVScale;
  DWORD  myPWidth, myPWPush;
  HPEN   myPPen, myPPenPush;
  HBRUSH myPBrush, myPBrushPush;
  BOOL   myfXpenPush;
  PDWORD myPStyle;
  DWORD  myPNS;
  DWORD  myPBS;
  DWORD  myPIndex;
  DWORD  myPRlen;
  RECT   myURect;
  HBRUSH myBrush, myBrushPush;
  HBRUSH myOBrush;
  SIZE   mySize;
  PPOINT myDDA1;
  PPOINT myDDA2;
  DWORD  myNDDA;
  DWORD  myIDDA;

 friend VOID CALLBACK ___auxDDAF  ( int, int, LPARAM );

};

#endif /* __EHDC_HXX */
