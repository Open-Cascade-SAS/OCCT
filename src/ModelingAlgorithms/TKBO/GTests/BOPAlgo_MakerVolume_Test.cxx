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

#include <BOPAlgo_MakerVolume.hxx>
#include <BRepBuilderAPI_MakeFace.hxx>
#include <BRepCheck_Analyzer.hxx>
#include <BRepGProp.hxx>
#include <GProp_GProps.hxx>
#include <NCollection_List.hxx>
#include <TopExp_Explorer.hxx>
#include <TopoDS_Face.hxx>
#include <TopoDS_Shape.hxx>
#include <gp_Cone.hxx>
#include <gp_Dir.hxx>
#include <gp_Pnt.hxx>
#include <gp_Pln.hxx>
#include <gp_Ax3.hxx>
#include <gtest/gtest.h>

TEST(BOPAlgo_MakerVolumeTest, ConePlaneProjectionPreservesVolumes)
{
  const gp_Dir aSide(0.70710678118654746, 4.4408920985006262e-016, 0.70710678118654768);
  const gp_Dir aDown(0, -1, 1.1102230246251565e-016);
  const gp_Dir anUp(0, 1, -1.1102230246251565e-016);
  const auto   aPlane = [](const gp_Pnt& thePoint, const gp_Dir& theNormal) {
    return BRepBuilderAPI_MakeFace(gp_Pln(thePoint, theNormal), -1e6, 1e6, -1e6, 1e6).Face();
  };
  const auto aCone = [](const gp_Pnt& thePoint, const gp_Dir& theNormal, double theDegrees) {
    return BRepBuilderAPI_MakeFace(
             gp_Cone(gp_Ax3(thePoint, theNormal), theDegrees * M_PI / 180.0, 0.0),
             0.0,
             2.0 * M_PI,
             0.0,
             1e6)
      .Face();
  };
  NCollection_List<TopoDS_Shape> aFaces;
  aFaces.Append(aPlane(gp_Pnt(27.577164466275352, -1038.2137499999999, 27.577164466275359), aSide));
  aFaces.Append(aCone(gp_Pnt(0, 518.47000000000003, 0), aDown, 14.999999999912038));
  aFaces.Append(
    aPlane(gp_Pnt(-27.577164466275352, -1038.2137499999999, -27.577164466275359), aSide));
  aFaces.Append(aPlane(gp_Pnt(0, -786.50743635718959, 3.8387493219403887e-014), aDown));
  aFaces.Append(aPlane(gp_Pnt(0, -796.42849000000001, 9.2386370775798815e-015), anUp));
  aFaces.Append(aCone(gp_Pnt(0, -440.74363604000001, 0), aDown, 45.110284878807235));

  BOPAlgo_MakerVolume aMaker;
  aMaker.SetArguments(aFaces);
  aMaker.Perform();
  ASSERT_FALSE(aMaker.HasErrors());
  ASSERT_FALSE(aMaker.Shape().IsNull());
  const TopoDS_Shape& aResult   = aMaker.Shape();
  int                 aNbSolids = 0;
  for (TopExp_Explorer aSolid(aResult, TopAbs_SOLID); aSolid.More(); aSolid.Next())
  {
    ++aNbSolids;
  }
  GProp_GProps aProps;
  BRepGProp::SurfaceProperties(aResult, aProps, 1e-4);
  EXPECT_EQ(aNbSolids, 17);
  EXPECT_NEAR(aProps.Mass(), 5576802.09207427, 1.0);
  EXPECT_TRUE(BRepCheck_Analyzer(aResult).IsValid());
}
