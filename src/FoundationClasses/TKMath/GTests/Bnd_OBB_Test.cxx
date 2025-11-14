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
#include <TColgp_Array1OfPnt.hxx>

#include <gtest/gtest.h>

TEST(Bnd_OBB_Test, OCC33009_ReBuildWithPoints)
{
  Bnd_OBB aBndBox;

  TColgp_Array1OfPnt aPoints(1, 5);
  aPoints.ChangeValue(1) = gp_Pnt(1, 2, 3);
  aPoints.ChangeValue(2) = gp_Pnt(3, 2, 1);
  aPoints.ChangeValue(3) = gp_Pnt(2, 3, 1);
  aPoints.ChangeValue(4) = gp_Pnt(1, 3, 2);
  aPoints.ChangeValue(5) = gp_Pnt(2, 1, 3);

  // Should not throw exception when rebuilding with points
  EXPECT_NO_THROW(aBndBox.ReBuild(aPoints, (const TColStd_Array1OfReal*)0, true));
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
  BRepBndLib::AddOBB(aBox, aOBB, Standard_False, Standard_False, Standard_False);
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
