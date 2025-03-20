// Created on: 1994-05-18
// Created by: Laurent PAINNOT
// Copyright (c) 1994-1999 Matra Datavision
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

#ifndef _StdPrs_ToolVertex_HeaderFile
#define _StdPrs_ToolVertex_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>

class TopoDS_Vertex;

class StdPrs_ToolVertex
{
public:
  DEFINE_STANDARD_ALLOC

  Standard_EXPORT static void Coord(const TopoDS_Vertex& aPoint,
                                    Standard_Real&       X,
                                    Standard_Real&       Y,
                                    Standard_Real&       Z);
};

#endif // _StdPrs_ToolVertex_HeaderFile
