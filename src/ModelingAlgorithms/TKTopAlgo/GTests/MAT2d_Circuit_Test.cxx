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

#include <Geom2d_Line.hxx>
#include <Geom2d_TrimmedCurve.hxx>
#include <MAT2d_Circuit.hxx>
#include <MAT2d_CutCurve.hxx>
#include <Geom2d_BSplineCurve.hxx>
#include <NCollection_Array1.hxx>
#include <cmath>

TEST(MAT2d_CutCurveTest, CurvatureJumpAtSplineKnot)
{
  NCollection_Array1<gp_Pnt2d> aPoles(1, 5);
  aPoles.ChangeAt(0) = gp_Pnt2d(-1.0, 1.0);
  aPoles.ChangeAt(1) = gp_Pnt2d(-1.0, 0.0);
  aPoles.ChangeAt(2) = gp_Pnt2d(0.0, 0.0);
  aPoles.ChangeAt(3) = gp_Pnt2d(1.0, 0.0);
  aPoles.ChangeAt(4) = gp_Pnt2d(1.0, -1.0);
  NCollection_Array1<double> aWeights(1, 5);
  aWeights.Init(1.0);
  aWeights.ChangeAt(1) = std::sqrt(0.5);
  aWeights.ChangeAt(3) = std::sqrt(0.5);
  NCollection_Array1<double> aKnots(1, 3);
  aKnots.ChangeAt(0) = 0.0;
  aKnots.ChangeAt(1) = 1.0;
  aKnots.ChangeAt(2) = 2.0;
  NCollection_Array1<int> aMultiplicities(1, 3);
  aMultiplicities.ChangeAt(0) = 3;
  aMultiplicities.ChangeAt(1) = 2;
  aMultiplicities.ChangeAt(2) = 3;
  const occ::handle<Geom2d_BSplineCurve> aCurve =
    new Geom2d_BSplineCurve(aPoles, aWeights, aKnots, aMultiplicities, 2);

  MAT2d_CutCurve aCutter(aCurve);
  ASSERT_FALSE(aCutter.UnModified());
  ASSERT_GE(aCutter.NbCurves(), 2);
  EXPECT_DOUBLE_EQ(aCutter.Value(1)->FirstParameter(), 0.0);
  EXPECT_DOUBLE_EQ(aCutter.Value(aCutter.NbCurves())->LastParameter(), 2.0);
  bool hasKnotBoundary = false;
  for (int i = 1; i <= aCutter.NbCurves(); ++i)
  {
    const occ::handle<Geom2d_TrimmedCurve> aPart = aCutter.Value(i);
    hasKnotBoundary                              = hasKnotBoundary || aPart->LastParameter() == 1.0;
    EXPECT_FALSE(aPart->FirstParameter() < 1.0 && aPart->LastParameter() > 1.0)
      << aPart->FirstParameter() << "," << aPart->LastParameter();
    for (int j = 0; j <= 10; ++j)
    {
      const double aParameter =
        aPart->FirstParameter() + (aPart->LastParameter() - aPart->FirstParameter()) * j / 10.0;
      EXPECT_DOUBLE_EQ(aPart->EvalD0(aParameter).Distance(aCurve->EvalD0(aParameter)), 0.0);
    }
  }
  EXPECT_TRUE(hasKnotBoundary);
}

namespace
{
int circuitSize(const double theLength, const gp_Dir2d& theSecondDirection, const bool theTrigo)
{
  const gp_Pnt2d                                     aJoin(61.0, 2.0);
  NCollection_Sequence<occ::handle<Geom2d_Geometry>> aSegments;
  aSegments.Append(
    new Geom2d_TrimmedCurve(new Geom2d_Line(aJoin, gp_Dir2d(1.0, 0.0)), -2.0 * theLength, 0.0));
  aSegments.Append(
    new Geom2d_TrimmedCurve(new Geom2d_Line(aJoin, theSecondDirection), 0.0, theLength));
  NCollection_Sequence<NCollection_Sequence<occ::handle<Geom2d_Geometry>>> aFigure;
  aFigure.Append(aSegments);
  NCollection_Sequence<bool> aClosed;
  aClosed.Append(false);
  MAT2d_Circuit aCircuit(GeomAbs_Arc, true);
  aCircuit.Perform(aFigure, aClosed, 1, theTrigo);
  return aCircuit.NumberOfItems();
}
} // namespace

TEST(MAT2d_CircuitTest, NearlyReversedLinesHaveOppositeCornerSides)
{
  for (const double aLength : {4.0e-6, 1.0, 1000.0})
  {
    for (const double aTurn : {-1.0e-9, 1.0e-9})
    {
      // Two segments and their endpoints, with a vertex generator only on the convex side.
      EXPECT_EQ(circuitSize(aLength, gp_Dir2d(-1.0, aTurn), true), aTurn < 0.0 ? 5 : 4);
      EXPECT_EQ(circuitSize(aLength, gp_Dir2d(-1.0, aTurn), false), aTurn > 0.0 ? 5 : 4);
    }
  }
}

TEST(MAT2d_CircuitTest, NearlyAlignedLinesRemainFlat)
{
  for (const double aTurn : {-1.0e-9, 0.0, 1.0e-9})
  {
    EXPECT_EQ(circuitSize(4.0e-6, gp_Dir2d(1.0, aTurn), true), 4);
    EXPECT_EQ(circuitSize(4.0e-6, gp_Dir2d(1.0, aTurn), false), 4);
  }
}

TEST(MAT2d_CircuitTest, ExactlyReversedLinesHaveACuspOnBothSides)
{
  EXPECT_EQ(circuitSize(4.0e-6, gp_Dir2d(-1.0, 0.0), true), 5);
  EXPECT_EQ(circuitSize(4.0e-6, gp_Dir2d(-1.0, 0.0), false), 5);
}
