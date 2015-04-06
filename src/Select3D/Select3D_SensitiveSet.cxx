// Created on: 2014-05-29
// Created by: Varvara POSKONINA
// Copyright (c) 2005-2014 OPEN CASCADE SAS
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

#include <Select3D_SensitiveSet.hxx>
#include <Select3D_BVHPrimitiveContent.hxx>

IMPLEMENT_STANDARD_HANDLE (Select3D_SensitiveSet, Select3D_SensitiveEntity)
IMPLEMENT_STANDARD_RTTIEXT(Select3D_SensitiveSet, Select3D_SensitiveEntity)

//=======================================================================
// function : Select3D_SensitiveSet
// purpose  : Creates new empty sensitive set and its content
//=======================================================================
Select3D_SensitiveSet::Select3D_SensitiveSet (const Handle(SelectBasics_EntityOwner)& theOwnerId)
: Select3D_SensitiveEntity (theOwnerId)
{
  myDetectedIdx = -1;
  myIsLeftChildQueuedFirst = Standard_False;
  myContent = new Select3D_BVHPrimitiveContent (this);
}

//=======================================================================
// function : BVH
// purpose  : Builds BVH tree for sensitive set
//=======================================================================
void Select3D_SensitiveSet::BVH()
{
  myContent->GetBVH();
}

//=======================================================================
// function : Matches
// purpose  : Checks whether one or more entities of the set overlap
//            current selecting volume. Implements the traverse of BVH
//            tree built for the set
//=======================================================================
Standard_Boolean Select3D_SensitiveSet::Matches (SelectBasics_SelectingVolumeManager& theMgr,
                                                 SelectBasics_PickResult& thePickResult)
{
  const NCollection_Handle<BVH_Tree<Standard_Real, 3> >& aBVHTree = myContent->GetBVH();

  Standard_Integer aNode = 0; // a root node
  if (!theMgr.Overlaps (aBVHTree->MinPoint (0),
                        aBVHTree->MaxPoint (0)))
  {
    return Standard_False;
  }

  Standard_Integer aStack[32];
  Standard_Integer aHead = -1;
  Standard_Real aDepth      = RealLast();
  Standard_Real aDistToCOG  = RealLast();
  SelectMgr_Vec3 aClosestPnt (RealLast());
  Standard_Integer aMatchesNb = -1;
  for (;;)
  {
    if (!aBVHTree->IsOuter (aNode))
    {
      const Standard_Integer aLeftChildIdx  = aBVHTree->LeftChild  (aNode);
      const Standard_Integer aRightChildIdx = aBVHTree->RightChild (aNode);
      const Standard_Boolean isLeftChildIn  = theMgr.Overlaps (aBVHTree->MinPoint (aLeftChildIdx),
                                                               aBVHTree->MaxPoint (aLeftChildIdx));
      const Standard_Boolean isRightChildIn = theMgr.Overlaps (aBVHTree->MinPoint (aRightChildIdx),
                                                               aBVHTree->MaxPoint (aRightChildIdx));
      if (isLeftChildIn
       && isRightChildIn)
      {
        aNode = aLeftChildIdx;
        ++aHead;
        aStack[aHead] = aRightChildIdx;
      }
      else if (isLeftChildIn
            || isRightChildIn)
      {
        aNode = isLeftChildIn ? aLeftChildIdx : aRightChildIdx;
      }
      else
      {
        if (aHead < 0)
        {
          break;
        }

        aNode = aStack[aHead];
        --aHead;
      }
    }
    else
    {
      Standard_Integer aStartIdx = aBVHTree->BegPrimitive (aNode);
      Standard_Integer anEndIdx = aBVHTree->EndPrimitive (aNode);
      Standard_Boolean isMatched = Standard_False;
      for (Standard_Integer anIdx = aStartIdx; anIdx <= anEndIdx; ++anIdx)
      {
        Standard_Real anElementDepth = 0.0;
        isMatched = overlapsElement (theMgr, anIdx, anElementDepth);
        if (isMatched)
        {
          if (aDepth > anElementDepth)
          {
            aDepth = anElementDepth;
            myDetectedIdx = anIdx;
          }
          aMatchesNb++;
        }
      }
      if (aHead < 0)
      {
        break;
      }

      aNode = aStack[aHead];
      --aHead;
    }
  }

  if (aMatchesNb != -1)
  {
    aDistToCOG = distanceToCOG (theMgr);
    thePickResult = SelectBasics_PickResult (aDepth, aDistToCOG);
    return Standard_True;
  }

  thePickResult = SelectBasics_PickResult (aDepth, aDistToCOG);
  return Standard_False;
}

//=======================================================================
// function : BoundingBox
// purpose  : This method should be redefined in Select3D_SensitiveSet
//            descendants
//=======================================================================
Select3D_BndBox3d Select3D_SensitiveSet::BoundingBox()
{
  return Select3D_BndBox3d (SelectMgr_Vec3 (RealLast()),
                            SelectMgr_Vec3 (RealFirst()));
}

//=======================================================================
// function : CenterOfGeometry
// purpose  : This method should be redefined in Select3D_SensitiveSet
//            descendants
//=======================================================================
gp_Pnt Select3D_SensitiveSet::CenterOfGeometry() const
{
  return gp_Pnt (RealLast(), RealLast(), RealLast());
}

//=======================================================================
// function : MarkDirty
// purpose  : Marks BVH tree of the set as outdated. It will be rebuild
//            at the next call of BVH()
//=======================================================================
void Select3D_SensitiveSet::MarkDirty()
{
  myContent->MarkDirty();
}

//=======================================================================
// function : Clear
// purpose  : Destroys cross-reference to avoid memory leak
//=======================================================================
void Select3D_SensitiveSet::Clear()
{
  myContent.Nullify();
}
