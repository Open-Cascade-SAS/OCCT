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

// =======================================================================
// function : OpenGl_BVHClipPrimitiveSet
// purpose  :
// =======================================================================
OpenGl_BVHClipPrimitiveSet::OpenGl_BVHClipPrimitiveSet()
{
  myBuilder = new BVH_BinnedBuilder<Standard_ShortReal, 4> (1, 32);
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
Graphic3d_BndBox4f OpenGl_BVHClipPrimitiveSet::Box (const Standard_Integer theIdx) const
{
  return myStructs (theIdx + 1)->BoundingBox();
}

// =======================================================================
// function : Center
// purpose  :
// =======================================================================
Standard_ShortReal OpenGl_BVHClipPrimitiveSet::Center (const Standard_Integer theIdx,
                                                       const Standard_Integer theAxis) const
{
  Graphic3d_BndBox4f aBndBox = myStructs (theIdx + 1)->BoundingBox();
  Standard_ShortReal aCenter =  theAxis == 0 ? (aBndBox.CornerMin().x() + aBndBox.CornerMax().x()) * 0.5f
                             : (theAxis == 1 ? (aBndBox.CornerMin().y() + aBndBox.CornerMax().y()) * 0.5f
                             : (theAxis == 2 ? (aBndBox.CornerMin().z() + aBndBox.CornerMax().z()) * 0.5f
                             : (aBndBox.CornerMin().w() + aBndBox.CornerMax().w()) * 0.5f));
  return aCenter;
}

// =======================================================================
// function : Swap
// purpose  :
// =======================================================================
void OpenGl_BVHClipPrimitiveSet::Swap (const Standard_Integer theIdx1,
                                       const Standard_Integer theIdx2)
{
  const OpenGl_Structure* aStruct1 = myStructs (theIdx1 + 1);
  const OpenGl_Structure* aStruct2 = myStructs (theIdx2 + 1);
  myStructs.ChangeValue (theIdx1 + 1) = aStruct2;
  myStructs.ChangeValue (theIdx2 + 1) = aStruct1;
}

// =======================================================================
// function : Assign
// purpose  :
// =======================================================================
void OpenGl_BVHClipPrimitiveSet::Assign (const OpenGl_ArrayOfStructure& theStructs)
{
  myStructs.Clear();

  const Standard_Integer aNbPriorities = theStructs.Length();
  OpenGl_SequenceOfStructure::Iterator aStructIter;
  for (Standard_Integer aPriorityIdx = 0; aPriorityIdx < aNbPriorities; ++aPriorityIdx)
  {
    const OpenGl_SequenceOfStructure& aSeq = theStructs (aPriorityIdx);
    for (aStructIter.Init (aSeq); aStructIter.More(); aStructIter.Next())
    {
      const OpenGl_Structure* aStruct = aStructIter.Value();
      if (!aStruct->IsAlwaysRendered())
        myStructs.Append (aStruct);
    }
  }

  MarkDirty();
}

// =======================================================================
// function : Add
// purpose  :
// =======================================================================
void OpenGl_BVHClipPrimitiveSet::Add (const OpenGl_Structure* theStruct)
{
  myStructs.Append (theStruct);
  MarkDirty();
}

// =======================================================================
// function : Remove
// purpose  :
// =======================================================================
void OpenGl_BVHClipPrimitiveSet::Remove (const OpenGl_Structure* theStruct)
{
  for (Standard_Integer anIdx = 1; anIdx <= myStructs.Size(); ++anIdx)
  {
    if (myStructs (anIdx) == theStruct)
    {
      myStructs.Remove (anIdx);
      MarkDirty();
      break;
    }
  }
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
  return myStructs (theId + 1);
}
