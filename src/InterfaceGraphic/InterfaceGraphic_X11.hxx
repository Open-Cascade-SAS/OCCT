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

#ifndef _WIN32
#ifndef InterfaceGraphic_X11Header
#define InterfaceGraphic_X11Header

#include <stdio.h>

#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/Xatom.h>
#include <GL/glx.h>

#if defined (sun) || defined (SUNOS) || defined (__alpha) || defined (DECOSF1) || defined (sgi) || defined (IRIX) || defined (__hpux)|| defined (HPUX)
  #ifndef icon_width
    #include <X11/bitmaps/icon>
  #endif
#endif // SUNOS or DECOSF1 or SOLARIS or HPUX or IRIX
#if defined(ULTRIX) && !defined(icon_width)
  #define icon_width  16
  #define icon_height 16
  static unsigned char icon_bits[] =
  {
    0xff, 0xff, 0xab, 0xaa, 0x55, 0xd5, 0xab, 0xaa, 0x05, 0xd0, 0x0b, 0xa0,
    0x05, 0xd0, 0x0b, 0xa0, 0x05, 0xd0, 0x0b, 0xa0, 0x05, 0xd0, 0x0b, 0xa0,
    0x55, 0xd5, 0xab, 0xaa, 0x55, 0xd5, 0xff, 0xff
  };
#endif // ULTRIX

// workaround name conflicts with OCCT methods (in class TopoDS_Shape for example)
#ifdef Convex
  #undef Convex
#endif
#ifdef Status
  #undef Status
#endif

#define WINDOW     Window
#define DISPLAY    Display
#define GLCONTEXT  GLXContext
#define GLDRAWABLE GLXDrawable

#define GET_GL_CONTEXT()       glXGetCurrentContext()
#define GET_GLDEV_CONTEXT()    glXGetCurrentDrawable()
#define GL_MAKE_CURRENT(a,b,c) glXMakeCurrent(a,b,c)

#ifndef EXPORT
  #define EXPORT
#endif  // EXPORT

#endif // InterfaceGraphic_X11Header
#endif // _WIN32
