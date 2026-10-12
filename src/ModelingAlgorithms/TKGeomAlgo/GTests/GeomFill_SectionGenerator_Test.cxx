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

#include <Geom_BSplineCurve.hxx>
#include <GeomFill_AppSurf.hxx>
#include <GeomFill_Line.hxx>
#include <GeomFill_SectionGenerator.hxx>
#include <Precision.hxx>
#include <gtest/gtest.h>

namespace
{
occ::handle<Geom_BSplineCurve> profile(const double z)
{
  NCollection_Array1<gp_Pnt> p(1, 3);
  p(1) = gp_Pnt(0, 0, z);
  p(2) = gp_Pnt(1, 1, z);
  p(3) = gp_Pnt(2, 0, z);
  NCollection_Array1<double> k(1, 2), w(1, 3);
  k(1) = 0;
  k(2) = 1;
  w(1) = 1;
  w(2) = 0.8;
  w(3) = 1.1;
  NCollection_Array1<int> m(1, 2);
  m.Init(3);
  return new Geom_BSplineCurve(p, w, k, m, 2);
}
} // namespace

TEST(GeomFill_SectionGenerator_Test, TangentCurvesShareProfileWithoutModifyingInputs)
{
  GeomFill_SectionGenerator g;
  auto                      first = profile(0), last = profile(5), tangent = profile(2);
  tangent->InsertKnot(0.4, 1, Precision::PConfusion());
  g.AddCurve(first);
  g.AddCurve(last);
  g.SetTangents(tangent, nullptr);
  GeomFill_Profiler& profiler = g;
  profiler.Perform(Precision::PConfusion());
  EXPECT_EQ(g.NbKnots(), 3);
  EXPECT_EQ(first->NbKnots(), 2);
  EXPECT_EQ(last->NbKnots(), 2);
  EXPECT_EQ(tangent->NbKnots(), 3);
  NCollection_Array1<gp_Pnt>   p(1, g.NbPoles());
  NCollection_Array1<gp_Vec>   d(1, g.NbPoles());
  NCollection_Array1<double>   w(1, g.NbPoles()), dw(1, g.NbPoles());
  NCollection_Array1<gp_Pnt2d> p2;
  NCollection_Array1<gp_Vec2d> d2;
  ASSERT_TRUE(g.Section(1, p, d, p2, d2, w, dw));
  for (const auto& v : d)
    EXPECT_LT(v.Crossed(gp_Vec(0, 0, 1)).Magnitude(), 1.e-10);
  g.SetTangents(nullptr, nullptr);
  g.Perform(Precision::PConfusion());
  EXPECT_FALSE(g.Section(1, p, d, p2, d2, w, dw));
}

TEST(GeomFill_SectionGenerator_Test, ZeroTangentFailsWithoutProducingInvalidSurface)
{
  GeomFill_SectionGenerator g;
  auto                      first = profile(0), last = profile(5);
  g.AddCurve(first);
  g.AddCurve(last);
  g.SetTangents(first, last);
  g.Perform(Precision::PConfusion());
  GeomFill_AppSurf app(2, 5, 1.e-7, 1.e-7, 0);
  app.Perform(new GeomFill_Line(2), g, true);
  EXPECT_FALSE(app.IsDone());
}
