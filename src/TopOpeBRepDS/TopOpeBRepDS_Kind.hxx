// Created on: 1993-06-17
// Created by: Jean Yves LEBEY
// Copyright (c) 1993-1999 Matra Datavision
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

#ifndef _TopOpeBRepDS_Kind_HeaderFile
#define _TopOpeBRepDS_Kind_HeaderFile

//! different types of objects in DataStructure
enum TopOpeBRepDS_Kind
{
TopOpeBRepDS_POINT,
TopOpeBRepDS_CURVE,
TopOpeBRepDS_SURFACE,
TopOpeBRepDS_VERTEX,
TopOpeBRepDS_EDGE,
TopOpeBRepDS_WIRE,
TopOpeBRepDS_FACE,
TopOpeBRepDS_SHELL,
TopOpeBRepDS_SOLID,
TopOpeBRepDS_COMPSOLID,
TopOpeBRepDS_COMPOUND,
TopOpeBRepDS_UNKNOWN
};

#endif // _TopOpeBRepDS_Kind_HeaderFile
