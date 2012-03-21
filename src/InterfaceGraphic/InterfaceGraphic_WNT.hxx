// Copyright (c) 1991-1999 Matra Datavision
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

#ifndef __INTERFACE_GRAPHIC_WNT_HXX
# define __INTERFACE_GRAPHIC_WNT_HXX

#ifdef WNT
# include <windows.h>
# ifdef DrawText
#  undef DrawText
# endif  /* DrawText */


//for common type naming 
# define DISPLAY    char
# define WINDOW     HWND
# define GLCONTEXT  HGLRC
# define GLDRAWABLE HDC

# define GET_GL_CONTEXT()       wglGetCurrentContext ()
# define GET_GLDEV_CONTEXT()    wglGetCurrentDC()
# define GL_MAKE_CURRENT(a,b,c) wglMakeCurrent(b,c)

#ifdef DrawText
# undef DrawText
#endif  /* DrawText */

#define WDF_NOERASEBKGRND 0x00000001
#define WDF_FOREIGN       0x00000002

typedef struct window_data {

  void*    WNT_Window_Ptr;   // pointer to WNT_Window
  void*    WNT_WDriver_Ptr;  // pointer to WNT_WDriver or Visual3d_View
  void*    WNT_VMgr;         // pointer to Visual3d_ViewManager
  HPALETTE hPal;             // palette handle or NULL
  HBITMAP  hBmp;             // double buffer bitmap handle or NULL
  DWORD    dwFlags;          // additional information

} WINDOW_DATA;

#define faUnderlined 0x00000001
#define faItalic     0x00000002
#define faStrikeOut  0x00000004
#define faBold       0x00000008
#define faHeight     0x00000010
#define faAngle      0x00000020
#define faWidth      0x00000040
#define faSlant      0x00000080

typedef struct font_data {

  BOOL fdUnderlined;
  BOOL fdItalic;
  BOOL fdStrikeOut;
  LONG fdBold;
  LONG fdHeight;
  LONG fdOrientation;
  LONG fdWidth;
  LONG fdSlant;

} FONT_DATA;

#ifdef __cplusplus
extern "C" {
#endif  /* __cplusplus */

  __declspec( dllimport ) long InterfaceGraphic_RealizePalette ( HDC, HPALETTE, BOOL, BOOL );

#ifdef __cplusplus
}
#endif  /* __cplusplus */
#endif //WNT

#endif  /* __INTERFACE_GRAPHIC_WNT_HXX */
