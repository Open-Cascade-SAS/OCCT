// Copyright (c) 1991-1999 Matra Datavision
// Copyright (c) 1999-2014 OPEN CASCADE SAS
//
// This file is part of Open CASCADE Technology software library.
//
// This library is free software; you can redistribute it and / or modify it
// under the terms of the GNU Lesser General Public version 2.1 as published
// by the Free Software Foundation, with special exception defined in the file
// OCCT_LGPL_EXCEPTION.txt. Consult the file LICENSE_LGPL_21.txt included in OCCT
// distribution for complete text of the license and disclaimer of any warranty.
//
// Alternatively, this file may be used under the terms of Open CASCADE
// commercial license or contractual agreement.

#ifndef __INTERFACE_GRAPHIC_HXX
#define __INTERFACE_GRAPHIC_HXX

#ifdef WNT
  #include <InterfaceGraphic_WNT.hxx>
#elif defined(__APPLE__) && !defined(MACOSX_USE_GLX)
  #include <InterfaceGraphic_Cocoa.hxx>
#else
  #include <InterfaceGraphic_X11.hxx>
#endif

#endif // __INTERFACE_GRAPHIC_HXX
