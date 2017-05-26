// Created on: 2013-12-25
// Created by: Varvara POSKONINA
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

#include <OpenGl_BVHClipPrimitiveSet.hxx>

#include <BVH_BinnedBuilder.hxx>
#include <Graphic3d_GraphicDriver.hxx>

IMPLEMENT_STANDARD_RTTIEXT(OpenGl_BVHClipPrimitiveSet, BVH_PrimitiveSet3d)

// =======================================================================
// function : OpenGl_BVHClipPrimitiveSet
// purpose  :
// =======================================================================
OpenGl_BVHClipPrimitiveSet::OpenGl_BVHClipPrimitiveSet()
{
  myBuilder = new BVH_BinnedBuilder<Standard_Real, 3> (BVH_Constants_LeafNodeSizeSingle, BVH_Constants_MaxTreeDepth);
}

// =======================================================================
// function : Size
// purpose  :
// =======================================================================
Standard_Integer OpenGl_BVHClipPrimitiveSet::Size() const
{
  return myStructs.Size();
}

// =======================================================================
// function : Box
// purpose  :
// =======================================================================
Graphic3d_BndBox3d OpenGl_BVHClipPrimitiveSet::Box (const Standard_Integer theIdx) const
{
  return myStructs.FindKey (theIdx + 1)->BoundingBox();
}

// =======================================================================
// function : Center
// purpose  :
// =======================================================================
Standard_Real OpenGl_BVHClipPrimitiveSet::Center (const Standard_Integer theIdx,
                                                  const Standard_Integer theAxis) const
{
  Graphic3d_BndBox3d aBndBox = myStructs.FindKey (theIdx + 1)->BoundingBox();

  const Standard_Real aMin = aBndBox.CornerMin()[theAxis];
  const Standard_Real aMax = aBndBox.CornerMax()[theAxis];
  const Standard_Real aCenter = (aMin + aMax) * 0.5;
  return aCenter;
}

// =======================================================================
// function : Swap
// purpose  :
// =======================================================================
void OpenGl_BVHClipPrimitiveSet::Swap (const Standard_Integer theIdx1,
                                       const Standard_Integer theIdx2)
{
  myStructs.Swap (theIdx1 + 1, theIdx2 + 1);
}

// =======================================================================
// function : Add
// purpose  :
// =======================================================================
Standard_Boolean OpenGl_BVHClipPrimitiveSet::Add (const OpenGl_Structure* theStruct)
{
  const Standard_Integer aSize = myStructs.Size();

  if (myStructs.Add (theStruct) > aSize) // new structure?
  {
    MarkDirty();

    return Standard_True;
  }

  return Standard_False;
}

// =======================================================================
// function : Remove
// purpose  :
// =======================================================================
Standard_Boolean OpenGl_BVHClipPrimitiveSet::Remove (const OpenGl_Structure* theStruct)
{
  const Standard_Integer anIndex = myStructs.FindIndex (theStruct);

  if (anIndex != 0)
  {
    myStructs.Swap (Size(), anIndex);
    myStructs.RemoveLast();
    MarkDirty();

    return Standard_True;
  }

  return Standard_False;
}

// =======================================================================
// function : Clear
// purpose  :
// =======================================================================
void OpenGl_BVHClipPrimitiveSet::Clear()
{
  myStructs.Clear();
  MarkDirty();
}

// =======================================================================
// function : GetStructureById
// purpose  :
// =======================================================================
const OpenGl_Structure* OpenGl_BVHClipPrimitiveSet::GetStructureById (Standard_Integer theId)
{
  return myStructs.FindKey (theId + 1);
}
