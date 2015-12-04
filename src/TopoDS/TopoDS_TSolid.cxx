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
#include <TopAbs.hxx>
#include <TopoDS_TShape.hxx>
#include <TopoDS_TSolid.hxx>

IMPLEMENT_STANDARD_RTTIEXT(TopoDS_TSolid,TopoDS_TShape)

//=======================================================================
//function : ShapeType
//purpose  : 
//=======================================================================
TopAbs_ShapeEnum TopoDS_TSolid::ShapeType() const
{
  return TopAbs_SOLID;
}

//=======================================================================
//function : EmptyCopy
//purpose  : 
//=======================================================================

Handle(TopoDS_TShape) TopoDS_TSolid::EmptyCopy() const
{
  return Handle(TopoDS_TSolid)(new TopoDS_TSolid());
}
