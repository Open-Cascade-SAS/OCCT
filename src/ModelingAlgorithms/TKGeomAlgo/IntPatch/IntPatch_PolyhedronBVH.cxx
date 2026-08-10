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

#include <Bnd_Box.hxx>
#include <IntPatch_Polyhedron.hxx>
#include <IntPatch_PolyhedronTool.hxx>
#include <gp_Pnt.hxx>

#include <algorithm>

//==================================================================================================

IntPatch_PolyhedronBVH::IntPatch_PolyhedronBVH()
    : BVH_PrimitiveSet<double, 3>(
        new BVH_LinearBuilder<double, 3>(BVH_Constants_LeafNodeSizeDefault,
                                         BVH_Constants_MaxTreeDepth)),
      myPoly(nullptr)
{
}

//==================================================================================================

IntPatch_PolyhedronBVH::IntPatch_PolyhedronBVH(const IntPatch_Polyhedron& thePoly)
    : BVH_PrimitiveSet<double, 3>(
        new BVH_LinearBuilder<double, 3>(BVH_Constants_LeafNodeSizeDefault,
                                         BVH_Constants_MaxTreeDepth)),
      myPoly(nullptr)
{
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

  // Filter out degenerate triangles (those with void component bounding boxes)
  // by only including triangles with valid boxes in the index mapping.
  // This matches the behavior of the former Bnd_BoundSortBox-based approach,
  // which never matched void boxes.
  const occ::handle<NCollection_HArray1<Bnd_Box>>& aCompBnd =
    IntPatch_PolyhedronTool::ComponentsBounding(thePoly);

  myIndexMap.Clear();
  for (int anIdx = 1; anIdx <= aNbTriangles; ++anIdx)
  {
    if (!aCompBnd->Value(anIdx).IsVoid())
    {
      myIndexMap.Append(anIdx); // Store 1-based original index
    }
  }

  // Mark as dirty to trigger BVH rebuild
  MarkDirty();
}

//==================================================================================================

void IntPatch_PolyhedronBVH::Clear()
{
  myPoly = nullptr;
  myIndexMap.Clear();
  MarkDirty();
}

//==================================================================================================

int IntPatch_PolyhedronBVH::Size() const
{
  if (myPoly == nullptr)
  {
    return 0;
  }
  return static_cast<int>(myIndexMap.Size());
}

//==================================================================================================

BVH_Box<double, 3> IntPatch_PolyhedronBVH::Box(const int theIndex) const
{
  BVH_Box<double, 3> aBox;

  if (myPoly == nullptr || theIndex < 0 || theIndex >= Size())
  {
    return aBox;
  }

  // Get original 1-based triangle index
  const int anOrigIdx = myIndexMap.Value(theIndex);

  // Use pre-computed component bounding boxes from the polyhedron.
  // These boxes include deflection enlargement, matching the behavior
  // of the former Bnd_BoundSortBox-based approach. Degenerate triangles
  // (with void boxes) are already excluded during Init().
  const occ::handle<NCollection_HArray1<Bnd_Box>>& aCompBnd =
    IntPatch_PolyhedronTool::ComponentsBounding(*myPoly);
  const Bnd_Box& aBndBox = aCompBnd->Value(anOrigIdx);

  double aXmin, aYmin, aZmin, aXmax, aYmax, aZmax;
  aBndBox.Get(aXmin, aYmin, aZmin, aXmax, aYmax, aZmax);

  aBox.Add(BVH_Vec3d(aXmin, aYmin, aZmin));
  aBox.Add(BVH_Vec3d(aXmax, aYmax, aZmax));

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
  if (theIndex1 != theIndex2)
  {
    std::swap(myIndexMap.ChangeValue(theIndex1), myIndexMap.ChangeValue(theIndex2));
  }
}

//==================================================================================================

int IntPatch_PolyhedronBVH::OriginalIndex(const int theIndex) const
{
  if (theIndex < 0 || theIndex >= Size())
  {
    return 0;
  }
  return myIndexMap.Value(theIndex);
}
