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
#include <MAT2d_Tool2d.hxx>
#include <MAT_Bisector.hxx>
#include <MAT_Edge.hxx>
#include <Geom2d_BSplineCurve.hxx>
#include <Geom2d_Curve.hxx>
#include <Geom2dAdaptor_Curve.hxx>
#include <NCollection_Array1.hxx>
#include <NCollection_Sequence.hxx>
#include <Precision.hxx>
#include <gp_Pnt2d.hxx>
#include <gp_Vec2d.hxx>
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

TEST(MAT2d_Tool2dTest, NormalBisectorsMeetAtSplineArcCentre)
{
  const gp_Pnt2d aPoleData[]   = {gp_Pnt2d(-12.65941428892841, -55.941830052432181),
                                  gp_Pnt2d(-32.894280745624549, -66.767337142742136),
                                  gp_Pnt2d(-33.195380490963274, -43.820643192936757),
                                  gp_Pnt2d(-33.496480236301991, -20.873949243131381),
                                  gp_Pnt2d(-12.984531313338412, -31.164788648370198),
                                  gp_Pnt2d(7.9621451885557706, -41.673730766159885),
                                  gp_Pnt2d(6.6943781881236495, -18.273006680297161),
                                  gp_Pnt2d(5.4266111876915071, 5.1277174055655692),
                                  gp_Pnt2d(-14.262123720122727, -7.582705035875037),
                                  gp_Pnt2d(-40.633813985406732, -24.607431374871783),
                                  gp_Pnt2d(-35.651100166346303, 6.3841790602629018),
                                  gp_Pnt2d(-30.668386347285889, 37.375789495397576),
                                  gp_Pnt2d(-10.962107231056169, 12.942821649492238)};
  const double   aWeightData[] = {1.0,
                                  0.51955875323282696,
                                  1.0,
                                  0.51955875323282696,
                                  1.0,
                                  0.50193380223419504,
                                  1.0,
                                  0.50193380223419504,
                                  1.0,
                                  0.40690693074211459,
                                  1.0,
                                  0.40690693074211459,
                                  1.0};
  const double   aKnotData[]   = {8.3452300125062404,
                                  11.287979115252854,
                                  14.230728217999468,
                                  17.133903201746506,
                                  20.037078185493545,
                                  23.189483209831039,
                                  26.341888234168536};
  const NCollection_Array1<gp_Pnt2d> aPoles(aPoleData[0], 1, 13);
  const NCollection_Array1<double>   aWeights(aWeightData[0], 1, 13);
  const NCollection_Array1<double>   aKnots(aKnotData[0], 1, 7);
  NCollection_Array1<int>            aMultiplicities(1, 7);
  aMultiplicities.Init(2);
  aMultiplicities.ChangeAt(0) = aMultiplicities.ChangeAt(6) = 3;
  const occ::handle<Geom2d_BSplineCurve> aSpline =
    new Geom2d_BSplineCurve(aPoles, aWeights, aKnots, aMultiplicities, 2);

  NCollection_Sequence<occ::handle<Geom2d_Geometry>> aSegments;
  for (size_t i = 0; i < 3; ++i)
  {
    aSegments.Append(new Geom2d_TrimmedCurve(aSpline, aKnots.At(2 * i), aKnots.At(2 * i + 2)));
  }
  NCollection_Sequence<NCollection_Sequence<occ::handle<Geom2d_Geometry>>> aFigure;
  aFigure.Append(aSegments);
  NCollection_Sequence<bool> aClosed;
  aClosed.Append(false);
  const occ::handle<MAT2d_Circuit> aCircuit = new MAT2d_Circuit(GeomAbs_Arc, true);
  aCircuit->Perform(aFigure, aClosed, 1, true);
  MAT2d_Tool2d aTool;
  aTool.InitItems(aCircuit);
  occ::handle<MAT_Edge> anEdges[3];
  size_t                aCurveCount = 0;
  for (int i = 1; i <= aCircuit->NumberOfItems(); ++i)
  {
    if (!occ::down_cast<Geom2d_Curve>(aCircuit->Value(i)).IsNull())
    {
      ASSERT_LT(aCurveCount, size_t(3));
      anEdges[aCurveCount] = new MAT_Edge;
      anEdges[aCurveCount++]->EdgeNumber(i);
    }
  }
  ASSERT_EQ(aCurveCount, size_t(3));
  occ::handle<MAT_Bisector> aBisectors[2];
  for (int i = 0; i < 2; ++i)
  {
    aBisectors[i] = new MAT_Bisector;
    aBisectors[i]->FirstEdge(anEdges[i]);
    aBisectors[i]->SecondEdge(anEdges[i + 1]);
    double aDistance;
    aBisectors[i]->IssuePoint(aTool.FirstPoint(anEdges[i + 1]->EdgeNumber(), aDistance));
    aBisectors[i]->FirstVector(aTool.TangentBefore(anEdges[i]->EdgeNumber(), true));
    aBisectors[i]->SecondVector(aTool.TangentAfter(anEdges[i + 1]->EdgeNumber(), true));
    aTool.CreateBisector(aBisectors[i]);
  }
  const auto [aPoint, aD1, aD2] =
    Geom2dAdaptor_Curve(aSpline, aKnots.At(2), aKnots.At(4)).EvalD2(aKnots.At(2));
  const double   aSpeed  = aD1.Magnitude();
  const double   aRadius = aSpeed * aSpeed * aSpeed / aD1.Crossed(aD2);
  const gp_Pnt2d aCentre = aPoint.Translated(aRadius / aSpeed * gp_Vec2d(-aD1.Y(), aD1.X()));
  for (const auto& aBisector : aBisectors)
  {
    EXPECT_LE(aTool.GeomBis(aBisector->BisectorNumber()).Value()->EndPoint().Distance(aCentre),
              Precision::Confusion());
  }
  int          aPointIndex;
  const double aDistance = aTool.IntersectBisector(aBisectors[0], aBisectors[1], aPointIndex);
  ASSERT_LT(aDistance, Precision::Infinite());
  EXPECT_NEAR(aDistance, aRadius, Precision::Confusion());
  EXPECT_LE(aTool.GeomPnt(aPointIndex).Distance(aCentre), Precision::Confusion());
}
