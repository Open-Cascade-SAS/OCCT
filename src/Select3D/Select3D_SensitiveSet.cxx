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
  const NCollection_Handle<BVH_Tree<Standard_Real, 3> >& aBVH = myContent->GetBVH();

  thePickResult = SelectBasics_PickResult (RealLast(), RealLast());

  if (myContent->Size() < 1 || !theMgr.Overlaps (aBVH->MinPoint (0),
                                                 aBVH->MaxPoint (0)))
  {
    return Standard_False;
  }

  Standard_Integer aStack[32];
  Standard_Integer aNode =  0;
  Standard_Integer aHead = -1;

  Standard_Integer aMatchesNb = -1;
  Standard_Real    aMinDepth  = RealLast();

  for (;;)
  {
    const BVH_Vec4i& aData = aBVH->NodeInfoBuffer()[aNode];

    if (aData.x() == 0) // is inner node
    {
      const Standard_Integer aLftIdx = aData.y();
      const Standard_Integer aRghIdx = aData.z();

      Standard_Boolean isLftInside = Standard_True;
      Standard_Boolean isRghInside = Standard_True;

      Standard_Boolean toCheckLft = theMgr.Overlaps (aBVH->MinPoint (aLftIdx),
                                                     aBVH->MaxPoint (aLftIdx),
                                                     theMgr.IsOverlapAllowed() ? NULL : &isLftInside);

      Standard_Boolean toCheckRgh = theMgr.Overlaps (aBVH->MinPoint (aRghIdx),
                                                     aBVH->MaxPoint (aRghIdx),
                                                     theMgr.IsOverlapAllowed() ? NULL : &isRghInside);

      if (!theMgr.IsOverlapAllowed()) // inclusion test
      {
        if (!toCheckLft || !toCheckRgh)
        {
          return Standard_False; // no inclusion
        }

        toCheckLft &= !isLftInside;
        toCheckRgh &= !isRghInside;
      }

      if (toCheckLft || toCheckRgh)
      {
        aNode = toCheckLft ? aLftIdx : aRghIdx;

        if (toCheckLft && toCheckRgh)
        {
          aStack[++aHead] = aRghIdx;
        }
      }
      else
      {
        if (aHead < 0)
          break;

        aNode = aStack[aHead--];
      }
    }
    else
    {
      for (Standard_Integer anElemIdx = aData.y(); anElemIdx <= aData.z(); ++anElemIdx)
      {
        if (!theMgr.IsOverlapAllowed()) // inclusion test
        {
          if (!elementIsInside (theMgr, anElemIdx))
          {
            return Standard_False;
          }
        }
        else // overlap test
        {
          Standard_Real aCurrentDepth = 0.0;

          if (!overlapsElement (theMgr, anElemIdx, aCurrentDepth))
          {
            continue;
          }

          if (aMinDepth > aCurrentDepth)
          {
            aMinDepth = aCurrentDepth;
            myDetectedIdx = anElemIdx;
          }

          ++aMatchesNb;
        }
      }

      if (aHead < 0)
        break;

      aNode = aStack[aHead--];
    }
  }

  if (aMatchesNb != -1)
  {
    thePickResult = SelectBasics_PickResult (aMinDepth, distanceToCOG (theMgr));
  }

  return !theMgr.IsOverlapAllowed() || aMatchesNb != -1;
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

//=======================================================================
// function : GetLeafNodeSize
// purpose  : Returns a number of nodes in 1 BVH leaf
//=======================================================================
Standard_Integer Select3D_SensitiveSet::GetLeafNodeSize() const
{
  return myContent->GetLeafNodeSize();
}
