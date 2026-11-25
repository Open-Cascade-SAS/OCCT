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

#include <BRepAdaptor_Curve.hxx>
#include <BRepBuilderAPI_MakeEdge.hxx>
#include <BRepBuilderAPI_MakePolygon.hxx>
#include <BRepBuilderAPI_MakeWire.hxx>
#include <BRepFilletAPI_MakeFillet.hxx>
#include <BRepOffsetAPI_ThruSections.hxx>
#include <NCollection_IndexedMap.hxx>
#include <TopExp.hxx>
#include <TopTools_ShapeMapHasher.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Edge.hxx>
#include <TopoDS_Shape.hxx>
#include <TopoDS_Wire.hxx>
#include <gce_MakeElips.hxx>
#include <gp_Ax2.hxx>
#include <gp_Dir.hxx>
#include <gp_Pnt.hxx>

#include <gtest/gtest.h>

namespace
{
//==================================================================================================

TopoDS_Wire makeRectangle(const double theZ, const double theHalfX, const double theHalfY)
{
  BRepBuilderAPI_MakePolygon aPolygon;
  aPolygon.Add(gp_Pnt(-theHalfX, -theHalfY, theZ));
  aPolygon.Add(gp_Pnt(theHalfX, -theHalfY, theZ));
  aPolygon.Add(gp_Pnt(theHalfX, theHalfY, theZ));
  aPolygon.Add(gp_Pnt(-theHalfX, theHalfY, theZ));
  aPolygon.Close();
  return aPolygon.Wire();
}

//==================================================================================================

//! Smooth loft through a rectangle at z = 0, an ellipse at z = 30 and a rectangle at z = 60.
TopoDS_Shape makeLoft()
{
  const gp_Ax2      anEllipseAxes(gp_Pnt(3.0, 0.0, 30.0), gp_Dir(0.0, 0.0, 1.0));
  const TopoDS_Wire anEllipse =
    BRepBuilderAPI_MakeWire(
      BRepBuilderAPI_MakeEdge(gce_MakeElips(anEllipseAxes, 20.0, 12.0).Value()).Edge())
      .Wire();

  BRepOffsetAPI_ThruSections aLoft(true, false);
  aLoft.AddWire(makeRectangle(0.0, 30.0, 20.0));
  aLoft.AddWire(anEllipse);
  aLoft.AddWire(makeRectangle(60.0, 25.0, 15.0));
  aLoft.Build();
  return aLoft.Shape();
}

//==================================================================================================

//! Returns the edge whose mid-parameter point is nearest to the given point.
TopoDS_Edge findEdgeNear(const TopoDS_Shape& theShape, const gp_Pnt& thePoint)
{
  NCollection_IndexedMap<TopoDS_Shape, TopTools_ShapeMapHasher> anEdges;
  TopExp::MapShapes(theShape, TopAbs_EDGE, anEdges);
  TopoDS_Edge aBest;
  double      aBestDistance = RealLast();
  for (int anIndex = 1; anIndex <= anEdges.Extent(); ++anIndex)
  {
    const TopoDS_Edge& anEdge = TopoDS::Edge(anEdges(anIndex));
    BRepAdaptor_Curve  aCurve(anEdge);
    const gp_Pnt aMiddle = aCurve.Value(0.5 * (aCurve.FirstParameter() + aCurve.LastParameter()));
    if (aMiddle.Distance(thePoint) < aBestDistance)
    {
      aBestDistance = aMiddle.Distance(thePoint);
      aBest         = anEdge;
    }
  }
  return aBest;
}

//==================================================================================================

//! Adds the three edges that meet at the vertex (25, 15, 60) of the loft.
void addCornerEdges(BRepFilletAPI_MakeFillet& theFillet,
                    const TopoDS_Shape&       theLoft,
                    const double              theRadius)
{
  theFillet.Add(theRadius, findEdgeNear(theLoft, gp_Pnt(17.21, 8.52, 29.09)));
  theFillet.Add(theRadius, findEdgeNear(theLoft, gp_Pnt(25.0, 7.5, 60.0)));
  theFillet.Add(theRadius, findEdgeNear(theLoft, gp_Pnt(23.75, 15.0, 60.0)));
}
} // namespace

//==================================================================================================

// At these radii the normal of a constraint curve of the corner is null on some platforms. Whether
// the corner is built or declined depends on the platform; the call must return either way.
TEST(BRepFilletAPI_MakeFilletTest, SmoothLoftCorner_NullNormalRadii_ReturnsWithoutCrash)
{
  const TopoDS_Shape aLoft = makeLoft();
  ASSERT_FALSE(aLoft.IsNull());
  for (const double aRadius : {1.0, 1.3, 2.1})
  {
    SCOPED_TRACE(aRadius);
    BRepFilletAPI_MakeFillet aFillet(aLoft);
    addCornerEdges(aFillet, aLoft, aRadius);
    aFillet.Build();
    if (aFillet.IsDone())
    {
      EXPECT_FALSE(aFillet.Shape().IsNull());
    }
  }
}
