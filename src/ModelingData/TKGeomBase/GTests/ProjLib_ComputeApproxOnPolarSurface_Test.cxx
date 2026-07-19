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

#include <ProjLib_ComputeApproxOnPolarSurface.hxx>

#include <Adaptor2d_Curve2d.hxx>
#include <Adaptor3d_Curve.hxx>
#include <Adaptor3d_Surface.hxx>
#include <Geom2d_BSplineCurve.hxx>
#include <GeomAdaptor_Curve.hxx>
#include <GeomAdaptor_Surface.hxx>
#include <Geom_Circle.hxx>
#include <Geom_CylindricalSurface.hxx>
#include <Precision.hxx>
#include <gp_Ax2.hxx>
#include <gp_Ax3.hxx>
#include <gp_Dir.hxx>
#include <gp_Pnt.hxx>
#include <gp_Pnt2d.hxx>

#include <gtest/gtest.h>

#include <algorithm>

namespace
{
static void checkProjectionAtParam(const occ::handle<Geom2d_BSplineCurve>&     thePCurve,
                                   const occ::handle<Geom_CylindricalSurface>& theSurface,
                                   const occ::handle<Geom_Circle>&             theCurve,
                                   const double                                theParam,
                                   const double                                theTol)
{
  const gp_Pnt2d aUV    = thePCurve->Value(theParam);
  const gp_Pnt   aProjP = theSurface->Value(aUV.X(), aUV.Y());
  const gp_Pnt   aRefP  = theCurve->Value(theParam);
  EXPECT_LE(aProjP.Distance(aRefP), theTol);
}
} // namespace

TEST(ProjLib_ComputeApproxOnPolarSurfaceTest, BuildInitialCurveAndProjectOnCylinder_Done)
{
  occ::handle<Geom_Circle> aCurve =
    new Geom_Circle(gp_Ax2(gp_Pnt(0.0, 0.0, 2.0), gp_Dir(0.0, 0.0, 1.0), gp_Dir(1.0, 0.0, 0.0)),
                    5.0);
  occ::handle<Geom_CylindricalSurface> aSurface = new Geom_CylindricalSurface(
    gp_Ax3(gp_Pnt(0.0, 0.0, 0.0), gp_Dir(0.0, 0.0, 1.0), gp_Dir(1.0, 0.0, 0.0)),
    5.0);

  occ::handle<Adaptor3d_Curve>   aCurveAdaptor = new GeomAdaptor_Curve(aCurve);
  occ::handle<Adaptor3d_Surface> aSurfAdaptor  = new GeomAdaptor_Surface(aSurface);

  ProjLib_ComputeApproxOnPolarSurface aProjector;
  aProjector.SetTolerance(1.0e-6);

  const occ::handle<Adaptor2d_Curve2d> anInitCurve2d =
    aProjector.BuildInitialCurve2d(aCurveAdaptor, aSurfAdaptor);
  ASSERT_TRUE(aProjector.IsDone());
  ASSERT_FALSE(anInitCurve2d.IsNull());

  const occ::handle<Geom2d_BSplineCurve> aPCurve =
    aProjector.ProjectUsingInitialCurve2d(aCurveAdaptor, aSurfAdaptor, anInitCurve2d);
  ASSERT_TRUE(aProjector.IsDone());
  ASSERT_FALSE(aPCurve.IsNull());

  const double aFirst = std::max(aCurve->FirstParameter(), aPCurve->FirstParameter());
  const double aLast  = std::min(aCurve->LastParameter(), aPCurve->LastParameter());
  ASSERT_LT(aFirst, aLast);

  checkProjectionAtParam(aPCurve, aSurface, aCurve, aFirst, 1.0e-3);
  checkProjectionAtParam(aPCurve, aSurface, aCurve, 0.5 * (aFirst + aLast), 1.0e-3);
  checkProjectionAtParam(aPCurve, aSurface, aCurve, aLast, 1.0e-3);
}
