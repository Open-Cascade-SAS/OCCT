// Copyright (c) 2026 OPEN CASCADE SAS
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

#include <BVH_BoxSet.hxx>
#include <BVH_LinearBuilder.hxx>
#include <BVH_Traverse.hxx>
#include <Bnd_Tools.hxx>
#include <BRepBndLib.hxx>
#include <BRepPrimAPI_MakeBox.hxx>
#include <NCollection_IndexedMap.hxx>
#include <TopExp.hxx>
#include <TopTools_ShapeMapHasher.hxx>
#include <TopoDS_Shape.hxx>

#include <gtest/gtest.h>

namespace
{
typedef BVH_BoxSet<double, 3, TopoDS_Shape> ShapeBoxSet;

occ::handle<ShapeBoxSet> makeShapeBoxSet(const TopoDS_Shape& theShape)
{
  const occ::handle<BVH_LinearBuilder<double, 3>> aBuilder =
    new BVH_LinearBuilder<double, 3>();
  const occ::handle<ShapeBoxSet> aSet = new ShapeBoxSet(aBuilder);

  NCollection_IndexedMap<TopoDS_Shape, TopTools_ShapeMapHasher> aShapes;
  TopExp::MapShapes(theShape, TopAbs_VERTEX, aShapes);
  TopExp::MapShapes(theShape, TopAbs_EDGE, aShapes);
  TopExp::MapShapes(theShape, TopAbs_FACE, aShapes);
  for (int anIndex = 1; anIndex <= aShapes.Extent(); ++anIndex)
  {
    Bnd_Box aBox;
    BRepBndLib::Add(aShapes(anIndex), aBox);
    aSet->Add(aShapes(anIndex), Bnd_Tools::Bnd2BVH(aBox));
  }
  aSet->Build();
  return aSet;
}

class ShapeSelector : public BVH_Traverse<double, 3, ShapeBoxSet, bool>
{
public:
  void SetSelectionBox(const TopoDS_Shape& theShape)
  {
    Bnd_Box aBox;
    BRepBndLib::Add(theShape, aBox);
    myBox = Bnd_Tools::Bnd2BVH(aBox);
  }

  bool RejectNode(const BVH_Vec3d& theMin,
                  const BVH_Vec3d& theMax,
                  bool&            theIsInside) const override
  {
    bool hasOverlap = false;
    theIsInside     = myBox.Contains(theMin, theMax, hasOverlap);
    return !hasOverlap;
  }

  bool AcceptMetric(const bool& theIsInside) const override { return theIsInside; }

  bool Accept(const int theIndex, const bool& theIsInside) override
  {
    if (theIsInside || !myBox.IsOut(myBVHSet->Box(theIndex)))
    {
      ++myAccepted;
      return true;
    }
    return false;
  }

  int Accepted() const { return myAccepted; }

private:
  BVH_Box<double, 3> myBox;
  int                 myAccepted = 0;
};

class ShapeSelectorVoid : public BVH_Traverse<double, 3, void, bool>
{
public:
  void SetSelectionBox(const TopoDS_Shape& theShape)
  {
    Bnd_Box aBox;
    BRepBndLib::Add(theShape, aBox);
    myBox = Bnd_Tools::Bnd2BVH(aBox);
  }

  void SetShapeBoxSet(const ShapeBoxSet* theSet) { mySet = theSet; }

  bool RejectNode(const BVH_Vec3d& theMin,
                  const BVH_Vec3d& theMax,
                  bool&            theIsInside) const override
  {
    bool hasOverlap = false;
    theIsInside     = myBox.Contains(theMin, theMax, hasOverlap);
    return !hasOverlap;
  }

  bool AcceptMetric(const bool& theIsInside) const override { return theIsInside; }

  bool Accept(const int theIndex, const bool& theIsInside) override
  {
    if (theIsInside || !myBox.IsOut(mySet->Box(theIndex)))
    {
      ++myAccepted;
      return true;
    }
    return false;
  }

  int Accepted() const { return myAccepted; }

private:
  const ShapeBoxSet* mySet = nullptr;
  BVH_Box<double, 3> myBox;
  int                 myAccepted = 0;
};

class PairSelector : public BVH_PairTraverse<double, 3, ShapeBoxSet, bool>
{
public:
  bool RejectNode(const BVH_Vec3d& theMin1,
                  const BVH_Vec3d& theMax1,
                  const BVH_Vec3d& theMin2,
                  const BVH_Vec3d& theMax2,
                  bool&            theMetric) const override
  {
    const BVH_Box<double, 3> aBox(theMin1, theMax1);
    theMetric = !aBox.IsOut(theMin2, theMax2);
    return !theMetric;
  }

  bool Accept(const int theIndex1, const int theIndex2) override
  {
    if (!myBVHSet1->Box(theIndex1).IsOut(myBVHSet2->Box(theIndex2)))
    {
      ++myAccepted;
      return true;
    }
    return false;
  }

  int Accepted() const { return myAccepted; }

private:
  int myAccepted = 0;
};

class PairSelectorVoid : public BVH_PairTraverse<double, 3, void, bool>
{
public:
  void SetShapeBoxSets(const ShapeBoxSet* theSet1, const ShapeBoxSet* theSet2)
  {
    mySet1 = theSet1;
    mySet2 = theSet2;
  }

  bool RejectNode(const BVH_Vec3d& theMin1,
                  const BVH_Vec3d& theMax1,
                  const BVH_Vec3d& theMin2,
                  const BVH_Vec3d& theMax2,
                  bool&            theMetric) const override
  {
    const BVH_Box<double, 3> aBox(theMin1, theMax1);
    theMetric = !aBox.IsOut(theMin2, theMax2);
    return !theMetric;
  }

  bool Accept(const int theIndex1, const int theIndex2) override
  {
    if (!mySet1->Box(theIndex1).IsOut(mySet2->Box(theIndex2)))
    {
      ++myAccepted;
      return true;
    }
    return false;
  }

  int Accepted() const { return myAccepted; }

private:
  const ShapeBoxSet* mySet1 = nullptr;
  const ShapeBoxSet* mySet2 = nullptr;
  int                 myAccepted = 0;
};
} // namespace

// Migrated from tests/lowalgos/bvh/bug30655_1.  A box has 8 vertices, 12
// edges, and 6 faces; selecting a vertex/edge/face must therefore find 7/11/17
// interfering sub-shape boxes, respectively.
TEST(BVH_ShapeSelectTest, Bug30655_ShapeSelectionTypedAndVoid)
{
  const TopoDS_Shape aBox = BRepPrimAPI_MakeBox(10., 10., 10.).Shape();
  const occ::handle<ShapeBoxSet> aSet = makeShapeBoxSet(aBox);

  const TopAbs_ShapeEnum aTypes[] = {TopAbs_VERTEX, TopAbs_EDGE, TopAbs_FACE};
  const int               anExpected[] = {7, 11, 17};
  for (int aTypeIndex = 0; aTypeIndex < 3; ++aTypeIndex)
  {
    NCollection_IndexedMap<TopoDS_Shape, TopTools_ShapeMapHasher> aShapes;
    TopExp::MapShapes(aBox, aTypes[aTypeIndex], aShapes);
    for (int aShapeIndex = 1; aShapeIndex <= aShapes.Extent(); ++aShapeIndex)
    {
      ShapeSelector aSelector;
      aSelector.SetSelectionBox(aShapes(aShapeIndex));
      aSelector.SetBVHSet(aSet.operator->());
      EXPECT_EQ(aSelector.Select(), anExpected[aTypeIndex]);
      EXPECT_EQ(aSelector.Accepted(), anExpected[aTypeIndex]);

      ShapeSelectorVoid aVoidSelector;
      aVoidSelector.SetSelectionBox(aShapes(aShapeIndex));
      aVoidSelector.SetShapeBoxSet(aSet.operator->());
      EXPECT_EQ(aVoidSelector.Select(aSet->BVH()), anExpected[aTypeIndex]);
      EXPECT_EQ(aVoidSelector.Accepted(), anExpected[aTypeIndex]);
    }
    EXPECT_EQ(aShapes.Extent(), aTypeIndex == 0 ? 8 : aTypeIndex == 1 ? 12 : 6);
  }
}

TEST(BVH_ShapeSelectTest, Bug30655_PairSelectionTypedAndVoid)
{
  const TopoDS_Shape aBox = BRepPrimAPI_MakeBox(10., 10., 10.).Shape();
  const occ::handle<ShapeBoxSet> aSet = makeShapeBoxSet(aBox);

  PairSelector aSelector;
  aSelector.SetBVHSets(aSet.operator->(), aSet.operator->());
  EXPECT_EQ(aSelector.Select(), 290);
  EXPECT_EQ(aSelector.Accepted(), 290);

  PairSelectorVoid aVoidSelector;
  aVoidSelector.SetShapeBoxSets(aSet.operator->(), aSet.operator->());
  EXPECT_EQ(aVoidSelector.Select(aSet->BVH(), aSet->BVH()), 290);
  EXPECT_EQ(aVoidSelector.Accepted(), 290);
}
