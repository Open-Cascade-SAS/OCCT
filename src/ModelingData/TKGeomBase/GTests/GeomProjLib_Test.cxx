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

#include <GCPnts_AbscissaPoint.hxx>
#include <GeomProjLib.hxx>
#include <GeomAdaptor_Curve.hxx>
#include <Geom_Hyperbola.hxx>
#include <Geom_Parabola.hxx>
#include <Geom_Plane.hxx>
#include <Geom_TrimmedCurve.hxx>
#include <Precision.hxx>
#include <gp_Ax2.hxx>
#include <gp_Dir.hxx>
#include <gp_Pln.hxx>
#include <gp_Pnt.hxx>

namespace
{
occ::handle<Geom_Plane> makeOCC31661Plane()
{
  return new Geom_Plane(gp_Pln(gp_Pnt(0.0, 0.0, 0.0), gp_Dir(0.0, 0.0, 1.0)));
}

gp_Ax2 makeOCC31661CurveAxis()
{
  return gp_Ax2(gp_Pnt(0.0, 0.0, 0.0), gp_Dir(1.0, 1.0, 1.0), gp_Dir(2.0, 0.0, -2.0));
}
} // namespace

// Migrated from tests/bugs/moddata_3/bug31661_1. Projection of a parabola onto
// the XY plane must preserve its conic type and the trimmed parameter range.
TEST(GeomProjLibTest, OCC31661_1_ProjectParabolaOnPlane)
{
  const occ::handle<Geom_Parabola> aParabola =
    new Geom_Parabola(makeOCC31661CurveAxis(), 10.0);
  const occ::handle<Geom_Plane> aPlane = makeOCC31661Plane();

  const occ::handle<Geom_Curve> aProjected =
    GeomProjLib::ProjectOnPlane(aParabola, aPlane, gp_Dir(0.0, 0.0, 1.0), false);
  ASSERT_FALSE(aProjected.IsNull());
  EXPECT_STREQ(aProjected->DynamicType()->Name(), "Geom_Parabola");

  const occ::handle<Geom_TrimmedCurve> aTrimmed =
    new Geom_TrimmedCurve(aParabola, -100.0, 100.0);
  const occ::handle<Geom_Curve> aProjectedTrimmed =
    GeomProjLib::ProjectOnPlane(aTrimmed, aPlane, gp_Dir(0.0, 0.0, 1.0), false);
  ASSERT_FALSE(aProjectedTrimmed.IsNull());
  EXPECT_STREQ(aProjectedTrimmed->DynamicType()->Name(), "Geom_TrimmedCurve");

  const GeomAdaptor_Curve anAdaptor(aProjectedTrimmed);
  EXPECT_NEAR(GCPnts_AbscissaPoint::Length(anAdaptor, Precision::Confusion()),
              408.40363195229503,
              2.0e-5);
  // The parameter checks retain DRAW's references; the wider bound reflects
  // the relative numerical tolerance used by its checkreal command.
  EXPECT_NEAR(aProjectedTrimmed->FirstParameter(), -91.077748943768597, 2.0e-6);
  EXPECT_NEAR(aProjectedTrimmed->LastParameter(), 72.221567418462357, 2.0e-6);
}

// Migrated from tests/bugs/moddata_3/bug31661_2. Projection of a hyperbola
// must not crash and must retain the hyperbolic conic type after trimming.
TEST(GeomProjLibTest, OCC31661_2_ProjectHyperbolaOnPlane)
{
  const occ::handle<Geom_Hyperbola> aHyperbola =
    new Geom_Hyperbola(makeOCC31661CurveAxis(), 10.0, 10.0);
  const occ::handle<Geom_Plane> aPlane = makeOCC31661Plane();

  const occ::handle<Geom_Curve> aProjected =
    GeomProjLib::ProjectOnPlane(aHyperbola, aPlane, gp_Dir(0.0, 0.0, 1.0), false);
  ASSERT_FALSE(aProjected.IsNull());
  EXPECT_STREQ(aProjected->DynamicType()->Name(), "Geom_Hyperbola");

  const occ::handle<Geom_TrimmedCurve> aTrimmed =
    new Geom_TrimmedCurve(aHyperbola, -5.0, 5.0);
  const occ::handle<Geom_Curve> aProjectedTrimmed =
    GeomProjLib::ProjectOnPlane(aTrimmed, aPlane, gp_Dir(0.0, 0.0, 1.0), false);
  ASSERT_FALSE(aProjectedTrimmed.IsNull());
  EXPECT_STREQ(aProjectedTrimmed->DynamicType()->Name(), "Geom_TrimmedCurve");

  const GeomAdaptor_Curve anAdaptor(aProjectedTrimmed);
  EXPECT_NEAR(GCPnts_AbscissaPoint::Length(anAdaptor, Precision::Confusion()),
              1664.3732976598988,
              1.0e-6);
  EXPECT_NEAR(aProjectedTrimmed->FirstParameter(), -5.23179933356147, 1.0e-7);
  EXPECT_NEAR(aProjectedTrimmed->LastParameter(), 4.76820064934972, 1.0e-7);
}
