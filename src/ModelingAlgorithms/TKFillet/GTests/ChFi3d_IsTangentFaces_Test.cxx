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

#include <ChFi3d.hxx>
#include <BRep_Builder.hxx>
#include <BRep_Tool.hxx>
#include <BRepBuilderAPI_MakeEdge.hxx>
#include <BRepBuilderAPI_MakePolygon.hxx>
#include <BRepPrimAPI_MakePrism.hxx>
#include <BRepFilletAPI_MakeFillet.hxx>
#include <BRepFilletAPI_MakeChamfer.hxx>
#include <BRepCheck_Analyzer.hxx>
#include <BRepAdaptor_Curve.hxx>
#include <BRepBuilderAPI_MakeFace.hxx>
#include <BRepPrimAPI_MakeCylinder.hxx>
#include <Geom_Plane.hxx>
#include <Geom_BezierSurface.hxx>
#include <Geom_BSplineSurface.hxx>
#include <BRepPrimAPI_MakeSphere.hxx>
#include <TopLoc_Location.hxx>
#include <gp_Trsf.hxx>
#include <Geom2d_Line.hxx>
#include <Geom2d_BezierCurve.hxx>
#include <NCollection_Array2.hxx>
#include <Precision.hxx>
#include <TopExp_Explorer.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Edge.hxx>
#include <TopoDS_Face.hxx>
#include <gp_Ax3.hxx>
#include <gp_Pln.hxx>
#include <gtest/gtest.h>
#include <cmath>
#include <limits>

namespace
{
struct ChFi3d_TestFaces
{
  TopoDS_Edge Edge;
  TopoDS_Face First;
  TopoDS_Face Second;

  ChFi3d_TestFaces(double theCurvature = 0.0);
};

//==================================================================================================

ChFi3d_TestFaces::ChFi3d_TestFaces(double theCurvature)
{
  First = BRepBuilderAPI_MakeFace(gp_Pln(gp::XOY()), 0.0, 1.0, -1.0, 0.0).Face();
  NCollection_Array2<gp_Pnt> aPoles(1, 2, 1, 3);
  for (int i = 1; i <= 2; ++i)
  {
    aPoles(i, 1) = gp_Pnt(i - 1.0, 0.0, 0.0);
    aPoles(i, 2) = gp_Pnt(i - 1.0, 0.5, 0.0);
    aPoles(i, 3) = gp_Pnt(i - 1.0, 1.0, 0.5 * theCurvature);
  }
  occ::handle<Geom_Surface> aSurface = new Geom_BezierSurface(aPoles);
  Second = BRepBuilderAPI_MakeFace(aSurface, Precision::Confusion()).Face();
  Edge   = BRepBuilderAPI_MakeEdge(gp_Pnt(0, 0, 0), gp_Pnt(1, 0, 0)).Edge();
  occ::handle<Geom2d_Curve> aPCurve = new Geom2d_Line(gp_Pnt2d(0, 0), gp_Dir2d(1, 0));
  BRep_Builder              aBuilder;
  aBuilder.UpdateEdge(Edge, aPCurve, First, Precision::Confusion());
  aBuilder.UpdateEdge(Edge, aPCurve, Second, Precision::Confusion());
  aBuilder.Range(Edge, 0.0, 1.0);
}
} // namespace

TEST(ChFi3d_IsTangentFacesTest, PlanesAndCurvatureMismatch)
{
  ChFi3d_TestFaces aFlat;
  EXPECT_TRUE(ChFi3d::IsTangentFaces(aFlat.Edge, aFlat.First, aFlat.Second));
  EXPECT_TRUE(ChFi3d::IsTangentFaces(aFlat.Edge, aFlat.First, aFlat.Second, GeomAbs_G2));
  ChFi3d_TestFaces aCurved(1.0);
  EXPECT_TRUE(ChFi3d::IsTangentFaces(aCurved.Edge, aCurved.First, aCurved.Second));
  EXPECT_FALSE(ChFi3d::IsTangentFaces(aCurved.Edge, aCurved.First, aCurved.Second, GeomAbs_G2));
}

TEST(ChFi3d_IsTangentFacesTest, StoredRegularityDoesNotOverrideOrientation)
{
  ChFi3d_TestFaces aFaces;
  BRep_Builder().Continuity(aFaces.Edge, aFaces.First, aFaces.Second, GeomAbs_G1);
  aFaces.Second.Reverse();
  EXPECT_FALSE(ChFi3d::IsTangentFaces(aFaces.Edge, aFaces.First, aFaces.Second));
  aFaces.First.Reverse();
  EXPECT_TRUE(ChFi3d::IsTangentFaces(aFaces.Edge, aFaces.First, aFaces.Second));
}

TEST(ChFi3d_IsTangentFacesTest, RejectInvalidRepresentations)
{
  ChFi3d_TestFaces aFaces;
  EXPECT_FALSE(ChFi3d::IsTangentFaces(TopoDS_Edge(), aFaces.First, aFaces.Second));
  EXPECT_FALSE(ChFi3d::IsTangentFaces(aFaces.Edge, TopoDS_Face(), aFaces.Second));
  EXPECT_FALSE(ChFi3d::IsTangentFaces(aFaces.Edge, aFaces.First, aFaces.Second, GeomAbs_C2));
  EXPECT_FALSE(ChFi3d::IsTangentFaces(aFaces.Edge, aFaces.First, aFaces.First));
  BRep_Builder aBuilder;
  aBuilder.Range(aFaces.Edge, aFaces.Second, 0.0, 0.5);
  EXPECT_FALSE(ChFi3d::IsTangentFaces(aFaces.Edge, aFaces.First, aFaces.Second));
}

TEST(ChFi3d_IsTangentFacesTest, UnsetRepresentationFlags)
{
  ChFi3d_TestFaces aFaces;
  BRep_Builder     aBuilder;
  for (const bool isSameParameter : {false, true})
  {
    for (const bool isSameRange : {false, true})
    {
      aBuilder.SameParameter(aFaces.Edge, isSameParameter);
      aBuilder.SameRange(aFaces.Edge, isSameRange);
      EXPECT_TRUE(ChFi3d::IsTangentFaces(aFaces.Edge, aFaces.First, aFaces.Second));
      EXPECT_TRUE(ChFi3d::IsTangentFaces(aFaces.Edge, aFaces.First, aFaces.Second, GeomAbs_G2));
      aFaces.Second.Reverse();
      EXPECT_FALSE(ChFi3d::IsTangentFaces(aFaces.Edge, aFaces.First, aFaces.Second));
      aFaces.Second.Reverse();
    }
  }
}

TEST(ChFi3d_IsTangentFacesTest, DegenerateEdgeWithDefinedNormals)
{
  ChFi3d_TestFaces aFaces;
  BRep_Builder     aBuilder;
  aBuilder.MakeEdge(aFaces.Edge);
  NCollection_Array1<gp_Pnt2d> aPoles(1, 2);
  aPoles.Init(gp_Pnt2d(0.0, 0.0));
  occ::handle<Geom2d_Curve> aPCurve = new Geom2d_BezierCurve(aPoles);
  aBuilder.UpdateEdge(aFaces.Edge, aPCurve, aFaces.First, Precision::Confusion());
  aBuilder.UpdateEdge(aFaces.Edge, aPCurve, aFaces.Second, Precision::Confusion());
  aBuilder.Range(aFaces.Edge, 0.0, 1.0);
  aBuilder.Degenerated(aFaces.Edge, true);
  EXPECT_TRUE(ChFi3d::IsTangentFaces(aFaces.Edge, aFaces.First, aFaces.Second));
  EXPECT_TRUE(ChFi3d::IsTangentFaces(aFaces.Edge, aFaces.First, aFaces.Second, GeomAbs_G2));
  aFaces.Second.Reverse();
  EXPECT_FALSE(ChFi3d::IsTangentFaces(aFaces.Edge, aFaces.First, aFaces.Second));
}

TEST(ChFi3d_IsTangentFacesTest, CylinderSeam)
{
  const TopoDS_Shape aCylinder = BRepPrimAPI_MakeCylinder(1.0, 2.0).Shape();
  bool               hasSeam   = false;
  for (TopExp_Explorer aFaces(aCylinder, TopAbs_FACE); aFaces.More(); aFaces.Next())
  {
    const TopoDS_Face aFace = TopoDS::Face(aFaces.Current());
    for (TopExp_Explorer anEdges(aFace, TopAbs_EDGE); anEdges.More(); anEdges.Next())
    {
      TopoDS_Edge anEdge = TopoDS::Edge(anEdges.Current());
      if (!BRep_Tool::IsClosed(anEdge, aFace))
      {
        continue;
      }
      hasSeam = true;
      EXPECT_TRUE(ChFi3d::IsTangentFaces(anEdge, aFace, aFace));
      EXPECT_TRUE(ChFi3d::IsTangentFaces(anEdge, aFace, aFace, GeomAbs_G2));
      anEdge.Reverse();
      EXPECT_TRUE(ChFi3d::IsTangentFaces(anEdge, aFace, aFace));
    }
  }
  EXPECT_TRUE(hasSeam);
}

TEST(ChFi3d_IsTangentFacesTest, NarrowSplineSpan)
{
  ChFi3d_TestFaces           aFaces;
  NCollection_Array2<gp_Pnt> aPoles(1, 5, 1, 2);
  const double               aKnotsData[] = {0.0, 0.501, 0.502, 0.503, 1.0};
  NCollection_Array1<double> aUKnots(1, 5), aVKnots(1, 2);
  NCollection_Array1<int>    aUMults(1, 5), aVMults(1, 2);
  for (int i = 1; i <= 5; ++i)
  {
    aUKnots(i)   = aKnotsData[i - 1];
    aUMults(i)   = i == 1 || i == 5 ? 2 : 1;
    aPoles(i, 1) = gp_Pnt(aUKnots(i), 0.0, 0.0);
    aPoles(i, 2) = gp_Pnt(aUKnots(i), 1.0, i == 3 ? 1.0 : 0.0);
  }
  aVKnots(1) = 0.0;
  aVKnots(2) = 1.0;
  aVMults.Init(2);
  occ::handle<Geom_Surface> aSurface =
    new Geom_BSplineSurface(aPoles, aUKnots, aVKnots, aUMults, aVMults, 1, 1);
  aFaces.Second = BRepBuilderAPI_MakeFace(aSurface, Precision::Confusion()).Face();
  BRep_Builder aBuilder;
  aBuilder.UpdateEdge(aFaces.Edge,
                      new Geom2d_Line(gp_Pnt2d(0, 0), gp_Dir2d(1, 0)),
                      aFaces.Second,
                      Precision::Confusion());
  aBuilder.Range(aFaces.Edge, aFaces.Second, 0.0, 1.0);
  // The common edge is straight. Curve deflection sampling cannot see the
  // transverse normal variation confined to [0.501, 0.503].
  EXPECT_FALSE(ChFi3d::IsTangentFaces(aFaces.Edge, aFaces.First, aFaces.Second));
}

TEST(ChFi3d_IsTangentFacesTest, InteriorSingularityIsNotSkipped)
{
  ChFi3d_TestFaces           aFaces;
  NCollection_Array2<gp_Pnt> aPoles(1, 3, 1, 2);
  const double               aWidths[] = {1.0, -1.0, 1.0};
  for (int i = 1; i <= 3; ++i)
  {
    aPoles(i, 1) = gp_Pnt(0.5 * (i - 1), 0.0, 0.0);
    aPoles(i, 2) = gp_Pnt(0.5 * (i - 1), aWidths[i - 1], 0.0);
  }
  occ::handle<Geom_Surface> aSurface = new Geom_BezierSurface(aPoles);
  aFaces.Second = BRepBuilderAPI_MakeFace(aSurface, Precision::Confusion()).Face();
  BRep_Builder aBuilder;
  aBuilder.UpdateEdge(aFaces.Edge,
                      new Geom2d_Line(gp_Pnt2d(0, 0), gp_Dir2d(1, 0)),
                      aFaces.Second,
                      Precision::Confusion());
  aBuilder.Range(aFaces.Edge, aFaces.Second, 0.0, 1.0);
  EXPECT_FALSE(ChFi3d::IsTangentFaces(aFaces.Edge, aFaces.First, aFaces.Second));
}

TEST(ChFi3d_IsTangentFacesTest, LocatedFaces)
{
  ChFi3d_TestFaces aFaces;
  gp_Trsf          aTransform;
  aTransform.SetRotation(gp_Ax1(gp::Origin(), gp_Dir(1, 2, 3)), 0.87);
  aTransform.SetTranslationPart(gp_Vec(12, -34, 56));
  const TopLoc_Location aLocation(aTransform);
  aFaces.Edge.Move(aLocation);
  aFaces.First.Move(aLocation);
  aFaces.Second.Move(aLocation);
  EXPECT_TRUE(ChFi3d::IsTangentFaces(aFaces.Edge, aFaces.First, aFaces.Second, GeomAbs_G2));
}

TEST(ChFi3d_IsTangentFacesTest, SphereSeamAllowsSingularEndpoints)
{
  const TopoDS_Shape aSphere = BRepPrimAPI_MakeSphere(1.0).Shape();
  bool               hasSeam = false;
  for (TopExp_Explorer aFaces(aSphere, TopAbs_FACE); aFaces.More(); aFaces.Next())
  {
    const TopoDS_Face aFace = TopoDS::Face(aFaces.Current());
    for (TopExp_Explorer anEdges(aFace, TopAbs_EDGE); anEdges.More(); anEdges.Next())
    {
      const TopoDS_Edge anEdge = TopoDS::Edge(anEdges.Current());
      if (BRep_Tool::Degenerated(anEdge))
      {
        EXPECT_FALSE(ChFi3d::IsTangentFaces(anEdge, aFace, aFace));
      }
      else if (BRep_Tool::IsClosed(anEdge, aFace))
      {
        hasSeam = true;
        EXPECT_TRUE(ChFi3d::IsTangentFaces(anEdge, aFace, aFace));
        EXPECT_TRUE(ChFi3d::IsTangentFaces(anEdge, aFace, aFace, GeomAbs_G2));
      }
    }
  }
  EXPECT_TRUE(hasSeam);
}

TEST(ChFi3d_IsTangentFacesTest, StoredRegularityDoesNotHideGeometryMismatch)
{
  ChFi3d_TestFaces aFaces;
  BRep_Builder().Continuity(aFaces.Edge, aFaces.First, aFaces.Second, GeomAbs_G1);
  occ::handle<Geom_Surface> aSurface = BRep_Tool::Surface(aFaces.Second);
  aSurface->Rotate(gp_Ax1(gp::Origin(), gp::DX()), 0.2);
  EXPECT_FALSE(ChFi3d::IsTangentFaces(aFaces.Edge, aFaces.First, aFaces.Second));
  aSurface->Rotate(gp_Ax1(gp::Origin(), gp::DX()), -0.2);
  aSurface->Translate(gp_Vec(0, 0, 0.01));
  EXPECT_FALSE(ChFi3d::IsTangentFaces(aFaces.Edge, aFaces.First, aFaces.Second));
}

TEST(ChFi3d_IsTangentFacesTest, MissingPCurve)
{
  ChFi3d_TestFaces aFaces;
  BRep_Builder().UpdateEdge(aFaces.Edge,
                            occ::handle<Geom2d_Curve>(),
                            aFaces.Second,
                            Precision::Confusion());
  EXPECT_FALSE(ChFi3d::IsTangentFaces(aFaces.Edge, aFaces.First, aFaces.Second));
}

TEST(ChFi3d_IsTangentFacesTest, SeamOnDistinctFaces)
{
  const TopoDS_Shape aCylinder = BRepPrimAPI_MakeCylinder(1.0, 2.0).Shape();
  bool               hasSeam   = false;
  for (TopExp_Explorer aFaces(aCylinder, TopAbs_FACE); aFaces.More(); aFaces.Next())
  {
    TopoDS_Face aFace1 = TopoDS::Face(aFaces.Current());
    for (TopExp_Explorer anEdges(aFace1, TopAbs_EDGE); anEdges.More(); anEdges.Next())
    {
      TopoDS_Edge anEdge = TopoDS::Edge(anEdges.Current());
      if (!BRep_Tool::IsClosed(anEdge, aFace1))
      {
        continue;
      }
      hasSeam            = true;
      TopoDS_Face aFace2 = TopoDS::Face(aFace1.EmptyCopied());
      EXPECT_TRUE(ChFi3d::IsTangentFaces(anEdge, aFace1, aFace2, GeomAbs_G2));
      // The special branch requires an edge occurrence in the first face.
      EXPECT_FALSE(ChFi3d::IsTangentFaces(anEdge, aFace2, aFace1));
      anEdge.Reverse();
      EXPECT_TRUE(ChFi3d::IsTangentFaces(anEdge, aFace1, aFace2));
      aFace1.Reverse();
      aFace2.Reverse();
      EXPECT_TRUE(ChFi3d::IsTangentFaces(anEdge, aFace1, aFace2));
      aFace1.Reverse();
    }
  }
  EXPECT_TRUE(hasSeam);
}

TEST(ChFi3d_IsTangentFacesTest, DifferentSurfaceLocations)
{
  ChFi3d_TestFaces aFaces;
  const gp_Vec     aShift(12, -34, 56);
  BRep_Tool::Surface(aFaces.Second)->Translate(-aShift);
  gp_Trsf aPlacement;
  aPlacement.SetTranslation(aShift);
  aFaces.Second.Move(TopLoc_Location(aPlacement));
  BRep_Builder aBuilder;
  aBuilder.UpdateEdge(aFaces.Edge,
                      new Geom2d_Line(gp_Pnt2d(0, 0), gp_Dir2d(1, 0)),
                      aFaces.Second,
                      Precision::Confusion());
  aBuilder.Range(aFaces.Edge, aFaces.Second, 0.0, 1.0);
  EXPECT_TRUE(ChFi3d::IsTangentFaces(aFaces.Edge, aFaces.First, aFaces.Second, GeomAbs_G2));
}

TEST(ChFi3d_IsTangentFacesTest, AngularToleranceBoundary)
{
  for (double aTolerance : {Precision::Angular(), 1.e-8, 0.025})
  {
    for (double aFactor : {0.9, 1.1})
    {
      ChFi3d_TestFaces aFaces;
      const double     anAngle = aFactor * aTolerance;
      BRep_Tool::Surface(aFaces.Second)->Rotate(gp_Ax1(gp::Origin(), gp::DX()), anAngle);
      for (GeomAbs_Shape anOrder : {GeomAbs_G1, GeomAbs_G2})
      {
        EXPECT_EQ(
          ChFi3d::IsTangentFaces(aFaces.Edge, aFaces.First, aFaces.Second, anOrder, aTolerance),
          aFactor < 1.0);
      }
    }
  }
}

TEST(ChFi3d_IsTangentFacesTest, ShallowCreasesAreNotTangentByDefault)
{
  for (double anAngle : {1.e-9, 1.e-5, 0.001, 0.05})
  {
    ChFi3d_TestFaces aFaces;
    BRep_Tool::Surface(aFaces.Second)->Rotate(gp_Ax1(gp::Origin(), gp::DX()), anAngle);
    EXPECT_FALSE(ChFi3d::IsTangentFaces(aFaces.Edge, aFaces.First, aFaces.Second));
    EXPECT_FALSE(ChFi3d::IsTangentFaces(aFaces.Edge, aFaces.First, aFaces.Second, GeomAbs_G2));
    EXPECT_TRUE(
      ChFi3d::IsTangentFaces(aFaces.Edge, aFaces.First, aFaces.Second, GeomAbs_G1, 0.075));
  }
}

TEST(ChFi3d_IsTangentFacesTest, InvalidAngularTolerance)
{
  ChFi3d_TestFaces aFaces;
  for (double aTolerance : {-1.0,
                            M_PI / 2.0,
                            std::numeric_limits<double>::quiet_NaN(),
                            std::numeric_limits<double>::infinity()})
  {
    EXPECT_FALSE(
      ChFi3d::IsTangentFaces(aFaces.Edge, aFaces.First, aFaces.Second, GeomAbs_G1, aTolerance));
  }
}

TEST(ChFi3d_IsTangentFacesTest, ShallowCreaseFilletAndChamfer)
{
  for (double anAngle : {0.001, 0.005, 0.02, 0.05})
  {
    SCOPED_TRACE(anAngle);
    BRepBuilderAPI_MakePolygon aPolygon;
    aPolygon.Add(gp_Pnt(-10, 0, 0));
    aPolygon.Add(gp_Pnt(0, 0, 0));
    aPolygon.Add(gp_Pnt(10, -10 * std::tan(anAngle), 0));
    aPolygon.Add(gp_Pnt(10, -10, 0));
    aPolygon.Add(gp_Pnt(-10, -10, 0));
    aPolygon.Close();
    const TopoDS_Shape aShape =
      BRepPrimAPI_MakePrism(BRepBuilderAPI_MakeFace(aPolygon.Wire()).Face(), gp_Vec(0, 0, 10))
        .Shape();
    ASSERT_TRUE(BRepCheck_Analyzer(aShape).IsValid());
    TopoDS_Edge aCrease;
    for (TopExp_Explorer anEdges(aShape, TopAbs_EDGE); anEdges.More(); anEdges.Next())
    {
      const TopoDS_Edge anEdge = TopoDS::Edge(anEdges.Current());
      BRepAdaptor_Curve aCurve(anEdge);
      const gp_Pnt      aStart = aCurve.EvalD0(aCurve.FirstParameter());
      const gp_Pnt      anEnd  = aCurve.EvalD0(aCurve.LastParameter());
      if (std::abs(aStart.X()) < Precision::Confusion()
          && std::abs(aStart.Y()) < Precision::Confusion()
          && std::abs(anEnd.X()) < Precision::Confusion()
          && std::abs(anEnd.Y()) < Precision::Confusion())
      {
        aCrease = anEdge;
        break;
      }
    }
    ASSERT_FALSE(aCrease.IsNull());
    for (double aPropagationAngle : {1.e-4, 0.2})
    {
      BRepFilletAPI_MakeFillet aFillet(aShape);
      aFillet.SetParams(aPropagationAngle, 1.e-4, 1.e-5, 1.e-4, 1.e-5, 1.e-3);
      aFillet.Add(0.5, aCrease);
      ASSERT_EQ(aFillet.NbContours(), 1);
      aFillet.Build();
      ASSERT_TRUE(aFillet.IsDone());
      EXPECT_TRUE(BRepCheck_Analyzer(aFillet.Shape()).IsValid());
    }
    BRepFilletAPI_MakeChamfer aChamfer(aShape);
    aChamfer.Add(0.5, aCrease);
    ASSERT_EQ(aChamfer.NbContours(), 1);
    aChamfer.Build();
    ASSERT_TRUE(aChamfer.IsDone());
    EXPECT_TRUE(BRepCheck_Analyzer(aChamfer.Shape()).IsValid());
  }
}
