// Created on: 1991-04-09
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

#include <Standard_NullObject.hxx>
#include <TopoDS_Builder.hxx>
#include <TopoDS_FrozenShape.hxx>
#include <TopoDS_Shape.hxx>
#include <TopoDS_TCompound.hxx>
#include <TopoDS_TCompSolid.hxx>
#include <TopoDS_TEdge.hxx>
#include <TopoDS_TFace.hxx>
#include <TopoDS_TShape.hxx>
#include <TopoDS_TShell.hxx>
#include <TopoDS_TSolid.hxx>
#include <TopoDS_TWire.hxx>
#include <TopoDS_UnCompatibleShapes.hxx>

namespace
{
//! Helper to create bit for a shape type
constexpr unsigned int toBit(TopAbs_ShapeEnum theType)
{
  return 1u << static_cast<unsigned int>(theType);
}

//! Compatibility table: for each shape type, which parent types can contain it.
//! Index = TopAbs_ShapeEnum value of CHILD, bits = allowed PARENT types.
constexpr unsigned int THE_ADD_COMPATIBILITY[9] = {
  toBit(TopAbs_COMPOUND),                                            // COMPOUND
  toBit(TopAbs_COMPOUND),                                            // COMPSOLID
  toBit(TopAbs_COMPOUND) | toBit(TopAbs_COMPSOLID),                  // SOLID
  toBit(TopAbs_COMPOUND) | toBit(TopAbs_SOLID),                      // SHELL
  toBit(TopAbs_COMPOUND) | toBit(TopAbs_SHELL),                      // FACE
  toBit(TopAbs_COMPOUND) | toBit(TopAbs_FACE),                       // WIRE
  toBit(TopAbs_COMPOUND) | toBit(TopAbs_SOLID) | toBit(TopAbs_WIRE), // EDGE
  toBit(TopAbs_COMPOUND) | toBit(TopAbs_SOLID) | toBit(TopAbs_FACE) | toBit(TopAbs_EDGE), // VERTEX
  0                                                                                       // SHAPE
};

//! Helper to add a child shape using type-switch with non-virtual storage access
inline void addChildByType(TopoDS_TShape*      theTShape,
                           TopAbs_ShapeEnum    theShapeType,
                           const TopoDS_Shape& theChild)
{
  switch (theShapeType)
  {
    case TopAbs_EDGE:
      static_cast<TopoDS_TEdge*>(theTShape)->storageAppend(theChild);
      break;
    case TopAbs_WIRE:
      static_cast<TopoDS_TWire*>(theTShape)->storageAppend(theChild);
      break;
    case TopAbs_FACE:
      static_cast<TopoDS_TFace*>(theTShape)->storageAppend(theChild);
      break;
    case TopAbs_SHELL:
      static_cast<TopoDS_TShell*>(theTShape)->storageAppend(theChild);
      break;
    case TopAbs_SOLID:
      static_cast<TopoDS_TSolid*>(theTShape)->storageAppend(theChild);
      break;
    case TopAbs_COMPSOLID:
      static_cast<TopoDS_TCompSolid*>(theTShape)->storageAppend(theChild);
      break;
    case TopAbs_COMPOUND:
      static_cast<TopoDS_TCompound*>(theTShape)->storageAppend(theChild);
      break;
    default:
      break; // VERTEX and SHAPE cannot have children
  }
}

//! Helper to get number of children using type-switch with non-virtual storage access
inline int getNbChildrenByType(TopoDS_TShape* theTShape, TopAbs_ShapeEnum theShapeType)
{
  switch (theShapeType)
  {
    case TopAbs_EDGE:
      return static_cast<TopoDS_TEdge*>(theTShape)->storageSize();
    case TopAbs_WIRE:
      return static_cast<TopoDS_TWire*>(theTShape)->storageSize();
    case TopAbs_FACE:
      return static_cast<TopoDS_TFace*>(theTShape)->storageSize();
    case TopAbs_SHELL:
      return static_cast<TopoDS_TShell*>(theTShape)->storageSize();
    case TopAbs_SOLID:
      return static_cast<TopoDS_TSolid*>(theTShape)->storageSize();
    case TopAbs_COMPSOLID:
      return static_cast<TopoDS_TCompSolid*>(theTShape)->storageSize();
    case TopAbs_COMPOUND:
      return static_cast<TopoDS_TCompound*>(theTShape)->storageSize();
    default:
      return 0;
  }
}

//! Helper to get a child shape using type-switch with non-virtual storage access
inline const TopoDS_Shape& getChildByType(TopoDS_TShape*   theTShape,
                                          TopAbs_ShapeEnum theShapeType,
                                          int              theIndex)
{
  switch (theShapeType)
  {
    case TopAbs_EDGE:
      return static_cast<TopoDS_TEdge*>(theTShape)->storageValue(theIndex);
    case TopAbs_WIRE:
      return static_cast<TopoDS_TWire*>(theTShape)->storageValue(theIndex);
    case TopAbs_FACE:
      return static_cast<TopoDS_TFace*>(theTShape)->storageValue(theIndex);
    case TopAbs_SHELL:
      return static_cast<TopoDS_TShell*>(theTShape)->storageValue(theIndex);
    case TopAbs_SOLID:
      return static_cast<TopoDS_TSolid*>(theTShape)->storageValue(theIndex);
    case TopAbs_COMPSOLID:
      return static_cast<TopoDS_TCompSolid*>(theTShape)->storageValue(theIndex);
    case TopAbs_COMPOUND:
      return static_cast<TopoDS_TCompound*>(theTShape)->storageValue(theIndex);
    default:
      static TopoDS_Shape aNullShape;
      return aNullShape;
  }
}

//! Helper to remove a child shape using type-switch with non-virtual storage access
inline void removeChildByType(TopoDS_TShape* theTShape, TopAbs_ShapeEnum theShapeType, int theIndex)
{
  switch (theShapeType)
  {
    case TopAbs_EDGE:
      static_cast<TopoDS_TEdge*>(theTShape)->storageRemove(theIndex);
      break;
    case TopAbs_WIRE:
      static_cast<TopoDS_TWire*>(theTShape)->storageRemove(theIndex);
      break;
    case TopAbs_FACE:
      static_cast<TopoDS_TFace*>(theTShape)->storageRemove(theIndex);
      break;
    case TopAbs_SHELL:
      static_cast<TopoDS_TShell*>(theTShape)->storageRemove(theIndex);
      break;
    case TopAbs_SOLID:
      static_cast<TopoDS_TSolid*>(theTShape)->storageRemove(theIndex);
      break;
    case TopAbs_COMPSOLID:
      static_cast<TopoDS_TCompSolid*>(theTShape)->storageRemove(theIndex);
      break;
    case TopAbs_COMPOUND:
      static_cast<TopoDS_TCompound*>(theTShape)->storageRemove(theIndex);
      break;
    default:
      break;
  }
}
} // namespace

//==================================================================================================

void TopoDS_Builder::MakeShape(TopoDS_Shape& S, const Handle(TopoDS_TShape)& T) const
{
  S.TShape(T);
  S.Location(TopLoc_Location());
  S.Orientation(TopAbs_FORWARD);
}

//==================================================================================================

void TopoDS_Builder::Add(TopoDS_Shape& aShape, const TopoDS_Shape& aComponent) const
{
  // From now the Component cannot be edited
  aComponent.TShape()->Free(false);

  // Note that freezing aComponent before testing if aShape is free
  // prevents from self-insertion
  // but aShape will be frozen when the Exception is raised
  if (aShape.Free())
  {
    const TopAbs_ShapeEnum aShapeType     = aShape.ShapeType();
    const unsigned int     aComponentType = static_cast<unsigned int>(aComponent.ShapeType());
    const unsigned int     aParentType    = static_cast<unsigned int>(aShapeType);

    if ((THE_ADD_COMPATIBILITY[aComponentType] & (1u << aParentType)) != 0)
    {
      // Prepare the component with relative orientation and location
      TopoDS_Shape aRelativeComponent = aComponent;

      // Compute the relative Orientation
      if (aShape.Orientation() == TopAbs_REVERSED)
      {
        aRelativeComponent.Reverse();
      }

      // Compute the Relative Location
      const TopLoc_Location& aLoc = aShape.Location();
      if (!aLoc.IsIdentity())
      {
        aRelativeComponent.Move(aLoc.Inverted(), false);
      }

      // Add child using type-switch (no virtual call)
      TopoDS_TShape* aTShape = aShape.TShape().get();
      addChildByType(aTShape, aShapeType, aRelativeComponent);

      // Set the TShape as modified
      aTShape->Modified(true);
    }
    else
    {
      throw TopoDS_UnCompatibleShapes("TopoDS_Builder::Add");
    }
  }
  else
  {
    throw TopoDS_FrozenShape("TopoDS_Builder::Add");
  }
}

//==================================================================================================

void TopoDS_Builder::Remove(TopoDS_Shape& aShape, const TopoDS_Shape& aComponent) const
{
  // Check if aShape is not Frozen
  TopoDS_FrozenShape_Raise_if(!aShape.Free(), "TopoDS_Builder::Remove");

  // Compute the relative Orientation and Location of aComponent
  TopoDS_Shape aRelativeComponent = aComponent;
  if (aShape.Orientation() == TopAbs_REVERSED)
  {
    aRelativeComponent.Reverse();
  }
  aRelativeComponent.Location(aRelativeComponent.Location().Predivided(aShape.Location()), false);

  // Find and remove the matching child using type-switch (no virtual calls)
  TopoDS_TShape*         aTShape     = aShape.TShape().get();
  const TopAbs_ShapeEnum aShapeType  = aTShape->ShapeType();
  const int              aNbChildren = getNbChildrenByType(aTShape, aShapeType);

  for (int i = 0; i < aNbChildren; ++i)
  {
    if (getChildByType(aTShape, aShapeType, i) == aRelativeComponent)
    {
      removeChildByType(aTShape, aShapeType, i);
      aTShape->Modified(true);
      break;
    }
  }
}
