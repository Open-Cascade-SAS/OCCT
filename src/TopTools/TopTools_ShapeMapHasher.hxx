// Created on: 1993-01-14
// Created by: Remi LEQUETTE
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

#ifndef _TopTools_ShapeMapHasher_HeaderFile
#define _TopTools_ShapeMapHasher_HeaderFile

#include <TopoDS_Shape.hxx>
#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>

//! Hash tool, used for generating maps of shapes in topology.
class TopTools_ShapeMapHasher
{
public:
  DEFINE_STANDARD_ALLOC;

  size_t operator()(const TopoDS_Shape& theShape) const noexcept
  {
    return std::hash<TopoDS_Shape>{}(theShape);
  }

  bool operator()(const TopoDS_Shape& S1, const TopoDS_Shape& S2) const noexcept
  {
    return S1.IsSame(S2);
  }
};

#endif // _TopTools_ShapeMapHasher_HeaderFile
