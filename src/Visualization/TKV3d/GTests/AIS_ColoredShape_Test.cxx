// Copyright (c) 2025 OPEN CASCADE SAS
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

#include <gtest/gtest.h>

#include <AIS_ColoredShape.hxx>

#include <BRep_Builder.hxx>
#include <BRepPrimAPI_MakeBox.hxx>
#include <TopExp_Explorer.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Compound.hxx>
#include <TopoDS_Iterator.hxx>

//! Test accessor exposing protected members of AIS_ColoredShape.
class AIS_ColoredShapeTestAccessor : public AIS_ColoredShape
{
public:
  using DrawerCompdMap = AIS_ColoredShape::DataMapOfDrawerCompd;

  AIS_ColoredShapeTestAccessor(const TopoDS_Shape& theShape)
      : AIS_ColoredShape(theShape)
  {
  }

  static bool testDispatchColors(
    const occ::handle<AIS_ColoredDrawer>&               theParentDrawer,
    const TopoDS_Shape&                                 theShapeToParse,
    const NCollection_DataMap<TopoDS_Shape,
                              occ::handle<AIS_ColoredDrawer>,
                              TopTools_ShapeMapHasher>& theShapeDrawerMap,
    const TopAbs_ShapeEnum                              theParentType,
    const bool                                          theIsParentClosed,
    DataMapOfDrawerCompd*                               theDrawerOpenedShapePerType,
    DataMapOfDrawerCompd&                               theDrawerClosedFaces)
  {
    return dispatchColors(theParentDrawer,
                          theShapeToParse,
                          theShapeDrawerMap,
                          theParentType,
                          theIsParentClosed,
                          theDrawerOpenedShapePerType,
                          theDrawerClosedFaces);
  }
};

namespace
{

//! Count the total number of sub-shapes across all compounds in the dispatch map.
static int countDispatchedShapes(const AIS_ColoredShapeTestAccessor::DrawerCompdMap& theMap)
{
  int aCount = 0;
  for (AIS_ColoredShapeTestAccessor::DrawerCompdMap::Iterator anIter(theMap); anIter.More();
       anIter.Next())
  {
    for (TopoDS_Iterator aChildIter(anIter.Value()); aChildIter.More(); aChildIter.Next())
    {
      ++aCount;
    }
  }
  return aCount;
}

//! Count the number of faces in a shape.
static int countFaces(const TopoDS_Shape& theShape)
{
  int aCount = 0;
  for (TopExp_Explorer anExp(theShape, TopAbs_FACE); anExp.More(); anExp.Next())
  {
    ++aCount;
  }
  return aCount;
}

} // namespace

class AIS_ColoredShapeDispatchTest : public testing::Test
{
protected:
  void SetUp() override
  {
    // Create a box with 6 faces
    myBox = BRepPrimAPI_MakeBox(10.0, 20.0, 30.0).Shape();
    ASSERT_FALSE(myBox.IsNull());
    ASSERT_EQ(countFaces(myBox), 6);

    // Collect faces for individual manipulation
    for (TopExp_Explorer anExp(myBox, TopAbs_FACE); anExp.More(); anExp.Next())
    {
      myFaces.Append(anExp.Current());
    }

    myDefaultDrawer = new Prs3d_Drawer();
  }

  TopoDS_Shape                       myBox;
  NCollection_Sequence<TopoDS_Shape> myFaces;
  occ::handle<Prs3d_Drawer>          myDefaultDrawer;
};

// Test: no overrides on a shell -> all faces dispatched, no override reported.
TEST_F(AIS_ColoredShapeDispatchTest, Shell_NoOverrides)
{
  NCollection_DataMap<TopoDS_Shape, occ::handle<AIS_ColoredDrawer>, TopTools_ShapeMapHasher>
                                               aShapeDrawerMap;
  AIS_ColoredShapeTestAccessor::DrawerCompdMap aOpenedPerType[(size_t)TopAbs_SHAPE];
  AIS_ColoredShapeTestAccessor::DrawerCompdMap aClosedFaces;

  occ::handle<AIS_ColoredDrawer> aParentDrawer;
  // Dispatch on the box shell (first shell child of solid)
  for (TopoDS_Iterator anIter(myBox); anIter.More(); anIter.Next())
  {
    if (anIter.Value().ShapeType() == TopAbs_SHELL)
    {
      const bool isOverride = AIS_ColoredShapeTestAccessor::testDispatchColors(aParentDrawer,
                                                                               anIter.Value(),
                                                                               aShapeDrawerMap,
                                                                               TopAbs_SOLID,
                                                                               false,
                                                                               aOpenedPerType,
                                                                               aClosedFaces);
      EXPECT_FALSE(isOverride);
      break;
    }
  }

  // All 6 faces should be dispatched as opened (not closed, since theIsParentClosed = false)
  EXPECT_EQ(countDispatchedShapes(aOpenedPerType[(size_t)TopAbs_FACE]), 6);
  EXPECT_EQ(countDispatchedShapes(aClosedFaces), 0);
}

// Test: one face overridden with custom color.
TEST_F(AIS_ColoredShapeDispatchTest, Shell_OneFaceOverridden)
{
  NCollection_DataMap<TopoDS_Shape, occ::handle<AIS_ColoredDrawer>, TopTools_ShapeMapHasher>
    aShapeDrawerMap;

  // Override a single face
  occ::handle<AIS_ColoredDrawer> aCustomDrawer = new AIS_ColoredDrawer(myDefaultDrawer);
  aCustomDrawer->SetOwnColor(Quantity_NOC_RED);
  aShapeDrawerMap.Bind(myFaces.First(), aCustomDrawer);

  AIS_ColoredShapeTestAccessor::DrawerCompdMap aOpenedPerType[(size_t)TopAbs_SHAPE];
  AIS_ColoredShapeTestAccessor::DrawerCompdMap aClosedFaces;

  occ::handle<AIS_ColoredDrawer> aParentDrawer;
  for (TopoDS_Iterator anIter(myBox); anIter.More(); anIter.Next())
  {
    if (anIter.Value().ShapeType() == TopAbs_SHELL)
    {
      const bool isOverride = AIS_ColoredShapeTestAccessor::testDispatchColors(aParentDrawer,
                                                                               anIter.Value(),
                                                                               aShapeDrawerMap,
                                                                               TopAbs_SOLID,
                                                                               false,
                                                                               aOpenedPerType,
                                                                               aClosedFaces);
      EXPECT_TRUE(isOverride);
      break;
    }
  }

  // All 6 faces dispatched: 1 with custom drawer + 5 with default
  EXPECT_EQ(countDispatchedShapes(aOpenedPerType[(size_t)TopAbs_FACE]), 6);
}

// Test: one face hidden -> override reported, only 5 faces dispatched.
TEST_F(AIS_ColoredShapeDispatchTest, Shell_OneFaceHidden)
{
  NCollection_DataMap<TopoDS_Shape, occ::handle<AIS_ColoredDrawer>, TopTools_ShapeMapHasher>
    aShapeDrawerMap;

  // Hide one face
  occ::handle<AIS_ColoredDrawer> aHiddenDrawer = new AIS_ColoredDrawer(myDefaultDrawer);
  aHiddenDrawer->SetHidden(true);
  aShapeDrawerMap.Bind(myFaces.First(), aHiddenDrawer);

  AIS_ColoredShapeTestAccessor::DrawerCompdMap aOpenedPerType[(size_t)TopAbs_SHAPE];
  AIS_ColoredShapeTestAccessor::DrawerCompdMap aClosedFaces;

  occ::handle<AIS_ColoredDrawer> aParentDrawer;
  for (TopoDS_Iterator anIter(myBox); anIter.More(); anIter.Next())
  {
    if (anIter.Value().ShapeType() == TopAbs_SHELL)
    {
      const bool isOverride = AIS_ColoredShapeTestAccessor::testDispatchColors(aParentDrawer,
                                                                               anIter.Value(),
                                                                               aShapeDrawerMap,
                                                                               TopAbs_SOLID,
                                                                               false,
                                                                               aOpenedPerType,
                                                                               aClosedFaces);
      EXPECT_TRUE(isOverride);
      break;
    }
  }

  // Only 5 visible faces dispatched
  EXPECT_EQ(countDispatchedShapes(aOpenedPerType[(size_t)TopAbs_FACE]), 5);
}

// Test: all faces overridden -> all dispatched individually.
TEST_F(AIS_ColoredShapeDispatchTest, Shell_AllFacesOverridden)
{
  NCollection_DataMap<TopoDS_Shape, occ::handle<AIS_ColoredDrawer>, TopTools_ShapeMapHasher>
    aShapeDrawerMap;

  // Override all faces
  for (NCollection_Sequence<TopoDS_Shape>::Iterator aFaceIter(myFaces); aFaceIter.More();
       aFaceIter.Next())
  {
    occ::handle<AIS_ColoredDrawer> aDrawer = new AIS_ColoredDrawer(myDefaultDrawer);
    aDrawer->SetOwnColor(Quantity_NOC_GREEN);
    aShapeDrawerMap.Bind(aFaceIter.Value(), aDrawer);
  }

  AIS_ColoredShapeTestAccessor::DrawerCompdMap aOpenedPerType[(size_t)TopAbs_SHAPE];
  AIS_ColoredShapeTestAccessor::DrawerCompdMap aClosedFaces;

  occ::handle<AIS_ColoredDrawer> aParentDrawer;
  for (TopoDS_Iterator anIter(myBox); anIter.More(); anIter.Next())
  {
    if (anIter.Value().ShapeType() == TopAbs_SHELL)
    {
      const bool isOverride = AIS_ColoredShapeTestAccessor::testDispatchColors(aParentDrawer,
                                                                               anIter.Value(),
                                                                               aShapeDrawerMap,
                                                                               TopAbs_SOLID,
                                                                               false,
                                                                               aOpenedPerType,
                                                                               aClosedFaces);
      EXPECT_TRUE(isOverride);
      break;
    }
  }

  // All 6 faces dispatched with their custom drawers
  EXPECT_EQ(countDispatchedShapes(aOpenedPerType[(size_t)TopAbs_FACE]), 6);
}

// Test: compound of two boxes, one box's shell overridden.
TEST_F(AIS_ColoredShapeDispatchTest, Compound_PartialOverride)
{
  TopoDS_Shape aBox2 = BRepPrimAPI_MakeBox(5.0, 5.0, 5.0).Shape();

  BRep_Builder    aBBuilder;
  TopoDS_Compound aCompound;
  aBBuilder.MakeCompound(aCompound);
  aBBuilder.Add(aCompound, myBox);
  aBBuilder.Add(aCompound, aBox2);

  NCollection_DataMap<TopoDS_Shape, occ::handle<AIS_ColoredDrawer>, TopTools_ShapeMapHasher>
    aShapeDrawerMap;

  // Override one face from the first box
  occ::handle<AIS_ColoredDrawer> aCustomDrawer = new AIS_ColoredDrawer(myDefaultDrawer);
  aCustomDrawer->SetOwnColor(Quantity_NOC_BLUE);
  aShapeDrawerMap.Bind(myFaces.First(), aCustomDrawer);

  AIS_ColoredShapeTestAccessor::DrawerCompdMap aOpenedPerType[(size_t)TopAbs_SHAPE];
  AIS_ColoredShapeTestAccessor::DrawerCompdMap aClosedFaces;

  occ::handle<AIS_ColoredDrawer> aParentDrawer;
  const bool isOverride = AIS_ColoredShapeTestAccessor::testDispatchColors(aParentDrawer,
                                                                           aCompound,
                                                                           aShapeDrawerMap,
                                                                           TopAbs_COMPOUND,
                                                                           false,
                                                                           aOpenedPerType,
                                                                           aClosedFaces);
  EXPECT_TRUE(isOverride);

  // 12 faces total (6+6), all should be dispatched
  EXPECT_EQ(countDispatchedShapes(aOpenedPerType[(size_t)TopAbs_FACE]), 12);
}

// Test: all faces hidden in a shell -> early return, empty dispatch.
TEST_F(AIS_ColoredShapeDispatchTest, Shell_AllFacesHidden)
{
  NCollection_DataMap<TopoDS_Shape, occ::handle<AIS_ColoredDrawer>, TopTools_ShapeMapHasher>
    aShapeDrawerMap;

  // Hide all faces
  for (NCollection_Sequence<TopoDS_Shape>::Iterator aFaceIter(myFaces); aFaceIter.More();
       aFaceIter.Next())
  {
    occ::handle<AIS_ColoredDrawer> aDrawer = new AIS_ColoredDrawer(myDefaultDrawer);
    aDrawer->SetHidden(true);
    aShapeDrawerMap.Bind(aFaceIter.Value(), aDrawer);
  }

  AIS_ColoredShapeTestAccessor::DrawerCompdMap aOpenedPerType[(size_t)TopAbs_SHAPE];
  AIS_ColoredShapeTestAccessor::DrawerCompdMap aClosedFaces;

  occ::handle<AIS_ColoredDrawer> aParentDrawer;
  for (TopoDS_Iterator anIter(myBox); anIter.More(); anIter.Next())
  {
    if (anIter.Value().ShapeType() == TopAbs_SHELL)
    {
      const bool isOverride = AIS_ColoredShapeTestAccessor::testDispatchColors(aParentDrawer,
                                                                               anIter.Value(),
                                                                               aShapeDrawerMap,
                                                                               TopAbs_SOLID,
                                                                               false,
                                                                               aOpenedPerType,
                                                                               aClosedFaces);
      EXPECT_TRUE(isOverride);
      break;
    }
  }

  // No faces dispatched - all hidden
  EXPECT_EQ(countDispatchedShapes(aOpenedPerType[(size_t)TopAbs_FACE]), 0);
  EXPECT_EQ(countDispatchedShapes(aClosedFaces), 0);
}
