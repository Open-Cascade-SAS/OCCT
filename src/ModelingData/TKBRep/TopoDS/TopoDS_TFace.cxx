// Created on: 1991-04-12
// Created by: Remi LEQUETTE
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

#include <Standard_Type.hxx>
#include <TopoDS_TFace.hxx>
#include <TopoDS_Shape.hxx>

IMPLEMENT_STANDARD_RTTIEXT(TopoDS_TFace, TopoDS_TShape)

//=================================================================================================

TopAbs_ShapeEnum TopoDS_TFace::ShapeType() const
{
  return TopAbs_FACE;
}

//=================================================================================================

Handle(TopoDS_TShape) TopoDS_TFace::EmptyCopy() const
{
  return Handle(TopoDS_TFace)(new TopoDS_TFace());
}
