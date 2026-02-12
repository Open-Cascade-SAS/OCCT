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

#include <Bnd_OBB.hxx>
#include <BRepBndLib.hxx>
#include <BRepPrimAPI_MakeBox.hxx>
#include <gp_Pnt.hxx>
#include <NCollection_Array1.hxx>

#include <gtest/gtest.h>

TEST(Bnd_OBB_Test, OCC33009_ReBuildWithPoints)
{
  Bnd_OBB aBndBox;

  NCollection_Array1<gp_Pnt> aPoints(1, 5);
  aPoints.ChangeValue(1) = gp_Pnt(1, 2, 3);
  aPoints.ChangeValue(2) = gp_Pnt(3, 2, 1);
  aPoints.ChangeValue(3) = gp_Pnt(2, 3, 1);
  aPoints.ChangeValue(4) = gp_Pnt(1, 3, 2);
  aPoints.ChangeValue(5) = gp_Pnt(2, 1, 3);

  // Should not throw exception when rebuilding with points
  EXPECT_NO_THROW(aBndBox.ReBuild(aPoints, (const NCollection_Array1<double>*)nullptr, true));
}

TEST(Bnd_OBB_Test, OCC30704_AddBoundingBoxToVoidBox)
{
  // Bug OCC30704: Oriented bounding box (Bnd_OBB) gives a wrong result
  // if a box is added to a void box
  // This test verifies that adding a bounding box to a void bounding box
  // correctly sets the center of the void box

  // Make a shape somewhere far from (0, 0, 0)
  BRepPrimAPI_MakeBox aMkBox(gp_Pnt(100, 100, 100), 100, 100, 100);
  const TopoDS_Shape& aBox = aMkBox.Shape();

  // Add a bounding box of a shape to a void bounding box
  Bnd_OBB aVoidBox, aOBB;
  BRepBndLib::AddOBB(aBox, aOBB, false, false, false);
  aVoidBox.Add(aOBB);

  // Check the center point of the bounding box
  const gp_XYZ& aCenter = aVoidBox.Center();
  EXPECT_DOUBLE_EQ(aCenter.X(), 150.0);
  EXPECT_DOUBLE_EQ(aCenter.Y(), 150.0);
  EXPECT_DOUBLE_EQ(aCenter.Z(), 150.0);
}

TEST(Bnd_OBB_Test, OCC30704_AddPointToVoidBox)
{
  // Bug OCC30704: Oriented bounding box (Bnd_OBB) gives a wrong result
  // if a point is added to a void box
  // This test verifies that adding a point to a void bounding box
  // correctly sets the center of the void box to that point

  // A point
  gp_Pnt aP(100, 200, 300);

  // Add the point to a void bounding box
  Bnd_OBB aVoidBox;
  aVoidBox.Add(aP);

  // Check the center point of the bounding box
  const gp_XYZ& aCenter = aVoidBox.Center();
  EXPECT_DOUBLE_EQ(aCenter.X(), 100.0);
  EXPECT_DOUBLE_EQ(aCenter.Y(), 200.0);
  EXPECT_DOUBLE_EQ(aCenter.Z(), 300.0);
}

TEST(Bnd_OBB_Test, Contains_Point)
{
  Bnd_OBB anOBB(gp_Pnt(0, 0, 0), gp_Dir(1, 0, 0), gp_Dir(0, 1, 0), gp_Dir(0, 0, 1), 5.0, 5.0, 5.0);
  EXPECT_TRUE(anOBB.Contains(gp_Pnt(0, 0, 0)));
  EXPECT_TRUE(anOBB.Contains(gp_Pnt(4, 4, 4)));
  EXPECT_FALSE(anOBB.Contains(gp_Pnt(10, 0, 0)));
}

TEST(Bnd_OBB_Test, Intersects_OBB)
{
  Bnd_OBB anOBB1(gp_Pnt(0, 0, 0), gp_Dir(1, 0, 0), gp_Dir(0, 1, 0), gp_Dir(0, 0, 1), 5.0, 5.0, 5.0);
  Bnd_OBB anOBB2(gp_Pnt(8, 0, 0), gp_Dir(1, 0, 0), gp_Dir(0, 1, 0), gp_Dir(0, 0, 1), 5.0, 5.0, 5.0);
  Bnd_OBB
    anOBB3(gp_Pnt(20, 0, 0), gp_Dir(1, 0, 0), gp_Dir(0, 1, 0), gp_Dir(0, 0, 1), 5.0, 5.0, 5.0);
  EXPECT_TRUE(anOBB1.Intersects(anOBB2));
  EXPECT_FALSE(anOBB1.Intersects(anOBB3));
}

TEST(Bnd_OBB_Test, GetHalfSizes_StructuredBindings)
{
  Bnd_OBB anOBB(gp_Pnt(0, 0, 0), gp_Dir(1, 0, 0), gp_Dir(0, 1, 0), gp_Dir(0, 0, 1), 3.0, 5.0, 7.0);
  const auto [aHX, aHY, aHZ] = anOBB.GetHalfSizes();
  EXPECT_DOUBLE_EQ(aHX, 3.0);
  EXPECT_DOUBLE_EQ(aHY, 5.0);
  EXPECT_DOUBLE_EQ(aHZ, 7.0);
}
