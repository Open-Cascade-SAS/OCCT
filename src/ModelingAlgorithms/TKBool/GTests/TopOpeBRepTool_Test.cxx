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
// Alternatively, this file may be distributed under the terms of Open CASCADE
// commercial license or contractual agreement.

#include <gtest/gtest.h>

#include <BRep_Builder.hxx>
#include <BRepAdaptor_Surface.hxx>
#include <BRepBuilderAPI_MakeFace.hxx>
#include <BRepPrimAPI_MakeCylinder.hxx>
#include <GeomAbs_SurfaceType.hxx>
#include <TopExp_Explorer.hxx>
#include <TopOpeBRepTool.hxx>
#include <TopOpeBRepTool_tol.hxx>
#include <TopTools_ShapeMapHasher.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Face.hxx>
#include <TopoDS_Shape.hxx>
#include <gp.hxx>
#include <gp_Pln.hxx>

namespace
{
TopoDS_Face MakePlanarFace()
{
  return BRepBuilderAPI_MakeFace(gp_Pln(gp::XOY()), 0.0, 10.0, 0.0, 10.0).Face();
}

TopoDS_Face FindCylindricalFace(const TopoDS_Shape& theShape)
{
  for (TopExp_Explorer anExplorer(theShape, TopAbs_FACE); anExplorer.More(); anExplorer.Next())
  {
    const TopoDS_Face aFace = TopoDS::Face(anExplorer.Current());
    BRepAdaptor_Surface aSurface(aFace, false);
    if (aSurface.GetType() == GeomAbs_Cylinder)
    {
      return aFace;
    }
  }
  return TopoDS_Face();
}
} // namespace

TEST(TopOpeBRepToolTest, MakeFacesKeepsValidFaces)
{
  const TopoDS_Face aFace = MakePlanarFace();

  NCollection_List<TopoDS_Shape> aFaces;
  aFaces.Append(aFace);
  NCollection_IndexedMap<TopoDS_Shape> aFaultyShapes;
  NCollection_List<TopoDS_Shape>       aResult;

  ASSERT_TRUE(TopOpeBRepTool::MakeFaces(aFace, aFaces, aFaultyShapes, aResult));
  ASSERT_EQ(aResult.Extent(), 1);
  EXPECT_TRUE(aResult.First().IsSame(aFace));
}

TEST(TopOpeBRepToolTest, PurgeClosingEdgesHandlesNonPeriodicFace)
{
  const TopoDS_Face aFace = MakePlanarFace();
  NCollection_DataMap<TopoDS_Shape, int, TopTools_ShapeMapHasher> aWireStates;
  NCollection_IndexedMap<TopoDS_Shape>                             aFaultyShapes;

  EXPECT_FALSE(TopOpeBRepTool::PurgeClosingEdges(aFace, aFace, aWireStates, aFaultyShapes));

  NCollection_List<TopoDS_Shape> aFaces;
  aFaces.Append(aFace);
  EXPECT_TRUE(TopOpeBRepTool::PurgeClosingEdges(aFace, aFaces, aWireStates, aFaultyShapes));
  EXPECT_TRUE(aFaultyShapes.IsEmpty());
}

TEST(TopOpeBRepToolTest, CorrectONUVISORejectsNonPeriodicFace)
{
  TopoDS_Face aFace = MakePlanarFace();

  EXPECT_FALSE(TopOpeBRepTool::CorrectONUVISO(aFace, aFace));
}

TEST(TopOpeBRepToolTest, PeriodicCylinderFacePassesPurgeAndUVCorrection)
{
  const TopoDS_Face aFace = FindCylindricalFace(BRepPrimAPI_MakeCylinder(5.0, 10.0).Shape());
  ASSERT_FALSE(aFace.IsNull());

  NCollection_DataMap<TopoDS_Shape, int, TopTools_ShapeMapHasher> aWireStates;
  NCollection_IndexedMap<TopoDS_Shape>                             aFaultyShapes;
  NCollection_List<TopoDS_Shape>                                   aFaces;
  aFaces.Append(aFace);

  EXPECT_TRUE(TopOpeBRepTool::PurgeClosingEdges(aFace, aFaces, aWireStates, aFaultyShapes));
  EXPECT_TRUE(aFaultyShapes.IsEmpty());

  TopoDS_Face aFaceToCorrect = aFace;
  EXPECT_TRUE(TopOpeBRepTool::CorrectONUVISO(aFace, aFaceToCorrect));
}

TEST(TopOpeBRepToolTest, FaceTolerances3dIsIndependentBetweenCalls)
{
  TopoDS_Face aFirstFace  = MakePlanarFace();
  TopoDS_Face aSecondFace = MakePlanarFace();
  BRep_Builder aBuilder;
  aBuilder.UpdateFace(aFirstFace, 2.e-5);
  aBuilder.UpdateFace(aSecondFace, 3.e-5);

  double aFirstTolerance = 0.0;
  FTOL_FaceTolerances3d(aFirstFace, aSecondFace, aFirstTolerance);
  EXPECT_NEAR(aFirstTolerance, 5.e-5, 1.e-12);

  aBuilder.UpdateFace(aFirstFace, 8.e-5);
  aBuilder.UpdateFace(aSecondFace, 8.e-5);
  double aClampedTolerance = 0.0;
  FTOL_FaceTolerances3d(aFirstFace, aSecondFace, aClampedTolerance);
  EXPECT_NEAR(aClampedTolerance, 1.e-4, 1.e-12);

  aBuilder.UpdateFace(aFirstFace, 2.e-5);
  aBuilder.UpdateFace(aSecondFace, 3.e-5);
  double aFirstToleranceAgain = 0.0;
  FTOL_FaceTolerances3d(aFirstFace, aSecondFace, aFirstToleranceAgain);
  EXPECT_NEAR(aFirstToleranceAgain, aFirstTolerance, 1.e-12);
}
