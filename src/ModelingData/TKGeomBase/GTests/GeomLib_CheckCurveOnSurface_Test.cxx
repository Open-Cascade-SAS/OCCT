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

#include <Adaptor3d_CurveOnSurface.hxx>
#include <Geom2dAdaptor_Curve.hxx>
#include <Geom2d_Line.hxx>
#include <GeomAdaptor_Curve.hxx>
#include <GeomAdaptor_Surface.hxx>
#include <GeomLib_CheckCurveOnSurface.hxx>
#include <Geom_Circle.hxx>
#include <Geom_CylindricalSurface.hxx>
#include <Geom_Line.hxx>
#include <Geom_Plane.hxx>
#include <Precision.hxx>
#include <gp_Ax2.hxx>
#include <gp_Ax3.hxx>
#include <gp_Dir.hxx>
#include <gp_Dir2d.hxx>
#include <gp_Pnt.hxx>
#include <gp_Pnt2d.hxx>

namespace
{
// Circle of radius theRadius at height theHeight on a cylinder of radius
// theCylRadius around OZ; the pcurve is the corresponding iso line in UV.
GeomLib_CheckCurveOnSurface makeCircleOnCylinderCheck(const double theCircRadius,
                                                      const double theCylRadius,
                                                      const double theHeight,
                                                      double&      theMaxDistance)
{
  const double aFirst = 0.0, aLast = 2.0 * M_PI;

  const occ::handle<Geom_Circle> aCirc = new Geom_Circle(
    gp_Ax2(gp_Pnt(0.0, 0.0, theHeight), gp_Dir(0.0, 0.0, 1.0), gp_Dir(1.0, 0.0, 0.0)),
    theCircRadius);
  const occ::handle<GeomAdaptor_Curve> aC3d = new GeomAdaptor_Curve(aCirc, aFirst, aLast);

  const occ::handle<Geom_CylindricalSurface> aCyl = new Geom_CylindricalSurface(
    gp_Ax3(gp_Pnt(0.0, 0.0, 0.0), gp_Dir(0.0, 0.0, 1.0), gp_Dir(1.0, 0.0, 0.0)),
    theCylRadius);
  const occ::handle<Geom2d_Line> aPLine =
    new Geom2d_Line(gp_Pnt2d(0.0, theHeight), gp_Dir2d(1.0, 0.0));

  const occ::handle<Geom2dAdaptor_Curve>      aC2d = new Geom2dAdaptor_Curve(aPLine, aFirst, aLast);
  const occ::handle<GeomAdaptor_Surface>      aSurf = new GeomAdaptor_Surface(aCyl);
  const occ::handle<Adaptor3d_CurveOnSurface> aCoS  = new Adaptor3d_CurveOnSurface(aC2d, aSurf);

  GeomLib_CheckCurveOnSurface aCheck;
  aCheck.Init(aC3d, Precision::PConfusion());
  aCheck.Perform(aCoS);
  theMaxDistance = aCheck.MaxDistance();
  return aCheck;
}
} // namespace

TEST(GeomLib_CheckCurveOnSurfaceTest, AnalyticCoincident_ReportsZeroDeviation)
{
  double     aMaxDist = RealLast();
  const auto aCheck   = makeCircleOnCylinderCheck(2.0, 2.0, 5.0, aMaxDist);

  EXPECT_TRUE(aCheck.IsDone());
  EXPECT_LE(aMaxDist, Precision::Confusion());
}

TEST(GeomLib_CheckCurveOnSurfaceTest, AnalyticDeviating_ReportsActualDeviation)
{
  // The 3D circle is 0.05 smaller than the cylinder carrying the pcurve: the
  // deviation is constant and equal to the radius difference. This guards the
  // sampled flat-deviation shortcut against swallowing real deviations.
  double     aMaxDist = RealLast();
  const auto aCheck   = makeCircleOnCylinderCheck(1.95, 2.0, 5.0, aMaxDist);

  EXPECT_TRUE(aCheck.IsDone());
  EXPECT_NEAR(aMaxDist, 0.05, 1.0e-9);
}

TEST(GeomLib_CheckCurveOnSurfaceTest, LineOnPlane_ReportsZeroDeviation)
{
  const double aFirst = -10.0, aLast = 10.0;

  const occ::handle<Geom_Line> aLine = new Geom_Line(gp_Pnt(0.0, 0.0, 0.0), gp_Dir(1.0, 0.0, 0.0));
  const occ::handle<GeomAdaptor_Curve> aC3d = new GeomAdaptor_Curve(aLine, aFirst, aLast);

  const occ::handle<Geom_Plane> aPlane =
    new Geom_Plane(gp_Ax3(gp_Pnt(0.0, 0.0, 0.0), gp_Dir(0.0, 0.0, 1.0), gp_Dir(1.0, 0.0, 0.0)));
  const occ::handle<Geom2d_Line> aPLine = new Geom2d_Line(gp_Pnt2d(0.0, 0.0), gp_Dir2d(1.0, 0.0));

  const occ::handle<Geom2dAdaptor_Curve>      aC2d = new Geom2dAdaptor_Curve(aPLine, aFirst, aLast);
  const occ::handle<GeomAdaptor_Surface>      aSurf = new GeomAdaptor_Surface(aPlane);
  const occ::handle<Adaptor3d_CurveOnSurface> aCoS  = new Adaptor3d_CurveOnSurface(aC2d, aSurf);

  GeomLib_CheckCurveOnSurface aCheck;
  aCheck.Init(aC3d, Precision::PConfusion());
  aCheck.Perform(aCoS);

  EXPECT_TRUE(aCheck.IsDone());
  EXPECT_LE(aCheck.MaxDistance(), Precision::Confusion());
}
