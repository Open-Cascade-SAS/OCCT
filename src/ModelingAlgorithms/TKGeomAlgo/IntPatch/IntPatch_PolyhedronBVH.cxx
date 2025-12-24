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

#include <IntPatch_PolyhedronBVH.hxx>

#include <IntPatch_Polyhedron.hxx>
#include <IntPatch_PolyhedronTool.hxx>
#include <gp_Pnt.hxx>

#include <algorithm>

//==================================================================================================

IntPatch_PolyhedronBVH::IntPatch_PolyhedronBVH()
    : BVH_PrimitiveSet<Standard_Real, 3>(
        new BVH_LinearBuilder<Standard_Real, 3>(BVH_Constants_LeafNodeSizeDefault,
                                                BVH_Constants_MaxTreeDepth)),
      myPoly(nullptr),
      myIndexMap(1, 1)
{
  myIndexMap.Init(0);
}

//==================================================================================================

IntPatch_PolyhedronBVH::IntPatch_PolyhedronBVH(const IntPatch_Polyhedron& thePoly)
    : BVH_PrimitiveSet<Standard_Real, 3>(
        new BVH_LinearBuilder<Standard_Real, 3>(BVH_Constants_LeafNodeSizeDefault,
                                                BVH_Constants_MaxTreeDepth)),
      myPoly(nullptr),
      myIndexMap(1, 1)
{
  myIndexMap.Init(0);
  Init(thePoly);
}

//==================================================================================================

IntPatch_PolyhedronBVH::~IntPatch_PolyhedronBVH()
{
  //
}

//==================================================================================================

void IntPatch_PolyhedronBVH::Init(const IntPatch_Polyhedron& thePoly)
{
  myPoly = &thePoly;

  const int aNbTriangles = IntPatch_PolyhedronTool::NbTriangles(thePoly);

  // Initialize index mapping: 0-based index -> 1-based original index
  if (aNbTriangles > 0)
  {
    myIndexMap.Resize(0, aNbTriangles - 1, Standard_False);
    for (int anIdx = 0; anIdx < aNbTriangles; ++anIdx)
    {
      myIndexMap.SetValue(anIdx, anIdx + 1); // Map to 1-based index
    }
  }
  else
  {
    myIndexMap.Resize(0, 0, Standard_False);
    myIndexMap.SetValue(0, 0);
  }

  // Mark as dirty to trigger BVH rebuild
  MarkDirty();
}

//==================================================================================================

void IntPatch_PolyhedronBVH::Clear()
{
  myPoly = nullptr;
  myIndexMap.Resize(0, 0, Standard_False);
  myIndexMap.SetValue(0, 0);
  MarkDirty();
}

//==================================================================================================

int IntPatch_PolyhedronBVH::Size() const
{
  if (myPoly == nullptr)
  {
    return 0;
  }
  return IntPatch_PolyhedronTool::NbTriangles(*myPoly);
}

//==================================================================================================

BVH_Box<Standard_Real, 3> IntPatch_PolyhedronBVH::Box(const int theIndex) const
{
  BVH_Box<Standard_Real, 3> aBox;

  if (myPoly == nullptr || theIndex < 0 || theIndex >= Size())
  {
    return aBox;
  }

  // Get original 1-based triangle index
  const int anOrigIdx = myIndexMap.Value(theIndex);

  // Get triangle vertex indices
  int aP1, aP2, aP3;
  IntPatch_PolyhedronTool::Triangle(*myPoly, anOrigIdx, aP1, aP2, aP3);

  // Get vertex coordinates
  const gp_Pnt& aPnt1 = IntPatch_PolyhedronTool::Point(*myPoly, aP1);
  const gp_Pnt& aPnt2 = IntPatch_PolyhedronTool::Point(*myPoly, aP2);
  const gp_Pnt& aPnt3 = IntPatch_PolyhedronTool::Point(*myPoly, aP3);

  // Compute min corner
  const double aMinX = std::min({aPnt1.X(), aPnt2.X(), aPnt3.X()});
  const double aMinY = std::min({aPnt1.Y(), aPnt2.Y(), aPnt3.Y()});
  const double aMinZ = std::min({aPnt1.Z(), aPnt2.Z(), aPnt3.Z()});

  // Compute max corner
  const double aMaxX = std::max({aPnt1.X(), aPnt2.X(), aPnt3.X()});
  const double aMaxY = std::max({aPnt1.Y(), aPnt2.Y(), aPnt3.Y()});
  const double aMaxZ = std::max({aPnt1.Z(), aPnt2.Z(), aPnt3.Z()});

  aBox.Add(BVH_Vec3d(aMinX, aMinY, aMinZ));
  aBox.Add(BVH_Vec3d(aMaxX, aMaxY, aMaxZ));

  return aBox;
}

//==================================================================================================

double IntPatch_PolyhedronBVH::Center(const int theIndex, const int theAxis) const
{
  if (myPoly == nullptr || theIndex < 0 || theIndex >= Size())
  {
    return 0.0;
  }

  // Get original 1-based triangle index
  const int anOrigIdx = myIndexMap.Value(theIndex);

  // Get triangle vertex indices
  int aP1, aP2, aP3;
  IntPatch_PolyhedronTool::Triangle(*myPoly, anOrigIdx, aP1, aP2, aP3);

  // Get vertex coordinates
  const gp_Pnt& aPnt1 = IntPatch_PolyhedronTool::Point(*myPoly, aP1);
  const gp_Pnt& aPnt2 = IntPatch_PolyhedronTool::Point(*myPoly, aP2);
  const gp_Pnt& aPnt3 = IntPatch_PolyhedronTool::Point(*myPoly, aP3);

  // Compute centroid coordinate along specified axis
  switch (theAxis)
  {
    case 0:
      return (aPnt1.X() + aPnt2.X() + aPnt3.X()) / 3.0;
    case 1:
      return (aPnt1.Y() + aPnt2.Y() + aPnt3.Y()) / 3.0;
    case 2:
      return (aPnt1.Z() + aPnt2.Z() + aPnt3.Z()) / 3.0;
    default:
      return 0.0;
  }
}

//==================================================================================================

void IntPatch_PolyhedronBVH::Swap(const int theIndex1, const int theIndex2)
{
  if (theIndex1 == theIndex2)
  {
    return;
  }

  // Swap indices in the mapping
  const int aTmp = myIndexMap.Value(theIndex1);
  myIndexMap.SetValue(theIndex1, myIndexMap.Value(theIndex2));
  myIndexMap.SetValue(theIndex2, aTmp);
}

//==================================================================================================

int IntPatch_PolyhedronBVH::OriginalIndex(const int theIndex) const
{
  if (theIndex < 0 || theIndex >= myIndexMap.Size())
  {
    return 0;
  }
  return myIndexMap.Value(theIndex);
}
