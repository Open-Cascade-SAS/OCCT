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

#include <GeomHash_GeometryAppender.hxx>

#include <Geom2d_BSplineCurve.hxx>
#include <Geom2d_BezierCurve.hxx>
#include <Geom2d_Circle.hxx>
#include <Geom2d_Ellipse.hxx>
#include <Geom2d_Hyperbola.hxx>
#include <Geom2d_Line.hxx>
#include <Geom2d_OffsetCurve.hxx>
#include <Geom2d_Parabola.hxx>
#include <Geom2d_TrimmedCurve.hxx>
#include <Geom_BSplineCurve.hxx>
#include <Geom_BSplineSurface.hxx>
#include <Geom_BezierCurve.hxx>
#include <Geom_BezierSurface.hxx>
#include <Geom_Circle.hxx>
#include <Geom_ConicalSurface.hxx>
#include <Geom_CylindricalSurface.hxx>
#include <Geom_Ellipse.hxx>
#include <Geom_Hyperbola.hxx>
#include <Geom_Line.hxx>
#include <Geom_OffsetCurve.hxx>
#include <Geom_OffsetSurface.hxx>
#include <Geom_Parabola.hxx>
#include <Geom_Plane.hxx>
#include <Geom_RectangularTrimmedSurface.hxx>
#include <Geom_SphericalSurface.hxx>
#include <Geom_SurfaceOfLinearExtrusion.hxx>
#include <Geom_SurfaceOfRevolution.hxx>
#include <Geom_ToroidalSurface.hxx>
#include <Geom_TrimmedCurve.hxx>
#include <gp_Pln.hxx>

#include <gtest/gtest.h>

#include <utility>
#include <vector>

namespace
{
occ::handle<Geom_BezierCurve> bezier3d(const double theY)
{
  NCollection_Array1<gp_Pnt> aPoles(1, 3);
  aPoles.SetValue(1, gp_Pnt(0.0, 0.0, 0.0));
  aPoles.SetValue(2, gp_Pnt(1.0, theY, 0.0));
  aPoles.SetValue(3, gp_Pnt(2.0, 0.0, 0.0));
  return new Geom_BezierCurve(aPoles);
}

occ::handle<Geom_BSplineCurve> bspline3d(const double theY)
{
  NCollection_Array1<gp_Pnt> aPoles(1, 3);
  aPoles.SetValue(1, gp_Pnt(0.0, 0.0, 0.0));
  aPoles.SetValue(2, gp_Pnt(1.0, theY, 0.0));
  aPoles.SetValue(3, gp_Pnt(2.0, 0.0, 0.0));
  NCollection_Array1<double> aKnots(1, 2);
  aKnots.SetValue(1, 0.0);
  aKnots.SetValue(2, 1.0);
  NCollection_Array1<int> aMultiplicities(1, 2);
  aMultiplicities.SetValue(1, 3);
  aMultiplicities.SetValue(2, 3);
  return new Geom_BSplineCurve(aPoles, aKnots, aMultiplicities, 2);
}

occ::handle<Geom2d_BezierCurve> bezier2d(const double theY)
{
  NCollection_Array1<gp_Pnt2d> aPoles(1, 3);
  aPoles.SetValue(1, gp_Pnt2d(0.0, 0.0));
  aPoles.SetValue(2, gp_Pnt2d(1.0, theY));
  aPoles.SetValue(3, gp_Pnt2d(2.0, 0.0));
  return new Geom2d_BezierCurve(aPoles);
}

occ::handle<Geom2d_BSplineCurve> bspline2d(const double theY)
{
  NCollection_Array1<gp_Pnt2d> aPoles(1, 3);
  aPoles.SetValue(1, gp_Pnt2d(0.0, 0.0));
  aPoles.SetValue(2, gp_Pnt2d(1.0, theY));
  aPoles.SetValue(3, gp_Pnt2d(2.0, 0.0));
  NCollection_Array1<double> aKnots(1, 2);
  aKnots.SetValue(1, 0.0);
  aKnots.SetValue(2, 1.0);
  NCollection_Array1<int> aMultiplicities(1, 2);
  aMultiplicities.SetValue(1, 3);
  aMultiplicities.SetValue(2, 3);
  return new Geom2d_BSplineCurve(aPoles, aKnots, aMultiplicities, 2);
}

occ::handle<Geom_BezierSurface> bezierSurface(const double theZ)
{
  NCollection_Array2<gp_Pnt> aPoles(1, 2, 1, 2);
  aPoles.SetValue(1, 1, gp_Pnt(0.0, 0.0, 0.0));
  aPoles.SetValue(1, 2, gp_Pnt(0.0, 1.0, 0.0));
  aPoles.SetValue(2, 1, gp_Pnt(1.0, 0.0, 0.0));
  aPoles.SetValue(2, 2, gp_Pnt(1.0, 1.0, theZ));
  return new Geom_BezierSurface(aPoles);
}

occ::handle<Geom_BSplineSurface> bsplineSurface(const double theZ)
{
  NCollection_Array2<gp_Pnt> aPoles(1, 2, 1, 2);
  aPoles.SetValue(1, 1, gp_Pnt(0.0, 0.0, 0.0));
  aPoles.SetValue(1, 2, gp_Pnt(0.0, 1.0, 0.0));
  aPoles.SetValue(2, 1, gp_Pnt(1.0, 0.0, 0.0));
  aPoles.SetValue(2, 2, gp_Pnt(1.0, 1.0, theZ));
  NCollection_Array1<double> aUKnots(1, 2);
  NCollection_Array1<double> aVKnots(1, 2);
  aUKnots.SetValue(1, 0.0);
  aUKnots.SetValue(2, 1.0);
  aVKnots.SetValue(1, 0.0);
  aVKnots.SetValue(2, 1.0);
  NCollection_Array1<int> aUMultiplicities(1, 2);
  NCollection_Array1<int> aVMultiplicities(1, 2);
  aUMultiplicities.Init(2);
  aVMultiplicities.Init(2);
  return new Geom_BSplineSurface(aPoles,
                                 aUKnots,
                                 aVKnots,
                                 aUMultiplicities,
                                 aVMultiplicities,
                                 1,
                                 1);
}
} // namespace

TEST(GeomHash_GeometryAppenderTest, AllCurve3dKindsHashTheirDefinition)
{
  const gp_Ax2                 anAxis(gp_Pnt(0.0, 0.0, 0.0), gp::DZ(), gp::DX());
  const occ::handle<Geom_Line> aBasis = new Geom_Line(gp_Pnt(), gp::DX());
  const std::vector<std::pair<occ::handle<Geom_Curve>, occ::handle<Geom_Curve>>> aPairs = {
    {new Geom_Line(gp_Pnt(), gp::DX()), new Geom_Line(gp_Pnt(1.0, 0.0, 0.0), gp::DX())},
    {new Geom_Circle(anAxis, 1.0), new Geom_Circle(anAxis, 2.0)},
    {new Geom_Ellipse(anAxis, 2.0, 1.0), new Geom_Ellipse(anAxis, 3.0, 1.0)},
    {new Geom_Parabola(anAxis, 1.0), new Geom_Parabola(anAxis, 2.0)},
    {new Geom_Hyperbola(anAxis, 2.0, 1.0), new Geom_Hyperbola(anAxis, 3.0, 1.0)},
    {bezier3d(0.5), bezier3d(1.0)},
    {bspline3d(0.5), bspline3d(1.0)},
    {new Geom_TrimmedCurve(new Geom_Circle(anAxis, 2.0), 0.0, 1.0),
     new Geom_TrimmedCurve(new Geom_Circle(anAxis, 2.0), 0.0, 2.0)},
    {new Geom_OffsetCurve(aBasis, 1.0, gp::DZ()), new Geom_OffsetCurve(aBasis, 2.0, gp::DZ())}};

  const GeomHash_GeometryAppender<> aHasher;
  for (const auto& aPair : aPairs)
  {
    EXPECT_NE(aHasher(aPair.first), aHasher(aPair.second));
  }
}

TEST(GeomHash_GeometryAppenderTest, AllCurve2dKindsHashTheirDefinition)
{
  const gp_Ax22d                 anAxis(gp_Pnt2d(), gp::DX2d(), gp::DY2d());
  const occ::handle<Geom2d_Line> aBasis = new Geom2d_Line(gp_Pnt2d(), gp::DX2d());
  const std::vector<std::pair<occ::handle<Geom2d_Curve>, occ::handle<Geom2d_Curve>>> aPairs = {
    {new Geom2d_Line(gp_Pnt2d(), gp::DX2d()), new Geom2d_Line(gp_Pnt2d(1.0, 0.0), gp::DX2d())},
    {new Geom2d_Circle(anAxis, 1.0), new Geom2d_Circle(anAxis, 2.0)},
    {new Geom2d_Ellipse(anAxis, 2.0, 1.0), new Geom2d_Ellipse(anAxis, 3.0, 1.0)},
    {new Geom2d_Parabola(anAxis, 1.0), new Geom2d_Parabola(anAxis, 2.0)},
    {new Geom2d_Hyperbola(anAxis, 2.0, 1.0), new Geom2d_Hyperbola(anAxis, 3.0, 1.0)},
    {bezier2d(0.5), bezier2d(1.0)},
    {bspline2d(0.5), bspline2d(1.0)},
    {new Geom2d_TrimmedCurve(new Geom2d_Circle(anAxis, 2.0), 0.0, 1.0),
     new Geom2d_TrimmedCurve(new Geom2d_Circle(anAxis, 2.0), 0.0, 2.0)},
    {new Geom2d_OffsetCurve(aBasis, 1.0), new Geom2d_OffsetCurve(aBasis, 2.0)}};

  const GeomHash_GeometryAppender<> aHasher;
  for (const auto& aPair : aPairs)
  {
    EXPECT_NE(aHasher(aPair.first), aHasher(aPair.second));
  }
}

TEST(GeomHash_GeometryAppenderTest, AllSurfaceKindsHashTheirDefinition)
{
  const gp_Ax3                  anAxis(gp_Pnt(), gp::DZ(), gp::DX());
  const occ::handle<Geom_Line>  aBasis = new Geom_Line(gp_Pnt(1.0, 0.0, 0.0), gp::DZ());
  const occ::handle<Geom_Plane> aPlane = new Geom_Plane(gp_Pln(anAxis));
  const std::vector<std::pair<occ::handle<Geom_Surface>, occ::handle<Geom_Surface>>> aPairs = {
    {new Geom_Plane(gp_Pln(anAxis)), new Geom_Plane(gp_Pln(gp_Pnt(0.0, 0.0, 1.0), gp::DZ()))},
    {new Geom_CylindricalSurface(anAxis, 1.0), new Geom_CylindricalSurface(anAxis, 2.0)},
    {new Geom_ConicalSurface(anAxis, 0.2, 1.0), new Geom_ConicalSurface(anAxis, 0.3, 1.0)},
    {new Geom_SphericalSurface(anAxis, 1.0), new Geom_SphericalSurface(anAxis, 2.0)},
    {new Geom_ToroidalSurface(anAxis, 3.0, 1.0), new Geom_ToroidalSurface(anAxis, 4.0, 1.0)},
    {new Geom_SurfaceOfLinearExtrusion(aBasis, gp::DX()),
     new Geom_SurfaceOfLinearExtrusion(aBasis, gp::DY())},
    {new Geom_SurfaceOfRevolution(aBasis, gp_Ax1(gp_Pnt(), gp::DZ())),
     new Geom_SurfaceOfRevolution(aBasis, gp_Ax1(gp_Pnt(), gp::DX()))},
    {bezierSurface(0.5), bezierSurface(1.0)},
    {bsplineSurface(0.5), bsplineSurface(1.0)},
    {new Geom_RectangularTrimmedSurface(aPlane, 0.0, 1.0, 0.0, 1.0),
     new Geom_RectangularTrimmedSurface(aPlane, 0.0, 2.0, 0.0, 1.0)},
    {new Geom_OffsetSurface(aPlane, 1.0), new Geom_OffsetSurface(aPlane, 2.0)}};

  const GeomHash_GeometryAppender<> aHasher;
  for (const auto& aPair : aPairs)
  {
    EXPECT_NE(aHasher(aPair.first), aHasher(aPair.second));
  }
}

TEST(GeomHash_GeometryAppenderTest, DefaultHashIsFixedWidth)
{
  using Hasher = GeomHash_GeometryAppender<>;
  EXPECT_EQ(sizeof(Hasher::result_type), sizeof(uint64_t));
}

TEST(GeomHash_AccumulatorTest, HashDependsOnBytesNotAppendBoundaries)
{
  GeomHash_Accumulator<uint64_t> aWhole;
  aWhole.AppendUInt64(0x1122334455667788ull);

  GeomHash_Accumulator<uint64_t> aSplit;
  aSplit.AppendUInt32(0x55667788u);
  aSplit.AppendUInt32(0x11223344u);

  EXPECT_EQ(aWhole.Finish(), aSplit.Finish());
}
