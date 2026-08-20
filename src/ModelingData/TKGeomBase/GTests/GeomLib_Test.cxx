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

#include <GeomLib.hxx>
#include <Geom2dAdaptor_Curve.hxx>
#include <Geom2d_BezierCurve.hxx>
#include <Geom2d_Line.hxx>
#include <Geom_BezierCurve.hxx>
#include <Geom_BezierSurface.hxx>
#include <Geom_BSplineSurface.hxx>
#include <Geom_BoundedCurve.hxx>
#include <Geom_BoundedSurface.hxx>
#include <Geom_Plane.hxx>
#include <Geom_SphericalSurface.hxx>
#include <NCollection_Array1.hxx>
#include <NCollection_Array2.hxx>
#include <NCollection_LinearVector.hxx>
#include <gp.hxx>
#include <gp_Ax2.hxx>
#include <gp_GTrsf2d.hxx>
#include <gp_Mat2d.hxx>
#include <gp_Pnt.hxx>
#include <gp_Vec.hxx>

namespace
{
occ::handle<Geom_BSplineSurface> makeSurface(const bool theRational)
{
  NCollection_Array2<gp_Pnt> aPoles(1, 3, 1, 3);
  NCollection_Array2<double> aWeights(1, 3, 1, 3);
  for (size_t aUIndex = 0; aUIndex < aPoles.RowSize(); ++aUIndex)
  {
    const double aX = 0.5 * static_cast<double>(aUIndex);
    for (size_t aVIndex = 0; aVIndex < aPoles.ColSize(); ++aVIndex)
    {
      const double aY                   = 0.5 * static_cast<double>(aVIndex);
      aPoles.ChangeAt(aUIndex, aVIndex) = gp_Pnt(aX, aY, aX * aX + 0.2 * aX * aY);
      aWeights.ChangeAt(aUIndex, aVIndex) =
        theRational && aUIndex == 1 && aVIndex == 1 ? 1.75 : 1.0;
    }
  }

  NCollection_Array1<double> aUKnots(1, 2), aVKnots(1, 2);
  aUKnots(1) = aVKnots(1) = 0.0;
  aUKnots(2) = aVKnots(2) = 1.0;
  NCollection_Array1<int> aUMults(1, 2), aVMults(1, 2);
  aUMults.Init(3);
  aVMults.Init(3);

  if (theRational)
  {
    return new Geom_BSplineSurface(aPoles, aWeights, aUKnots, aVKnots, aUMults, aVMults, 2, 2);
  }
  return new Geom_BSplineSurface(aPoles, aUKnots, aVKnots, aUMults, aVMults, 2, 2);
}

occ::handle<Geom_BSplineSurface> makeRationalBoundarySurface(const bool theProjectivelyEqual)
{
  NCollection_Array2<gp_Pnt> aPoles(1, 2, 1, 3);
  NCollection_Array2<double> aWeights(1, 2, 1, 3);
  const double               aFirstWeights[] = {1.0, 2.0, 1.0};
  const double               aLastWeights[]  = {2.0, theProjectivelyEqual ? 4.0 : 1.0, 2.0};
  for (size_t aVIndex = 0; aVIndex < aPoles.ColSize(); ++aVIndex)
  {
    const gp_Pnt aFirstPole(1.0, 1.0 + static_cast<double>(aVIndex), 0.2 * aVIndex);
    aPoles.ChangeAt(0, aVIndex)   = aFirstPole;
    aWeights.ChangeAt(0, aVIndex) = aFirstWeights[aVIndex];
    aWeights.ChangeAt(1, aVIndex) = aLastWeights[aVIndex];
    aPoles.ChangeAt(1, aVIndex) =
      theProjectivelyEqual
        ? aFirstPole
        : gp_Pnt(aFirstPole.XYZ() * (aFirstWeights[aVIndex] / aLastWeights[aVIndex]));
  }

  NCollection_Array1<double> aUKnots(1, 2), aVKnots(1, 2);
  aUKnots(1) = aVKnots(1) = 0.0;
  aUKnots(2) = aVKnots(2) = 1.0;
  NCollection_Array1<int> aUMults(1, 2), aVMults(1, 2);
  aUMults.Init(2);
  aVMults.Init(3);
  return new Geom_BSplineSurface(aPoles, aWeights, aUKnots, aVKnots, aUMults, aVMults, 1, 2);
}

void checkExtendedSurface(const bool theRational, const bool theInU, const bool theAfter)
{
  const occ::handle<Geom_BSplineSurface> aSource              = makeSurface(theRational);
  const double                           aBoundary            = theAfter ? 1.0 : 0.0;
  constexpr double                       anExtensionLength    = 0.4;
  constexpr double                       aContinuityTolerance = 1.0e-10;
  // Pointwise length is approximate because normalized boundary tangents are represented by the
  // extended surface pole grid.
  constexpr double                       aRelativeLengthTolerance = 5.0e-3;
  const double                           aLengthTolerance =
    anExtensionLength * aRelativeLengthTolerance;

  NCollection_Array1<gp_Pnt> aBoundaryPoints(size_t{3});
  NCollection_Array1<gp_Vec> aBoundaryDerivatives(size_t{3});
  for (size_t anIndex = 0; anIndex < aBoundaryPoints.Size(); ++anIndex)
  {
    const double aParameter = 0.25 + 0.25 * static_cast<double>(anIndex);
    gp_Vec       aUDerivative, aVDerivative;
    aSource->D1(theInU ? aBoundary : aParameter,
                theInU ? aParameter : aBoundary,
                aBoundaryPoints.ChangeAt(anIndex),
                aUDerivative,
                aVDerivative);
    aBoundaryDerivatives.ChangeAt(anIndex) = theInU ? aUDerivative : aVDerivative;
  }

  occ::handle<Geom_BoundedSurface> anExtended = aSource;
  GeomLib::ExtendSurfByLength(anExtended, anExtensionLength, 1, theInU, theAfter);
  const occ::handle<Geom_BSplineSurface> aResult = occ::down_cast<Geom_BSplineSurface>(anExtended);
  ASSERT_FALSE(aResult.IsNull());

  double aUMin, aUMax, aVMin, aVMax;
  aResult->Bounds(aUMin, aUMax, aVMin, aVMax);
  const double anEndParameter = theInU ? (theAfter ? aUMax : aUMin) : (theAfter ? aVMax : aVMin);
  EXPECT_TRUE(theAfter ? anEndParameter > aBoundary : anEndParameter < aBoundary);
  for (size_t anIndex = 0; anIndex < aBoundaryPoints.Size(); ++anIndex)
  {
    const double aParameter = 0.25 + 0.25 * static_cast<double>(anIndex);
    gp_Pnt       aPoint;
    gp_Vec       aUDerivative, aVDerivative;
    aResult->D1(theInU ? aBoundary : aParameter,
                theInU ? aParameter : aBoundary,
                aPoint,
                aUDerivative,
                aVDerivative);
    EXPECT_LE(aPoint.Distance(aBoundaryPoints.At(anIndex)), aContinuityTolerance);
    const gp_Vec& aDerivative = theInU ? aUDerivative : aVDerivative;
    EXPECT_LE((aDerivative - aBoundaryDerivatives.At(anIndex)).Magnitude(), aContinuityTolerance);

    aResult->D0(theInU ? anEndParameter : aParameter, theInU ? aParameter : anEndParameter, aPoint);
    EXPECT_NEAR(aPoint.Distance(aBoundaryPoints.At(anIndex)), anExtensionLength, aLengthTolerance);
  }

  if (theRational)
  {
    ASSERT_TRUE(aResult->IsURational() || aResult->IsVRational());
    for (const double aWeight : aResult->WeightsArray())
    {
      EXPECT_GT(aWeight, 0.0);
    }
  }
}
} // namespace

TEST(GeomLibTest, ExtendSurfByLength_CoversAllDirectionsAndRationality)
{
  for (const bool isRational : {false, true})
  {
    for (const bool isInU : {false, true})
    {
      for (const bool isAfter : {false, true})
      {
        SCOPED_TRACE(testing::Message() << "rational=" << isRational << ", inU=" << isInU
                                        << ", after=" << isAfter);
        checkExtendedSurface(isRational, isInU, isAfter);
      }
    }
  }
}

TEST(GeomLibTest, CancelDenominatorDerivative_PreservesSurfaceGeometry)
{
  occ::handle<Geom_BSplineSurface> aSurface = makeSurface(true);
  NCollection_Array2<gp_Pnt>       aReferencePoints(size_t{5}, size_t{5});
  for (size_t aUIndex = 0; aUIndex < aReferencePoints.RowSize(); ++aUIndex)
  {
    const double aU = 0.25 * static_cast<double>(aUIndex);
    for (size_t aVIndex = 0; aVIndex < aReferencePoints.ColSize(); ++aVIndex)
    {
      const double aV = 0.25 * static_cast<double>(aVIndex);
      aSurface->D0(aU, aV, aReferencePoints.ChangeAt(aUIndex, aVIndex));
    }
  }

  GeomLib::CancelDenominatorDerivative(aSurface, true, false);

  for (size_t aUIndex = 0; aUIndex < aReferencePoints.RowSize(); ++aUIndex)
  {
    const double aU = 0.25 * static_cast<double>(aUIndex);
    for (size_t aVIndex = 0; aVIndex < aReferencePoints.ColSize(); ++aVIndex)
    {
      const double aV = 0.25 * static_cast<double>(aVIndex);
      gp_Pnt       aPoint;
      aSurface->D0(aU, aV, aPoint);
      EXPECT_LE(aPoint.Distance(aReferencePoints.At(aUIndex, aVIndex)), 1.0e-9);
    }
  }
}

TEST(GeomLibTest, CancelDenominatorDerivative_IdempotentCallPreservesParameterRange)
{
  occ::handle<Geom_BSplineSurface> aSurface = makeSurface(true);
  NCollection_Array1<double>       aUKnots(aSurface->UKnots());
  aUKnots.ChangeFirst() = 2.0;
  aUKnots.ChangeLast()  = 5.0;
  aSurface->SetUKnots(aUKnots);

  GeomLib::CancelDenominatorDerivative(aSurface, true, false);
  EXPECT_DOUBLE_EQ(aSurface->UKnot(1), 2.0);
  EXPECT_DOUBLE_EQ(aSurface->UKnot(aSurface->NbUKnots()), 5.0);

  GeomLib::CancelDenominatorDerivative(aSurface, true, false);
  EXPECT_DOUBLE_EQ(aSurface->UKnot(1), 2.0);
  EXPECT_DOUBLE_EQ(aSurface->UKnot(aSurface->NbUKnots()), 5.0);
}

TEST(GeomLibTest, ExtendCurveToPoint_ReachesTargetAndPreservesInitialEnd)
{
  NCollection_Array1<gp_Pnt> aPoles(1, 2);
  aPoles(1)                             = gp_Pnt(0.0, 0.0, 0.0);
  aPoles(2)                             = gp_Pnt(1.0, 0.0, 0.0);
  occ::handle<Geom_BoundedCurve> aCurve = new Geom_BezierCurve(aPoles);

  const gp_Pnt aTarget(2.0, 0.0, 0.0);
  GeomLib::ExtendCurveToPoint(aCurve, aTarget, 1, true);

  EXPECT_LE(aCurve->Value(aCurve->FirstParameter()).Distance(aPoles(1)), 1.0e-12);
  EXPECT_LE(aCurve->Value(aCurve->LastParameter()).Distance(aTarget), 1.0e-12);
}

TEST(GeomLibTest, AdjustExtremity_ReachesEndpointsAndTangentDirections)
{
  NCollection_Array1<gp_Pnt> aPoles(1, 2);
  aPoles(1)                             = gp_Pnt(0.0, 0.0, 0.0);
  aPoles(2)                             = gp_Pnt(1.0, 0.0, 0.0);
  occ::handle<Geom_BoundedCurve> aCurve = new Geom_BezierCurve(aPoles);

  const gp_Pnt aFirst(0.0, 1.0, 0.0), aLast(1.0, 1.0, 0.0);
  const gp_Vec aTangent(1.0, 0.0, 0.0);
  GeomLib::AdjustExtremity(aCurve, aFirst, aLast, aTangent, aTangent);

  gp_Pnt aPoint;
  gp_Vec aDerivative;
  aCurve->D1(aCurve->FirstParameter(), aPoint, aDerivative);
  EXPECT_LE(aPoint.Distance(aFirst), 1.0e-12);
  EXPECT_TRUE(aDerivative.IsParallel(aTangent, 1.0e-12));
  aCurve->D1(aCurve->LastParameter(), aPoint, aDerivative);
  EXPECT_LE(aPoint.Distance(aLast), 1.0e-12);
  EXPECT_TRUE(aDerivative.IsParallel(aTangent, 1.0e-12));
}

TEST(GeomLibTest, ParameterArrays_SupportArbitraryLowerBounds)
{
  NCollection_Array1<double> anInput(-2, 4);
  const double               anInputValues[] = {0.0, 0.1, 0.2, 0.4, 0.6, 0.8, 1.0};
  for (size_t anIndex = 0; anIndex < anInput.Size(); ++anIndex)
  {
    anInput.ChangeAt(anIndex) = anInputValues[anIndex];
  }

  NCollection_Array1<double> aReduced;
  GeomLib::RemovePointsFromArray(4, anInput, aReduced);
  const double aReducedValues[] = {0.0, 0.1, 0.4, 0.8, 1.0};
  ASSERT_EQ(aReduced.Size(), std::size(aReducedValues));
  for (size_t anIndex = 0; anIndex < aReduced.Size(); ++anIndex)
  {
    EXPECT_DOUBLE_EQ(aReduced.At(anIndex), aReducedValues[anIndex]);
  }

  NCollection_Array1<double> aSparse(-1, 1);
  aSparse.ChangeAt(0) = 0.0;
  aSparse.ChangeAt(1) = 0.6;
  aSparse.ChangeAt(2) = 1.0;
  NCollection_Array1<double> aDense;
  GeomLib::DensifyArray1OfReal(5, aSparse, aDense);
  const double aDenseValues[] = {0.0, 1.0 / 3.0, 0.6, 2.0 / 3.0, 1.0};
  ASSERT_EQ(aDense.Size(), std::size(aDenseValues));
  for (size_t anIndex = 0; anIndex < aDense.Size(); ++anIndex)
  {
    EXPECT_DOUBLE_EQ(aDense.At(anIndex), aDenseValues[anIndex]);
  }
}

TEST(GeomLibTest, DensifyArray_ConstantRangeInitializesEveryValue)
{
  NCollection_Array1<double> aConstantParameters(-1, 1);
  aConstantParameters.Init(4.25);

  NCollection_Array1<double> aDense;
  GeomLib::DensifyArray1OfReal(7, aConstantParameters, aDense);
  ASSERT_EQ(aDense.Size(), 7U);
  for (const double aParameter : aDense)
  {
    EXPECT_DOUBLE_EQ(aParameter, 4.25);
  }
}

TEST(GeomLibTest, FuseIntervals_SupportsArbitraryLowerBounds)
{
  NCollection_Array1<double> aFirst(-1, 1), aSecond(3, 5);
  aFirst.ChangeAt(0)  = 0.0;
  aFirst.ChangeAt(1)  = 0.5;
  aFirst.ChangeAt(2)  = 1.0;
  aSecond.ChangeAt(0) = 0.25;
  aSecond.ChangeAt(1) = 0.5000000001;
  aSecond.ChangeAt(2) = 0.75;

  NCollection_LinearVector<double> aResult;
  aResult.Append(-1.0); // Verify that reusable output storage is cleared before filling.
  GeomLib::FuseIntervals(aFirst, aSecond, aResult, 1.0e-8);
  const double anExpected[] = {0.0, 0.25, 0.50000000005, 0.75, 1.0};
  ASSERT_EQ(aResult.Size(), std::size(anExpected));
  for (size_t anIndex = 0; anIndex < aResult.Size(); ++anIndex)
  {
    EXPECT_DOUBLE_EQ(aResult.Value(anIndex), anExpected[anIndex]);
  }
}

TEST(GeomLibTest, FuseIntervals_AdjustsMatchesToFirstInput)
{
  NCollection_Array1<double> aFirst(size_t{2}), aSecond(size_t{2});
  aFirst.ChangeAt(0)  = 0.0;
  aFirst.ChangeAt(1)  = 1.0;
  aSecond.ChangeAt(0) = 1.0e-10;
  aSecond.ChangeAt(1) = 1.0 + 1.0e-10;

  NCollection_LinearVector<double> aResult;
  GeomLib::FuseIntervals(aFirst, aSecond, aResult, 1.0e-8, true);
  ASSERT_EQ(aResult.Size(), 2U);
  EXPECT_DOUBLE_EQ(aResult.Value(0), aFirst.At(0));
  EXPECT_DOUBLE_EQ(aResult.Value(1), aFirst.At(1));
}

TEST(GeomLibTest, GTransformLine_UsesExactVectorialDirection)
{
  const occ::handle<Geom2d_Line> aLine = new Geom2d_Line(gp_Pnt2d(1.0, 2.0), gp_Dir2d(1.0, 0.0));
  const gp_Mat2d                 aMatrix(gp_XY(2.0, 0.5), gp_XY(1.0, 3.0));
  const gp_GTrsf2d               aTransform(aMatrix, gp_XY(4.0, -1.0));

  const occ::handle<Geom2d_Line> aResult =
    occ::down_cast<Geom2d_Line>(GeomLib::GTransform(aLine, aTransform));
  ASSERT_FALSE(aResult.IsNull());
  EXPECT_LE(aResult->Location().Distance(gp_Pnt2d(8.0, 5.5)), 1.0e-12);
  EXPECT_TRUE(aResult->Direction().IsParallel(gp_Dir2d(gp_XY(2.0, 0.5)), 1.0e-12));
}

TEST(GeomLibTest, To3d_PreservesRationalBezierGeometry)
{
  NCollection_Array1<gp_Pnt2d> aPoles(1, 3);
  aPoles(1) = gp_Pnt2d(0.0, 0.0);
  aPoles(2) = gp_Pnt2d(0.5, 1.0);
  aPoles(3) = gp_Pnt2d(1.0, 0.0);
  NCollection_Array1<double> aWeights(1, 3);
  aWeights(1)                                    = 1.0;
  aWeights(2)                                    = 2.0;
  aWeights(3)                                    = 1.0;
  const occ::handle<Geom2d_BezierCurve> aCurve2d = new Geom2d_BezierCurve(aPoles, aWeights);

  const gp_Ax2                        aPosition(gp::Origin(), gp::DZ(), gp::DX());
  const occ::handle<Geom_BezierCurve> aCurve3d =
    occ::down_cast<Geom_BezierCurve>(GeomLib::To3d(aPosition, aCurve2d));
  ASSERT_FALSE(aCurve3d.IsNull());
  ASSERT_TRUE(aCurve3d->IsRational());
  for (const double aParameter : {0.0, 0.3, 1.0})
  {
    const gp_Pnt2d aPoint2d = aCurve2d->Value(aParameter);
    const gp_Pnt   aPoint3d = aCurve3d->Value(aParameter);
    EXPECT_LE(aPoint3d.Distance(gp_Pnt(aPoint2d.X(), aPoint2d.Y(), 0.0)), 1.0e-12);
  }
}

TEST(GeomLibTest, IsIsoLine_AcceptsShortNonDegenerateLinearBezier)
{
  NCollection_Array1<gp_Pnt2d> aPoles(1, 2);
  aPoles(1)                                      = gp_Pnt2d(2.0, 3.0);
  aPoles(2)                                      = gp_Pnt2d(2.0 + 1.0e-5, 3.0);
  const occ::handle<Geom2d_Curve>      aCurve    = new Geom2d_BezierCurve(aPoles);
  const occ::handle<Adaptor2d_Curve2d> anAdaptor = new Geom2dAdaptor_Curve(aCurve);

  bool   isU = true, isForward = false;
  double aParameter = 0.0;
  EXPECT_TRUE(GeomLib::isIsoLine(anAdaptor, isU, aParameter, isForward));
  EXPECT_FALSE(isU);
  EXPECT_TRUE(isForward);
  EXPECT_DOUBLE_EQ(aParameter, 3.0);
}

TEST(GeomLibTest, RationalBSplineClosedness_UsesCompleteHomogeneousPoles)
{
  const occ::handle<Geom_BSplineSurface> aDifferentSurface = makeRationalBoundarySurface(false);
  EXPECT_GT(aDifferentSurface->Value(0.0, 0.25).Distance(aDifferentSurface->Value(1.0, 0.25)), 0.1);
  EXPECT_FALSE(GeomLib::IsBSplUClosed(aDifferentSurface, 0.0, 1.0, 1.0e-10));

  const occ::handle<Geom_BSplineSurface> anEquivalentSurface = makeRationalBoundarySurface(true);
  EXPECT_LE(anEquivalentSurface->Value(0.0, 0.25).Distance(anEquivalentSurface->Value(1.0, 0.25)),
            1.0e-12);
  EXPECT_TRUE(GeomLib::IsBSplUClosed(anEquivalentSurface, 0.0, 1.0, 1.0e-10));
}

TEST(GeomLibTest, RationalBezierClosedness_AccountsForWeights)
{
  NCollection_Array2<gp_Pnt> aPoles(1, 2, 1, 3);
  NCollection_Array2<double> aWeights(1, 2, 1, 3);
  for (size_t aUIndex = 0; aUIndex < aPoles.RowSize(); ++aUIndex)
  {
    for (size_t aVIndex = 0; aVIndex < aPoles.ColSize(); ++aVIndex)
    {
      aPoles.ChangeAt(aUIndex, aVIndex)   = gp_Pnt(1.0, 1.0 + static_cast<double>(aVIndex), 0.0);
      aWeights.ChangeAt(aUIndex, aVIndex) = aUIndex == 0 && aVIndex == 1 ? 2.0 : 1.0;
    }
  }
  const occ::handle<Geom_BezierSurface> aSurface = new Geom_BezierSurface(aPoles, aWeights);

  EXPECT_GT(aSurface->Value(0.0, 0.25).Distance(aSurface->Value(1.0, 0.25)), 0.1);
  EXPECT_FALSE(GeomLib::IsBzUClosed(aSurface, 0.0, 1.0, 1.0e-10));
}

TEST(GeomLibTest, NormEstim_HandlesRegularAndPoleParameters)
{
  const gp_Ax3 aPosition(gp::Origin(), gp::DZ(), gp::DX());
  gp_Dir       aNormal;

  const occ::handle<Geom_Surface> aPlane = new Geom_Plane(aPosition);
  EXPECT_EQ(GeomLib::NormEstim(aPlane, gp_Pnt2d(2.0, -3.0), 1.0e-9, aNormal), 0);
  EXPECT_TRUE(aNormal.IsParallel(gp::DZ(), 1.0e-12));

  const occ::handle<Geom_Surface> aSphere = new Geom_SphericalSurface(aPosition, 2.0);
  const double                    aHalfPi = 0.5 * std::acos(-1.0);
  EXPECT_EQ(GeomLib::NormEstim(aSphere, gp_Pnt2d(0.4, aHalfPi), 1.0e-9, aNormal), 1);
  EXPECT_TRUE(aNormal.IsParallel(gp::DZ(), 1.0e-7));
}
