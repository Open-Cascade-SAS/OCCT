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

#include <BVH_LinearBuilder.hxx>

IMPLEMENT_STANDARD_RTTIEXT(Select3D_SensitiveSet,Select3D_SensitiveEntity)

namespace
{
  //! Default BVH tree builder for sensitive set (optimal for large set of small primitives - for not too long construction time).
  static Handle(Select3D_BVHBuilder3d) THE_SENS_SET_BUILDER = new BVH_LinearBuilder<Standard_Real, 3> (BVH_Constants_LeafNodeSizeSmall, BVH_Constants_MaxTreeDepth);
}

//=======================================================================
// function : DefaultBVHBuilder
// purpose  :
//=======================================================================
const Handle(Select3D_BVHBuilder3d)& Select3D_SensitiveSet::DefaultBVHBuilder()
{
  return THE_SENS_SET_BUILDER;
}

//=======================================================================
// function : SetDefaultBVHBuilder
// purpose  :
//=======================================================================
void Select3D_SensitiveSet::SetDefaultBVHBuilder (const Handle(Select3D_BVHBuilder3d)& theBuilder)
{
  THE_SENS_SET_BUILDER = theBuilder;
}

//=======================================================================
// function : Select3D_SensitiveSet
// purpose  : Creates new empty sensitive set and its content
//=======================================================================
Select3D_SensitiveSet::Select3D_SensitiveSet (const Handle(SelectMgr_EntityOwner)& theOwnerId)
: Select3D_SensitiveEntity (theOwnerId),
  myDetectedIdx (-1)
{
  myContent.SetSensitiveSet (this);
  myContent.SetBuilder (THE_SENS_SET_BUILDER);
  myContent.MarkDirty();
}

//=======================================================================
// function : BVH
// purpose  : Builds BVH tree for sensitive set
//=======================================================================
void Select3D_SensitiveSet::BVH()
{
  myContent.GetBVH();
}

namespace
{
  //! This structure describes the node in BVH
  struct NodeInStack
  {
    NodeInStack (Standard_Integer theId = 0,
                 Standard_Boolean theIsFullInside = false) : Id (theId), IsFullInside (theIsFullInside) {}

    Standard_Integer Id;           //!< node identifier
    Standard_Boolean IsFullInside; //!< if the node is completely inside the current selection volume
  };
}

//=======================================================================
// function : Matches
// purpose  :
//=======================================================================
Standard_Boolean Select3D_SensitiveSet::matches (SelectBasics_SelectingVolumeManager& theMgr,
                                                 SelectBasics_PickResult& thePickResult,
                                                 Standard_Boolean theToCheckAllInside)
{
  myDetectedIdx = -1;
  const BVH_Tree<Standard_Real, 3, BVH_BinaryTree>* aBVH = myContent.GetBVH().get();
  if (myContent.Size() < 1 || !theMgr.Overlaps (aBVH->MinPoint (0),
                                                aBVH->MaxPoint (0)))
  {
    return Standard_False;
  }

  NodeInStack aStack[BVH_Constants_MaxTreeDepth];
  NodeInStack aNode;

  Standard_Integer aHead = -1;

  Standard_Integer aMatchesNb = -1;
  SelectBasics_PickResult aPickResult;
  const bool toCheckFullInside = (theMgr.GetActiveSelectionType() != SelectBasics_SelectingVolumeManager::Point);
  for (;;)
  {
    const BVH_Vec4i& aData = aBVH->NodeInfoBuffer()[aNode.Id];

    if (aData.x() == 0) // is inner node
    {
      NodeInStack aLeft (aData.y(), toCheckFullInside), aRight(aData.z(), toCheckFullInside);
      Standard_Boolean toCheckLft = Standard_True, toCheckRgh = Standard_True;
      if (!aNode.IsFullInside)
      {
        toCheckLft = theMgr.Overlaps (aBVH->MinPoint (aLeft.Id), aBVH->MaxPoint (aLeft.Id), toCheckFullInside ? &aLeft.IsFullInside : NULL);
        if (!toCheckLft)
        {
          aLeft.IsFullInside = Standard_False;
        }

        toCheckRgh = theMgr.Overlaps (aBVH->MinPoint (aRight.Id), aBVH->MaxPoint (aRight.Id), toCheckFullInside ? &aRight.IsFullInside : NULL);
        if (!toCheckRgh)
        {
          aRight.IsFullInside = Standard_False;
        }
      }

      if (!theMgr.IsOverlapAllowed()) // inclusion test
      {
        if (!theToCheckAllInside)
        {
          if (!toCheckLft || !toCheckRgh)
          {
            return Standard_False; // no inclusion
          }

          // skip extra checks
          toCheckLft &= !aLeft.IsFullInside;
          toCheckRgh &= !aRight.IsFullInside;
        }
      }

      if (toCheckLft || toCheckRgh)
      {
        aNode = toCheckLft ? aLeft : aRight;
        if (toCheckLft && toCheckRgh)
        {
          aStack[++aHead] = aRight;
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
          if (!elementIsInside (theMgr, anElemIdx, aNode.IsFullInside))
          {
            if (theToCheckAllInside)
            {
              continue;
            }
            return Standard_False;
          }
        }
        else // overlap test
        {
          if (!overlapsElement (aPickResult, theMgr, anElemIdx, aNode.IsFullInside))
          {
            continue;
          }

          if (thePickResult.Depth() > aPickResult.Depth())
          {
            thePickResult = aPickResult;
            myDetectedIdx = anElemIdx;
          }
        }
        ++aMatchesNb;
      }

      if (aHead < 0)
        break;

      aNode = aStack[aHead--];
    }
  }

  if (aMatchesNb != -1)
  {
    thePickResult.SetDistToGeomCenter(distanceToCOG(theMgr));
  }

  return aMatchesNb != -1
     || (!theToCheckAllInside && !theMgr.IsOverlapAllowed());
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
// function : Clear
// purpose  : Destroys cross-reference to avoid memory leak
//=======================================================================
void Select3D_SensitiveSet::Clear()
{
  //
}
