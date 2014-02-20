// Created on: 2013-07-15
// Created by: Anton POLETAEV
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

#ifndef _Graphic3d_SequenceOfHClipPlane_HeaderFile
#define _Graphic3d_SequenceOfHClipPlane_HeaderFile

#include <NCollection_Sequence.hxx>
#include <Graphic3d_ClipPlane.hxx>

// CDL-header shortcut for sequence of graphical clipping planes.
typedef NCollection_Sequence<Handle(Graphic3d_ClipPlane)> Graphic3d_SequenceOfHClipPlane;

#endif
