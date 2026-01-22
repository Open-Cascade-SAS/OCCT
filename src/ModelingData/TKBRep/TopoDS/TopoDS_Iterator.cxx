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
#include <TopoDS_TShapeDispatch.hxx>

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
    myTShape     = nullptr;
    myIndex      = 0U;
    myNbChildren = 0U;
    myShapeType  = TopAbs_SHAPE;
  }
  else
  {
    myTShape    = S.TShape().get();
    myShapeType = myTShape->ShapeType();
    myIndex     = 0U;
    // Use dispatch helper to get NbChildren through concrete type (devirtualized)
    myNbChildren = static_cast<size_t>(
        TopoDS_TShapeDispatch::Apply(myTShape, [](auto* theTyped) { return theTyped->NbChildren(); }));
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
  // Use dispatch helper with cached shape type for devirtualized GetChild() call
  myShape = TopoDS_TShapeDispatch::ApplyWithType(
      myTShape, myShapeType, [this](auto* theTyped) { return theTyped->GetChild(myIndex); });
  myShape.Orientation(TopAbs::Compose(myOrientation, myShape.Orientation()));
  if (!myLocation.IsIdentity())
    myShape.Move(myLocation, false);
}
