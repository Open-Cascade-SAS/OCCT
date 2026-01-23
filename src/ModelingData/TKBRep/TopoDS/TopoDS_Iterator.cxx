// Created on: 1993-01-21
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

#define No_Standard_NoSuchObject

#include <TopoDS_Iterator.hxx>
#include <TopoDS_TCompound.hxx>
#include <TopoDS_TCompSolid.hxx>
#include <TopoDS_TEdge.hxx>
#include <TopoDS_TFace.hxx>
#include <TopoDS_TShell.hxx>
#include <TopoDS_TSolid.hxx>
#include <TopoDS_TWire.hxx>

//=================================================================================================

NCollection_DynamicArray<TopoDS_Shape>* TopoDS_Iterator::getSubShapesArray(TopoDS_TShape* theTShape)
{
  switch (theTShape->ShapeType())
  {
    case TopAbs_EDGE:
      return &static_cast<TopoDS_TEdge*>(theTShape)->mySubShapes;
    case TopAbs_WIRE:
      return &static_cast<TopoDS_TWire*>(theTShape)->mySubShapes;
    case TopAbs_FACE:
      return &static_cast<TopoDS_TFace*>(theTShape)->mySubShapes;
    case TopAbs_SHELL:
      return &static_cast<TopoDS_TShell*>(theTShape)->mySubShapes;
    case TopAbs_SOLID:
      return &static_cast<TopoDS_TSolid*>(theTShape)->mySubShapes;
    case TopAbs_COMPSOLID:
      return &static_cast<TopoDS_TCompSolid*>(theTShape)->mySubShapes;
    case TopAbs_COMPOUND:
      return &static_cast<TopoDS_TCompound*>(theTShape)->mySubShapes;
    default:
      return nullptr; // Vertex has no children
  }
}

//=================================================================================================

void TopoDS_Iterator::Initialize(const TopoDS_Shape& S, const bool cumOri, const bool cumLoc)
{
  if (cumLoc)
    myLocation = S.Location();
  else
    myLocation.Identity();

  if (cumOri)
    myOrientation = S.Orientation();
  else
    myOrientation = TopAbs_FORWARD;

  if (S.IsNull())
  {
    mySubShapes = nullptr;
    myIndex     = 0U;
  }
  else
  {
    // Get direct pointer to child array (type-switch only once during init)
    mySubShapes = getSubShapesArray(S.TShape().get());
    myIndex     = 0U;
  }

  if (More())
  {
    updateCurrentShape();
  }
}

//=================================================================================================

void TopoDS_Iterator::Next()
{
  ++myIndex;
  if (More())
  {
    updateCurrentShape();
  }
}

//=================================================================================================

void TopoDS_Iterator::updateCurrentShape()
{
  // Direct array access - no dispatch overhead
  myShape = mySubShapes->Value(static_cast<int>(myIndex));
  myShape.Orientation(TopAbs::Compose(myOrientation, myShape.Orientation()));
  if (!myLocation.IsIdentity())
    myShape.Move(myLocation, false);
}
