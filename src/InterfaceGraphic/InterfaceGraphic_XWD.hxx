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

#ifndef InterfaceGraphic_XWDHeader
#define InterfaceGraphic_XWDHeader

/*
 * InterfaceGraphic_SWAPTEST =	0 sur sun, sgi, hp
 *				1 sur ao1
 */

#define InterfaceGraphic_SWAPTEST (*(char*)&InterfaceGraphic_swaptest)
static unsigned long InterfaceGraphic_swaptest = 1;

#include <InterfaceGraphic_X11.hxx>

#include <X11/XWDFile.h>

#endif /* InterfaceGraphic_XWDHeader */
