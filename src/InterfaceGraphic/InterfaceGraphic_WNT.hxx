// Copyright (c) 1991-1999 Matra Datavision
// Copyright (c) 1999-2014 OPEN CASCADE SAS
//
// This file is part of Open CASCADE Technology software library.
//
// This library is free software; you can redistribute it and/or modify it under
// the terms of the GNU Lesser General Public License version 2.1 as published
// by the Free Software Foundation, with special exception defined in the file
// OCCT_LGPL_EXCEPTION.txt. Consult the file LICENSE_LGPL_21.txt included in OCCT
// distribution for complete text of the license and disclaimer of any warranty.
//
// Alternatively, this file may be used under the terms of Open CASCADE
// commercial license or contractual agreement.

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
