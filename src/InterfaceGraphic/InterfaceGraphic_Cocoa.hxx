// Copyright (c) 2013-2014 OPEN CASCADE SAS
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

#ifdef __APPLE__

#ifndef InterfaceGraphic_CocoaHeader
#define InterfaceGraphic_CocoaHeader

#include <stdio.h>

#define WINDOW     void*
#define DISPLAY    void*
#define GLCONTEXT  void*
#define GLDRAWABLE void*

#define GET_GL_CONTEXT()       NULL
#define GET_GLDEV_CONTEXT()    NULL
#define GL_MAKE_CURRENT(a,b,c) {}

#ifndef EXPORT
  #define EXPORT
#endif

#endif // InterfaceGraphic_CocoaHeader

#endif // __APPLE__
