// Created on: 2014-12-18
// Created by: Kirill Gavrilov
// Copyright (c) 2014 OPEN CASCADE SAS
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

#ifndef _Graphic3d_MapOfStructure
#define _Graphic3d_MapOfStructure

#include <NCollection_Map.hxx>

class Graphic3d_Structure;
typedef NCollection_Map<Handle(Graphic3d_Structure)> Graphic3d_MapOfStructure;

#endif // _Graphic3d_MapOfStructure
