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

#include <Bisector_BisecCC.hxx>
#include <Bisector_Inter.hxx>
#include <Bisector_FunctionH.hxx>
#include <BSplCLib.hxx>
#include <Geom2d_Circle.hxx>
#include <Geom2d_Ellipse.hxx>
#include <Geom2dConvert.hxx>
#include <Bisector_Bisec.hxx>
#include <Geom2d_BSplineCurve.hxx>
#include <Geom2d_BezierCurve.hxx>
#include <Geom2dAPI_ProjectPointOnCurve.hxx>
#include <Geom2d_TrimmedCurve.hxx>
#include <Geom2dAdaptor_Curve.hxx>
#include <Geom2d_Line.hxx>
#include <Geom2d_Curve.hxx>
#include <NCollection_Array1.hxx>
#include <gp_Ax2d.hxx>
#include <gp_Ax22d.hxx>
#include <gp_Trsf2d.hxx>
#include <gp_Dir2d.hxx>
#include <gp_Pnt2d.hxx>
#include <gp_Vec2d.hxx>
#include <Precision.hxx>
#include <Standard_Real.hxx>
#include <MathRoot_Multiple.hxx>
#include <IntRes2d_Domain.hxx>

#include <algorithm>
#include <cmath>

TEST(Bisector_BisecCCTest, ShortBranchStartsAtSeparateCurveEndpoint)
{
  const occ::handle<Geom2d_Curve> aSecond =
    new Geom2d_TrimmedCurve(new Geom2d_Line(gp_Pnt2d(0.0, 0.0), gp_Dir2d(0.0, 1.0)), 1.0, 1.01);
  for (const double aGuideLast : {2.0, 100.0})
  {
    const occ::handle<Geom2d_Curve> aGuide =
      new Geom2d_TrimmedCurve(new Geom2d_Line(gp_Pnt2d(0.0, 0.0), gp_Dir2d(1.0, 0.0)),
                              0.0,
                              aGuideLast);
    for (const double aStartError : {0.0, 0.25 * Precision::Confusion()})
    {
      SCOPED_TRACE(aGuideLast);
      SCOPED_TRACE(aStartError);
      const gp_Pnt2d   aStart(1.0 - aStartError, 1.0 - aStartError);
      Bisector_BisecCC aBisector(aGuide, aSecond, 1.0, -1.0, aStart);
      ASSERT_FALSE(aBisector.IsEmpty());
      EXPECT_LE(aBisector.EvalD0(aBisector.FirstParameter()).Distance(aStart),
                Precision::Confusion());
      for (int i = 0; i <= 10; ++i)
      {
        const double aParameter =
          aBisector.FirstParameter()
          + (aBisector.LastParameter() - aBisector.FirstParameter()) * i / 10.0;
        const gp_Pnt2d aPoint = aBisector.EvalD0(aParameter);
        EXPECT_NEAR(aPoint.X(), aPoint.Y(), Precision::Confusion());
        EXPECT_GE(aPoint.Y(), 1.0 - Precision::Confusion());
        EXPECT_LE(aPoint.Y(), 1.01 + Precision::Confusion());
      }
    }
  }
}

TEST(Bisector_FunctionHTest, CachedBasisPreservesTrimBoundaryDerivatives)
{
  const gp_Pnt2d                  aPoles[]          = {gp_Pnt2d(-1.0, 1.0),
                                                       gp_Pnt2d(-0.5, 0.0),
                                                       gp_Pnt2d(0.0, 0.0),
                                                       gp_Pnt2d(0.5, 0.0),
                                                       gp_Pnt2d(1.0, 2.0)};
  const double                    aKnots[]          = {0.0, 0.5, 1.0};
  const int                       aMultiplicities[] = {3, 2, 3};
  const occ::handle<Geom2d_Curve> aBasis =
    new Geom2d_BSplineCurve(NCollection_Array1<gp_Pnt2d>(aPoles[0], 1, 5),
                            NCollection_Array1<double>(aKnots[0], 1, 3),
                            NCollection_Array1<int>(aMultiplicities[0], 1, 3),
                            2);
  const occ::handle<Geom2d_Curve> aTrimmed = new Geom2d_TrimmedCurve(aBasis, 0.0, 0.5);
  const Geom2dAdaptor_Curve       anEvaluator(aBasis);
  Bisector_FunctionH              aSource(aTrimmed, gp_Pnt2d(1.0, 1.0), gp_Vec2d(0.0, 1.0), 0.25);
  Bisector_FunctionH aCached(anEvaluator, gp_Pnt2d(1.0, 1.0), gp_Vec2d(0.0, 1.0), 0.25);
  EXPECT_NEAR(aBasis->EvalD2(0.5).D2.Y(), 16.0, Precision::Confusion());
  for (const double aParameter : {0.0, 0.25, 0.5})
  {
    SCOPED_TRACE(aParameter);
    double aSourceValue, aSourceDerivative, aCachedValue, aCachedDerivative;
    ASSERT_TRUE(aSource.Values(aParameter, aSourceValue, aSourceDerivative));
    ASSERT_TRUE(aCached.Values(aParameter, aCachedValue, aCachedDerivative));
    EXPECT_NEAR(aCachedValue, aSourceValue, Precision::Confusion());
    EXPECT_NEAR(aCachedDerivative, aSourceDerivative, Precision::Confusion());
    double aValueOnly;
    ASSERT_TRUE(aCached.Value(aParameter, aValueOnly));
    EXPECT_NEAR(aValueOnly, aSourceValue, Precision::Confusion());
  }
}

TEST(Bisector_FunctionHTest, ParallelTangentsDoNotAddCircleRoot)
{
  const occ::handle<Geom2d_Curve> aCurve =
    new Geom2d_TrimmedCurve(new Geom2d_Circle(gp_Ax2d(gp_Pnt2d(0.0, 0.0), gp_Dir2d(1.0, 0.0)), 1.0),
                            M_PI,
                            2.0 * M_PI);
  Bisector_FunctionH       aFunction(aCurve, gp_Pnt2d(5.0, 0.0), gp_Vec2d(1.0, 0.0));
  MathRoot::MultipleConfig aConfig;
  aConfig.NbSamples  = 20;
  aConfig.XTolerance = Precision::PConfusion();
  aConfig.FTolerance = Precision::Confusion();
  const MathRoot::MultipleResult aRoots =
    MathRoot::FindAllRootsWithDerivative(aFunction, M_PI, 2.0 * M_PI, aConfig);
  ASSERT_TRUE(aRoots.IsDone());
  ASSERT_EQ(aRoots.NbRoots(), 1);
  const double aParameter = aRoots.Roots.Value(0);
  EXPECT_NEAR(aParameter, 2.0 * M_PI + std::atan2(-12.0, 5.0), Precision::PConfusion());
  const auto [aPoint, aTangent] = aCurve->EvalD1(aParameter);
  EXPECT_NEAR(gp_Vec2d(aPoint, gp_Pnt2d(5.0, -12.0)).Dot(aTangent), 0.0, Precision::Confusion());
}

TEST(Bisector_FunctionHTest, AntiparallelTangentsDoNotAddCircleRoot)
{
  const occ::handle<Geom2d_Curve> aCurve =
    new Geom2d_TrimmedCurve(new Geom2d_Circle(gp_Ax2d(gp_Pnt2d(0.0, 0.0), gp_Dir2d(1.0, 0.0)), 1.0),
                            0.0,
                            M_PI);
  Bisector_FunctionH       aFunction(aCurve, gp_Pnt2d(5.0, 0.0), gp_Vec2d(1.0, 0.0));
  MathRoot::MultipleConfig aConfig;
  aConfig.NbSamples = 20;
  const MathRoot::MultipleResult aRoots =
    MathRoot::FindAllRootsWithDerivative(aFunction, 0.0, M_PI, aConfig);
  ASSERT_TRUE(aRoots.IsDone());
  EXPECT_EQ(aRoots.NbRoots(), 0);

  const double aParameter = 0.5 * M_PI;
  const double aStep      = std::cbrt(Epsilon(1.0));
  double       aValue, aDerivative, aBefore, anAfter;
  ASSERT_TRUE(aFunction.Values(aParameter, aValue, aDerivative));
  ASSERT_TRUE(aFunction.Value(aParameter - aStep, aBefore));
  ASSERT_TRUE(aFunction.Value(aParameter + aStep, anAfter));
  EXPECT_NEAR(aDerivative, (anAfter - aBefore) / (2.0 * aStep), Precision::Confusion());
}

TEST(Bisector_FunctionHTest, CircleTangencyNearAntiparallelTangents)
{
  const occ::handle<Geom2d_Curve> aCurve =
    new Geom2d_TrimmedCurve(new Geom2d_Circle(gp_Ax2d(gp_Pnt2d(0.0, 0.0), gp_Dir2d(1.0, 0.0)), 1.0),
                            0.0,
                            M_PI);
  for (const double anX : {-1.0e-5, 0.0, 1.0e-5})
  {
    SCOPED_TRACE(anX);
    Bisector_FunctionH       aFunction(aCurve, gp_Pnt2d(anX, 0.0), gp_Vec2d(1.0, 0.0));
    MathRoot::MultipleConfig aConfig;
    aConfig.NbSamples = 20;
    const MathRoot::MultipleResult aRoots =
      MathRoot::FindAllRootsWithDerivative(aFunction, 0.0, M_PI, aConfig);
    ASSERT_TRUE(aRoots.IsDone());
    ASSERT_EQ(aRoots.NbRoots(), 1);
    const auto [aPoint, aTangent] = aCurve->EvalD1(aRoots.Roots.Value(0));
    const double   aRadius        = 0.5 * (1.0 - anX * anX);
    const gp_Pnt2d aCentre(anX, aRadius);
    EXPECT_NEAR(aCentre.Distance(aPoint), aRadius, Precision::Confusion());
    EXPECT_NEAR(gp_Vec2d(aPoint, aCentre).Dot(aTangent), 0.0, Precision::Confusion());
  }
}

TEST(Bisector_BisecCCTest, SplineEndpointTangencyRetainsOffsetBranch)
{
  NCollection_Array1<gp_Pnt2d> aPoles(1, 6);
  aPoles.ChangeAt(0) = gp_Pnt2d(1255.0231560677553, 3005.95393292995);
  aPoles.ChangeAt(1) = gp_Pnt2d(1255.0232190702313, 3003.0100100761015);
  aPoles.ChangeAt(2) = gp_Pnt2d(1255.0184659224899, 3000.1711483925742);
  aPoles.ChangeAt(3) = gp_Pnt2d(1255.0185568593738, 2996.1854878390418);
  aPoles.ChangeAt(4) = gp_Pnt2d(1255.0179824861941, 2995.0379088192185);
  aPoles.ChangeAt(5) = gp_Pnt2d(1255.0179629461943, 2995.0376836173023);
  NCollection_Array1<double> aKnots(1, 3);
  aKnots.ChangeAt(0) = 0.0;
  aKnots.ChangeAt(1) = 0.500309227505778;
  aKnots.ChangeAt(2) = 1.0;
  NCollection_Array1<int> aMultiplicities(1, 3);
  aMultiplicities.ChangeAt(0) = 4;
  aMultiplicities.ChangeAt(1) = 2;
  aMultiplicities.ChangeAt(2) = 4;
  const occ::handle<Geom2d_Curve> aSpline =
    new Geom2d_TrimmedCurve(new Geom2d_BSplineCurve(aPoles, aKnots, aMultiplicities, 3),
                            aKnots.At(1),
                            1.0);
  const occ::handle<Geom2d_Curve> aLine =
    new Geom2d_TrimmedCurve(new Geom2d_Line(gp_Pnt2d(1255.0144891820614, 2994.7620926478912),
                                            gp_Dir2d(-0.012529478436019164, -0.9999215030041716)),
                            -0.27561469222034779,
                            0.0);
  for (const double aSide : {1.0, -1.0})
  {
    SCOPED_TRACE(aSide);
    const occ::handle<Geom2d_Curve> aGuide  = occ::down_cast<Geom2d_Curve>(aLine->Copy());
    const occ::handle<Geom2d_Curve> anOther = occ::down_cast<Geom2d_Curve>(aSpline->Copy());
    gp_Pnt2d                        anOrigin(1255.0254143585616, 2995.0370370837445);
    if (aSide < 0.0)
    {
      gp_Trsf2d aMirror;
      aMirror.SetMirror(gp_Ax2d(gp_Pnt2d(0.0, 0.0), gp_Dir2d(1.0, 0.0)));
      aGuide->Transform(aMirror);
      anOther->Transform(aMirror);
      anOrigin.Transform(aMirror);
    }
    Bisector_BisecCC aBisector(aGuide, anOther, aSide, aSide, anOrigin);
    ASSERT_FALSE(aBisector.IsEmpty());
    const double aParameter = -0.25;
    ASSERT_LT(aBisector.FirstParameter(), aParameter);
    ASSERT_GT(aBisector.LastParameter(), aParameter);
    double         aU1, aU2, aSquaredDistance;
    const gp_Pnt2d aPoint = aBisector.ValueAndDist(aParameter, aU1, aU2, aSquaredDistance);
    const auto [aPoint1, aTangent1] = aGuide->EvalD1(aU1);
    const auto [aPoint2, aTangent2] = anOther->EvalD1(aU2);
    EXPECT_NEAR(aPoint.Distance(aPoint1), aPoint.Distance(aPoint2), Precision::Confusion());
    EXPECT_NEAR(gp_Vec2d(aPoint1, aPoint).Dot(aTangent1) / aTangent1.Magnitude(),
                0.0,
                Precision::Confusion());
    EXPECT_NEAR(gp_Vec2d(aPoint2, aPoint).Dot(aTangent2) / aTangent2.Magnitude(),
                0.0,
                Precision::Confusion());
  }
}

TEST(Bisector_BisecCCTest, ShortCurvedBranchBeforeFirstSample)
{
  for (const double aSide : {1.0, -1.0})
  {
    SCOPED_TRACE(aSide);
    NCollection_Array1<gp_Pnt2d> aPoles(1, 4);
    aPoles.ChangeAt(0)                            = gp_Pnt2d(0.0, 0.0);
    aPoles.ChangeAt(1)                            = gp_Pnt2d(1.0, aSide * 0.5);
    aPoles.ChangeAt(2)                            = gp_Pnt2d(2.0, -aSide);
    aPoles.ChangeAt(3)                            = gp_Pnt2d(3.0, aSide * 20.0);
    const occ::handle<Geom2d_BezierCurve>  aCurve = new Geom2d_BezierCurve(aPoles);
    const occ::handle<Geom2d_TrimmedCurve> aLine =
      new Geom2d_TrimmedCurve(new Geom2d_Line(gp_Pnt2d(0.0, 0.0), gp_Dir2d(1.0, aSide * 0.45)),
                              -2.0,
                              0.0);

    Bisector_BisecCC aBisector(aCurve, aLine, aSide, aSide, gp_Pnt2d(0.0, 0.0));
    ASSERT_FALSE(aBisector.IsEmpty());
    EXPECT_LT(aBisector.FirstParameter(), aBisector.LastParameter());
    EXPECT_LT(aBisector.LastParameter(), 0.05);
    for (int i = 1; i < 20; ++i)
    {
      const double aParameter =
        aBisector.FirstParameter()
        + (aBisector.LastParameter() - aBisector.FirstParameter()) * i / 20.0;
      const gp_Pnt2d                aPoint = aBisector.EvalD0(aParameter);
      Geom2dAPI_ProjectPointOnCurve aCurveProjection(aPoint, aCurve);
      Geom2dAPI_ProjectPointOnCurve aLineProjection(aPoint, aLine);
      ASSERT_GT(aCurveProjection.NbPoints(), 0);
      ASSERT_GT(aLineProjection.NbPoints(), 0);
      EXPECT_NEAR(aCurveProjection.LowerDistance(),
                  aLineProjection.LowerDistance(),
                  Precision::Confusion());
    }
  }
}

TEST(Bisector_BisecCCTest, SmallNonzeroDistancePreservesBisector)
{
  NCollection_Array1<gp_Pnt2d> aFirstPoles(1, 5);
  aFirstPoles.ChangeAt(0) = gp_Pnt2d(274.76521837596579, -560.23592614188806);
  aFirstPoles.ChangeAt(1) = gp_Pnt2d(275.80530458858192, -559.8117621325398);
  aFirstPoles.ChangeAt(2) = gp_Pnt2d(276.84179370140873, -559.37960139823031);
  aFirstPoles.ChangeAt(3) = gp_Pnt2d(277.87459494294785, -558.93949274952695);
  aFirstPoles.ChangeAt(4) = gp_Pnt2d(278.90418838874598, -558.49141145568012);
  NCollection_Array1<gp_Pnt2d> aSecondPoles(1, 5);
  aSecondPoles.ChangeAt(0) = gp_Pnt2d(278.9023830896171, -558.49210491459451);
  aSecondPoles.ChangeAt(1) = gp_Pnt2d(278.90235704463868, -558.49204328473695);
  aSecondPoles.ChangeAt(2) = gp_Pnt2d(278.90776202849247, -558.4896857659794);
  aSecondPoles.ChangeAt(3) = gp_Pnt2d(278.91887970157791, -558.48488457722658);
  aSecondPoles.ChangeAt(4) = gp_Pnt2d(278.93589511135389, -558.47753643940905);
  NCollection_Array1<double> aFirstKnots(1, 2), aSecondKnots(1, 2);
  aFirstKnots.ChangeAt(0)  = 0.71740585649605193;
  aFirstKnots.ChangeAt(1)  = 0.75850777941917302;
  aSecondKnots.ChangeAt(0) = 0.0013307603518307848;
  aSecondKnots.ChangeAt(1) = 0.0069996541768193058;
  NCollection_Array1<int> aMultiplicities(1, 2);
  aMultiplicities.Init(5);
  const occ::handle<Geom2d_BSplineCurve> aFirst =
    new Geom2d_BSplineCurve(aFirstPoles, aFirstKnots, aMultiplicities, 4);
  const occ::handle<Geom2d_BSplineCurve> aSecond =
    new Geom2d_BSplineCurve(aSecondPoles, aSecondKnots, aMultiplicities, 4);
  const gp_Pnt2d anOrigin(278.90230437958468, -558.49213817771044);

  Geom2dAPI_ProjectPointOnCurve aFirstProjection(anOrigin, aFirst);
  Geom2dAPI_ProjectPointOnCurve aSecondProjection(anOrigin, aSecond);
  ASSERT_GT(aFirstProjection.NbPoints(), 0);
  ASSERT_GT(aSecondProjection.NbPoints(), 0);
  const double aDistance = aFirstProjection.LowerDistance();
  ASSERT_GT(aDistance, Precision::Confusion());
  ASSERT_LT(aDistance * aDistance, Precision::Confusion());
  ASSERT_NEAR(aDistance, aSecondProjection.LowerDistance(), Precision::Confusion());

  Bisector_BisecCC aBisector;
  aBisector.Perform(aSecond, aFirst, 1.0, 1.0, anOrigin);
  ASSERT_FALSE(aBisector.IsEmpty());
  EXPECT_LT(aBisector.FirstParameter(), aBisector.LastParameter());
  EXPECT_LE(aBisector.Value(aBisector.FirstParameter()).Distance(anOrigin), Precision::Confusion());
}

TEST(Bisector_BisecCCTest, NarrowSourceParameterRangePreservesPointDerivative)
{
  const occ::handle<Geom2d_Curve> aGuide =
    new Geom2d_TrimmedCurve(new Geom2d_Line(gp_Pnt2d(0.0, 0.0), gp_Dir2d(1.0, 0.0)), 1.0, 2.0);
  const gp_Pnt2d aPoles[]          = {gp_Pnt2d(0.0, 1.0), gp_Pnt2d(0.0, 1.25), gp_Pnt2d(0.0, 2.0)};
  const int      aMultiplicities[] = {3, 3};
  for (const double aParameterSpan : {1.e-10, 1.e-8, 1.e-6})
  {
    for (const bool isReversed : {false, true})
    {
      SCOPED_TRACE(aParameterSpan);
      SCOPED_TRACE(isReversed);
      const double                    aKnots[] = {0.0, aParameterSpan};
      const occ::handle<Geom2d_Curve> aSource =
        new Geom2d_BSplineCurve(NCollection_Array1<gp_Pnt2d>(aPoles[0], 1, 3),
                                NCollection_Array1<double>(aKnots[0], 1, 2),
                                NCollection_Array1<int>(aMultiplicities[0], 1, 2),
                                2);
      if (isReversed)
      {
        aSource->Reverse();
      }
      const Bisector_BisecCC aBisector(aGuide,
                                       aSource,
                                       1.0,
                                       isReversed ? 1.0 : -1.0,
                                       gp_Pnt2d(1.0, 1.0));
      ASSERT_FALSE(aBisector.IsEmpty());
      for (const double aParameter : {1.025, 1.275, 1.725})
      {
        SCOPED_TRACE(aParameter);
        constexpr double aStep = 1.e-4;
        ASSERT_GT(aParameter - aStep, aBisector.FirstParameter());
        ASSERT_LT(aParameter + aStep, aBisector.LastParameter());
        const auto [aPoint, aD1] = aBisector.EvalD1(aParameter);
        const gp_Vec2d aNumerical(aBisector.EvalD0(aParameter - aStep),
                                  aBisector.EvalD0(aParameter + aStep));
        EXPECT_NEAR(aD1.X(), aNumerical.X() / (2.0 * aStep), Precision::Confusion());
        EXPECT_NEAR(aD1.Y(), aNumerical.Y() / (2.0 * aStep), Precision::Confusion());
      }
    }
  }
}

TEST(Bisector_BisecCCTest, SmallNormalChordRetainsCircleCentre)
{
  NCollection_Array1<gp_Pnt2d> aPoles(3);
  aPoles.ChangeAt(0) = gp_Pnt2d(0.0, 0.0);
  aPoles.ChangeAt(1) = gp_Pnt2d(0.0005, 0.0);
  aPoles.ChangeAt(2) = gp_Pnt2d(0.001, 0.001);
  const occ::handle<Geom2d_Curve> aCurve =
    new Geom2d_TrimmedCurve(new Geom2d_BezierCurve(aPoles), 0.001, 0.1);
  const occ::handle<Geom2d_Curve> aLine =
    new Geom2d_TrimmedCurve(new Geom2d_Line(gp_Pnt2d(0.0, 0.0), gp_Dir2d(1.0, 0.0)), -0.001, 0.001);
  // For y = 1000*x*x, the circle is tangent to both the curve and y = 0.
  const double aU          = aCurve->FirstParameter();
  const auto [aPoint, aD1] = aCurve->EvalD1(aU);
  const double   aSlope    = aD1.Y() / aD1.X();
  const double   aNorm     = std::sqrt(1.0 + aSlope * aSlope);
  const double   aRadius   = aPoint.Y() * aNorm * (aNorm + 1.0) / (aSlope * aSlope);
  const gp_Pnt2d aCentre(aPoint.X() - aSlope * (aRadius - aPoint.Y()), aRadius);
  ASSERT_LT(aPoint.Y(), Precision::Confusion());
  ASSERT_GT(aPoint.Distance(gp_Pnt2d(aCentre.X(), 0.0)), Precision::Confusion());

  Bisector_BisecCC aBisector(aCurve, aLine, 1.0, 1.0, aCentre);
  ASSERT_FALSE(aBisector.IsEmpty());
  EXPECT_LE(aBisector.EvalD0(aBisector.FirstParameter()).Distance(aCentre), Precision::Confusion());
}

TEST(Bisector_BisecCCTest, TangentEndpointResidualPreservesCircleTangency)
{
  const occ::handle<Geom2d_Curve> aGuide = new Geom2d_TrimmedCurve(
    new Geom2d_Ellipse(gp_Ax22d(gp_Pnt2d(4.3999999761580142, -4.1300523962914042),
                                gp_Dir2d(0.0, 1.0),
                                gp_Dir2d(-1.0, 0.0)),
                       7.5213493612978652,
                       7.5),
    3.0806790980118501,
    M_PI);
  const occ::handle<Geom2d_Curve> aSource =
    new Geom2d_TrimmedCurve(new Geom2d_Line(gp_Pnt2d(-11.27697735558136, -10.706532242628995),
                                            gp_Dir2d(0.99813479862132282, -0.061048536273784489)),
                            13.4270208573496,
                            15.248850310852999);
  const gp_Pnt2d   anOrigin = aGuide->EvalD0(aGuide->FirstParameter());
  Bisector_BisecCC aBisector(aGuide, aSource, 1.0, 1.0, anOrigin);
  double           aGuideParameter, aSourceParameter, aDistance;
  const gp_Pnt2d   aCentre =
    aBisector.ValueByInt(3.080680585159203, aGuideParameter, aSourceParameter, aDistance);
  ASSERT_LT(aDistance, Precision::Infinite());
  const auto [aContact, aTangent] = aSource->EvalD1(aSourceParameter);
  EXPECT_LE(std::abs(gp_Vec2d(aContact, aCentre).Dot(aTangent)) / aTangent.Magnitude(),
            Precision::Confusion());
  EXPECT_NEAR(aCentre.Distance(aGuide->EvalD0(aGuideParameter)),
              aCentre.Distance(aContact),
              Precision::Confusion());
}

TEST(Bisector_BisecCCTest, NonparallelEndTangentsDoNotExtend)
{
  NCollection_Array1<gp_Pnt2d> aParabolaPoles(1, 3);
  aParabolaPoles.ChangeAt(0) = gp_Pnt2d(0.0, 0.0);
  aParabolaPoles.ChangeAt(1) = gp_Pnt2d(0.5, 0.0);
  aParabolaPoles.ChangeAt(2) = gp_Pnt2d(1.0, 1.0);
  NCollection_Array1<gp_Pnt2d> aLinePoles(1, 2);
  aLinePoles.ChangeAt(0)                          = gp_Pnt2d(0.0, 0.0);
  aLinePoles.ChangeAt(1)                          = gp_Pnt2d(1.0, 1.0);
  const occ::handle<Geom2d_BezierCurve> aParabola = new Geom2d_BezierCurve(aParabolaPoles);
  const occ::handle<Geom2d_BezierCurve> aLine     = new Geom2d_BezierCurve(aLinePoles);

  Bisector_BisecCC aBisector(aParabola, aLine, 1.0, -1.0, gp_Pnt2d(0.0, 0.0));
  ASSERT_FALSE(aBisector.IsEmpty());
  EXPECT_FALSE(aBisector.IsExtendAtEnd());
}

TEST(Bisector_BisecCCTest, ParameterStaysWithinNearlyCircularBranch)
{
  const gp_Pnt2d aPoleData[]         = {gp_Pnt2d(-31.530625216996484, 23.859423730057628),
                                        gp_Pnt2d(-31.530625210929674, 23.798756098572916),
                                        gp_Pnt2d(-31.526208465256019, 23.738060974408796),
                                        gp_Pnt2d(-31.517367988913236, 23.678289822364299),
                                        gp_Pnt2d(-31.504409786366033, 23.620176437857918),
                                        gp_Pnt2d(-31.487764484296555, 23.564246470601368),
                                        gp_Pnt2d(-31.44660184401269, 23.453563633843057),
                                        gp_Pnt2d(-31.421587886959518, 23.399164692375859),
                                        gp_Pnt2d(-31.393394591775287, 23.347958627470646),
                                        gp_Pnt2d(-31.362538013724272, 23.300044583624356),
                                        gp_Pnt2d(-31.329441952358991, 23.255407816419858),
                                        gp_Pnt2d(-31.255166808362624, 23.167515186576196),
                                        gp_Pnt2d(-31.21351145557464, 23.125098879468069),
                                        gp_Pnt2d(-31.169904989513654, 23.086601057427178),
                                        gp_Pnt2d(-31.124709339059923, 23.051902074845888),
                                        gp_Pnt2d(-31.078212143113095, 23.020873056365907),
                                        gp_Pnt2d(-31.030624452968375, 22.993398326288165)};
  const double   aKnotData[]         = {0.0, 0.305295660122757, 0.63285189486502802, 1.0};
  const int      aMultiplicityData[] = {7, 5, 5, 7};
  const NCollection_Array1<gp_Pnt2d>     aPoles(aPoleData[0], 1, 17);
  const NCollection_Array1<double>       aKnots(aKnotData[0], 1, 4);
  const NCollection_Array1<int>          aMultiplicities(aMultiplicityData[0], 1, 4);
  const occ::handle<Geom2d_BSplineCurve> aSpline =
    new Geom2d_BSplineCurve(aPoles, aKnots, aMultiplicities, 6);
  const occ::handle<Geom2d_Curve> aFirst =
    new Geom2d_TrimmedCurve(aSpline, 0.10818122299071242, 0.19255631813506446);
  const occ::handle<Geom2d_Curve> aSecond =
    new Geom2d_TrimmedCurve(aSpline, 0.26610689588223424, 0.34558714688674985);
  Bisector_Bisec aBuilder;
  aBuilder.Perform(aFirst,
                   aSecond,
                   gp_Pnt2d(-30.530626929579295, 23.859423649144048),
                   gp_Vec2d(0.97424658249742857, 0.22548524672820799),
                   gp_Vec2d(0.95176178694741975, 0.3068379065673833),
                   1.0,
                   GeomAbs_Arc,
                   Precision::Confusion(),
                   false);
  const occ::handle<Bisector_BisecCC> aBisector =
    occ::down_cast<Bisector_BisecCC>(aBuilder.Value()->BasisCurve());
  ASSERT_FALSE(aBisector.IsNull());
  const double aParameter = aBisector->Parameter(gp_Pnt2d(-30.530627, 23.859423));
  EXPECT_GE(aParameter, aBisector->FirstParameter());
  EXPECT_LE(aParameter, aBisector->LastParameter());
}

TEST(Bisector_BisecCCTest, TangentConcaveCurvesHaveEquidistantBranch)
{
  NCollection_Array1<gp_Pnt2d> aLeftPoles(1, 3), aRightPoles(1, 3);
  aLeftPoles.ChangeAt(0)                     = gp_Pnt2d(-1.0, 1.0);
  aLeftPoles.ChangeAt(1)                     = gp_Pnt2d(-0.5, 0.0);
  aLeftPoles.ChangeAt(2)                     = gp_Pnt2d(0.0, 0.0);
  aRightPoles.ChangeAt(0)                    = gp_Pnt2d(0.0, 0.0);
  aRightPoles.ChangeAt(1)                    = gp_Pnt2d(0.5, 0.0);
  aRightPoles.ChangeAt(2)                    = gp_Pnt2d(1.0, 2.0);
  const occ::handle<Geom2d_Curve> aCurves[2] = {new Geom2d_BezierCurve(aLeftPoles),
                                                new Geom2d_BezierCurve(aRightPoles)};

  for (double aSide : {-1.0, 1.0})
  {
    SCOPED_TRACE(aSide);
    Bisector_Bisec aBisector;
    aBisector.Perform(aCurves[0],
                      aCurves[1],
                      gp_Pnt2d(0.0, 0.0),
                      gp_Vec2d(1.0, 0.0),
                      gp_Vec2d(-1.0, 0.0),
                      aSide,
                      GeomAbs_Arc,
                      Precision::Confusion(),
                      true);
    const occ::handle<Geom2d_TrimmedCurve>& aCurve = aBisector.Value();
    const double                            aFirst = aCurve->FirstParameter();
    const double                            aLast = std::min(aCurve->LastParameter(), aFirst + 2.0);
    ASSERT_GT(aLast, aFirst);
    for (int i = 0; i <= 40; ++i)
    {
      const gp_Pnt2d aPoint = aCurve->Value(aFirst + (aLast - aFirst) * i / 40.0);
      double         aDistances[2];
      for (size_t j = 0; j < 2; ++j)
      {
        aDistances[j] = std::min(aPoint.Distance(aCurves[j]->Value(0.0)),
                                 aPoint.Distance(aCurves[j]->Value(1.0)));
        Geom2dAPI_ProjectPointOnCurve aProjection(aPoint, aCurves[j]);
        if (aProjection.NbPoints() > 0)
        {
          aDistances[j] = std::min(aDistances[j], aProjection.LowerDistance());
        }
      }
      EXPECT_NEAR(aDistances[0], aDistances[1], Precision::Confusion());
    }
  }
}

TEST(Bisector_BisecCCTest, NormalExtensionUsesJoinTangent)
{
  const gp_Pnt2d aFirstPoles[]           = {gp_Pnt2d(-8.6466313011803031, -6.7725882578006633),
                                            gp_Pnt2d(-8.8545174466236887, -6.4889970375265307),
                                            gp_Pnt2d(-9.3482286710899203, -5.9750147717201969),
                                            gp_Pnt2d(-9.9603202280660277, -5.6099497520241641),
                                            gp_Pnt2d(-10.283491422594457, -5.4713820475944743)};
  const gp_Pnt2d aSecondPoles[]          = {gp_Pnt2d(-10.283491422594457, -5.4713820475944743),
                                            gp_Pnt2d(-10.813126218748909, -5.2442879280322785),
                                            gp_Pnt2d(-11.812016041211624, -4.6315208288689007),
                                            gp_Pnt2d(-12.978267430100217, -3.3232410558865695),
                                            gp_Pnt2d(-13.721084472948302, -1.7358025193333617),
                                            gp_Pnt2d(-13.890625000000046, -0.57626795541398224),
                                            gp_Pnt2d(-13.890625000000046, 4.2632564145606011e-14)};
  const double   aFirstKnots[]           = {0.0, 0.5, 1.0};
  const double   aSecondKnots[]          = {0.0, 0.25, 0.5, 0.75, 1.0};
  const int      aFirstMults[]           = {4, 1, 4};
  const int      aSecondMults[]          = {4, 1, 1, 1, 4};
  const occ::handle<Geom2d_Curve> aFirst = new Geom2d_TrimmedCurve(
    new Geom2d_BSplineCurve(NCollection_Array1<gp_Pnt2d>(aFirstPoles[0], 1, 5),
                            NCollection_Array1<double>(aFirstKnots[0], 1, 3),
                            NCollection_Array1<int>(aFirstMults[0], 1, 3),
                            3),
    0.75092218310622139,
    1.0);
  const occ::handle<Geom2d_Curve> aSecond = new Geom2d_TrimmedCurve(
    new Geom2d_BSplineCurve(NCollection_Array1<gp_Pnt2d>(aSecondPoles[0], 1, 7),
                            NCollection_Array1<double>(aSecondKnots[0], 1, 5),
                            NCollection_Array1<int>(aSecondMults[0], 1, 5),
                            3),
    0.0,
    0.1296965077408333);
  const auto [anOrigin, aTangent] = aSecond->EvalD1(aSecond->FirstParameter());
  const gp_Dir2d aNormal(-aTangent.Y(), aTangent.X());
  for (const double aSide : {-1.0, 1.0})
  {
    SCOPED_TRACE(aSide);
    Bisector_BisecCC aBisector(aSecond, aFirst, aSide, aSide, anOrigin);
    ASSERT_FALSE(aBisector.IsEmpty());
    ASSERT_TRUE(aBisector.IsExtendAtStart());
    ASSERT_GT(aBisector.IntervalLast(1) - aBisector.FirstParameter(), 2.0);
    for (const double aDistance : {0.0, 1.0, 2.0})
    {
      const gp_Pnt2d aPoint = aBisector.EvalD0(aBisector.FirstParameter() + aDistance);
      EXPECT_LE(aPoint.Distance(anOrigin.Translated(aSide * aDistance * gp_Vec2d(aNormal))),
                Precision::Confusion());
    }
  }
}

TEST(Bisector_InterTest, NormalExtensionIntersectsAtCurveBoundary)
{
  const gp_Pnt2d aPoles[2][8]      = {{gp_Pnt2d(-75.67710997046251, -22.408871432878346),
                                       gp_Pnt2d(-75.288269201283327, -22.76447680477299),
                                       gp_Pnt2d(-74.907109465686105, -23.10638734899085),
                                       gp_Pnt2d(-74.537481043374015, -23.431761374141313),
                                       gp_Pnt2d(-74.18370709607666, -23.737403487611896),
                                       gp_Pnt2d(-73.850583667549998, -24.019764595568244),
                                       gp_Pnt2d(-73.543379683576433, -24.274941902954126),
                                       gp_Pnt2d(-73.267836951964753, -24.498678913491442)},
                                      {gp_Pnt2d(-73.267836951964739, -24.498678913491446),
                                       gp_Pnt2d(-73.252324599739183, -24.511274735900002),
                                       gp_Pnt2d(-73.236981605748767, -24.523744664177755),
                                       gp_Pnt2d(-73.221803958175968, -24.536091588256355),
                                       gp_Pnt2d(-73.206787680029677, -24.548318372397414),
                                       gp_Pnt2d(-73.191928829145155, -24.560427855192501),
                                       gp_Pnt2d(-73.177223498184119, -24.572422849563139),
                                       gp_Pnt2d(-73.162667814634659, -24.584306142760813)}};
  const double   aKnots[]          = {0.095625632643507777, 0.122881439114574, 0.12441587142457548};
  const int      aMultiplicities[] = {8, 8};
  occ::handle<Geom2d_Curve> aCurves[2];
  for (size_t i = 0; i < 2; ++i)
  {
    aCurves[i] = new Geom2d_BSplineCurve(NCollection_Array1<gp_Pnt2d>(aPoles[i][0], 1, 8),
                                         NCollection_Array1<double>(aKnots[i], 1, 2),
                                         NCollection_Array1<int>(aMultiplicities[0], 1, 2),
                                         7);
  }
  const gp_Pnt2d                  aLineOrigin(-83.995099645739714, 11.903384517783138);
  const gp_Dir2d                  aLineDirection(0.003881270217383874, -0.99999246784238327);
  const occ::handle<Geom2d_Curve> aLine =
    new Geom2d_TrimmedCurve(new Geom2d_Line(aLineOrigin, aLineDirection), 0.0, 24.458611941531402);
  const auto [aJoin, aTangent] = aCurves[1]->EvalD1(aCurves[1]->FirstParameter());
  const gp_Vec2d aFirstTangent = aCurves[0]->EvalD1(aCurves[0]->LastParameter()).D1;
  Bisector_Bisec aFirst, aSecond;
  aFirst.Perform(aLine,
                 aCurves[0],
                 aCurves[0]->EvalD0(0.11154717792651131),
                 gp_Vec2d(aLineDirection),
                 aFirstTangent,
                 1.0,
                 GeomAbs_Arc,
                 Precision::Confusion(),
                 false);
  aSecond.Perform(aCurves[0],
                  aCurves[1],
                  aJoin,
                  aFirstTangent,
                  -aTangent,
                  1.0,
                  GeomAbs_Arc,
                  Precision::Confusion(),
                  true);
  const gp_Vec2d aNormal = gp_Vec2d(-aTangent.Y(), aTangent.X()).Normalized();
  const gp_Vec2d aLineNormal(-aLineDirection.Y(), aLineDirection.X());
  const double   aRadius =
    gp_Vec2d(aLineOrigin, aJoin).Dot(aLineNormal) / (1.0 - aNormal.Dot(aLineNormal));
  ASSERT_GT(aRadius, 0.0);
  const gp_Pnt2d                          aCentre = aJoin.Translated(aRadius * aNormal);
  const occ::handle<Geom2d_TrimmedCurve>& aC1     = aFirst.Value();
  const occ::handle<Geom2d_TrimmedCurve>& aC2     = aSecond.Value();
  const occ::handle<Bisector_BisecCC>     aNormalBisector =
    occ::down_cast<Bisector_BisecCC>(aC2->BasisCurve());
  ASSERT_FALSE(aNormalBisector.IsNull());
  ASSERT_TRUE(aNormalBisector->IsExtendAtStart());
  const double aNormalFirst = aNormalBisector->IntervalFirst(1);
  const double aNormalLast  = aNormalBisector->IntervalLast(1);
  for (const double aFraction : {0.25, 0.5, 0.75})
  {
    const double   aParameter         = aNormalFirst + aFraction * (aNormalLast - aNormalFirst);
    const gp_Pnt2d aPoint             = aNormalBisector->EvalD0(aParameter);
    const double   anInverseParameter = aNormalBisector->Parameter(aPoint);
    EXPECT_NEAR(anInverseParameter, aParameter, Precision::Confusion());
    EXPECT_LE(aNormalBisector->EvalD0(anInverseParameter).Distance(aPoint), Precision::Confusion());
  }
  const IntRes2d_Domain aD1(aC1->EvalD0(aC1->FirstParameter()),
                            aC1->FirstParameter(),
                            Precision::Confusion(),
                            aC1->EvalD0(aC1->LastParameter()),
                            aC1->LastParameter(),
                            Precision::Confusion());
  const IntRes2d_Domain aD2(aC2->EvalD0(aC2->FirstParameter()),
                            aC2->FirstParameter(),
                            Precision::Confusion(),
                            aC2->EvalD0(aC2->LastParameter()),
                            aC2->LastParameter(),
                            Precision::Confusion());
  const Bisector_Inter
    anIntersection(aFirst, aD1, aSecond, aD2, Precision::Confusion(), Precision::Confusion(), true);
  ASSERT_TRUE(anIntersection.IsDone());
  ASSERT_GT(anIntersection.NbPoints(), 0);
  for (int i = 1; i <= anIntersection.NbPoints(); ++i)
  {
    EXPECT_LE(anIntersection.Point(i).Value().Distance(aCentre), Precision::Confusion());
  }
}

TEST(Bisector_BisecCCTest, TangentCurvesWithOppositeConcavityHaveEquidistantBranch)
{
  NCollection_Array1<gp_Pnt2d> aLeftPoles(1, 3), aRightPoles(1, 3);
  aLeftPoles.ChangeAt(0)                     = gp_Pnt2d(-1.0, 1.0);
  aLeftPoles.ChangeAt(1)                     = gp_Pnt2d(-0.5, 0.0);
  aLeftPoles.ChangeAt(2)                     = gp_Pnt2d(0.0, 0.0);
  aRightPoles.ChangeAt(0)                    = gp_Pnt2d(0.0, 0.0);
  aRightPoles.ChangeAt(1)                    = gp_Pnt2d(0.5, 0.0);
  aRightPoles.ChangeAt(2)                    = gp_Pnt2d(1.0, -2.0);
  const occ::handle<Geom2d_Curve> aCurves[2] = {new Geom2d_BezierCurve(aLeftPoles),
                                                new Geom2d_BezierCurve(aRightPoles)};

  for (double aSide : {-1.0, 1.0})
  {
    SCOPED_TRACE(aSide);
    Bisector_Bisec aBisector;
    aBisector.Perform(aCurves[0],
                      aCurves[1],
                      gp_Pnt2d(0.0, 0.0),
                      gp_Vec2d(1.0, 0.0),
                      gp_Vec2d(-1.0, 0.0),
                      aSide,
                      GeomAbs_Arc,
                      Precision::Confusion(),
                      true);
    const occ::handle<Geom2d_TrimmedCurve>& aCurve = aBisector.Value();
    ASSERT_FALSE(aCurve.IsNull());
    const double aFirst = aCurve->FirstParameter();
    const double aLast  = std::min(aCurve->LastParameter(), aFirst + 2.0);
    ASSERT_GT(aLast, aFirst);
    for (int i = 0; i <= 40; ++i)
    {
      const gp_Pnt2d aPoint = aCurve->Value(aFirst + (aLast - aFirst) * i / 40.0);
      double         aDistances[2];
      for (size_t j = 0; j < 2; ++j)
      {
        aDistances[j] = std::min(aPoint.Distance(aCurves[j]->Value(0.0)),
                                 aPoint.Distance(aCurves[j]->Value(1.0)));
        Geom2dAPI_ProjectPointOnCurve aProjection(aPoint, aCurves[j]);
        if (aProjection.NbPoints() > 0)
        {
          aDistances[j] = std::min(aDistances[j], aProjection.LowerDistance());
        }
      }
      EXPECT_NEAR(aDistances[0], aDistances[1], Precision::Confusion());
    }
  }
}

TEST(Bisector_BisecCCTest, TangentCircularArcsRetainFiniteNormalSegment)
{
  NCollection_Array1<gp_Pnt2d> aLeftPoles(1, 5), aRightPoles(1, 5);
  aLeftPoles.ChangeAt(0)  = gp_Pnt2d(0.0, -4.0);
  aLeftPoles.ChangeAt(1)  = gp_Pnt2d(-2.0, -4.0);
  aLeftPoles.ChangeAt(2)  = gp_Pnt2d(-2.0, -2.0);
  aLeftPoles.ChangeAt(3)  = gp_Pnt2d(-2.0, 0.0);
  aLeftPoles.ChangeAt(4)  = gp_Pnt2d(0.0, 0.0);
  aRightPoles.ChangeAt(0) = gp_Pnt2d(0.0, 0.0);
  aRightPoles.ChangeAt(1) = gp_Pnt2d(1.0, 0.0);
  aRightPoles.ChangeAt(2) = gp_Pnt2d(1.0, 1.0);
  aRightPoles.ChangeAt(3) = gp_Pnt2d(1.0, 2.0);
  aRightPoles.ChangeAt(4) = gp_Pnt2d(0.0, 2.0);
  NCollection_Array1<double> aWeights(1, 5), aKnots(1, 3);
  NCollection_Array1<int>    aMultiplicities(1, 3);
  aWeights.Init(1.0);
  aWeights.ChangeAt(1) = std::sqrt(0.5);
  aWeights.ChangeAt(3) = std::sqrt(0.5);
  aKnots.ChangeAt(0)   = 0.0;
  aKnots.ChangeAt(1)   = 0.5;
  aKnots.ChangeAt(2)   = 1.0;
  aMultiplicities.Init(2);
  aMultiplicities.ChangeAt(0) = 3;
  aMultiplicities.ChangeAt(2) = 3;
  const occ::handle<Geom2d_BSplineCurve> aLeft =
    new Geom2d_BSplineCurve(aLeftPoles, aWeights, aKnots, aMultiplicities, 2);
  const occ::handle<Geom2d_BSplineCurve> aRight =
    new Geom2d_BSplineCurve(aRightPoles, aWeights, aKnots, aMultiplicities, 2);
  for (const bool isReflected : {false, true})
  {
    const occ::handle<Geom2d_Curve> aGuide  = occ::down_cast<Geom2d_Curve>(aRight->Copy());
    const occ::handle<Geom2d_Curve> anOther = occ::down_cast<Geom2d_Curve>(aLeft->Copy());
    const double                    aSide   = isReflected ? -1.0 : 1.0;
    if (isReflected)
    {
      gp_Trsf2d aMirror;
      aMirror.SetMirror(gp_Ax2d(gp_Pnt2d(0.0, 0.0), gp_Dir2d(1.0, 0.0)));
      aGuide->Transform(aMirror);
      anOther->Transform(aMirror);
    }
    for (const double anOriginY : {0.0, 0.25})
    {
      for (const double aDistMax : {0.5, 2.0})
      {
        SCOPED_TRACE(::testing::Message() << isReflected << "," << anOriginY << "," << aDistMax);
        Bisector_BisecCC aBisector(aGuide,
                                   anOther,
                                   aSide,
                                   aSide,
                                   gp_Pnt2d(0.0, aSide * anOriginY),
                                   aDistMax);
        ASSERT_FALSE(aBisector.IsEmpty());
        const double aLastY = std::min(1.0, aDistMax);
        EXPECT_NEAR(aBisector.LastParameter() - aBisector.FirstParameter(),
                    aLastY - anOriginY,
                    Precision::Confusion());
        EXPECT_EQ(aBisector.NbIntervals(), 1);
        for (int i = 0; i <= 10; ++i)
        {
          const double aFraction   = i / 10.0;
          const double anExpectedY = anOriginY + aFraction * (aLastY - anOriginY);
          const double aParameter =
            aBisector.FirstParameter()
            + aFraction * (aBisector.LastParameter() - aBisector.FirstParameter());
          double         aU1, aU2, aDistance;
          const gp_Pnt2d aPoint = aBisector.ValueAndDist(aParameter, aU1, aU2, aDistance);
          EXPECT_NEAR(aPoint.X(), 0.0, Precision::Confusion());
          EXPECT_NEAR(aPoint.Y(), aSide * anExpectedY, Precision::Confusion());
          EXPECT_NEAR(aDistance, anExpectedY, Precision::Confusion());
          EXPECT_DOUBLE_EQ(aU1, aGuide->FirstParameter());
          EXPECT_DOUBLE_EQ(aU2, anOther->LastParameter());
          EXPECT_NEAR(aBisector.Parameter(aPoint), aParameter, Precision::PConfusion());
        }
      }
    }
  }
}

TEST(Bisector_BisecCCTest, CoincidentTangentEndpointsHaveNoIsolatedIntersection)
{
  for (const double anOriginY : {0.0, 1000.0, -1000.0})
  {
    SCOPED_TRACE(anOriginY);
    const gp_Pnt2d               anOrigin(0.0, anOriginY);
    const double                 aGap = Epsilon(std::max(1.0, std::abs(anOriginY)));
    NCollection_Array1<gp_Pnt2d> aLeftPoles(1, 3), aRightPoles(1, 3);
    aLeftPoles.ChangeAt(0)                 = gp_Pnt2d(-1.0, anOriginY + 1.0);
    aLeftPoles.ChangeAt(1)                 = gp_Pnt2d(-0.5, anOriginY);
    aLeftPoles.ChangeAt(2)                 = anOrigin;
    aRightPoles.ChangeAt(0)                = gp_Pnt2d(0.0, anOriginY - aGap);
    aRightPoles.ChangeAt(1)                = gp_Pnt2d(0.5, anOriginY - aGap);
    aRightPoles.ChangeAt(2)                = gp_Pnt2d(1.0, anOriginY + 2.0 - aGap);
    const occ::handle<Geom2d_Curve> aLeft  = new Geom2d_BezierCurve(aLeftPoles);
    const occ::handle<Geom2d_Curve> aRight = new Geom2d_BezierCurve(aRightPoles);

    for (const double aSide : {-1.0, 1.0})
    {
      SCOPED_TRACE(aSide);
      Bisector_BisecCC aBisector;
      aBisector.Perform(aRight, aLeft, aSide, aSide, anOrigin);
      if (aSide < 0.0)
      {
        Bisector_Bisec aNormal;
        aNormal.Perform(aLeft,
                        aRight,
                        anOrigin,
                        gp_Vec2d(1.0, 0.0),
                        gp_Vec2d(-1.0, 0.0),
                        aSide,
                        GeomAbs_Arc,
                        Precision::Confusion(),
                        true);
        const occ::handle<Geom2d_TrimmedCurve>& aNormalCurve = aNormal.Value();
        const double                            aDistance    = 0.1;
        const gp_Pnt2d                          aNormalPoint =
          aNormalCurve->EvalD0(aNormalCurve->FirstParameter() + aDistance);
        EXPECT_NEAR(aNormalPoint.X(), anOrigin.X(), Precision::Confusion());
        EXPECT_NEAR(aNormalPoint.Y(), anOriginY - aDistance, Precision::Confusion());
      }
      EXPECT_EQ(aBisector.IsEmpty(), aSide < 0.0);
      EXPECT_EQ(aBisector.IsExtendAtStart(), aSide > 0.0);
      const Bisector_BisecCC aConstructed(aRight, aLeft, aSide, aSide, anOrigin);
      EXPECT_EQ(aConstructed.LinkBisCurve(aRight->FirstParameter()), aRight->FirstParameter());
      double         aFirstParameter, aSecondParameter, aDistance;
      const gp_Pnt2d aPoint = aBisector.ValueByInt(aRight->FirstParameter(),
                                                   aFirstParameter,
                                                   aSecondParameter,
                                                   aDistance);
      EXPECT_LE(aPoint.Distance(anOrigin), Precision::Confusion());
      EXPECT_EQ(aFirstParameter, aRight->FirstParameter());
      EXPECT_EQ(aSecondParameter, aLeft->LastParameter());
      EXPECT_EQ(aDistance, aSide > 0.0 ? Precision::Infinite() : 0.0);
      const double aNearJoin =
        aRight->FirstParameter()
        + 0.25 * Precision::Confusion() / aRight->EvalD1(aRight->FirstParameter()).D1.Magnitude();
      const gp_Pnt2d aNearPoint =
        aBisector.ValueByInt(aNearJoin, aFirstParameter, aSecondParameter, aDistance);
      EXPECT_LE(aNearPoint.Distance(anOrigin), Precision::Confusion());
      EXPECT_EQ(aDistance, aSide > 0.0 ? Precision::Infinite() : 0.0);
    }
  }
}

TEST(Bisector_BisecCCTest, TangentSplineJoinDoesNotCreateZeroDistanceBranch)
{
  NCollection_Array1<int> aMultiplicities(1, 2);
  aMultiplicities.Init(9);
  NCollection_Array1<gp_Pnt2d> aPoles1(1, 9);
  aPoles1.ChangeAt(0) = gp_Pnt2d(-307.90510129766335, -1940.0660319358035);
  aPoles1.ChangeAt(1) = gp_Pnt2d(-314.63757106336578, -1945.8142194005986);
  aPoles1.ChangeAt(2) = gp_Pnt2d(-321.36902621150097, -1951.5633887030001);
  aPoles1.ChangeAt(3) = gp_Pnt2d(-328.09965872367707, -1957.3137036587514);
  aPoles1.ChangeAt(4) = gp_Pnt2d(-334.82947573080526, -1963.066955250099);
  aPoles1.ChangeAt(5) = gp_Pnt2d(-341.55815318455893, -1968.827555440163);
  aPoles1.ChangeAt(6) = gp_Pnt2d(-348.28483899361532, -1974.6038584825801);
  aPoles1.ChangeAt(7) = gp_Pnt2d(-355.00789547596219, -1980.4098735134642);
  aPoles1.ChangeAt(8) = gp_Pnt2d(-361.72457100928943, -1986.267432239546);
  NCollection_Array1<double> aKnots1(1, 2);
  aKnots1.ChangeAt(0) = 0.5823015797383948;
  aKnots1.ChangeAt(1) = 0.59682247666440191;
  const occ::handle<Geom2d_Curve> aSpline1 =
    new Geom2d_BSplineCurve(aPoles1, aKnots1, aMultiplicities, 8);
  const occ::handle<Geom2d_Curve> aCurve1 =
    new Geom2d_TrimmedCurve(aSpline1, aKnots1.First(), aKnots1.Last());
  NCollection_Array1<gp_Pnt2d> aPoles2(1, 9);
  aPoles2.ChangeAt(0) = gp_Pnt2d(-361.72457100928943, -1986.267432239546);
  aPoles2.ChangeAt(1) = gp_Pnt2d(-372.4080793643862, -1995.5844339735222);
  aPoles2.ChangeAt(2) = gp_Pnt2d(-383.03339604604821, -2005.0137664357874);
  aPoles2.ChangeAt(3) = gp_Pnt2d(-393.61304267256423, -2014.5303608158026);
  aPoles2.ChangeAt(4) = gp_Pnt2d(-404.15835402544838, -2024.111634054927);
  aPoles2.ChangeAt(5) = gp_Pnt2d(-414.67947804916571, -2033.7374888461734);
  aPoles2.ChangeAt(6) = gp_Pnt2d(-425.18537585096237, -2043.3903136340591);
  aPoles2.ChangeAt(7) = gp_Pnt2d(-435.6838217007608, -2053.0549826145161);
  aPoles2.ChangeAt(8) = gp_Pnt2d(-446.18140303109612, -2062.7188557348354);
  const double aGap   = Epsilon(std::abs(aPoles2.At(0).Y()));
  aPoles2.ChangeAt(0).SetY(aPoles2.At(0).Y() + aGap);
  aPoles2.ChangeAt(1).SetY(aPoles2.At(1).Y() + aGap);
  NCollection_Array1<double> aKnots2(1, 2);
  aKnots2.ChangeAt(0) = 0.59682247666440191;
  aKnots2.ChangeAt(1) = 0.61991933816188072;
  const occ::handle<Geom2d_Curve> aSpline2 =
    new Geom2d_BSplineCurve(aPoles2, aKnots2, aMultiplicities, 8);
  const occ::handle<Geom2d_Curve> aCurve2 =
    new Geom2d_TrimmedCurve(aSpline2, aKnots2.First(), aKnots2.Last());

  const auto aLeftEnd    = aCurve1->EvalD1(aCurve1->LastParameter());
  const auto aRightStart = aCurve2->EvalD1(aCurve2->FirstParameter());
  ASSERT_TRUE(aLeftEnd.D1.IsParallel(aRightStart.D1, Precision::Angular()));
  const gp_Pnt2d   anOrigin = aPoles1.Last();
  Bisector_BisecCC aBisector;
  aBisector.Perform(aCurve2, aCurve1, 1.0, 1.0, anOrigin);
  ASSERT_FALSE(aBisector.IsEmpty());
  EXPECT_TRUE(aBisector.IsExtendAtStart());
  double         aFirstParameter, aSecondParameter, aDistance;
  const gp_Pnt2d aPoint =
    aBisector.ValueByInt(aCurve2->FirstParameter(), aFirstParameter, aSecondParameter, aDistance);
  EXPECT_LE(aPoint.Distance(anOrigin), Precision::Confusion());
  EXPECT_EQ(aDistance, Precision::Infinite());
}

TEST(Bisector_BisecCCTest, TangentArcNormalSegmentIsStableAtLargeRadii)
{
  const gp_Dir2d             aTangent(-1.0, -0.3);
  const gp_Dir2d             aNormal(-aTangent.Y(), aTangent.X());
  NCollection_Array1<double> aKnots(1, 2);
  for (const gp_Pnt2d& aJoin :
       {gp_Pnt2d(0.0, 0.0), gp_Pnt2d(5000.0, 1000.0), gp_Pnt2d(-5000.0, -1000.0)})
  {
    SCOPED_TRACE(::testing::Message() << aJoin.X() << "," << aJoin.Y());
    occ::handle<Geom2d_Curve> aCurves[2];
    for (int i = 0; i < 2; ++i)
    {
      const double   aRadius = i == 0 ? 200000.0 : 20000.0;
      const double   aSense  = i == 0 ? 1.0 : -1.0;
      const gp_Pnt2d aCentre = aJoin.Translated(aSense * aRadius * gp_Vec2d(aNormal));
      const gp_Dir2d aRadial(-aSense * aNormal.X(), -aSense * aNormal.Y());
      const occ::handle<Geom2d_Circle> aCircle =
        new Geom2d_Circle(gp_Ax2d(aCentre, aRadial), aRadius, i == 0);
      const occ::handle<Geom2d_BSplineCurve> aCurve =
        Geom2dConvert::CurveToBSplineCurve(new Geom2d_TrimmedCurve(aCircle,
                                                                   i == 0 ? -300.0 / aRadius : 0.0,
                                                                   i == 0 ? 0.0 : 300.0 / aRadius));
      ASSERT_EQ(aCurve->NbKnots(), 2);
      aKnots.Assign(aCurve->Knots());
      BSplCLib::Reparametrize(i == 0 ? 0.0 : 0.5, i == 0 ? 0.5 : 1.0, aKnots);
      aCurve->SetKnots(aKnots);
      aCurves[i] = aCurve;
    }
    const gp_Pnt2d anOrigin = aCurves[0]->EvalD0(aCurves[0]->LastParameter());
    for (const double aSide : {-1.0, 1.0})
    {
      SCOPED_TRACE(aSide);
      Bisector_BisecCC aBisector(aCurves[1], aCurves[0], aSide, aSide, anOrigin, 500.0);
      ASSERT_FALSE(aBisector.IsEmpty());
      const double aLength = aBisector.LastParameter() - aBisector.FirstParameter();
      EXPECT_NEAR(aLength, 500.0, Precision::Confusion());
      if (std::abs(aLength - 500.0) > Precision::Confusion())
      {
        continue;
      }
      for (const double aDistance : {0.0, 157.0, 500.0})
      {
        double         aU1, aU2, aReportedDistance;
        const gp_Pnt2d aPoint     = aBisector.ValueAndDist(aBisector.FirstParameter() + aDistance,
                                                       aU1,
                                                       aU2,
                                                       aReportedDistance);
        const gp_Pnt2d anExpected = anOrigin.Translated(aSide * aDistance * gp_Vec2d(aNormal));
        EXPECT_LE(aPoint.Distance(anExpected), Precision::Confusion());
        EXPECT_NEAR(aReportedDistance, aDistance, Precision::Confusion());
        for (const auto& aCurve : aCurves)
        {
          double aClosest = std::min(aPoint.Distance(aCurve->EvalD0(aCurve->FirstParameter())),
                                     aPoint.Distance(aCurve->EvalD0(aCurve->LastParameter())));
          Geom2dAPI_ProjectPointOnCurve aProjection(aPoint, aCurve);
          if (aProjection.NbPoints() > 0)
          {
            aClosest = std::min(aClosest, aProjection.LowerDistance());
          }
          EXPECT_NEAR(aClosest, aDistance, Precision::Confusion());
        }
      }
    }
  }
}
