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

#ifndef __INTERFACE_GRAPHIC_HXX
#define __INTERFACE_GRAPHIC_HXX

#if defined(_WIN32)

#include <windows.h>

#ifdef DrawText
  #undef DrawText
#endif

#elif !defined(__ANDROID__) && (!defined(__APPLE__) || defined(MACOSX_USE_GLX))

#include <stdio.h>

// exclude modern definitions and system-provided glext.h, should be defined before gl.h inclusion
#define GL_GLEXT_LEGACY
#define GLX_GLXEXT_LEGACY

#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/Xatom.h>
#include <GL/glx.h>

// workaround name conflicts with OCCT methods (in class TopoDS_Shape for example)
#ifdef Convex
  #undef Convex
#endif
#ifdef Status
  #undef Status
#endif

#endif

#endif // __INTERFACE_GRAPHIC_HXX
