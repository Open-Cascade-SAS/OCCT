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

#include <Bnd_Box.hxx>
#include <gp_Pnt.hxx>
#include <gp_Dir.hxx>
#include <gp_Pln.hxx>
#include <gp_Lin.hxx>
#include <gp_Trsf.hxx>
#include <Standard_Dump.hxx>

#include <cmath>
#include <limits>

//==================================================================================================
// Constructor Tests
//==================================================================================================

TEST(Bnd_BoxTest, DefaultConstructor)
{
  Bnd_Box aBox;

  EXPECT_TRUE(aBox.IsVoid()) << "Default constructed box should be void";
  EXPECT_FALSE(aBox.IsWhole()) << "Default constructed box should not be whole";
  EXPECT_DOUBLE_EQ(0.0, aBox.GetGap()) << "Default gap should be 0.0";
}

TEST(Bnd_BoxTest, PointConstructor)
{
  gp_Pnt aMinPnt(1.0, 2.0, 3.0);
  gp_Pnt aMaxPnt(4.0, 5.0, 6.0);

  Bnd_Box aBox(aMinPnt, aMaxPnt);

  EXPECT_FALSE(aBox.IsVoid()) << "Box constructed with points should not be void";
  EXPECT_FALSE(aBox.IsWhole()) << "Box constructed with points should not be whole";

  Standard_Real aXmin, aYmin, aZmin, aXmax, aYmax, aZmax;
  aBox.Get(aXmin, aYmin, aZmin, aXmax, aYmax, aZmax);

  EXPECT_DOUBLE_EQ(1.0, aXmin) << "Xmin should match constructor input";
  EXPECT_DOUBLE_EQ(2.0, aYmin) << "Ymin should match constructor input";
  EXPECT_DOUBLE_EQ(3.0, aZmin) << "Zmin should match constructor input";
  EXPECT_DOUBLE_EQ(4.0, aXmax) << "Xmax should match constructor input";
  EXPECT_DOUBLE_EQ(5.0, aYmax) << "Ymax should match constructor input";
  EXPECT_DOUBLE_EQ(6.0, aZmax) << "Zmax should match constructor input";
}

//==================================================================================================
// Set and Update Tests
//==================================================================================================

TEST(Bnd_BoxTest, SetWithPoint)
{
  Bnd_Box aBox;
  gp_Pnt  aPnt(1.5, 2.5, 3.5);

  aBox.Set(aPnt);

  EXPECT_FALSE(aBox.IsVoid()) << "Box should not be void after Set";

  Standard_Real aXmin, aYmin, aZmin, aXmax, aYmax, aZmax;
  aBox.Get(aXmin, aYmin, aZmin, aXmax, aYmax, aZmax);

  EXPECT_DOUBLE_EQ(1.5, aXmin) << "Point coordinates should set both min and max";
  EXPECT_DOUBLE_EQ(1.5, aXmax) << "Point coordinates should set both min and max";
  EXPECT_DOUBLE_EQ(2.5, aYmin) << "Point coordinates should set both min and max";
  EXPECT_DOUBLE_EQ(2.5, aYmax) << "Point coordinates should set both min and max";
  EXPECT_DOUBLE_EQ(3.5, aZmin) << "Point coordinates should set both min and max";
  EXPECT_DOUBLE_EQ(3.5, aZmax) << "Point coordinates should set both min and max";
}

TEST(Bnd_BoxTest, SetWithPointAndDirection)
{
  Bnd_Box aBox;
  gp_Pnt  aPnt(1.0, 2.0, 3.0);
  gp_Dir  aDir(gp_Dir::D::X);

  aBox.Set(aPnt, aDir);

  EXPECT_FALSE(aBox.IsVoid()) << "Box should not be void after Set";
  EXPECT_TRUE(aBox.IsOpenXmax()) << "Box should be open in positive X direction";
  EXPECT_FALSE(aBox.IsOpenXmin()) << "Box should not be open in negative X direction";
}

TEST(Bnd_BoxTest, UpdateWithBounds)
{
  Bnd_Box aBox;

  aBox.Update(1.0, 2.0, 3.0, 4.0, 5.0, 6.0);

  EXPECT_FALSE(aBox.IsVoid()) << "Box should not be void after Update";

  Standard_Real aXmin, aYmin, aZmin, aXmax, aYmax, aZmax;
  aBox.Get(aXmin, aYmin, aZmin, aXmax, aYmax, aZmax);

  EXPECT_DOUBLE_EQ(1.0, aXmin) << "Update should set correct bounds";
  EXPECT_DOUBLE_EQ(2.0, aYmin) << "Update should set correct bounds";
  EXPECT_DOUBLE_EQ(3.0, aZmin) << "Update should set correct bounds";
  EXPECT_DOUBLE_EQ(4.0, aXmax) << "Update should set correct bounds";
  EXPECT_DOUBLE_EQ(5.0, aYmax) << "Update should set correct bounds";
  EXPECT_DOUBLE_EQ(6.0, aZmax) << "Update should set correct bounds";
}

TEST(Bnd_BoxTest, UpdateWithPoint)
{
  Bnd_Box aBox;

  // First point establishes the box
  aBox.Update(1.0, 2.0, 3.0);

  Standard_Real aXmin, aYmin, aZmin, aXmax, aYmax, aZmax;
  aBox.Get(aXmin, aYmin, aZmin, aXmax, aYmax, aZmax);

  EXPECT_DOUBLE_EQ(1.0, aXmin) << "First point should set both min and max";
  EXPECT_DOUBLE_EQ(1.0, aXmax) << "First point should set both min and max";

  // Second point expands the box
  aBox.Update(0.5, 2.5, 3.5);
  aBox.Get(aXmin, aYmin, aZmin, aXmax, aYmax, aZmax);

  EXPECT_DOUBLE_EQ(0.5, aXmin) << "Box should expand to include new point";
  EXPECT_DOUBLE_EQ(1.0, aXmax) << "Box should maintain previous max";
  EXPECT_DOUBLE_EQ(2.0, aYmin) << "Box should maintain previous min";
  EXPECT_DOUBLE_EQ(2.5, aYmax) << "Box should expand to include new point";
}

TEST(Bnd_BoxTest, UpdateExpansion)
{
  Bnd_Box aBox;
  aBox.Update(0.0, 0.0, 0.0, 1.0, 1.0, 1.0);

  // Update with bounds that expand the box
  aBox.Update(-1.0, -1.0, -1.0, 2.0, 2.0, 2.0);

  Standard_Real aXmin, aYmin, aZmin, aXmax, aYmax, aZmax;
  aBox.Get(aXmin, aYmin, aZmin, aXmax, aYmax, aZmax);

  EXPECT_DOUBLE_EQ(-1.0, aXmin) << "Box should expand to include new bounds";
  EXPECT_DOUBLE_EQ(-1.0, aYmin) << "Box should expand to include new bounds";
  EXPECT_DOUBLE_EQ(-1.0, aZmin) << "Box should expand to include new bounds";
  EXPECT_DOUBLE_EQ(2.0, aXmax) << "Box should expand to include new bounds";
  EXPECT_DOUBLE_EQ(2.0, aYmax) << "Box should expand to include new bounds";
  EXPECT_DOUBLE_EQ(2.0, aZmax) << "Box should expand to include new bounds";
}

//==================================================================================================
// Gap and Tolerance Tests
//==================================================================================================

TEST(Bnd_BoxTest, GapOperations)
{
  Bnd_Box aBox;

  EXPECT_DOUBLE_EQ(0.0, aBox.GetGap()) << "Initial gap should be 0";

  aBox.SetGap(0.5);
  EXPECT_DOUBLE_EQ(0.5, aBox.GetGap()) << "SetGap should update the gap value";

  aBox.Enlarge(0.3);
  EXPECT_DOUBLE_EQ(0.5, aBox.GetGap()) << "Enlarge with smaller value should not change gap";

  aBox.Enlarge(0.8);
  EXPECT_DOUBLE_EQ(0.8, aBox.GetGap()) << "Enlarge with larger value should update gap";

  aBox.Enlarge(-1.0);
  EXPECT_DOUBLE_EQ(1.0, aBox.GetGap()) << "Enlarge should use absolute value";
}

//==================================================================================================
// Corner Methods Tests
//==================================================================================================

TEST(Bnd_BoxTest, CornerMethods)
{
  Bnd_Box aBox;
  aBox.Update(1.0, 2.0, 3.0, 4.0, 5.0, 6.0);
  aBox.SetGap(0.1);

  gp_Pnt aCornerMin = aBox.CornerMin();
  gp_Pnt aCornerMax = aBox.CornerMax();

  EXPECT_DOUBLE_EQ(0.9, aCornerMin.X()) << "CornerMin should account for gap";
  EXPECT_DOUBLE_EQ(1.9, aCornerMin.Y()) << "CornerMin should account for gap";
  EXPECT_DOUBLE_EQ(2.9, aCornerMin.Z()) << "CornerMin should account for gap";

  EXPECT_DOUBLE_EQ(4.1, aCornerMax.X()) << "CornerMax should account for gap";
  EXPECT_DOUBLE_EQ(5.1, aCornerMax.Y()) << "CornerMax should account for gap";
  EXPECT_DOUBLE_EQ(6.1, aCornerMax.Z()) << "CornerMax should account for gap";
}

TEST(Bnd_BoxTest, CornerMethodsVoidBox)
{
  Bnd_Box aBox; // void box

  EXPECT_THROW((void)aBox.CornerMin(), Standard_ConstructionError)
    << "CornerMin should throw for void box";
  EXPECT_THROW((void)aBox.CornerMax(), Standard_ConstructionError)
    << "CornerMax should throw for void box";
}

TEST(Bnd_BoxTest, CornerMethodsOpenBox)
{
  Bnd_Box aBox;
  aBox.Update(1.0, 2.0, 3.0, 4.0, 5.0, 6.0);
  aBox.OpenXmax();
  aBox.OpenYmin();

  gp_Pnt aCornerMin = aBox.CornerMin();
  gp_Pnt aCornerMax = aBox.CornerMax();

  EXPECT_DOUBLE_EQ(-1e100, aCornerMin.Y()) << "Open Ymin should return -infinite";
  EXPECT_DOUBLE_EQ(1e100, aCornerMax.X()) << "Open Xmax should return +infinite";
}

//==================================================================================================
// Thin Methods Tests
//==================================================================================================

TEST(Bnd_BoxTest, ThinnessMethods)
{
  Bnd_Box aBox;

  EXPECT_TRUE(aBox.IsXThin(1.0)) << "Void box should be considered thin";
  EXPECT_TRUE(aBox.IsYThin(1.0)) << "Void box should be considered thin";
  EXPECT_TRUE(aBox.IsZThin(1.0)) << "Void box should be considered thin";
  EXPECT_TRUE(aBox.IsThin(1.0)) << "Void box should be considered thin";

  // Create a thin box in X direction
  aBox.Update(1.0, 0.0, 0.0, 1.01, 2.0, 2.0);

  EXPECT_TRUE(aBox.IsXThin(0.1)) << "Thin X dimension should be detected";
  EXPECT_FALSE(aBox.IsYThin(0.1)) << "Thick Y dimension should not be thin";
  EXPECT_FALSE(aBox.IsZThin(0.1)) << "Thick Z dimension should not be thin";
  EXPECT_FALSE(aBox.IsThin(0.1)) << "Box with only one thin dimension should not be overall thin";

  // Create a box that is thin in all dimensions
  Bnd_Box aThinBox;
  aThinBox.Update(0.0, 0.0, 0.0, 0.01, 0.01, 0.01);

  EXPECT_TRUE(aThinBox.IsXThin(0.1)) << "All dimensions should be thin";
  EXPECT_TRUE(aThinBox.IsYThin(0.1)) << "All dimensions should be thin";
  EXPECT_TRUE(aThinBox.IsZThin(0.1)) << "All dimensions should be thin";
  EXPECT_TRUE(aThinBox.IsThin(0.1)) << "Box thin in all dimensions should be overall thin";
}

TEST(Bnd_BoxTest, ThinnessWithOpenBox)
{
  Bnd_Box aBox;
  aBox.Update(0.0, 0.0, 0.0, 1.0, 1.0, 1.0);
  aBox.OpenXmin();

  EXPECT_FALSE(aBox.IsXThin(0.1)) << "Open dimension should not be considered thin";

  aBox.SetWhole();
  EXPECT_FALSE(aBox.IsXThin(0.1)) << "Whole box should not be considered thin";
  EXPECT_FALSE(aBox.IsThin(0.1)) << "Whole box should not be considered thin";
}

//==================================================================================================
// Transformation Tests
//==================================================================================================

TEST(Bnd_BoxTest, TransformationIdentity)
{
  Bnd_Box aBox;
  aBox.Update(1.0, 2.0, 3.0, 4.0, 5.0, 6.0);
  aBox.SetGap(0.1);

  gp_Trsf anIdentity;
  Bnd_Box aTransformed = aBox.Transformed(anIdentity);

  Standard_Real aXmin1, aYmin1, aZmin1, aXmax1, aYmax1, aZmax1;
  Standard_Real aXmin2, aYmin2, aZmin2, aXmax2, aYmax2, aZmax2;

  aBox.Get(aXmin1, aYmin1, aZmin1, aXmax1, aYmax1, aZmax1);
  aTransformed.Get(aXmin2, aYmin2, aZmin2, aXmax2, aYmax2, aZmax2);

  EXPECT_DOUBLE_EQ(aXmin1, aXmin2) << "Identity transformation should preserve coordinates";
  EXPECT_DOUBLE_EQ(aYmin1, aYmin2) << "Identity transformation should preserve coordinates";
  EXPECT_DOUBLE_EQ(aZmin1, aZmin2) << "Identity transformation should preserve coordinates";
  EXPECT_DOUBLE_EQ(aXmax1, aXmax2) << "Identity transformation should preserve coordinates";
  EXPECT_DOUBLE_EQ(aYmax1, aYmax2) << "Identity transformation should preserve coordinates";
  EXPECT_DOUBLE_EQ(aZmax1, aZmax2) << "Identity transformation should preserve coordinates";
  EXPECT_DOUBLE_EQ(aBox.GetGap(), aTransformed.GetGap()) << "Gap should be preserved";
}

TEST(Bnd_BoxTest, TransformationTranslation)
{
  Bnd_Box aBox;
  aBox.Update(0.0, 0.0, 0.0, 1.0, 1.0, 1.0);

  gp_Trsf aTranslation;
  aTranslation.SetTranslation(gp_Vec(2.0, 3.0, 4.0));

  Bnd_Box aTransformed = aBox.Transformed(aTranslation);

  Standard_Real aXmin, aYmin, aZmin, aXmax, aYmax, aZmax;
  aTransformed.Get(aXmin, aYmin, aZmin, aXmax, aYmax, aZmax);

  EXPECT_DOUBLE_EQ(2.0, aXmin) << "Translation should shift coordinates";
  EXPECT_DOUBLE_EQ(3.0, aYmin) << "Translation should shift coordinates";
  EXPECT_DOUBLE_EQ(4.0, aZmin) << "Translation should shift coordinates";
  EXPECT_DOUBLE_EQ(3.0, aXmax) << "Translation should shift coordinates";
  EXPECT_DOUBLE_EQ(4.0, aYmax) << "Translation should shift coordinates";
  EXPECT_DOUBLE_EQ(5.0, aZmax) << "Translation should shift coordinates";
}

TEST(Bnd_BoxTest, TransformationVoidBox)
{
  Bnd_Box aVoidBox;
  gp_Trsf aRotation;
  aRotation.SetRotation(gp_Ax1(gp_Pnt(0, 0, 0), gp_Dir(gp_Dir::D::Z)), M_PI / 4);

  Bnd_Box aTransformed = aVoidBox.Transformed(aRotation);

  EXPECT_TRUE(aTransformed.IsVoid()) << "Transformed void box should remain void";
}

//==================================================================================================
// Add Methods Tests
//==================================================================================================

TEST(Bnd_BoxTest, AddBox)
{
  Bnd_Box aBox1;
  aBox1.Update(0.0, 0.0, 0.0, 1.0, 1.0, 1.0);
  aBox1.SetGap(0.1);

  Bnd_Box aBox2;
  aBox2.Update(0.5, 0.5, 0.5, 2.0, 2.0, 2.0);
  aBox2.SetGap(0.2);

  aBox1.Add(aBox2);

  Standard_Real aXmin, aYmin, aZmin, aXmax, aYmax, aZmax;
  aBox1.Get(aXmin, aYmin, aZmin, aXmax, aYmax, aZmax);

  // Get() returns bounds including gap, so expect gap-adjusted values
  EXPECT_DOUBLE_EQ(-0.2, aXmin) << "Add should expand to union of boxes (with gap)";
  EXPECT_DOUBLE_EQ(-0.2, aYmin) << "Add should expand to union of boxes (with gap)";
  EXPECT_DOUBLE_EQ(-0.2, aZmin) << "Add should expand to union of boxes (with gap)";
  EXPECT_DOUBLE_EQ(2.2, aXmax) << "Add should expand to union of boxes (with gap)";
  EXPECT_DOUBLE_EQ(2.2, aYmax) << "Add should expand to union of boxes (with gap)";
  EXPECT_DOUBLE_EQ(2.2, aZmax) << "Add should expand to union of boxes (with gap)";
  EXPECT_DOUBLE_EQ(0.2, aBox1.GetGap()) << "Gap should be maximum of both boxes";
}

TEST(Bnd_BoxTest, AddVoidBox)
{
  Bnd_Box aBox;
  aBox.Update(1.0, 1.0, 1.0, 2.0, 2.0, 2.0);

  Bnd_Box aVoidBox;
  aBox.Add(aVoidBox);

  Standard_Real aXmin, aYmin, aZmin, aXmax, aYmax, aZmax;
  aBox.Get(aXmin, aYmin, aZmin, aXmax, aYmax, aZmax);

  EXPECT_DOUBLE_EQ(1.0, aXmin) << "Adding void box should not change original";
  EXPECT_DOUBLE_EQ(2.0, aXmax) << "Adding void box should not change original";
}

TEST(Bnd_BoxTest, AddToVoidBox)
{
  Bnd_Box aVoidBox;
  Bnd_Box aBox;
  aBox.Update(1.0, 1.0, 1.0, 2.0, 2.0, 2.0);

  aVoidBox.Add(aBox);

  EXPECT_FALSE(aVoidBox.IsVoid()) << "Adding to void box should make it non-void";

  Standard_Real aXmin, aYmin, aZmin, aXmax, aYmax, aZmax;
  aVoidBox.Get(aXmin, aYmin, aZmin, aXmax, aYmax, aZmax);

  EXPECT_DOUBLE_EQ(1.0, aXmin) << "Should adopt added box's bounds";
  EXPECT_DOUBLE_EQ(2.0, aXmax) << "Should adopt added box's bounds";
}

TEST(Bnd_BoxTest, AddPoint)
{
  Bnd_Box aBox;
  aBox.Update(0.0, 0.0, 0.0, 1.0, 1.0, 1.0);

  gp_Pnt aPnt(2.0, -1.0, 0.5);
  aBox.Add(aPnt);

  Standard_Real aXmin, aYmin, aZmin, aXmax, aYmax, aZmax;
  aBox.Get(aXmin, aYmin, aZmin, aXmax, aYmax, aZmax);

  EXPECT_DOUBLE_EQ(0.0, aXmin) << "Box should expand to include point";
  EXPECT_DOUBLE_EQ(-1.0, aYmin) << "Box should expand to include point";
  EXPECT_DOUBLE_EQ(0.0, aZmin) << "Box should maintain existing bounds";
  EXPECT_DOUBLE_EQ(2.0, aXmax) << "Box should expand to include point";
  EXPECT_DOUBLE_EQ(1.0, aYmax) << "Box should maintain existing bounds";
  EXPECT_DOUBLE_EQ(1.0, aZmax) << "Box should maintain existing bounds";
}

TEST(Bnd_BoxTest, AddDirection)
{
  Bnd_Box aBox;
  aBox.Update(0.0, 0.0, 0.0, 1.0, 1.0, 1.0);

  gp_Dir aDir(gp_Dir::D::X);
  aBox.Add(aDir);

  EXPECT_TRUE(aBox.IsOpenXmax()) << "Positive X direction should open Xmax";
  EXPECT_FALSE(aBox.IsOpenXmin()) << "Positive X direction should not open Xmin";
  EXPECT_FALSE(aBox.IsOpenYmax()) << "X direction should not affect Y";
  EXPECT_FALSE(aBox.IsOpenYmin()) << "X direction should not affect Y";
}

TEST(Bnd_BoxTest, AddPointWithDirection)
{
  Bnd_Box aBox;
  gp_Pnt  aPnt(1.0, 2.0, 3.0);
  gp_Dir  aDir(gp_Dir::D::NX);

  aBox.Add(aPnt, aDir);

  EXPECT_FALSE(aBox.IsVoid()) << "Box should not be void after adding point";
  EXPECT_TRUE(aBox.IsOpenXmin()) << "Negative X direction should open Xmin";
  EXPECT_FALSE(aBox.IsOpenXmax()) << "Negative X direction should not open Xmax";
}

//==================================================================================================
// IsOut Methods Tests
//==================================================================================================

TEST(Bnd_BoxTest, IsOutPoint)
{
  Bnd_Box aBox;
  aBox.Update(0.0, 0.0, 0.0, 2.0, 2.0, 2.0);

  gp_Pnt anInsidePnt(1.0, 1.0, 1.0);
  gp_Pnt anOutsidePnt(3.0, 1.0, 1.0);
  gp_Pnt aBoundaryPnt(2.0, 2.0, 2.0);

  EXPECT_FALSE(aBox.IsOut(anInsidePnt)) << "Point inside should not be out";
  EXPECT_TRUE(aBox.IsOut(anOutsidePnt)) << "Point outside should be out";
  EXPECT_FALSE(aBox.IsOut(aBoundaryPnt)) << "Point on boundary should not be out";
}

TEST(Bnd_BoxTest, IsOutPointWithGap)
{
  Bnd_Box aBox;
  aBox.Update(0.0, 0.0, 0.0, 2.0, 2.0, 2.0);
  aBox.SetGap(0.1);

  gp_Pnt aNearBoundaryPnt(2.05, 1.0, 1.0);
  gp_Pnt aFarBoundaryPnt(2.15, 1.0, 1.0);

  EXPECT_FALSE(aBox.IsOut(aNearBoundaryPnt)) << "Point within gap should not be out";
  EXPECT_TRUE(aBox.IsOut(aFarBoundaryPnt)) << "Point beyond gap should be out";
}

TEST(Bnd_BoxTest, IsOutVoidBox)
{
  Bnd_Box aVoidBox;
  gp_Pnt  anAnyPnt(1.0, 2.0, 3.0);

  EXPECT_TRUE(aVoidBox.IsOut(anAnyPnt)) << "Void box should exclude all points";
}

TEST(Bnd_BoxTest, IsOutWholeBox)
{
  Bnd_Box aWholeBox;
  aWholeBox.SetWhole();

  gp_Pnt anAnyPnt(1000.0, 2000.0, 3000.0);

  EXPECT_FALSE(aWholeBox.IsOut(anAnyPnt)) << "Whole box should include all points";
}

TEST(Bnd_BoxTest, IsOutBox)
{
  Bnd_Box aBox1;
  aBox1.Update(0.0, 0.0, 0.0, 2.0, 2.0, 2.0);

  Bnd_Box anOverlappingBox;
  anOverlappingBox.Update(1.0, 1.0, 1.0, 3.0, 3.0, 3.0);

  Bnd_Box aSeparateBox;
  aSeparateBox.Update(3.0, 3.0, 3.0, 4.0, 4.0, 4.0);

  Bnd_Box aTouchingBox;
  aTouchingBox.Update(2.0, 2.0, 2.0, 3.0, 3.0, 3.0);

  EXPECT_FALSE(aBox1.IsOut(anOverlappingBox)) << "Overlapping boxes should not be out";
  EXPECT_TRUE(aBox1.IsOut(aSeparateBox)) << "Separate boxes should be out";
  EXPECT_FALSE(aBox1.IsOut(aTouchingBox)) << "Touching boxes should not be out";
}

TEST(Bnd_BoxTest, IsOutPlane)
{
  Bnd_Box aBox;
  aBox.Update(-1.0, -1.0, -1.0, 1.0, 1.0, 1.0);

  // Plane that intersects the box
  gp_Pln anIntersectingPln(gp_Pnt(0, 0, 0), gp_Dir(gp_Dir::D::X));

  // Plane that doesn't intersect the box
  gp_Pln aSeparatePln(gp_Pnt(2, 0, 0), gp_Dir(gp_Dir::D::X));

  EXPECT_FALSE(aBox.IsOut(anIntersectingPln)) << "Intersecting plane should not be out";
  EXPECT_TRUE(aBox.IsOut(aSeparatePln)) << "Separate plane should be out";
}

TEST(Bnd_BoxTest, IsOutLine)
{
  Bnd_Box aBox;
  aBox.Update(0.0, 0.0, 0.0, 2.0, 2.0, 2.0);

  // Line that passes through the box
  gp_Lin anIntersectingLin(gp_Pnt(-1, 1, 1), gp_Dir(gp_Dir::D::X));

  // Line that misses the box
  gp_Lin aSeparateLin(gp_Pnt(-1, 3, 1), gp_Dir(gp_Dir::D::X));

  EXPECT_FALSE(aBox.IsOut(anIntersectingLin)) << "Intersecting line should not be out";
  EXPECT_TRUE(aBox.IsOut(aSeparateLin)) << "Separate line should be out";
}

//==================================================================================================
// Distance Method Tests
//==================================================================================================

TEST(Bnd_BoxTest, Distance)
{
  Bnd_Box aBox1;
  aBox1.Update(0.0, 0.0, 0.0, 1.0, 1.0, 1.0);

  Bnd_Box aBox2;
  aBox2.Update(2.0, 0.0, 0.0, 3.0, 1.0, 1.0); // 1 unit away in X direction

  Standard_Real aDistance = aBox1.Distance(aBox2);
  EXPECT_DOUBLE_EQ(1.0, aDistance) << "Distance between separated boxes should be 1.0";

  // Overlapping boxes
  Bnd_Box anOverlappingBox;
  anOverlappingBox.Update(0.5, 0.5, 0.5, 1.5, 1.5, 1.5);

  Standard_Real anOverlapDistance = aBox1.Distance(anOverlappingBox);
  EXPECT_DOUBLE_EQ(0.0, anOverlapDistance) << "Distance between overlapping boxes should be 0.0";
}

//==================================================================================================
// Open/Close Methods Tests
//==================================================================================================

TEST(Bnd_BoxTest, OpenCloseMethods)
{
  Bnd_Box aBox;
  aBox.Update(0.0, 0.0, 0.0, 1.0, 1.0, 1.0);

  EXPECT_FALSE(aBox.IsOpen()) << "Initially box should not be open";

  aBox.OpenXmin();
  EXPECT_TRUE(aBox.IsOpenXmin()) << "OpenXmin should make Xmin open";
  EXPECT_TRUE(aBox.IsOpen()) << "Box with any open side should be considered open";

  aBox.OpenXmax();
  aBox.OpenYmin();
  aBox.OpenYmax();
  aBox.OpenZmin();
  aBox.OpenZmax();

  EXPECT_TRUE(aBox.IsOpenXmax()) << "All directions should be openable";
  EXPECT_TRUE(aBox.IsOpenYmin()) << "All directions should be openable";
  EXPECT_TRUE(aBox.IsOpenYmax()) << "All directions should be openable";
  EXPECT_TRUE(aBox.IsOpenZmin()) << "All directions should be openable";
  EXPECT_TRUE(aBox.IsOpenZmax()) << "All directions should be openable";
  EXPECT_TRUE(aBox.IsWhole()) << "Box open in all directions should be whole";
}

//==================================================================================================
// Void/Whole State Tests
//==================================================================================================

TEST(Bnd_BoxTest, VoidWholeStates)
{
  Bnd_Box aBox;

  EXPECT_TRUE(aBox.IsVoid()) << "Default box should be void";
  EXPECT_FALSE(aBox.IsWhole()) << "Default box should not be whole";

  aBox.SetWhole();
  EXPECT_FALSE(aBox.IsVoid()) << "Whole box should not be void";
  EXPECT_TRUE(aBox.IsWhole()) << "SetWhole should make box whole";

  aBox.SetVoid();
  EXPECT_TRUE(aBox.IsVoid()) << "SetVoid should make box void";
  EXPECT_FALSE(aBox.IsWhole()) << "Void box should not be whole";

  aBox.Update(0.0, 0.0, 0.0, 1.0, 1.0, 1.0);
  EXPECT_FALSE(aBox.IsVoid()) << "Box with bounds should not be void";
  EXPECT_FALSE(aBox.IsWhole()) << "Bounded box should not be whole";

  EXPECT_TRUE(aBox.HasFinitePart()) << "Bounded box should have finite part";
}

//==================================================================================================
// Edge Cases and Error Conditions
//==================================================================================================

TEST(Bnd_BoxTest, EdgeCases)
{
  Bnd_Box aBox;

  // Test with very large numbers
  const Standard_Real aLargeValue = 1e10;
  aBox.Update(-aLargeValue, -aLargeValue, -aLargeValue, aLargeValue, aLargeValue, aLargeValue);

  EXPECT_FALSE(aBox.IsVoid()) << "Box with large values should be valid";

  // Test with very small differences
  Bnd_Box aSmallBox;
  aSmallBox.Update(0.0, 0.0, 0.0, 1e-10, 1e-10, 1e-10);

  EXPECT_FALSE(aSmallBox.IsVoid()) << "Box with tiny dimensions should be valid";
  EXPECT_TRUE(aSmallBox.IsXThin(1e-9)) << "Very small box should be considered thin";
}

TEST(Bnd_BoxTest, TransformationWithOpenBox)
{
  Bnd_Box aBox;
  aBox.Update(0.0, 0.0, 0.0, 1.0, 1.0, 1.0);
  aBox.OpenXmax();

  gp_Trsf aRotation;
  aRotation.SetRotation(gp_Ax1(gp_Pnt(0, 0, 0), gp_Dir(gp_Dir::D::Z)), M_PI / 2);

  Bnd_Box aTransformed = aBox.Transformed(aRotation);

  EXPECT_TRUE(aTransformed.IsOpen()) << "Transformed open box should remain open";
  EXPECT_FALSE(aTransformed.IsVoid()) << "Transformed open box should have finite part";
}

//==================================================================================================
// Additional Coverage Tests
//==================================================================================================

TEST(Bnd_BoxTest, SquareExtent)
{
  Bnd_Box aBox;
  aBox.Update(0.0, 0.0, 0.0, 2.0, 3.0, 4.0);

  // SquareExtent appears to calculate the square of the diagonal length
  // For a box (0,0,0) to (2,3,4), diagonal = sqrt(2^2+3^2+4^2) = sqrt(29)
  // So SquareExtent = 29
  EXPECT_DOUBLE_EQ(29.0, aBox.SquareExtent())
    << "Square extent should be square of diagonal length";
}

TEST(Bnd_BoxTest, SetVoidAndWholeTransitions)
{
  Bnd_Box aBox;
  aBox.Update(0.0, 0.0, 0.0, 1.0, 1.0, 1.0);

  EXPECT_FALSE(aBox.IsVoid()) << "Box with bounds should not be void";

  aBox.SetVoid();
  EXPECT_TRUE(aBox.IsVoid()) << "SetVoid should make box void";

  aBox.SetWhole();
  EXPECT_TRUE(aBox.IsWhole()) << "SetWhole should make box whole";
  EXPECT_FALSE(aBox.IsVoid()) << "Whole box should not be void";
}

TEST(Bnd_BoxTest, DirectOpenOperations)
{
  Bnd_Box aBox;
  aBox.Update(0.0, 0.0, 0.0, 1.0, 1.0, 1.0);

  // Test individual open operations
  aBox.OpenXmin();
  EXPECT_TRUE(aBox.IsOpenXmin()) << "OpenXmin should work";

  aBox.OpenXmax();
  EXPECT_TRUE(aBox.IsOpenXmax()) << "OpenXmax should work";

  aBox.OpenYmin();
  EXPECT_TRUE(aBox.IsOpenYmin()) << "OpenYmin should work";

  aBox.OpenYmax();
  EXPECT_TRUE(aBox.IsOpenYmax()) << "OpenYmax should work";

  aBox.OpenZmin();
  EXPECT_TRUE(aBox.IsOpenZmin()) << "OpenZmin should work";

  aBox.OpenZmax();
  EXPECT_TRUE(aBox.IsOpenZmax()) << "OpenZmax should work";
}

TEST(Bnd_BoxTest, IsOutWithTransformation)
{
  Bnd_Box aBox1;
  aBox1.Update(0.0, 0.0, 0.0, 1.0, 1.0, 1.0);

  Bnd_Box aBox2;
  aBox2.Update(2.0, 0.0, 0.0, 3.0, 1.0, 1.0);

  gp_Trsf aTransf;
  EXPECT_TRUE(aBox1.IsOut(aBox2, aTransf)) << "Separate boxes should be out";

  // Transform to overlap
  aTransf.SetTranslation(gp_Vec(-1.5, 0.0, 0.0));
  EXPECT_FALSE(aBox1.IsOut(aBox2, aTransf)) << "Overlapping transformed boxes should not be out";
}

TEST(Bnd_BoxTest, IsOutWithTwoTransformations)
{
  Bnd_Box aBox1;
  aBox1.Update(0.0, 0.0, 0.0, 1.0, 1.0, 1.0);

  Bnd_Box aBox2;
  aBox2.Update(0.0, 0.0, 0.0, 1.0, 1.0, 1.0);

  gp_Trsf aT1, aT2;
  aT1.SetTranslation(gp_Vec(0.0, 0.0, 0.0));
  aT2.SetTranslation(gp_Vec(2.0, 0.0, 0.0));

  EXPECT_TRUE(aBox1.IsOut(aT1, aBox2, aT2)) << "Separated transformed boxes should be out";
}

TEST(Bnd_BoxTest, IsOutLineSegment)
{
  Bnd_Box aBox;
  aBox.Update(0.0, 0.0, 0.0, 2.0, 2.0, 2.0);

  // Line segment that passes through box
  gp_Pnt aP1(-1.0, 1.0, 1.0);
  gp_Pnt aP2(3.0, 1.0, 1.0);
  gp_Dir aDir(gp_Dir::D::X);

  EXPECT_FALSE(aBox.IsOut(aP1, aP2, aDir)) << "Line segment through box should not be out";

  // Line segment that misses box
  gp_Pnt aP3(-1.0, 3.0, 1.0);
  gp_Pnt aP4(3.0, 3.0, 1.0);

  EXPECT_TRUE(aBox.IsOut(aP3, aP4, aDir)) << "Line segment missing box should be out";
}

TEST(Bnd_BoxTest, UpdateExpandsCorrectly)
{
  Bnd_Box aBox;

  // Start with a point
  aBox.Update(1.0, 1.0, 1.0);

  Standard_Real aXmin, aYmin, aZmin, aXmax, aYmax, aZmax;
  aBox.Get(aXmin, aYmin, aZmin, aXmax, aYmax, aZmax);

  EXPECT_DOUBLE_EQ(1.0, aXmin) << "Point should create degenerate box";
  EXPECT_DOUBLE_EQ(1.0, aXmax) << "Point should create degenerate box";

  // Add another point that expands in all directions
  aBox.Update(-0.5, 2.5, 0.5);
  aBox.Get(aXmin, aYmin, aZmin, aXmax, aYmax, aZmax);

  EXPECT_DOUBLE_EQ(-0.5, aXmin) << "Box should expand to include new point";
  EXPECT_DOUBLE_EQ(1.0, aXmax) << "Box should maintain previous bounds";
  EXPECT_DOUBLE_EQ(1.0, aYmin) << "Box should maintain previous bounds";
  EXPECT_DOUBLE_EQ(2.5, aYmax) << "Box should expand to include new point";
}

TEST(Bnd_BoxTest, DumpJsonAndInitFromJson)
{
  // Create a bounding box with specific values
  Bnd_Box aBox(gp_Pnt(0, 0, 0), gp_Pnt(10, 5, 3));
  aBox.SetGap(0.1);

  // Serialize to JSON
  std::ostringstream anOStream;
  aBox.DumpJson(anOStream);
  std::string aJsonStr = anOStream.str();

  // Try to deserialize
  std::stringstream anIStream(aJsonStr);
  Bnd_Box           aDeserializedBox;
  Standard_Integer  aStreamPos = 1;

  EXPECT_TRUE(aDeserializedBox.InitFromJson(anIStream, aStreamPos))
    << "Deserialization should succeed with proper JSON format";

  // Verify the deserialized box matches the original
  Standard_Real aXmin1, aYmin1, aZmin1, aXmax1, aYmax1, aZmax1;
  Standard_Real aXmin2, aYmin2, aZmin2, aXmax2, aYmax2, aZmax2;

  aBox.Get(aXmin1, aYmin1, aZmin1, aXmax1, aYmax1, aZmax1);
  aDeserializedBox.Get(aXmin2, aYmin2, aZmin2, aXmax2, aYmax2, aZmax2);

  EXPECT_DOUBLE_EQ(aXmin1, aXmin2) << "Deserialized box should match original";
  EXPECT_DOUBLE_EQ(aYmin1, aYmin2) << "Deserialized box should match original";
  EXPECT_DOUBLE_EQ(aZmin1, aZmin2) << "Deserialized box should match original";
  EXPECT_DOUBLE_EQ(aXmax1, aXmax2) << "Deserialized box should match original";
  EXPECT_DOUBLE_EQ(aYmax1, aYmax2) << "Deserialized box should match original";
  EXPECT_DOUBLE_EQ(aZmax1, aZmax2) << "Deserialized box should match original";
  EXPECT_DOUBLE_EQ(aBox.GetGap(), aDeserializedBox.GetGap())
    << "Deserialized gap should match original";
}

//==================================================================================================
// Regression Tests
//==================================================================================================

// Test OCC16485: Bnd_Box tolerance issue with cumulative enlargement
// Migrated from QABugs_14.cxx
TEST(Bnd_BoxTest, OCC16485_CumulativeEnlargeTolerance)
{
  // Create points with X coordinate varying from 0 to 1000
  // and compute cumulative bounding box by adding boxes for all the
  // points, enlarged on tolerance
  const Standard_Real    aTol    = 1e-3;
  const Standard_Integer aNbStep = 1000;
  Bnd_Box                aBox;

  for (Standard_Integer i = 0; i <= aNbStep; i++)
  {
    gp_Pnt  aP(i, 0., 0.);
    Bnd_Box aB;
    aB.Add(aP);
    aB.Enlarge(aTol);
    aB.Add(aBox);
    aBox = aB; // This was causing XMin to grow each time (regression)
  }

  Standard_Real aXmin, aYmin, aZmin, aXmax, aYmax, aZmax;
  aBox.Get(aXmin, aYmin, aZmin, aXmax, aYmax, aZmax);

  // Verify that Xmin is approximately -tolerance (not growing with iterations)
  EXPECT_NEAR(-aTol, aXmin, 1e-10) << "Xmin should be equal to -tolerance";
  EXPECT_NEAR(aNbStep + aTol, aXmax, 1e-10) << "Xmax should be equal to nbstep + tolerance";
}