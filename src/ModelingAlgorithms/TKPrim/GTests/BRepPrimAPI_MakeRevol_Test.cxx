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

#include <BRep_Tool.hxx>
#include <BRepBuilderAPI_MakeEdge.hxx>
#include <BRepBuilderAPI_MakeFace.hxx>
#include <BRepBuilderAPI_MakePolygon.hxx>
#include <BRepBuilderAPI_MakeVertex.hxx>
#include <BRepBuilderAPI_MakeWire.hxx>
#include <BRepCheck_Analyzer.hxx>
#include <BRepPrimAPI_MakeRevol.hxx>
#include <BRepPrimAPI_MakeSphere.hxx>
#include <Geom_Circle.hxx>
#include <Geom_Surface.hxx>
#include <Geom_SurfaceOfRevolution.hxx>
#include <Geom_ToroidalSurface.hxx>
#include <Precision.hxx>
#include <gp_Ax1.hxx>
#include <gp_Ax2.hxx>
#include <gp_Dir.hxx>
#include <gp_Pnt.hxx>
#include <TopExp.hxx>
#include <TopExp_Explorer.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Edge.hxx>
#include <TopoDS_Face.hxx>
#include <TopoDS_Shape.hxx>
#include <TopoDS_Vertex.hxx>
#include <TopoDS_Wire.hxx>

#include <gtest/gtest.h>

namespace
{

void CheckToroidalRevolution(const gp_Dir& theXDirection, const double theRadius)
{
  const gp_Ax2 anCircleAxis(gp_Pnt(0.0, 100.00001, 0.0), gp_Dir(1.0, 0.0, 0.0), theXDirection);
  occ::handle<Geom_Circle> aCircle = new Geom_Circle(anCircleAxis, theRadius);

  BRepBuilderAPI_MakeEdge aMakeEdge(aCircle);
  ASSERT_TRUE(aMakeEdge.IsDone());

  const gp_Ax1          aRevolutionAxis(gp_Pnt(0.0, 0.0, 0.0), gp_Dir(0.0, 0.0, 1.0));
  BRepPrimAPI_MakeRevol aMakeRevol(aMakeEdge.Edge(), aRevolutionAxis, M_PI / 2.0);
  ASSERT_TRUE(aMakeRevol.IsDone());

  const TopoDS_Shape aResult = aMakeRevol.Shape();
  ASSERT_FALSE(aResult.IsNull());
  EXPECT_TRUE(BRepCheck_Analyzer(aResult).IsValid());

  TopExp_Explorer anExplorer(aResult, TopAbs_FACE);
  ASSERT_TRUE(anExplorer.More());
  const TopoDS_Face&        aFace    = TopoDS::Face(anExplorer.Current());
  occ::handle<Geom_Surface> aSurface = BRep_Tool::Surface(aFace);
  ASSERT_FALSE(aSurface.IsNull());
  EXPECT_TRUE(aSurface->IsKind(STANDARD_TYPE(Geom_ToroidalSurface)));
  EXPECT_FALSE(aSurface->IsKind(STANDARD_TYPE(Geom_SurfaceOfRevolution)));
}

} // namespace

// Migrated from tests/bugs/modalg_5/bug23954.  Revolving circles in the four
// orientations used by DRAW must preserve their toroidal surface type.
TEST(BRepPrimAPI_MakeRevolTest, ModalgBug_23954_CircleRevolutionsKeepToroidalSurface)
{
  CheckToroidalRevolution(gp_Dir(0.0, 1.0, 0.0), 100.0);
  CheckToroidalRevolution(gp_Dir(0.0, 0.0, 1.0), 100.0);
  CheckToroidalRevolution(gp_Dir(0.0, -1.0, 0.0), 100.0);
  CheckToroidalRevolution(gp_Dir(0.0, 0.0, 1.0), 10.0);
}

// Migrated from tests/bugs/modalg_5/bug24328.  Revolving the two-edge wire
// must produce a shape accepted by the Boolean-operation validity checker.
TEST(BRepPrimAPI_MakeRevolTest, ModalgBug_24328_RevolvedWireIsValid)
{
  const TopoDS_Vertex aVertex1 = BRepBuilderAPI_MakeVertex(gp_Pnt(23.5, 11.123, 0.0));
  const TopoDS_Vertex aVertex2 = BRepBuilderAPI_MakeVertex(gp_Pnt(22.0, 11.1409, 0.0));
  const TopoDS_Vertex aVertex3 = BRepBuilderAPI_MakeVertex(gp_Pnt(20.5, 11.1807, 0.0));
  const TopoDS_Edge   anEdge1  = BRepBuilderAPI_MakeEdge(aVertex1, aVertex2);
  const TopoDS_Edge   anEdge2  = BRepBuilderAPI_MakeEdge(aVertex2, aVertex3);
  const TopoDS_Wire   aWire    = BRepBuilderAPI_MakeWire(anEdge1, anEdge2);

  BRepPrimAPI_MakeRevol aMakeRevol(aWire,
                                   gp_Ax1(gp_Pnt(0.0, 0.0, 0.0), gp_Dir(1.0, 0.0, 0.0)),
                                   2.0 * M_PI);
  ASSERT_TRUE(aMakeRevol.IsDone());
  const TopoDS_Shape aResult = aMakeRevol.Shape();
  ASSERT_FALSE(aResult.IsNull());
  EXPECT_TRUE(BRepCheck_Analyzer(aResult).IsValid());
}

// Migrated from tests/bugs/modalg_5/bug25578.  Revolving the partial-sphere
// face around both axes must complete without a singularity crash.
TEST(BRepPrimAPI_MakeRevolTest, ModalgBug_25578_PartialSphereRevolutions)
{
  BRepPrimAPI_MakeSphere aMakeSphere(5.0, 0.0, M_PI / 2.0, M_PI / 2.0);
  const TopoDS_Shape     aSphere = aMakeSphere.Shape();
  ASSERT_TRUE(aMakeSphere.IsDone());

  TopExp_Explorer aFaceExplorer(aSphere, TopAbs_FACE);
  ASSERT_TRUE(aFaceExplorer.More());
  const TopoDS_Face aFace = TopoDS::Face(aFaceExplorer.Current());

  const gp_Ax1          anAxisZ(gp_Pnt(0.0, 0.0, 0.0), gp_Dir(0.0, 0.0, 1.0));
  BRepPrimAPI_MakeRevol aRevolZ(aFace, anAxisZ, 2.0 * M_PI);
  ASSERT_TRUE(aRevolZ.IsDone());
  ASSERT_FALSE(aRevolZ.Shape().IsNull());

  const gp_Ax1          anAxisX(gp_Pnt(0.0, 0.0, 0.0), gp_Dir(1.0, 0.0, 0.0));
  BRepPrimAPI_MakeRevol aRevolX(aFace, anAxisX, M_PI / 2.0);
  ASSERT_TRUE(aRevolX.IsDone());
  ASSERT_FALSE(aRevolX.Shape().IsNull());
}

namespace
{
// Revolves the planar rectangle with corners (theX0, 0, 0) and (theX1, theHeight, 0) around the
// Y axis by theAngle, and checks that every edge off the axis reports, through Generated(), the
// one face it sweeps, and that this face is in the result.
void CheckEdgesGenerateTheirFaces(const double theX0, const double theX1, const double theAngle)
{
  const double               aHeight = 5.0;
  BRepBuilderAPI_MakePolygon aPolygon(gp_Pnt(theX0, 0.0, 0.0),
                                      gp_Pnt(theX1, 0.0, 0.0),
                                      gp_Pnt(theX1, aHeight, 0.0),
                                      gp_Pnt(theX0, aHeight, 0.0),
                                      true);
  const TopoDS_Face          aProfile = BRepBuilderAPI_MakeFace(aPolygon.Wire(), true);
  BRepPrimAPI_MakeRevol      aMakeRevol(aProfile,
                                        gp_Ax1(gp_Pnt(0.0, 0.0, 0.0), gp_Dir(0.0, 1.0, 0.0)),
                                        theAngle);
  ASSERT_TRUE(aMakeRevol.IsDone());
  const TopoDS_Shape aResult = aMakeRevol.Shape();
  EXPECT_TRUE(BRepCheck_Analyzer(aResult).IsValid());

  for (TopExp_Explorer anEdgeExp(aProfile, TopAbs_EDGE); anEdgeExp.More(); anEdgeExp.Next())
  {
    const TopoDS_Edge anEdge = TopoDS::Edge(anEdgeExp.Current());
    TopoDS_Vertex     aFirst, aLast;
    TopExp::Vertices(anEdge, aFirst, aLast);
    if (std::abs(BRep_Tool::Pnt(aFirst).X()) < Precision::Confusion()
        && std::abs(BRep_Tool::Pnt(aLast).X()) < Precision::Confusion())
    {
      continue; // On the axis: it sweeps no face.
    }

    int aNbFaces = 0;
    for (const TopoDS_Shape& aGenerated : aMakeRevol.Generated(anEdge))
    {
      if (aGenerated.ShapeType() != TopAbs_FACE)
      {
        continue;
      }
      ++aNbFaces;
      bool isInResult = false;
      for (TopExp_Explorer aFaceExp(aResult, TopAbs_FACE); aFaceExp.More(); aFaceExp.Next())
      {
        isInResult = isInResult || aFaceExp.Current().IsSame(aGenerated);
      }
      EXPECT_TRUE(isInResult);
    }
    EXPECT_EQ(aNbFaces, 1) << "edge from (" << BRep_Tool::Pnt(aFirst).X() << ", "
                           << BRep_Tool::Pnt(aFirst).Y() << ") to (" << BRep_Tool::Pnt(aLast).X()
                           << ", " << BRep_Tool::Pnt(aLast).Y() << ")";
  }
}
} // namespace

// In a full turn, an edge square to the axis sweeps a planar ring. The ring has no seam, so the
// copy of the edge at the sweep's ends is unused, yet Generated() must still return the ring.
TEST(BRepPrimAPI_MakeRevolTest, FullTurnRingsAreGenerated)
{
  CheckEdgesGenerateTheirFaces(2.0, 4.0, 2.0 * M_PI);
}

// The same with a profile edge on the axis: the edges square to it sweep discs.
TEST(BRepPrimAPI_MakeRevolTest, FullTurnDiscsAreGenerated)
{
  CheckEdgesGenerateTheirFaces(0.0, 4.0, 2.0 * M_PI);
}

// A partial turn already reported them; it must keep doing so.
TEST(BRepPrimAPI_MakeRevolTest, PartialTurnSectorsAreGenerated)
{
  CheckEdgesGenerateTheirFaces(2.0, 4.0, M_PI);
}
