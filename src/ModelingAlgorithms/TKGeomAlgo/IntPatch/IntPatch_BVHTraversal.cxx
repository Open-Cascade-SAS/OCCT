// Copyright (c) 2024 OPEN CASCADE SAS
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

#include <IntPatch_BVHTraversal.hxx>

#include <IntPatch_PolyhedronBVH.hxx>

//==================================================================================================

IntPatch_BVHTraversal::IntPatch_BVHTraversal()
    : BVH_PairTraverse<Standard_Real, 3>(),
      mySet1(nullptr),
      mySet2(nullptr),
      mySelfInterference(false)
{
}

//==================================================================================================

IntPatch_BVHTraversal::~IntPatch_BVHTraversal() {}

//==================================================================================================

int IntPatch_BVHTraversal::Perform(IntPatch_PolyhedronBVH& theSet1,
                                   IntPatch_PolyhedronBVH& theSet2,
                                   bool                    theSelfInterference)
{
  myPairs.Clear();
  mySet1             = &theSet1;
  mySet2             = &theSet2;
  mySelfInterference = theSelfInterference;

  if (!theSet1.IsInitialized() || !theSet2.IsInitialized())
  {
    return 0;
  }

  if (theSet1.Size() == 0 || theSet2.Size() == 0)
  {
    return 0;
  }

  // Get BVH trees (builds them if necessary)
  const opencascade::handle<BVH_Tree<Standard_Real, 3>>& aBVH1 = theSet1.BVH();
  const opencascade::handle<BVH_Tree<Standard_Real, 3>>& aBVH2 = theSet2.BVH();

  if (aBVH1.IsNull() || aBVH2.IsNull())
  {
    return 0;
  }

  // Perform dual-tree traversal
  return Select(aBVH1, aBVH2);
}

//==================================================================================================

Standard_Boolean IntPatch_BVHTraversal::RejectNode(const BVH_Vec3d& theCMin1,
                                                   const BVH_Vec3d& theCMax1,
                                                   const BVH_Vec3d& theCMin2,
                                                   const BVH_Vec3d& theCMax2,
                                                   double& /*theMetric*/) const
{
  // AABB overlap test: reject if boxes don't overlap
  // Two boxes overlap if and only if they overlap on all three axes
  if (theCMin1.x() > theCMax2.x() || theCMax1.x() < theCMin2.x())
  {
    return Standard_True; // No overlap on X axis
  }
  if (theCMin1.y() > theCMax2.y() || theCMax1.y() < theCMin2.y())
  {
    return Standard_True; // No overlap on Y axis
  }
  if (theCMin1.z() > theCMax2.z() || theCMax1.z() < theCMin2.z())
  {
    return Standard_True; // No overlap on Z axis
  }

  return Standard_False; // Boxes overlap
}

//==================================================================================================

Standard_Boolean IntPatch_BVHTraversal::Accept(const int theIndex1, const int theIndex2)
{
  if (mySet1 == nullptr || mySet2 == nullptr)
  {
    return Standard_False;
  }

  // Get original 1-based triangle indices
  const int anOrigIdx1 = mySet1->OriginalIndex(theIndex1);
  const int anOrigIdx2 = mySet2->OriginalIndex(theIndex2);

  // In self-interference mode, skip pairs where first index >= second index
  // to avoid testing the same pair twice (and to avoid self-intersection)
  if (mySelfInterference && anOrigIdx1 >= anOrigIdx2)
  {
    return Standard_False;
  }

  // Store the pair
  myPairs.Append(TrianglePair(anOrigIdx1, anOrigIdx2));

  return Standard_True;
}
