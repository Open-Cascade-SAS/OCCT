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
#include <TopoDS_TVertex.hxx>
#include <TopoDS_TWire.hxx>

//==================================================================================================

void TopoDS_Iterator::Initialize(const TopoDS_Shape& S, bool cumOri, bool cumLoc)
{
  if (cumLoc)
  {
    myLocation = S.Location();
  }
  else
  {
    myLocation.Identity();
  }

  if (cumOri)
  {
    myOrientation = S.Orientation();
  }
  else
  {
    myOrientation = TopAbs_FORWARD;
  }

  if (S.IsNull())
  {
    myTShape     = nullptr;
    myIndex      = 0;
    myNbChildren = 0;
    myShapeType  = TopAbs_SHAPE;
  }
  else
  {
    myTShape    = S.TShape().get();
    myIndex     = 0;
    myShapeType = myTShape->ShapeType();

    // Get NbChildren using type-switch for direct storage access (no virtual call)
    switch (myShapeType)
    {
      case TopAbs_VERTEX:
        myNbChildren = 0; // Vertices have no children
        break;
      case TopAbs_EDGE:
        myNbChildren = static_cast<TopoDS_TEdge*>(myTShape)->storageSize();
        break;
      case TopAbs_WIRE:
        myNbChildren = static_cast<TopoDS_TWire*>(myTShape)->storageSize();
        break;
      case TopAbs_FACE:
        myNbChildren = static_cast<TopoDS_TFace*>(myTShape)->storageSize();
        break;
      case TopAbs_SHELL:
        myNbChildren = static_cast<TopoDS_TShell*>(myTShape)->storageSize();
        break;
      case TopAbs_SOLID:
        myNbChildren = static_cast<TopoDS_TSolid*>(myTShape)->storageSize();
        break;
      case TopAbs_COMPSOLID:
        myNbChildren = static_cast<TopoDS_TCompSolid*>(myTShape)->storageSize();
        break;
      case TopAbs_COMPOUND:
        myNbChildren = static_cast<TopoDS_TCompound*>(myTShape)->storageSize();
        break;
      default:
        myNbChildren = 0;
        break;
    }
  }

  if (More())
  {
    updateCurrentShape();
  }
}

//==================================================================================================

void TopoDS_Iterator::Next()
{
  ++myIndex;
  if (More())
  {
    updateCurrentShape();
  }
}

//==================================================================================================

const TopoDS_Shape& TopoDS_Iterator::getChildByType() const
{
  // Type-switch for direct storage access (no virtual call)
  switch (myShapeType)
  {
    case TopAbs_EDGE:
      return static_cast<TopoDS_TEdge*>(myTShape)->storageValue(myIndex);
    case TopAbs_WIRE:
      return static_cast<TopoDS_TWire*>(myTShape)->storageValue(myIndex);
    case TopAbs_FACE:
      return static_cast<TopoDS_TFace*>(myTShape)->storageValue(myIndex);
    case TopAbs_SHELL:
      return static_cast<TopoDS_TShell*>(myTShape)->storageValue(myIndex);
    case TopAbs_SOLID:
      return static_cast<TopoDS_TSolid*>(myTShape)->storageValue(myIndex);
    case TopAbs_COMPSOLID:
      return static_cast<TopoDS_TCompSolid*>(myTShape)->storageValue(myIndex);
    case TopAbs_COMPOUND:
      return static_cast<TopoDS_TCompound*>(myTShape)->storageValue(myIndex);
    default:
      // TopAbs_VERTEX and TopAbs_SHAPE have no children - this is a programming error
      throw Standard_NoSuchObject("TopoDS_Iterator::getChildByType - shape type has no children");
  }
}

//==================================================================================================

void TopoDS_Iterator::updateCurrentShape()
{
  // Get child shape using type-switch for direct access (no virtual call)
  myShape = getChildByType();

  // Compose orientation with parent
  myShape.Orientation(TopAbs::Compose(myOrientation, myShape.Orientation()));

  // Apply location transformation if needed
  if (!myLocation.IsIdentity())
  {
    myShape.Move(myLocation, false);
  }
}
