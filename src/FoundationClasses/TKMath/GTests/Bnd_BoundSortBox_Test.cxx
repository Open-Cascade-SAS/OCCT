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

#include <Bnd_BoundSortBox.hxx>
#include <Bnd_Box.hxx>
#include <Bnd_HArray1OfBox.hxx>
#include <BRepBndLib.hxx>
#include <BRepPrimAPI_MakeBox.hxx>
#include <gp_Pln.hxx>
#include <gp_Pnt.hxx>
#include <gp_Dir.hxx>
#include <TColStd_ListOfInteger.hxx>
#include <TopoDS_Shape.hxx>
#include <TopExp_Explorer.hxx>

#include <random>
#include <chrono>

// Helper function to create a box from min and max points
static Bnd_Box CreateBox(const Standard_Real xmin,
                         const Standard_Real ymin,
                         const Standard_Real zmin,
                         const Standard_Real xmax,
                         const Standard_Real ymax,
                         const Standard_Real zmax)
{
  Bnd_Box box;
  box.Update(xmin, ymin, zmin, xmax, ymax, zmax);
  return box;
}

// Test fixture for Bnd_BoundSortBox tests
class Bnd_BoundSortBoxTest : public ::testing::Test
{
protected:
  void SetUp() override
  {
    // Setup common testing environment
    mySmallBox           = CreateBox(0.0, 0.0, 0.0, 1.0, 1.0, 1.0);
    myLargeBox           = CreateBox(-10.0, -10.0, -10.0, 10.0, 10.0, 10.0);
    myOffsetBox          = CreateBox(5.0, 5.0, 5.0, 7.0, 7.0, 7.0);
    myNonIntersectingBox = CreateBox(20.0, 20.0, 20.0, 30.0, 30.0, 30.0);

    // Create array of test boxes
    myBoxes = new Bnd_HArray1OfBox(1, 4);
    myBoxes->SetValue(1, mySmallBox);
    myBoxes->SetValue(2, myLargeBox);
    myBoxes->SetValue(3, myOffsetBox);
    myBoxes->SetValue(4, myNonIntersectingBox);

    // Create a global bounding box that contains all test boxes
    myGlobalBox = CreateBox(-20.0, -20.0, -20.0, 40.0, 40.0, 40.0);
  }

  // Common test data
  Bnd_Box                  mySmallBox;
  Bnd_Box                  myLargeBox;
  Bnd_Box                  myOffsetBox;
  Bnd_Box                  myNonIntersectingBox;
  Bnd_Box                  myGlobalBox;
  Handle(Bnd_HArray1OfBox) myBoxes;
};

//==================================================================================================

// Test initialization with set of boxes
TEST_F(Bnd_BoundSortBoxTest, InitializeWithBoxes)
{
  Bnd_BoundSortBox sortBox;
  sortBox.Initialize(myBoxes);

  // Test comparing with a box that intersects mySmallBox
  Bnd_Box                      testBox = CreateBox(0.5, 0.5, 0.5, 1.5, 1.5, 1.5);
  const TColStd_ListOfInteger& result  = sortBox.Compare(testBox);

  EXPECT_EQ(2, result.Extent()) << "Expected to find 2 intersections";

  // Check that box indices 1 (mySmallBox) and 2 (myLargeBox) are in the result
  bool foundSmall = false;
  bool foundLarge = false;
  for (TColStd_ListOfInteger::Iterator it(result); it.More(); it.Next())
  {
    if (it.Value() == 1)
      foundSmall = true;
    if (it.Value() == 2)
      foundLarge = true;
  }
  EXPECT_TRUE(foundSmall) << "Small box (index 1) should be in the result";
  EXPECT_TRUE(foundLarge) << "Large box (index 2) should be in the result";
}

//==================================================================================================

// Test initialization with provided enclosing box
TEST_F(Bnd_BoundSortBoxTest, InitializeWithEnclosingBox)
{
  Bnd_BoundSortBox sortBox;
  sortBox.Initialize(myGlobalBox, myBoxes);

  // Test comparing with myOffsetBox
  const TColStd_ListOfInteger& result = sortBox.Compare(myOffsetBox);

  EXPECT_EQ(2, result.Extent()) << "Expected to find 2 intersections";

  // Check that box indices 2 (myLargeBox) and 3 (myOffsetBox) are in the result
  bool foundLarge  = false;
  bool foundOffset = false;
  for (TColStd_ListOfInteger::Iterator it(result); it.More(); it.Next())
  {
    if (it.Value() == 2)
      foundLarge = true;
    if (it.Value() == 3)
      foundOffset = true;
  }
  EXPECT_TRUE(foundLarge) << "Large box (index 2) should be in the result";
  EXPECT_TRUE(foundOffset) << "Offset box (index 3) should be in the result";
}

//==================================================================================================

// Test initialization with enclosing box and expected count
TEST_F(Bnd_BoundSortBoxTest, InitializeWithCount)
{
  Bnd_BoundSortBox sortBox;
  sortBox.Initialize(myGlobalBox, 3);

  // Add boxes manually
  sortBox.Add(mySmallBox, 1);
  sortBox.Add(myLargeBox, 2);
  sortBox.Add(myNonIntersectingBox, 3);

  // Test comparing with a box that should only intersect myLargeBox
  Bnd_Box                      testBox = CreateBox(-5.0, -5.0, -5.0, -2.0, -2.0, -2.0);
  const TColStd_ListOfInteger& result  = sortBox.Compare(testBox);

  EXPECT_EQ(1, result.Extent()) << "Expected to find 1 intersection";

  // Verify that only the large box was found
  EXPECT_EQ(2, result.First()) << "Large box (index 2) should be the only result";
}

//==================================================================================================

// Test comparing with a plane
TEST_F(Bnd_BoundSortBoxTest, CompareWithPlane)
{
  Bnd_BoundSortBox sortBox;
  sortBox.Initialize(myBoxes);

  // Create a plane that intersects the large box but not others
  gp_Pnt point(0.0, 0.0, 9.0);
  gp_Dir direction(gp_Dir::D::Z);
  gp_Pln plane(point, direction);

  const TColStd_ListOfInteger& result = sortBox.Compare(plane);

  // Only the large box should intersect this plane
  EXPECT_EQ(1, result.Extent()) << "Expected to find 1 intersection";
  EXPECT_EQ(2, result.First()) << "Large box (index 2) should be the only result";
}

//==================================================================================================

// Test with void boxes
TEST_F(Bnd_BoundSortBoxTest, VoidBoxes)
{
  Handle(Bnd_HArray1OfBox) boxes = new Bnd_HArray1OfBox(1, 2);
  Bnd_Box                  void_box; // Default constructed box is void
  boxes->SetValue(1, void_box);
  boxes->SetValue(2, mySmallBox);

  Bnd_BoundSortBox sortBox;
  sortBox.Initialize(boxes);

  // Test comparing with mySmallBox - should only find itself
  const TColStd_ListOfInteger& result = sortBox.Compare(mySmallBox);

  EXPECT_EQ(1, result.Extent()) << "Expected to find 1 intersection";
  EXPECT_EQ(2, result.First()) << "Small box (index 2) should be the only result";

  // Test comparing with void box - should find nothing
  const TColStd_ListOfInteger& void_result = sortBox.Compare(void_box);
  EXPECT_EQ(0, void_result.Extent()) << "Expected to find 0 intersections with void box";
}

//==================================================================================================

// Test with touching boxes
TEST_F(Bnd_BoundSortBoxTest, TouchingBoxes)
{
  // Create boxes that touch at a single point
  Bnd_Box box1 = CreateBox(0.0, 0.0, 0.0, 1.0, 1.0, 1.0);
  Bnd_Box box2 = CreateBox(1.0, 1.0, 1.0, 2.0, 2.0, 2.0); // Touches box1 at (1,1,1)

  Handle(Bnd_HArray1OfBox) boxes = new Bnd_HArray1OfBox(1, 2);
  boxes->SetValue(1, box1);
  boxes->SetValue(2, box2);

  Bnd_BoundSortBox sortBox;
  sortBox.Initialize(boxes);

  // Test comparing with box1
  const TColStd_ListOfInteger& result1 = sortBox.Compare(box1);
  EXPECT_EQ(2, result1.Extent()) << "Expected to find 2 intersections";

  // Test comparing with box2
  const TColStd_ListOfInteger& result2 = sortBox.Compare(box2);
  EXPECT_EQ(2, result2.Extent()) << "Expected to find 2 intersections";
}

//==================================================================================================

// Test with boxes that are far apart
TEST_F(Bnd_BoundSortBoxTest, DisjointBoxes)
{
  Bnd_Box farBox = CreateBox(100.0, 100.0, 100.0, 110.0, 110.0, 110.0);

  Handle(Bnd_HArray1OfBox) boxes = new Bnd_HArray1OfBox(1, 2);
  boxes->SetValue(1, mySmallBox);
  boxes->SetValue(2, farBox);

  // Create enclosing box large enough
  Bnd_Box enclosingBox = CreateBox(-10.0, -10.0, -10.0, 120.0, 120.0, 120.0);

  Bnd_BoundSortBox sortBox;
  sortBox.Initialize(enclosingBox, boxes);

  // Test comparing with a box near mySmallBox
  Bnd_Box                      testBox = CreateBox(0.5, 0.5, 0.5, 1.5, 1.5, 1.5);
  const TColStd_ListOfInteger& result  = sortBox.Compare(testBox);

  EXPECT_EQ(1, result.Extent()) << "Expected to find 1 intersection";
  EXPECT_EQ(1, result.First()) << "Small box (index 1) should be the only result";
}

//==================================================================================================

// Test with degenerate boxes (points, lines, planes)
TEST_F(Bnd_BoundSortBoxTest, DegenerateBoxes)
{
  // Create degenerate boxes
  Bnd_Box pointBox;
  pointBox.Update(1.0, 1.0, 1.0, 1.0, 1.0, 1.0); // Point at (1,1,1)

  Bnd_Box lineBox;
  lineBox.Update(2.0, 0.0, 0.0, 5.0, 0.0, 0.0); // Line along X from (2,0,0) to (5,0,0)

  Bnd_Box planeBox;
  planeBox.Update(0.0, 0.0, 3.0, 3.0, 3.0, 3.0); // XY plane at Z=3

  Handle(Bnd_HArray1OfBox) boxes = new Bnd_HArray1OfBox(1, 3);
  boxes->SetValue(1, pointBox);
  boxes->SetValue(2, lineBox);
  boxes->SetValue(3, planeBox);

  Bnd_BoundSortBox sortBox;
  sortBox.Initialize(boxes);

  // Test with a box that should intersect all three degenerate boxes
  Bnd_Box                      testBox = CreateBox(0.0, 0.0, 0.0, 6.0, 6.0, 6.0);
  const TColStd_ListOfInteger& result  = sortBox.Compare(testBox);

  EXPECT_EQ(3, result.Extent()) << "Expected to find 3 intersections with degenerate boxes";

  // Test with a box that should intersect only the point
  Bnd_Box                      pointTestBox = CreateBox(0.5, 0.5, 0.5, 1.5, 1.5, 1.5);
  const TColStd_ListOfInteger& pointResult  = sortBox.Compare(pointTestBox);

  EXPECT_EQ(1, pointResult.Extent()) << "Expected to find only the point box";
  EXPECT_EQ(1, pointResult.First()) << "Point box (index 1) should be the only result";
}

//==================================================================================================

// Test BUC60729: Initialize Bnd_BoundSortBox with face boxes from a solid
// Migrated from QABugs_3.cxx
TEST(Bnd_BoundSortBox_Test, BUC60729_InitializeWithFaceBoxes)
{
  // Create a simple box solid
  TopoDS_Shape aShape = BRepPrimAPI_MakeBox(1.0, 1.0, 1.0).Solid();

  // Create main bounding box for the shape
  Bnd_Box aMainBox;
  BRepBndLib::Add(aShape, aMainBox);

  // Initialize BoundSortBox with 6 boxes (for 6 faces of the cube)
  const Standard_Integer aMaxNbrBox = 6;
  Bnd_BoundSortBox       aBoundSortBox;
  aBoundSortBox.Initialize(aMainBox, aMaxNbrBox);

  // Iterate through faces and add their bounding boxes
  TopExp_Explorer  aExplorer(aShape, TopAbs_FACE);
  Standard_Integer i;

  for (i = 1, aExplorer.ReInit(); aExplorer.More(); aExplorer.Next(), i++)
  {
    const TopoDS_Shape& aFace = aExplorer.Current();
    Bnd_Box             aBox;
    BRepBndLib::Add(aFace, aBox);
    aBoundSortBox.Add(aBox, i);
  }

  // Verify that 6 faces were processed
  EXPECT_EQ(7, i) << "Expected to process 6 faces (i should be 7 after loop)";

  // The test passes if no exceptions occurred during initialization and addition
  // This test originally verified that the Bnd_BoundSortBox could handle
  // initialization and addition of multiple boxes without crashing
}
