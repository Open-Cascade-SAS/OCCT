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

#include <BRepAlgoAPI_Fuse.hxx>
#include <BRepBuilderAPI_MakePolygon.hxx>
#include <BRepOffsetAPI_ThruSections.hxx>
#include <TopoDS_Shape.hxx>
#include <gp_Pnt.hxx>

// Test OCC10006: BRepOffsetAPI_ThruSections loft operation with Boolean fusion
TEST(BRepOffsetAPI_ThruSections_Test, OCC10006_LoftAndFusion)
{
  // Define bottom and top polygon coordinates for first loft
  double aBottomPoints1[12] = {10, -10, 0, 100, -10, 0, 100, -100, 0, 10, -100, 0};
  double aTopPoints1[12]    = {0, 0, 10, 100, 0, 10, 100, -100, 10, 0, -100, 10};

  // Define bottom and top polygon coordinates for second loft
  double aBottomPoints2[12] = {0, 0, 10.00, 100, 0, 10.00, 100, -100, 10.00, 0, -100, 10.00};
  double aTopPoints2[12]    = {0, 0, 250, 100, 0, 250, 100, -100, 250, 0, -100, 250};

  // Create polygons
  BRepBuilderAPI_MakePolygon aBottomPolygon1, aTopPolygon1, aBottomPolygon2, aTopPolygon2;
  gp_Pnt                     aTmpPnt;

  for (int i = 0; i < 4; i++)
  {
    aTmpPnt.SetCoord(aBottomPoints1[3 * i], aBottomPoints1[3 * i + 1], aBottomPoints1[3 * i + 2]);
    aBottomPolygon1.Add(aTmpPnt);

    aTmpPnt.SetCoord(aTopPoints1[3 * i], aTopPoints1[3 * i + 1], aTopPoints1[3 * i + 2]);
    aTopPolygon1.Add(aTmpPnt);

    aTmpPnt.SetCoord(aBottomPoints2[3 * i], aBottomPoints2[3 * i + 1], aBottomPoints2[3 * i + 2]);
    aBottomPolygon2.Add(aTmpPnt);

    aTmpPnt.SetCoord(aTopPoints2[3 * i], aTopPoints2[3 * i + 1], aTopPoints2[3 * i + 2]);
    aTopPolygon2.Add(aTmpPnt);
  }

  // Close polygons
  aBottomPolygon1.Close();
  aTopPolygon1.Close();
  aBottomPolygon2.Close();
  aTopPolygon2.Close();

  // Create first loft (ThruSections)
  BRepOffsetAPI_ThruSections aLoft1(Standard_True, Standard_True);
  aLoft1.AddWire(aBottomPolygon1.Wire());
  aLoft1.AddWire(aTopPolygon1.Wire());
  aLoft1.Build();

  // Create second loft (ThruSections)
  BRepOffsetAPI_ThruSections aLoft2(Standard_True, Standard_True);
  aLoft2.AddWire(aBottomPolygon2.Wire());
  aLoft2.AddWire(aTopPolygon2.Wire());
  aLoft2.Build();

  // Verify that loft operations succeeded
  EXPECT_FALSE(aLoft1.Shape().IsNull()) << "First loft operation should produce a valid shape";
  EXPECT_FALSE(aLoft2.Shape().IsNull()) << "Second loft operation should produce a valid shape";

  // Perform Boolean fusion of the two lofted shapes
  BRepAlgoAPI_Fuse aFusion(aLoft1.Shape(), aLoft2.Shape());

  // Verify that fusion operation succeeded
  EXPECT_FALSE(aFusion.Shape().IsNull()) << "Boolean fusion of lofted shapes should produce a valid shape";
}
