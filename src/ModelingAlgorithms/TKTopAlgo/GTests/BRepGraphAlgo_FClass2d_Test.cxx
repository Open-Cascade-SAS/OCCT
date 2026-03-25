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

#include <BRepAlgoAPI_Cut.hxx>
#include <BRepAlgoAPI_Fuse.hxx>
#include <BRepGraph.hxx>
#include <BRepGraph_DefsView.hxx>
#include <BRepGraphAlgo_FClass2d.hxx>
#include <BRepGraph_NodeId.hxx>
#include <BRepPrimAPI_MakeBox.hxx>
#include <BRepPrimAPI_MakeCylinder.hxx>
#include <BRepPrimAPI_MakeSphere.hxx>
#include <BRepTools.hxx>
#include <BRepTopAdaptor_FClass2d.hxx>
#include <Precision.hxx>
#include <TopExp_Explorer.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Face.hxx>

#include <gtest/gtest.h>

TEST(BRepGraphAlgo_FClass2dTest, PlanarBoxFace_CenterIsIN)
{
  const TopoDS_Shape aBox = BRepPrimAPI_MakeBox(10.0, 20.0, 30.0).Shape();

  BRepGraph aGraph;
  aGraph.Build(aBox);
  ASSERT_TRUE(aGraph.IsDone());
  ASSERT_GT(aGraph.Defs().NbFaces(), 0);

  // Pick the first face.
  TopExp_Explorer anExp(aBox, TopAbs_FACE);
  ASSERT_TRUE(anExp.More());
  const TopoDS_Face& aFace = TopoDS::Face(anExp.Current());

  // Get UV bounds of the face surface.
  double aUmin, aUmax, aVmin, aVmax;
  BRepTools::UVBounds(aFace, aUmin, aUmax, aVmin, aVmax);

  const double           aTolUV = 1.0e-6;
  BRepGraphAlgo_FClass2d aClassifier(aGraph, BRepGraph_FaceId(0), aTolUV);

  // Center of UV domain should be IN.
  const gp_Pnt2d aCenter(0.5 * (aUmin + aUmax), 0.5 * (aVmin + aVmax));
  EXPECT_EQ(aClassifier.Perform(aCenter), TopAbs_IN);
}

TEST(BRepGraphAlgo_FClass2dTest, PlanarBoxFace_OutsideIsOUT)
{
  const TopoDS_Shape aBox = BRepPrimAPI_MakeBox(10.0, 20.0, 30.0).Shape();

  BRepGraph aGraph;
  aGraph.Build(aBox);
  ASSERT_TRUE(aGraph.IsDone());

  TopExp_Explorer anExp(aBox, TopAbs_FACE);
  ASSERT_TRUE(anExp.More());
  const TopoDS_Face& aFace = TopoDS::Face(anExp.Current());

  double aUmin, aUmax, aVmin, aVmax;
  BRepTools::UVBounds(aFace, aUmin, aUmax, aVmin, aVmax);

  BRepGraphAlgo_FClass2d aClassifier(aGraph, BRepGraph_FaceId(0), 1.0e-6);

  // Point far outside the UV domain.
  const gp_Pnt2d aOutside(aUmax + 100.0, aVmax + 100.0);
  EXPECT_EQ(aClassifier.Perform(aOutside, false), TopAbs_OUT);
}

TEST(BRepGraphAlgo_FClass2dTest, CylindricalFace_CenterIsIN)
{
  const TopoDS_Shape aCyl = BRepPrimAPI_MakeCylinder(5.0, 20.0).Shape();

  BRepGraph aGraph;
  aGraph.Build(aCyl);
  ASSERT_TRUE(aGraph.IsDone());

  // Find the cylindrical (lateral) face.
  int aFaceIdx = -1;
  int aIdx     = 0;
  for (TopExp_Explorer anExp(aCyl, TopAbs_FACE); anExp.More(); anExp.Next(), aIdx++)
  {
    const TopoDS_Face& aFace = TopoDS::Face(anExp.Current());
    double             aUmin, aUmax, aVmin, aVmax;
    BRepTools::UVBounds(aFace, aUmin, aUmax, aVmin, aVmax);
    // Cylindrical face has U range close to [0, 2*PI].
    if (aUmax - aUmin > 5.0)
    {
      aFaceIdx = aIdx;
      break;
    }
  }
  ASSERT_GE(aFaceIdx, 0);

  BRepGraphAlgo_FClass2d aClassifier(aGraph, BRepGraph_FaceId(aFaceIdx), 1.0e-6);

  // Mid-height, mid-angle point should be IN.
  const gp_Pnt2d aMid(M_PI, 10.0);
  EXPECT_EQ(aClassifier.Perform(aMid), TopAbs_IN);
}

TEST(BRepGraphAlgo_FClass2dTest, Sphere_CenterIsIN)
{
  const TopoDS_Shape aSphere = BRepPrimAPI_MakeSphere(10.0).Shape();

  BRepGraph aGraph;
  aGraph.Build(aSphere);
  ASSERT_TRUE(aGraph.IsDone());

  // Find the spherical face (there is one face in a sphere).
  ASSERT_GE(aGraph.Defs().NbFaces(), 1);

  BRepGraphAlgo_FClass2d aClassifier(aGraph, BRepGraph_FaceId(0), 1.0e-6);

  // Mid-point on the spherical face parametric domain.
  const gp_Pnt2d aMid(M_PI, 0.0);
  EXPECT_EQ(aClassifier.Perform(aMid), TopAbs_IN);
}

TEST(BRepGraphAlgo_FClass2dTest, FaceWithHole_InsideHoleIsOUT)
{
  // Create a box with a cylindrical hole.
  const TopoDS_Shape aBox = BRepPrimAPI_MakeBox(20.0, 20.0, 5.0).Shape();
  const TopoDS_Shape aCyl =
    BRepPrimAPI_MakeCylinder(gp_Ax2(gp_Pnt(10.0, 10.0, -1.0), gp_Dir(0.0, 0.0, 1.0)), 3.0, 7.0)
      .Shape();
  BRepAlgoAPI_Cut aCutter(aBox, aCyl);
  ASSERT_TRUE(aCutter.IsDone());
  const TopoDS_Shape& aCutShape = aCutter.Shape();

  BRepGraph aGraph;
  aGraph.Build(aCutShape);
  ASSERT_TRUE(aGraph.IsDone());

  // Find a planar face with a hole (2 wires).
  int aFaceIdx = -1;
  int aIdx     = 0;
  for (TopExp_Explorer anExp(aCutShape, TopAbs_FACE); anExp.More(); anExp.Next(), aIdx++)
  {
    const TopoDS_Face& aFace    = TopoDS::Face(anExp.Current());
    int                aNbWires = 0;
    for (TopExp_Explorer aWireExp(aFace, TopAbs_WIRE); aWireExp.More(); aWireExp.Next())
      aNbWires++;
    if (aNbWires >= 2)
    {
      aFaceIdx = aIdx;
      break;
    }
  }

  if (aFaceIdx >= 0)
  {
    BRepGraphAlgo_FClass2d aClassifier(aGraph, BRepGraph_FaceId(aFaceIdx), 1.0e-6);

    // Center of the hole (approximately 10,10 in face UV space) should be OUT.
    const gp_Pnt2d aHoleCenter(10.0, 10.0);
    TopAbs_State   aState = aClassifier.Perform(aHoleCenter);
    EXPECT_EQ(aState, TopAbs_OUT);

    // Corner should be IN (outside the hole, inside the face).
    const gp_Pnt2d aCorner(1.0, 1.0);
    aState = aClassifier.Perform(aCorner);
    EXPECT_EQ(aState, TopAbs_IN);
  }
}

TEST(BRepGraphAlgo_FClass2dTest, PerformInfinitePoint_ReturnsOUT)
{
  const TopoDS_Shape aBox = BRepPrimAPI_MakeBox(10.0, 20.0, 30.0).Shape();

  BRepGraph aGraph;
  aGraph.Build(aBox);
  ASSERT_TRUE(aGraph.IsDone());

  BRepGraphAlgo_FClass2d aClassifier(aGraph, BRepGraph_FaceId(0), 1.0e-6);
  EXPECT_EQ(aClassifier.PerformInfinitePoint(), TopAbs_OUT);
}

TEST(BRepGraphAlgo_FClass2dTest, Consistency_VsLegacy_Box)
{
  const TopoDS_Shape aBox = BRepPrimAPI_MakeBox(10.0, 20.0, 30.0).Shape();

  BRepGraph aGraph;
  aGraph.Build(aBox);
  ASSERT_TRUE(aGraph.IsDone());

  int aFaceIdx = 0;
  for (TopExp_Explorer anExp(aBox, TopAbs_FACE); anExp.More(); anExp.Next(), aFaceIdx++)
  {
    const TopoDS_Face& aFace  = TopoDS::Face(anExp.Current());
    const double       aTolUV = 1.0e-6;

    BRepTopAdaptor_FClass2d aLegacy(aFace, aTolUV);
    BRepGraphAlgo_FClass2d  aGraphBased(aGraph, BRepGraph_FaceId(aFaceIdx), aTolUV);

    double aUmin, aUmax, aVmin, aVmax;
    BRepTools::UVBounds(aFace, aUmin, aUmax, aVmin, aVmax);

    // Test on a grid of UV points.
    constexpr int THE_NB_SAMPLES = 5;
    const double  aUStep         = (aUmax - aUmin) / (THE_NB_SAMPLES + 1);
    const double  aVStep         = (aVmax - aVmin) / (THE_NB_SAMPLES + 1);

    for (int i = 1; i <= THE_NB_SAMPLES; i++)
    {
      for (int j = 1; j <= THE_NB_SAMPLES; j++)
      {
        const gp_Pnt2d     aPnt(aUmin + i * aUStep, aVmin + j * aVStep);
        const TopAbs_State aLegacyState = aLegacy.Perform(aPnt, false);
        const TopAbs_State aGraphState  = aGraphBased.Perform(aPnt, false);
        EXPECT_EQ(aGraphState, aLegacyState)
          << "Mismatch at face " << aFaceIdx << " UV=(" << aPnt.X() << "," << aPnt.Y() << ")";
      }
    }
  }
}

TEST(BRepGraphAlgo_FClass2dTest, Consistency_VsLegacy_FusedBoxes)
{
  TopoDS_Shape     aBox1 = BRepPrimAPI_MakeBox(10.0, 10.0, 10.0).Shape();
  TopoDS_Shape     aBox2 = BRepPrimAPI_MakeBox(gp_Pnt(10.0, 0.0, 0.0), 10.0, 10.0, 10.0).Shape();
  BRepAlgoAPI_Fuse aFuser(aBox1, aBox2);
  ASSERT_TRUE(aFuser.IsDone());
  const TopoDS_Shape& aFused = aFuser.Shape();

  BRepGraph aGraph;
  aGraph.Build(aFused);
  ASSERT_TRUE(aGraph.IsDone());

  int aFaceIdx = 0;
  for (TopExp_Explorer anExp(aFused, TopAbs_FACE); anExp.More(); anExp.Next(), aFaceIdx++)
  {
    const TopoDS_Face& aFace  = TopoDS::Face(anExp.Current());
    const double       aTolUV = 1.0e-6;

    BRepTopAdaptor_FClass2d aLegacy(aFace, aTolUV);
    BRepGraphAlgo_FClass2d  aGraphBased(aGraph, BRepGraph_FaceId(aFaceIdx), aTolUV);

    double aUmin, aUmax, aVmin, aVmax;
    BRepTools::UVBounds(aFace, aUmin, aUmax, aVmin, aVmax);

    constexpr int THE_NB_SAMPLES = 5;
    const double  aUStep         = (aUmax - aUmin) / (THE_NB_SAMPLES + 1);
    const double  aVStep         = (aVmax - aVmin) / (THE_NB_SAMPLES + 1);

    for (int i = 1; i <= THE_NB_SAMPLES; i++)
    {
      for (int j = 1; j <= THE_NB_SAMPLES; j++)
      {
        const gp_Pnt2d     aPnt(aUmin + i * aUStep, aVmin + j * aVStep);
        const TopAbs_State aLegacyState = aLegacy.Perform(aPnt, false);
        const TopAbs_State aGraphState  = aGraphBased.Perform(aPnt, false);
        EXPECT_EQ(aGraphState, aLegacyState)
          << "Mismatch at face " << aFaceIdx << " UV=(" << aPnt.X() << "," << aPnt.Y() << ")";
      }
    }
  }
}
