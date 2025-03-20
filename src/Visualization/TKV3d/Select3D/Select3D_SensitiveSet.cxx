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

IMPLEMENT_STANDARD_RTTIEXT(Select3D_SensitiveSet, Select3D_SensitiveEntity)

namespace
{
//! Default BVH tree builder for sensitive set (optimal for large set of small primitives - for not
//! too long construction time).
static Handle(Select3D_BVHBuilder3d) THE_SENS_SET_BUILDER =
  new BVH_LinearBuilder<Standard_Real, 3>(BVH_Constants_LeafNodeSizeSmall,
                                          BVH_Constants_MaxTreeDepth);
} // namespace

//=================================================================================================

const Handle(Select3D_BVHBuilder3d)& Select3D_SensitiveSet::DefaultBVHBuilder()
{
  return THE_SENS_SET_BUILDER;
}

//=================================================================================================

void Select3D_SensitiveSet::SetDefaultBVHBuilder(const Handle(Select3D_BVHBuilder3d)& theBuilder)
{
  THE_SENS_SET_BUILDER = theBuilder;
}

//=======================================================================
// function : Select3D_SensitiveSet
// purpose  : Creates new empty sensitive set and its content
//=======================================================================
Select3D_SensitiveSet::Select3D_SensitiveSet(const Handle(SelectMgr_EntityOwner)& theOwnerId)
    : Select3D_SensitiveEntity(theOwnerId),
      myDetectedIdx(-1)
{
  myContent.SetSensitiveSet(this);
  myContent.SetBuilder(THE_SENS_SET_BUILDER);
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
  NodeInStack(Standard_Integer theId = 0, Standard_Boolean theIsFullInside = false)
      : Id(theId),
        IsFullInside(theIsFullInside)
  {
  }

  Standard_Integer Id;          //!< node identifier
                                // clang-format off
    Standard_Boolean IsFullInside; //!< if the node is completely inside the current selection volume
                                // clang-format on
};
} // namespace

//=================================================================================================

Standard_Boolean Select3D_SensitiveSet::processElements(SelectBasics_SelectingVolumeManager& theMgr,
                                                        Standard_Integer theFirstElem,
                                                        Standard_Integer theLastElem,
                                                        Standard_Boolean theIsFullInside,
                                                        Standard_Boolean theToCheckAllInside,
                                                        SelectBasics_PickResult& thePickResult,
                                                        Standard_Integer&        theMatchesNb)
{
  SelectBasics_PickResult aPickResult;
  for (Standard_Integer anIdx = theFirstElem; anIdx <= theLastElem; anIdx++)
  {
    if (!theMgr.IsOverlapAllowed()) // inclusion test
    {
      if (!elementIsInside(theMgr, anIdx, theIsFullInside))
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
      if (!overlapsElement(aPickResult, theMgr, anIdx, theIsFullInside))
      {
        continue;
      }

      if (thePickResult.Depth() > aPickResult.Depth())
      {
        thePickResult = aPickResult;
        myDetectedIdx = anIdx;
      }
    }
    ++theMatchesNb;
  }

  return Standard_True;
}

//=================================================================================================

Standard_Boolean Select3D_SensitiveSet::matches(SelectBasics_SelectingVolumeManager& theMgr,
                                                SelectBasics_PickResult&             thePickResult,
                                                Standard_Boolean theToCheckAllInside)
{
  myDetectedIdx = -1;

  if (myContent.Size() < 1)
  {
    return Standard_False;
  }

  const Select3D_BndBox3d& aGlobalBox   = myContent.Box();
  Standard_Boolean         isFullInside = Standard_True;

  if (!theMgr.OverlapsBox(aGlobalBox.CornerMin(), aGlobalBox.CornerMax(), &isFullInside))
  {
    return Standard_False;
  }

  Standard_Integer aMatchesNb = -1;

  const bool toCheckFullInside = (theMgr.GetActiveSelectionType() != SelectMgr_SelectionType_Point);
  if (toCheckFullInside && isFullInside)
  {
    Standard_Integer aSize = myContent.Size();
    if (!processElements(theMgr,
                         0,
                         aSize - 1,
                         Standard_True,
                         theToCheckAllInside,
                         thePickResult,
                         aMatchesNb))
    {
      return Standard_False;
    }
  }
  else
  {
    const BVH_Tree<Standard_Real, 3, BVH_BinaryTree>* aBVH = myContent.GetBVH().get();
    NodeInStack                                       aStack[BVH_Constants_MaxTreeDepth];
    NodeInStack                                       aNode;

    Standard_Integer aHead = -1;

    for (;;)
    {
      const BVH_Vec4i& aData = aBVH->NodeInfoBuffer()[aNode.Id];

      if (aData.x() == 0) // is inner node
      {
        NodeInStack      aLeft(aData.y(), toCheckFullInside), aRight(aData.z(), toCheckFullInside);
        Standard_Boolean toCheckLft = Standard_True, toCheckRgh = Standard_True;
        if (!aNode.IsFullInside)
        {
          toCheckLft = theMgr.OverlapsBox(aBVH->MinPoint(aLeft.Id),
                                          aBVH->MaxPoint(aLeft.Id),
                                          toCheckFullInside ? &aLeft.IsFullInside : NULL);
          if (!toCheckLft)
          {
            aLeft.IsFullInside = Standard_False;
          }

          toCheckRgh = theMgr.OverlapsBox(aBVH->MinPoint(aRight.Id),
                                          aBVH->MaxPoint(aRight.Id),
                                          toCheckFullInside ? &aRight.IsFullInside : NULL);
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
        if (!processElements(theMgr,
                             aData.y(),
                             aData.z(),
                             aNode.IsFullInside,
                             theToCheckAllInside,
                             thePickResult,
                             aMatchesNb))
        {
          return Standard_False;
        }

        if (aHead < 0)
          break;

        aNode = aStack[aHead--];
      }
    }
  }

  if (aMatchesNb != -1)
  {
    thePickResult.SetDistToGeomCenter(distanceToCOG(theMgr));
  }

  return aMatchesNb != -1 || (!theToCheckAllInside && !theMgr.IsOverlapAllowed());
}

//=======================================================================
// function : BoundingBox
// purpose  : This method should be redefined in Select3D_SensitiveSet
//            descendants
//=======================================================================
Select3D_BndBox3d Select3D_SensitiveSet::BoundingBox()
{
  return Select3D_BndBox3d(SelectMgr_Vec3(RealLast()), SelectMgr_Vec3(RealFirst()));
}

//=======================================================================
// function : CenterOfGeometry
// purpose  : This method should be redefined in Select3D_SensitiveSet
//            descendants
//=======================================================================
gp_Pnt Select3D_SensitiveSet::CenterOfGeometry() const
{
  return gp_Pnt(RealLast(), RealLast(), RealLast());
}

//=======================================================================
// function : Clear
// purpose  : Destroys cross-reference to avoid memory leak
//=======================================================================
void Select3D_SensitiveSet::Clear()
{
  //
}

//=================================================================================================

void Select3D_SensitiveSet::DumpJson(Standard_OStream& theOStream, Standard_Integer theDepth) const
{
  OCCT_DUMP_TRANSIENT_CLASS_BEGIN(theOStream)
  OCCT_DUMP_BASE_CLASS(theOStream, theDepth, Select3D_SensitiveEntity)

  OCCT_DUMP_FIELD_VALUES_DUMPED(theOStream, theDepth, &myContent)

  OCCT_DUMP_FIELD_VALUE_NUMERICAL(theOStream, myDetectedIdx)

  Select3D_BndBox3d aBoundingBox = ((Select3D_SensitiveSet*)this)->BoundingBox();
  OCCT_DUMP_FIELD_VALUES_DUMPED(theOStream, theDepth, &aBoundingBox)
}
