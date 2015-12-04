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
#include <TopoDS_TCompSolid.hxx>
#include <TopoDS_TShape.hxx>

IMPLEMENT_STANDARD_RTTIEXT(TopoDS_TCompSolid,TopoDS_TShape)

//=======================================================================
//function : ShapeType
//purpose  : 
//=======================================================================
TopAbs_ShapeEnum TopoDS_TCompSolid::ShapeType() const
{
  return TopAbs_COMPSOLID;
}

//=======================================================================
//function : EmptyCopy
//purpose  : 
//=======================================================================

Handle(TopoDS_TShape) TopoDS_TCompSolid::EmptyCopy() const
{
  return Handle(TopoDS_TCompSolid)(new TopoDS_TCompSolid());
}
