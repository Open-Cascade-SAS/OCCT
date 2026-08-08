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

#include <GeomAPI_IntCS.hxx>
#include <Geom_BSplineSurface.hxx>
#include <Geom_BezierSurface.hxx>
#include <Geom_Circle.hxx>
#include <Geom_Parabola.hxx>
#include <Geom_SurfaceOfLinearExtrusion.hxx>
#include <Geom_SurfaceOfRevolution.hxx>
#include <Geom_ToroidalSurface.hxx>
#include <Geom_TrimmedCurve.hxx>
#include <NCollection_Array1.hxx>
#include <NCollection_Array2.hxx>
#include <gp_Ax1.hxx>
#include <gp_Ax2.hxx>
#include <gp_Ax3.hxx>
#include <gp_Dir.hxx>
#include <gp_Pnt.hxx>

namespace
{
static occ::handle<Geom_Curve> makeOCC26979Curve()
{
  const occ::handle<Geom_Parabola> aParabola =
    new Geom_Parabola(gp_Ax2(gp_Pnt(-5.0, -1.0, 0.25), gp_Dir(0.0, 0.0, 1.0)), 0.25);
  return new Geom_TrimmedCurve(aParabola, 0.0, 1000.0);
}

static void expectOCC26979Intersection(const occ::handle<Geom_Surface>& theSurface)
{
  GeomAPI_IntCS anIntersection(makeOCC26979Curve(), theSurface);
  ASSERT_TRUE(anIntersection.IsDone());
  EXPECT_GT(anIntersection.NbPoints() + anIntersection.NbSegments(), 0);
}

static occ::handle<Geom_BezierSurface> makeOCC26979BezierSurface()
{
  NCollection_Array2<gp_Pnt> aPoles(1, 3, 1, 3);
  aPoles(1, 1) = gp_Pnt(-4.0, 0.0, -1.0);
  aPoles(2, 1) = gp_Pnt(-4.0, 1.0, 0.0);
  aPoles(3, 1) = gp_Pnt(-4.0, 0.0, 1.0);
  aPoles(1, 2) = gp_Pnt(-3.0, 1.0, -1.0);
  aPoles(2, 2) = gp_Pnt(-3.0, 2.0, 0.0);
  aPoles(3, 2) = gp_Pnt(-3.0, 1.0, 1.0);
  aPoles(1, 3) = gp_Pnt(-2.0, 0.0, -1.0);
  aPoles(2, 3) = gp_Pnt(-2.0, 1.0, 0.0);
  aPoles(3, 3) = gp_Pnt(-2.0, 0.0, 1.0);
  return new Geom_BezierSurface(aPoles);
}

static occ::handle<Geom_BSplineSurface> makeOCC26979BSplineSurface()
{
  NCollection_Array1<double> aUKnots(1, 5), aVKnots(1, 5);
  NCollection_Array1<int>    aUMultiplicities(1, 5), aVMultiplicities(1, 5);
  for (int anIndex = 1; anIndex <= 5; ++anIndex)
  {
    aUKnots(anIndex)            = anIndex - 1;
    aVKnots(anIndex)            = anIndex - 1;
    aUMultiplicities(anIndex)   = 1;
    aVMultiplicities(anIndex)   = 1;
  }

  NCollection_Array2<gp_Pnt> aPoles(1, 3, 1, 3);
  aPoles(1, 1) = gp_Pnt(-3.0, 0.5, -1.0);
  aPoles(2, 1) = gp_Pnt(-2.0, 0.5, 0.0);
  aPoles(3, 1) = gp_Pnt(-3.0, 0.5, 1.0);
  aPoles(1, 2) = gp_Pnt(-2.0, 1.5, -1.0);
  aPoles(2, 2) = gp_Pnt(-1.0, 1.5, 0.0);
  aPoles(3, 2) = gp_Pnt(-2.0, 1.5, 1.0);
  aPoles(1, 3) = gp_Pnt(-3.0, 2.5, -1.0);
  aPoles(2, 3) = gp_Pnt(-2.0, 2.5, 0.0);
  aPoles(3, 3) = gp_Pnt(-3.0, 2.5, 1.0);

  NCollection_Array2<double> aWeights(1, 3, 1, 3);
  for (int aU = 1; aU <= 3; ++aU)
  {
    for (int aV = 1; aV <= 3; ++aV)
    {
      aWeights(aU, aV) = 1.0;
    }
  }
  return new Geom_BSplineSurface(aPoles,
                                 aWeights,
                                 aUKnots,
                                 aVKnots,
                                 aUMultiplicities,
                                 aVMultiplicities,
                                 1,
                                 1);
}

static occ::handle<Geom_SurfaceOfRevolution> makeOCC26979RevolutionSurface()
{
  const occ::handle<Geom_Circle> aCircle = new Geom_Circle(
    gp_Ax2(gp_Pnt(0.0, 0.0, 0.0), gp_Dir(0.0, -1.0, 0.0), gp_Dir(1.0, 0.0, 0.0)), 3.0);
  const occ::handle<Geom_TrimmedCurve> aTrimmedCircle =
    new Geom_TrimmedCurve(aCircle, -M_PI / 2.0, M_PI / 2.0);
  return new Geom_SurfaceOfRevolution(
    aTrimmedCircle, gp_Ax1(gp_Pnt(0.0, 0.0, 0.0), gp_Dir(0.0, 0.0, 1.0)));
}
} // namespace

// Migrated from tests/lowalgos/intcs/bug26979.
TEST(GeomAPI_IntCSTest, OCC26979_ParabolaBezierSurface)
{
  expectOCC26979Intersection(makeOCC26979BezierSurface());
}

// Migrated from tests/lowalgos/intcs/bug26979.
TEST(GeomAPI_IntCSTest, OCC26979_ParabolaBSplineSurface)
{
  expectOCC26979Intersection(makeOCC26979BSplineSurface());
}

// Migrated from tests/lowalgos/intcs/bug26979.
TEST(GeomAPI_IntCSTest, OCC26979_ParabolaTorus)
{
  expectOCC26979Intersection(
    new Geom_ToroidalSurface(gp_Ax3(gp_Pnt(0.0, 0.0, 0.0), gp_Dir(0.0, 0.0, 1.0)), 3.0, 1.0));
}

// Migrated from tests/lowalgos/intcs/bug26979.
TEST(GeomAPI_IntCSTest, OCC26979_ParabolaLinearExtrusion)
{
  const occ::handle<Geom_Circle> aCircle =
    new Geom_Circle(gp_Ax2(gp_Pnt(0.0, 0.0, 0.0), gp_Dir(0.0, 0.0, 1.0)), 4.0);
  expectOCC26979Intersection(new Geom_SurfaceOfLinearExtrusion(aCircle, gp_Dir(0.0, 0.0, 1.0)));
}

// Migrated from tests/lowalgos/intcs/bug26979.
TEST(GeomAPI_IntCSTest, OCC26979_ParabolaRevolution)
{
  expectOCC26979Intersection(makeOCC26979RevolutionSurface());
}
