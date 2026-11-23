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
#include <BRepBuilderAPI_MakeSolid.hxx>
#include <BRepBuilderAPI_MakeWire.hxx>
#include <BRepBuilderAPI_Sewing.hxx>
#include <BRepBuilderAPI_Transform.hxx>
#include <BRepCheck_Analyzer.hxx>
#include <BRepFilletAPI_MakeFillet.hxx>
#include <BRepGProp.hxx>
#include <BRepLib.hxx>
#include <BRepOffsetAPI_ThruSections.hxx>
#include <BRepTools_Modification.hxx>
#include <BRepTools_Modifier.hxx>
#include <Geom2d_BSplineCurve.hxx>
#include <Geom2d_Line.hxx>
#include <Geom_BezierCurve.hxx>
#include <Geom_BSplineCurve.hxx>
#include <Geom_CylindricalSurface.hxx>
#include <Geom_Line.hxx>
#include <GProp_GProps.hxx>
#include <NCollection_Array1.hxx>
#include <NCollection_DynamicArray.hxx>
#include <NCollection_IndexedMap.hxx>
#include <TopExp.hxx>
#include <TopExp_Explorer.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Edge.hxx>
#include <TopoDS_Face.hxx>
#include <TopoDS_Shell.hxx>
#include <TopoDS_Solid.hxx>
#include <TopoDS_Vertex.hxx>
#include <TopoDS_Wire.hxx>
#include <TopTools_ShapeMapHasher.hxx>
#include <gp_Ax3.hxx>

#include <gtest/gtest.h>

#include <cmath>

namespace
{
constexpr double THE_RISE  = 3.0;
constexpr double THE_TWIST = -M_PI / 16.0;

// A ruled loft makes each straight edge a Bezier or BSpline curve of degree 1
// with two poles. This gives each one a line instead, with each pcurve and
// vertex moved to the parameter of the line, so the edges reach the fillet as
// lines.
class StraightEdges : public BRepTools_Modification
{
public:
  bool NewSurface(const TopoDS_Face&,
                  occ::handle<Geom_Surface>&,
                  TopLoc_Location&,
                  double&,
                  bool&,
                  bool&) override
  {
    return false;
  }

  bool NewCurve(const TopoDS_Edge&       theEdge,
                occ::handle<Geom_Curve>& theCurve,
                TopLoc_Location&         theLoc,
                double&                  theTol) override
  {
    Span aSpan;
    if (!spanOf(theEdge, theLoc, aSpan))
    {
      return false;
    }
    theCurve = new Geom_Line(aSpan.Origin, aSpan.Direction);
    theTol   = BRep_Tool::Tolerance(theEdge);
    return true;
  }

  bool NewPoint(const TopoDS_Vertex&, gp_Pnt&, double&) override { return false; }

  bool NewCurve2d(const TopoDS_Edge& theEdge,
                  const TopoDS_Face& theFace,
                  const TopoDS_Edge&,
                  const TopoDS_Face&,
                  occ::handle<Geom2d_Curve>& theCurve,
                  double&                    theTol) override
  {
    TopLoc_Location aLoc;
    Span            aSpan;
    double          aFirst = 0.0, aLast = 0.0;
    if (!spanOf(theEdge, aLoc, aSpan))
    {
      return false;
    }
    occ::handle<Geom2d_Curve> anOld = BRep_Tool::CurveOnSurface(theEdge, theFace, aFirst, aLast);
    if (anOld.IsNull())
    {
      return false;
    }
    // The pcurves of a ruling are straight in UV, so two poles keep them.
    NCollection_Array1<gp_Pnt2d> aPoles(1, 2);
    aPoles(1) = anOld->Value(aFirst);
    aPoles(2) = anOld->Value(aLast);
    NCollection_Array1<double> aKnots(1, 2);
    aKnots(1) = aSpan.Map(aFirst);
    aKnots(2) = aSpan.Map(aLast);
    NCollection_Array1<int> aMults(1, 2);
    aMults.Init(2);
    theCurve = new Geom2d_BSplineCurve(aPoles, aKnots, aMults, 1);
    theTol   = BRep_Tool::Tolerance(theEdge);
    return true;
  }

  bool NewParameter(const TopoDS_Vertex& theVertex,
                    const TopoDS_Edge&   theEdge,
                    double&              theParam,
                    double&              theTol) override
  {
    TopLoc_Location aLoc;
    Span            aSpan;
    if (!spanOf(theEdge, aLoc, aSpan))
    {
      return false;
    }
    theParam = aSpan.Map(BRep_Tool::Parameter(theVertex, theEdge));
    theTol   = BRep_Tool::Tolerance(theVertex);
    return true;
  }

  GeomAbs_Shape Continuity(const TopoDS_Edge& theEdge,
                           const TopoDS_Face& theFace1,
                           const TopoDS_Face& theFace2,
                           const TopoDS_Edge&,
                           const TopoDS_Face&,
                           const TopoDS_Face&) override
  {
    return BRep_Tool::Continuity(theEdge, theFace1, theFace2);
  }

private:
  struct Span
  {
    gp_Pnt Origin;
    gp_Dir Direction;
    double Start = 0.0;
    double Scale = 1.0;

    double Map(const double theParam) const { return (theParam - Start) * Scale; }
  };

  static bool spanOf(const TopoDS_Edge& theEdge, TopLoc_Location& theLoc, Span& theSpan)
  {
    double                  aFirst = 0.0, aLast = 0.0;
    occ::handle<Geom_Curve> aCurve = BRep_Tool::Curve(theEdge, theLoc, aFirst, aLast);
    gp_Pnt                  aFrom, aTo;
    if (occ::handle<Geom_BezierCurve> aBezier = occ::down_cast<Geom_BezierCurve>(aCurve))
    {
      if (aBezier->NbPoles() != 2 || aBezier->IsRational())
      {
        return false;
      }
      aFrom = aBezier->Pole(1);
      aTo   = aBezier->Pole(2);
    }
    else if (occ::handle<Geom_BSplineCurve> aSpline = occ::down_cast<Geom_BSplineCurve>(aCurve))
    {
      if (aSpline->NbPoles() != 2 || aSpline->Degree() != 1 || aSpline->IsRational())
      {
        return false;
      }
      aFrom = aSpline->Pole(1);
      aTo   = aSpline->Pole(2);
    }
    else
    {
      return false;
    }
    theSpan.Origin    = aFrom;
    theSpan.Direction = gp_Dir(gp_Vec(aFrom, aTo));
    theSpan.Start     = aCurve->FirstParameter();
    theSpan.Scale     = aFrom.Distance(aTo) / (aCurve->LastParameter() - aCurve->FirstParameter());
    return true;
  }
};

gp_Pnt corner(const double theRadius, const double theAngle, const double theZ)
{
  return gp_Pnt(theRadius * std::cos(theAngle), theRadius * std::sin(theAngle), theZ);
}

TopoDS_Wire helix(const double theRadius, const double theAngle)
{
  occ::handle<Geom_CylindricalSurface> aCylinder =
    new Geom_CylindricalSurface(gp_Ax3(gp::Origin(), gp::DZ(), gp::DX()), theRadius);
  const gp_Vec2d           aStep(THE_TWIST, THE_RISE);
  occ::handle<Geom2d_Line> aLine = new Geom2d_Line(gp_Pnt2d(theAngle, 0.0), gp_Dir2d(aStep));
  TopoDS_Edge anEdge = BRepBuilderAPI_MakeEdge(aLine, aCylinder, 0.0, aStep.Magnitude()).Edge();
  BRepLib::BuildCurves3d(anEdge);
  return BRepBuilderAPI_MakeWire(anEdge).Wire();
}

// A herringbone gear: ruled lofts between helices, mirrored at half the
// height and sewn with two caps. A tooth has a short foot edge between its
// flanks, and each straight edge is a line.
TopoDS_Shape herringboneGear()
{
  NCollection_DynamicArray<gp_Pnt2d> aCorners; // radius, angle
  const double                       anOffsets[4][2] = {{12.5, 0.0},
                                                        {15.0, 0.03 * M_PI},
                                                        {15.0, 0.12 * M_PI},
                                                        {12.5, 0.15 * M_PI}};
  for (int aTooth = 0; aTooth < 12; ++aTooth)
  {
    const double aStart = -M_PI + M_PI / 120.0 + aTooth * M_PI / 6.0;
    for (const auto& anOffset : anOffsets)
    {
      aCorners.Append(gp_Pnt2d(anOffset[0], aStart + anOffset[1]));
    }
  }

  BRepBuilderAPI_Sewing aSewing(1.e-5);
  gp_Trsf               aMirror;
  aMirror.SetMirror(gp_Ax2(gp_Pnt(0.0, 0.0, THE_RISE), gp::DZ()));
  for (int i = 0; i < aCorners.Length(); ++i)
  {
    const gp_Pnt2d&            aFrom = aCorners(i);
    const gp_Pnt2d&            aTo   = aCorners((i + 1) % aCorners.Length());
    BRepOffsetAPI_ThruSections aLoft(false, true);
    aLoft.AddWire(helix(aFrom.X(), aFrom.Y()));
    aLoft.AddWire(helix(aTo.X(), aTo.Y()));
    aLoft.Build();
    BRepTools_Modifier aStraighten(aLoft.Shape(), new StraightEdges());
    const TopoDS_Shape aLower = aStraighten.ModifiedShape(aLoft.Shape());
    aSewing.Add(aLower);
    aSewing.Add(BRepBuilderAPI_Transform(aLower, aMirror, true).Shape());
  }
  for (const double aZ : {0.0, 2.0 * THE_RISE})
  {
    BRepBuilderAPI_MakePolygon aPolygon;
    for (const gp_Pnt2d& aCorner : aCorners)
    {
      aPolygon.Add(corner(aCorner.X(), aCorner.Y(), aZ));
    }
    aPolygon.Close();
    aSewing.Add(BRepBuilderAPI_MakeFace(aPolygon.Wire(), true).Face());
  }
  aSewing.Perform();

  TopExp_Explorer    aShells(aSewing.SewedShape(), TopAbs_SHELL);
  const TopoDS_Shell aShell = TopoDS::Shell(aShells.Current());
  const TopoDS_Solid aSolid = BRepBuilderAPI_MakeSolid(aShell).Solid();
  GProp_GProps       aProps;
  BRepGProp::VolumeProperties(aSolid, aProps);
  if (aProps.Mass() > 0.0)
  {
    return aSolid;
  }
  return BRepBuilderAPI_MakeSolid(TopoDS::Shell(aShell.Reversed())).Solid();
}

// The straight edges where the two halves meet, between a foot and a tip.
NCollection_DynamicArray<TopoDS_Edge> flankEdges(const TopoDS_Shape& theSolid)
{
  NCollection_DynamicArray<TopoDS_Edge>                         aFlanks;
  NCollection_IndexedMap<TopoDS_Shape, TopTools_ShapeMapHasher> anEdges;
  TopExp::MapShapes(theSolid, TopAbs_EDGE, anEdges);
  for (int i = 1; i <= anEdges.Extent(); ++i)
  {
    const TopoDS_Edge& anEdge = TopoDS::Edge(anEdges(i));
    TopoDS_Vertex      aV1, aV2;
    TopExp::Vertices(anEdge, aV1, aV2);
    const gp_Pnt aP1 = BRep_Tool::Pnt(aV1);
    const gp_Pnt aP2 = BRep_Tool::Pnt(aV2);
    if (std::abs(aP1.Z() - THE_RISE) > 1.e-6 || std::abs(aP2.Z() - THE_RISE) > 1.e-6)
    {
      continue;
    }
    if (std::abs(std::hypot(aP1.X(), aP1.Y()) - std::hypot(aP2.X(), aP2.Y())) > 0.5)
    {
      aFlanks.Append(anEdge);
    }
  }
  return aFlanks;
}

double volumeOf(const TopoDS_Shape& theShape)
{
  GProp_GProps aProps;
  BRepGProp::VolumeProperties(theShape, aProps);
  return aProps.Mass();
}

int faceCount(const TopoDS_Shape& theShape)
{
  NCollection_IndexedMap<TopoDS_Shape, TopTools_ShapeMapHasher> aFaces;
  TopExp::MapShapes(theShape, TopAbs_FACE, aFaces);
  return aFaces.Extent();
}
} // namespace

// Where a fillet ends at a vertex of more than three edges, the next edge there
// is a line of 0.654 next to BSpline faces. The fillet meets the line beyond
// that edge, outside the faces, so the end must come from the extension of the
// faces, as it does when the edge is a BSpline.
TEST(ChFi3d_Builder_C1Test, FilletEndsOnLineEdgeOfBSplineFaces)
{
  const TopoDS_Shape aGear = herringboneGear();
  ASSERT_TRUE(BRepCheck_Analyzer(aGear).IsValid());
  const double aVolume = volumeOf(aGear);
  const int    aFaces  = faceCount(aGear);

  const NCollection_DynamicArray<TopoDS_Edge> aFlanks = flankEdges(aGear);
  ASSERT_EQ(aFlanks.Length(), 24);
  for (const TopoDS_Edge& aFlank : aFlanks)
  {
    TopLoc_Location aLoc;
    double          aFirst = 0.0, aLast = 0.0;
    EXPECT_FALSE(occ::down_cast<Geom_Line>(BRep_Tool::Curve(aFlank, aLoc, aFirst, aLast)).IsNull());

    BRepFilletAPI_MakeFillet aFillet(aGear);
    aFillet.Add(0.3, aFlank);
    aFillet.Build();
    ASSERT_TRUE(aFillet.IsDone());
    EXPECT_TRUE(BRepCheck_Analyzer(aFillet.Shape()).IsValid());
    EXPECT_EQ(faceCount(aFillet.Shape()), aFaces + 1);
    EXPECT_LT(std::abs(volumeOf(aFillet.Shape()) - aVolume), 0.05);
  }
}

TEST(ChFi3d_Builder_C1Test, FilletsEndOnLineEdgesOfBSplineFacesTogether)
{
  const TopoDS_Shape       aGear = herringboneGear();
  BRepFilletAPI_MakeFillet aFillet(aGear);
  for (const TopoDS_Edge& aFlank : flankEdges(aGear))
  {
    aFillet.Add(0.3, aFlank);
  }
  aFillet.Build();
  ASSERT_TRUE(aFillet.IsDone());
  EXPECT_TRUE(BRepCheck_Analyzer(aFillet.Shape()).IsValid());
  EXPECT_EQ(faceCount(aFillet.Shape()), faceCount(aGear) + 24);
  EXPECT_LT(std::abs(volumeOf(aFillet.Shape()) - volumeOf(aGear)), 0.05);
}
