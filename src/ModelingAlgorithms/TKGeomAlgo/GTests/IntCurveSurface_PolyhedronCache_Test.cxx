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

#include <GeomAdaptor_Curve.hxx>
#include <GeomAdaptor_Surface.hxx>
#include <Geom_BSplineSurface.hxx>
#include <Geom_Line.hxx>
#include <Geom_TrimmedCurve.hxx>
#include <IntCurveSurface_HInter.hxx>
#include <IntCurveSurface_IntersectionPoint.hxx>
#include <IntCurveSurface_PolyhedronCache.hxx>
#include <IntCurveSurface_ThePolyhedronOfHInter.hxx>
#include <NCollection_Array1.hxx>
#include <NCollection_Array2.hxx>
#include <gp_Lin.hxx>

#include <cmath>

#include <gtest/gtest.h>

namespace
{
//! Bicubic B-spline surface with two spans in each direction, waving in Z over [0, 4] x [0, 4]
occ::handle<GeomAdaptor_Surface> makeWavySurface()
{
  NCollection_Array2<gp_Pnt> aPoles(1, 5, 1, 5);
  for (int i = 1; i <= 5; ++i)
  {
    for (int j = 1; j <= 5; ++j)
    {
      aPoles(i, j) = gp_Pnt(i - 1., j - 1., 0.5 * std::sin(1.3 * i) * std::cos(0.7 * j));
    }
  }
  NCollection_Array1<double> aKnots(1, 3);
  aKnots(1) = 0.;
  aKnots(2) = 0.5;
  aKnots(3) = 1.;
  NCollection_Array1<int> aMults(1, 3);
  aMults(1) = 4;
  aMults(2) = 1;
  aMults(3) = 4;
  occ::handle<Geom_BSplineSurface> aSurface =
    new Geom_BSplineSurface(aPoles, aKnots, aKnots, aMults, aMults, 3, 3);
  return new GeomAdaptor_Surface(aSurface);
}

//! Segment of a line crossing the surface
occ::handle<GeomAdaptor_Curve> makeSegment(const int theIndex)
{
  const double            aX = 0.3 + 0.37 * (theIndex % 9);
  const double            aY = 0.2 + 0.41 * (theIndex % 7);
  const gp_Dir            aDir(0.1 * (theIndex % 3), -0.1 * (theIndex % 4), 1.);
  occ::handle<Geom_Curve> aLine = new Geom_Line(gp_Lin(gp_Pnt(aX, aY, -2.), aDir));
  return new GeomAdaptor_Curve(new Geom_TrimmedCurve(aLine, 0., 4.));
}
} // namespace

//=================================================================================================

TEST(IntCurveSurface_PolyhedronCache, Polyhedron_BuiltOncePerDiscretizationAndBounds)
{
  occ::handle<GeomAdaptor_Surface> aSurface = makeWavySurface();
  IntCurveSurface_PolyhedronCache  aCache;
  EXPECT_EQ(aCache.NbPolyhedra(), 0);

  const IntCurveSurface_ThePolyhedronOfHInter& aPoly1 =
    aCache.Polyhedron(aSurface, 20, 20, 0., 0., 0.5, 1.);
  EXPECT_EQ(&aCache.Polyhedron(aSurface, 20, 20, 0., 0., 0.5, 1.), &aPoly1);
  EXPECT_EQ(aCache.NbPolyhedra(), 1);

  // Other bounds or discretization give another polyhedron
  const IntCurveSurface_ThePolyhedronOfHInter& aPoly2 =
    aCache.Polyhedron(aSurface, 20, 20, 0.5, 0., 1., 1.);
  const IntCurveSurface_ThePolyhedronOfHInter& aPoly3 =
    aCache.Polyhedron(aSurface, 21, 20, 0., 0., 0.5, 1.);
  EXPECT_NE(&aPoly2, &aPoly1);
  EXPECT_NE(&aPoly3, &aPoly1);
  EXPECT_EQ(aCache.NbPolyhedra(), 3);

  int aNbU = 0, aNbV = 0;
  aPoly3.Size(aNbU, aNbV);
  EXPECT_EQ(aNbU, 21);
  EXPECT_EQ(aNbV, 20);

  aCache.Clear();
  EXPECT_EQ(aCache.NbPolyhedra(), 0);
}

//=================================================================================================

TEST(IntCurveSurface_PolyhedronCache, HInter_SameResultsWithTheCache)
{
  occ::handle<GeomAdaptor_Surface> aSurface = makeWavySurface();
  IntCurveSurface_PolyhedronCache  aCache;
  int                              aNbPoints = 0;
  for (int i = 0; i < 40; ++i)
  {
    occ::handle<GeomAdaptor_Curve> aSegment = makeSegment(i);

    IntCurveSurface_HInter anInter;
    anInter.Perform(aSegment, aSurface);

    IntCurveSurface_HInter anInterCached;
    anInterCached.SetPolyhedronCache(&aCache);
    anInterCached.Perform(aSegment, aSurface);

    ASSERT_TRUE(anInter.IsDone());
    ASSERT_TRUE(anInterCached.IsDone());
    ASSERT_EQ(anInterCached.NbPoints(), anInter.NbPoints()) << "segment " << i;
    ASSERT_EQ(anInterCached.NbSegments(), anInter.NbSegments()) << "segment " << i;
    for (int j = 1; j <= anInter.NbPoints(); ++j)
    {
      const IntCurveSurface_IntersectionPoint& aPoint       = anInter.Point(j);
      const IntCurveSurface_IntersectionPoint& aPointCached = anInterCached.Point(j);
      EXPECT_EQ(aPointCached.W(), aPoint.W());
      EXPECT_EQ(aPointCached.U(), aPoint.U());
      EXPECT_EQ(aPointCached.V(), aPoint.V());
      EXPECT_TRUE(aPointCached.Pnt().IsEqual(aPoint.Pnt(), 0.));
    }
    aNbPoints += anInter.NbPoints();
  }
  // Most segments cross the surface
  EXPECT_GE(aNbPoints, 30);
  // The polyhedra are built once for all segments: one per interval of continuity at most
  EXPECT_GE(aCache.NbPolyhedra(), 1);
  EXPECT_LE(aCache.NbPolyhedra(), 4);
}
