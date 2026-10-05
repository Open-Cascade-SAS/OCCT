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

#include <gtest/gtest.h>

#include <BRepAlgoAPI_Section.hxx>
#include <BRepCheck_Analyzer.hxx>
#include <BRepBuilderAPI_MakeFace.hxx>
#include <BRepGProp.hxx>
#include <BRep_Tool.hxx>
#include <GeomConvert.hxx>
#include <Geom_BSplineCurve.hxx>
#include <Geom_BSplineSurface.hxx>
#include <Geom_Curve.hxx>
#include <Geom_CylindricalSurface.hxx>
#include <Geom_Plane.hxx>
#include <Geom_RectangularTrimmedSurface.hxx>
#include <BRepPrimAPI_MakeCylinder.hxx>
#include <BRepPrimAPI_MakeSphere.hxx>
#include <GProp_GProps.hxx>
#include <TopExp_Explorer.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Edge.hxx>
#include <Precision.hxx>
#include <gp.hxx>
#include <gp_Ax3.hxx>
#include <gp_Dir.hxx>
#include <gp_Pln.hxx>
#include <gp_Pnt.hxx>
#include <TopoDS_Shape.hxx>

namespace
{
static TopoDS_Face MakeHalfCylinderFace(const gp_Ax3& theAxis,
                                        const double  theRadius,
                                        const double  theFirstU,
                                        const double  theLastU)
{
  const occ::handle<Geom_CylindricalSurface> aCylinder =
    new Geom_CylindricalSurface(theAxis, theRadius);
  const occ::handle<Geom_RectangularTrimmedSurface> aTrimmedCylinder =
    new Geom_RectangularTrimmedSurface(aCylinder, theFirstU, theLastU, true);
  return BRepBuilderAPI_MakeFace(aTrimmedCylinder, Precision::Confusion()).Face();
}

static void CheckCylinderSection(const gp_Ax3& theFirstAxis,
                                 const double  theFirstRadius,
                                 const gp_Ax3& theSecondAxis,
                                 const double  theSecondRadius,
                                 const double  theExpectedLength)
{
  const double      aPi         = 3.14159265358979323846;
  const TopoDS_Face aFirstFace  = MakeHalfCylinderFace(theFirstAxis, theFirstRadius, 0.0, aPi);
  const TopoDS_Face aSecondFace = MakeHalfCylinderFace(theSecondAxis, theSecondRadius, 0.0, aPi);
  ASSERT_FALSE(aFirstFace.IsNull());
  ASSERT_FALSE(aSecondFace.IsNull());

  BRepAlgoAPI_Section aSection(aFirstFace, aSecondFace);
  aSection.Build();
  ASSERT_TRUE(aSection.IsDone());
  const TopoDS_Shape aResult = aSection.Shape();
  ASSERT_FALSE(aResult.IsNull());
  EXPECT_TRUE(BRepCheck_Analyzer(aResult).IsValid());

  GProp_GProps aProperties;
  BRepGProp::LinearProperties(aResult, aProperties);
  EXPECT_NEAR(aProperties.Mass(), theExpectedLength, 0.001);
}
} // namespace

// Test OCCN2: BRepAlgoAPI_Section of a cylinder intersecting a sphere.
// Migrated from QABugs_17.cxx OCCN2
TEST(BRepAlgoAPI_SectionTest, OCCN2_CylinderSphereSectionIsDone)
{
  BRepPrimAPI_MakeCylinder aCylMaker(50., 200.);
  const TopoDS_Shape&      aCylinder = aCylMaker.Shape();

  BRepPrimAPI_MakeSphere aSphereMaker(gp_Pnt(60., 0., 100.), 50.);
  const TopoDS_Shape&    aSphere = aSphereMaker.Shape();

  BRepAlgoAPI_Section aSection(aCylinder, aSphere);
  EXPECT_TRUE(aSection.IsDone());
  EXPECT_FALSE(aSection.Shape().IsNull());
}

// Migrated from tests/bugs/modalg_5/bug21564.  Intersections with a converted
// trimmed plane must stay two-pole curves, both with and without approximation.
TEST(BRepAlgoAPI_SectionTest, OCC21564_PlaneIntersectionKeepsTwoPoles)
{
  const occ::handle<Geom_Plane>                     aHorizontalPlane = new Geom_Plane(gp::XOY());
  const occ::handle<Geom_RectangularTrimmedSurface> aTrimmedPlane =
    new Geom_RectangularTrimmedSurface(aHorizontalPlane, -10.0, 10.0, -10.0, 10.0);
  const occ::handle<Geom_BSplineSurface> aBSplinePlane =
    GeomConvert::SurfaceToBSplineSurface(aTrimmedPlane);
  ASSERT_FALSE(aBSplinePlane.IsNull());

  BRepBuilderAPI_MakeFace aHorizontalFaceBuilder(aBSplinePlane, Precision::Confusion());
  ASSERT_TRUE(aHorizontalFaceBuilder.IsDone());

  const gp_Pln            aVerticalPlane(gp_Pnt(0.0, 0.0, 0.0), gp_Dir(1.0, 0.0, 0.0));
  BRepBuilderAPI_MakeFace aVerticalFaceBuilder(aVerticalPlane, -10.0, 10.0, -10.0, 10.0);
  ASSERT_TRUE(aVerticalFaceBuilder.IsDone());

  const auto checkTwoPoleSection = [&](const bool theApproximation) {
    BRepAlgoAPI_Section aSection(aHorizontalFaceBuilder.Face(), aVerticalFaceBuilder.Face(), false);
    aSection.Approximation(theApproximation);
    aSection.Build();
    ASSERT_TRUE(aSection.IsDone());

    int aNbEdges = 0;
    for (TopExp_Explorer anExplorer(aSection.Shape(), TopAbs_EDGE); anExplorer.More();
         anExplorer.Next())
    {
      double                        aFirst = 0.0;
      double                        aLast  = 0.0;
      const occ::handle<Geom_Curve> aCurve =
        BRep_Tool::Curve(TopoDS::Edge(anExplorer.Current()), aFirst, aLast);
      const occ::handle<Geom_BSplineCurve> aBSpline = occ::down_cast<Geom_BSplineCurve>(aCurve);
      ASSERT_FALSE(aBSpline.IsNull());
      EXPECT_EQ(aBSpline->NbPoles(), 2);
      ++aNbEdges;
    }
    EXPECT_EQ(aNbEdges, 1);
  };

  checkTwoPoleSection(false);
  checkTwoPoleSection(true);
}

// Migrated from tests/bugs/modalg_5/bug23932_1.  Intersecting two trimmed
// cylinders must complete without Standard_NoSuchObject and preserve length.
TEST(BRepAlgoAPI_SectionTest, ModalgBug_23932_SectionBetweenTrimmedCylinders1)
{
  CheckCylinderSection(gp_Ax3(gp_Pnt(538.57646417050069, 347.77316708315834, 183.375),
                              gp_Dir(0.0, -1.0, 0.0),
                              gp_Dir(1.0, 0.0, 0.0)),
                       44.194173824159222,
                       gp_Ax3(gp_Pnt(859.00000000000011, 463.75990629028450, 80.865093709715211),
                              gp_Dir(-1.0, 0.0, 0.0),
                              gp_Dir(0.0, -1.0, 0.0)),
                       186.78835506149036,
                       326.401);
}

// Migrated from tests/bugs/modalg_5/bug23932_2.  The nearby cylinder radii
// exercise the second historical section configuration.
TEST(BRepAlgoAPI_SectionTest, ModalgBug_23932_SectionBetweenTrimmedCylinders2)
{
  CheckCylinderSection(gp_Ax3(gp_Pnt(538.57646417050069, 347.77316708315834, 183.375),
                              gp_Dir(0.0, -1.0, 0.0),
                              gp_Dir(1.0, 0.0, 0.0)),
                       44.819173824159222,
                       gp_Ax3(gp_Pnt(859.00000000000011, 463.75990629028450, 80.865093709715211),
                              gp_Dir(-1.0, 0.0, 0.0),
                              gp_Dir(0.0, -1.0, 0.0)),
                       187.41335506149036,
                       330.903);
}
