// Created on: 1993-07-23
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


#include <BRepLib_MakeShape.hxx>
#include <StdFail_NotDone.hxx>
#include <TopoDS_Edge.hxx>
#include <TopoDS_Face.hxx>
#include <TopoDS_Shape.hxx>

//=======================================================================
//function : BRepLib_MakeShape
//purpose  : 
//=======================================================================
BRepLib_MakeShape::BRepLib_MakeShape()
{
}

//=======================================================================
//function : Build
//purpose  : 
//=======================================================================

void BRepLib_MakeShape::Build()
{
}

//=======================================================================
//function : Shape
//purpose  : 
//=======================================================================

const TopoDS_Shape&  BRepLib_MakeShape::Shape()
{
  if (!IsDone()) {
    // the following is const cast away
    ((BRepLib_MakeShape*) (void*) this)->Build();
    Check();
  }
  return myShape;
}


//=======================================================================
//function : operator
//purpose  : 
//=======================================================================

BRepLib_MakeShape::operator TopoDS_Shape()
{
  return Shape();
}



//=======================================================================
//function : HasDescendants
//purpose  : 
//=======================================================================

Standard_Boolean BRepLib_MakeShape::HasDescendants(const TopoDS_Face&)const
{
  return (Standard_True);
}



//=======================================================================
//function : FaceStatus
//purpose  : 
//=======================================================================

BRepLib_ShapeModification BRepLib_MakeShape::FaceStatus
  (const TopoDS_Face&) const
{
  BRepLib_ShapeModification myStatus = BRepLib_Trimmed;
  return myStatus;
}


//=======================================================================
//function : GeneratedFaces
//purpose  : 
//=======================================================================

const TopTools_ListOfShape& BRepLib_MakeShape::DescendantFaces
  (const TopoDS_Face&)
{
  return myGenFaces;
}

//=======================================================================
//function : NbSurfaces
//purpose  : 
//=======================================================================

Standard_Integer BRepLib_MakeShape::NbSurfaces() const
{
  return (0);
}

//=======================================================================
//function : NewFaces
//purpose  : 
//=======================================================================

const TopTools_ListOfShape& BRepLib_MakeShape::NewFaces(const Standard_Integer)
{
  return myNewFaces;
}


//=======================================================================
//function : FacesFromEdges
//purpose  : 
//=======================================================================

const TopTools_ListOfShape& BRepLib_MakeShape::FacesFromEdges
  (const TopoDS_Edge&)
{
  return myEdgFaces;
}
