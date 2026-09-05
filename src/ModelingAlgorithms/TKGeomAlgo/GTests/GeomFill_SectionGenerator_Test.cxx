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
#include <GeomFill_Profiler.hxx>
#include <GeomFill_SectionGenerator.hxx>
#include <NCollection_Array1.hxx>
#include <Precision.hxx>
#include <Standard_ConstructionError.hxx>
#include <gp_Pnt.hxx>
#include <gp_Pnt2d.hxx>
#include <gp_Vec.hxx>
#include <gp_Vec2d.hxx>

#include <gtest/gtest.h>

namespace
{
occ::handle<Geom_BSplineCurve> makeQuadraticCurve(const double theZ)
{
  NCollection_Array1<gp_Pnt> aPoles(1, 3);
  aPoles(1) = gp_Pnt(0.0, 0.0, theZ);
  aPoles(2) = gp_Pnt(1.0, 1.0, theZ);
  aPoles(3) = gp_Pnt(2.0, 0.0, theZ);
  NCollection_Array1<double> aKnots(1, 2);
  aKnots(1) = 0.0;
  aKnots(2) = 1.0;
  NCollection_Array1<int> aMultiplicities(1, 2);
  aMultiplicities(1) = 3;
  aMultiplicities(2) = 3;
  return new Geom_BSplineCurve(aPoles, aKnots, aMultiplicities, 2);
}

occ::handle<Geom_BSplineCurve> makeQuadraticCurveWithMiddleKnot(const double theZ)
{
  NCollection_Array1<gp_Pnt> aPoles(1, 4);
  aPoles(1) = gp_Pnt(0.0, 0.0, theZ);
  aPoles(2) = gp_Pnt(0.7, 1.0, theZ);
  aPoles(3) = gp_Pnt(1.3, 1.0, theZ);
  aPoles(4) = gp_Pnt(2.0, 0.0, theZ);
  NCollection_Array1<double> aKnots(1, 3);
  aKnots(1) = 0.0;
  aKnots(2) = 0.5;
  aKnots(3) = 1.0;
  NCollection_Array1<int> aMultiplicities(1, 3);
  aMultiplicities(1) = 3;
  aMultiplicities(2) = 1;
  aMultiplicities(3) = 3;
  return new Geom_BSplineCurve(aPoles, aKnots, aMultiplicities, 2);
}
} // namespace

TEST(GeomFill_SectionGenerator_Test,
     RationalTangentSection_EndpointDerivatives_MatchHomogeneousForm)
{
  NCollection_Array1<gp_Pnt> aSectionPoles(1, 3), anAdjacentPoles(1, 3), aTangentPoles(1, 3);
  aSectionPoles(1)   = gp_Pnt(0.0, 0.0, 0.0);
  aSectionPoles(2)   = gp_Pnt(1.0, 1.0, 0.0);
  aSectionPoles(3)   = gp_Pnt(2.0, 0.0, 0.0);
  anAdjacentPoles(1) = gp_Pnt(0.0, 0.0, 2.0);
  anAdjacentPoles(2) = gp_Pnt(1.0, 1.0, 2.0);
  anAdjacentPoles(3) = gp_Pnt(2.0, 0.0, 2.0);
  aTangentPoles(1)   = gp_Pnt(0.2, 0.0, 1.0);
  aTangentPoles(2)   = gp_Pnt(1.0, 1.3, 1.0);
  aTangentPoles(3)   = gp_Pnt(1.8, 0.0, 1.0);

  NCollection_Array1<double> aSectionWeights(1, 3), aTangentWeights(1, 3);
  aSectionWeights(1) = 1.0;
  aSectionWeights(2) = 0.5;
  aSectionWeights(3) = 1.0;
  aTangentWeights(1) = 0.75;
  aTangentWeights(2) = 1.0;
  aTangentWeights(3) = 0.75;
  NCollection_Array1<double> aKnots(1, 2);
  NCollection_Array1<int>    aMultiplicities(1, 2);
  aKnots(1)          = 0.0;
  aKnots(2)          = 1.0;
  aMultiplicities(1) = 3;
  aMultiplicities(2) = 3;

  const occ::handle<Geom_BSplineCurve> aSection =
    new Geom_BSplineCurve(aSectionPoles, aSectionWeights, aKnots, aMultiplicities, 2);
  const occ::handle<Geom_BSplineCurve> anAdjacent =
    new Geom_BSplineCurve(anAdjacentPoles, aSectionWeights, aKnots, aMultiplicities, 2);
  const occ::handle<Geom_BSplineCurve> aTangent =
    new Geom_BSplineCurve(aTangentPoles, aTangentWeights, aKnots, aMultiplicities, 2);
  GeomFill_SectionGenerator aGenerator;
  aGenerator.AddCurve(aSection);
  aGenerator.AddCurve(anAdjacent);
  aGenerator.SetFirstTangentSection(aTangent);
  aGenerator.Perform(Precision::PConfusion());

  NCollection_Array1<gp_Pnt>   aPoles(1, 3);
  NCollection_Array1<gp_Vec>   aDerivatives(1, 3);
  NCollection_Array1<gp_Pnt2d> aPoles2d(1, 1);
  NCollection_Array1<gp_Vec2d> aDerivatives2d(1, 1);
  NCollection_Array1<double>   aWeights(1, 3), aWeightDerivatives(1, 3);
  ASSERT_TRUE(
    aGenerator
      .Section(1, aPoles, aDerivatives, aPoles2d, aDerivatives2d, aWeights, aWeightDerivatives));

  const double aWeightNormalization = (1.0 + 0.5 + 1.0) / 3.0;
  for (int anIndex = 1; anIndex <= 3; ++anIndex)
  {
    const double aNormalizedTangentWeight = aTangentWeights(anIndex) / aWeightNormalization;
    EXPECT_NEAR(aWeightDerivatives(anIndex), aNormalizedTangentWeight - aWeights(anIndex), 1.0e-12);
    const gp_Vec anExpectedDerivative = gp_Vec(aSectionPoles(anIndex), aTangentPoles(anIndex))
                                          .Multiplied(aNormalizedTangentWeight / aWeights(anIndex));
    EXPECT_LE(aDerivatives(anIndex).Subtracted(anExpectedDerivative).Magnitude(), 1.0e-12);
  }
}

TEST(GeomFill_SectionGenerator_Test,
     RationalCurvatureSection_EndpointDerivatives_MatchHomogeneousForm)
{
  NCollection_Array1<gp_Pnt> aSectionPoles(1, 3), anAdjacentPoles(1, 3), aTangentPoles(1, 3),
    aCurvaturePoles(1, 3);
  aSectionPoles(1)   = gp_Pnt(0.0, 0.0, 0.0);
  aSectionPoles(2)   = gp_Pnt(1.0, 1.0, 0.0);
  aSectionPoles(3)   = gp_Pnt(2.0, 0.0, 0.0);
  anAdjacentPoles(1) = gp_Pnt(0.0, 0.0, 2.0);
  anAdjacentPoles(2) = gp_Pnt(1.0, 1.0, 2.0);
  anAdjacentPoles(3) = gp_Pnt(2.0, 0.0, 2.0);
  aTangentPoles(1)   = gp_Pnt(0.1, 0.0, 0.8);
  aTangentPoles(2)   = gp_Pnt(1.0, 1.2, 1.0);
  aTangentPoles(3)   = gp_Pnt(1.9, 0.0, 0.9);
  aCurvaturePoles(1) = gp_Pnt(0.3, 0.1, 1.9);
  aCurvaturePoles(2) = gp_Pnt(1.0, 1.5, 2.2);
  aCurvaturePoles(3) = gp_Pnt(1.7, 0.1, 2.0);

  NCollection_Array1<double> aSectionWeights(1, 3), aTangentWeights(1, 3), aCurvatureWeights(1, 3);
  aSectionWeights(1)   = 1.0;
  aSectionWeights(2)   = 0.6;
  aSectionWeights(3)   = 1.0;
  aTangentWeights(1)   = 0.8;
  aTangentWeights(2)   = 1.1;
  aTangentWeights(3)   = 0.7;
  aCurvatureWeights(1) = 1.2;
  aCurvatureWeights(2) = 0.9;
  aCurvatureWeights(3) = 0.8;
  NCollection_Array1<double> aKnots(1, 2);
  NCollection_Array1<int>    aMultiplicities(1, 2);
  aKnots(1)          = 0.0;
  aKnots(2)          = 1.0;
  aMultiplicities(1) = 3;
  aMultiplicities(2) = 3;

  GeomFill_SectionGenerator aGenerator;
  aGenerator.AddCurve(
    new Geom_BSplineCurve(aSectionPoles, aSectionWeights, aKnots, aMultiplicities, 2));
  aGenerator.AddCurve(
    new Geom_BSplineCurve(anAdjacentPoles, aSectionWeights, aKnots, aMultiplicities, 2));
  aGenerator.SetFirstTangentSection(
    new Geom_BSplineCurve(aTangentPoles, aTangentWeights, aKnots, aMultiplicities, 2));
  aGenerator.SetFirstCurvatureSection(
    new Geom_BSplineCurve(aCurvaturePoles, aCurvatureWeights, aKnots, aMultiplicities, 2));
  aGenerator.Perform(Precision::PConfusion());

  const occ::handle<Geom_BSplineCurve> aSection =
    occ::down_cast<Geom_BSplineCurve>(aGenerator.Curve(1));
  const occ::handle<Geom_BSplineCurve> aTangent =
    occ::down_cast<Geom_BSplineCurve>(aGenerator.Curve(3));
  const occ::handle<Geom_BSplineCurve> aCurvature =
    occ::down_cast<Geom_BSplineCurve>(aGenerator.Curve(4));
  ASSERT_FALSE(aSection.IsNull());
  ASSERT_FALSE(aTangent.IsNull());
  ASSERT_FALSE(aCurvature.IsNull());

  NCollection_Array1<gp_Pnt>   aPoles(1, 3);
  NCollection_Array1<gp_Vec>   aD1(1, 3), aD2(1, 3);
  NCollection_Array1<gp_Pnt2d> aPoles2d(1, 1);
  NCollection_Array1<gp_Vec2d> aD1Poles2d(1, 1), aD2Poles2d(1, 1);
  NCollection_Array1<double>   aWeights(1, 3), aD1Weights(1, 3), aD2Weights(1, 3);
  ASSERT_TRUE(aGenerator.SectionWithCurvature(1,
                                              aPoles,
                                              aD1,
                                              aD2,
                                              aPoles2d,
                                              aD1Poles2d,
                                              aD2Poles2d,
                                              aWeights,
                                              aD1Weights,
                                              aD2Weights));

  for (int anIndex = 1; anIndex <= 3; ++anIndex)
  {
    const double aWeight          = aSection->Weight(anIndex);
    const double aTangentWeight   = aTangent->Weight(anIndex);
    const double aCurvatureWeight = aCurvature->Weight(anIndex);
    const double aD1Weight        = aTangentWeight - aWeight;
    const gp_Vec anExpectedD1 =
      gp_Vec(aSection->Pole(anIndex), aTangent->Pole(anIndex)).Multiplied(aTangentWeight / aWeight);
    const gp_XYZ aSecondHomogeneous = aCurvature->Pole(anIndex).XYZ() * aCurvatureWeight
                                      - 2.0 * aTangent->Pole(anIndex).XYZ() * aTangentWeight
                                      + aSection->Pole(anIndex).XYZ() * aWeight;
    const double aD2Weight = aCurvatureWeight - 2.0 * aTangentWeight + aWeight;
    gp_Vec       anExpectedD2;
    anExpectedD2.SetXYZ((aSecondHomogeneous - aSection->Pole(anIndex).XYZ() * aD2Weight
                         - 2.0 * anExpectedD1.XYZ() * aD1Weight)
                        / aWeight);

    EXPECT_NEAR(aD1Weights(anIndex), aD1Weight, 1.0e-12);
    EXPECT_NEAR(aD2Weights(anIndex), aD2Weight, 1.0e-12);
    EXPECT_LE(aD1(anIndex).Subtracted(anExpectedD1).Magnitude(), 1.0e-12);
    EXPECT_LE(aD2(anIndex).Subtracted(anExpectedD2).Magnitude(), 1.0e-12);
  }
}

TEST(GeomFill_SectionGenerator_Test, RepeatedEndpointConstraints_ReplaceAuxiliaryCurves)
{
  GeomFill_SectionGenerator aGenerator;
  aGenerator.AddCurve(makeQuadraticCurve(0.0));
  aGenerator.AddCurve(makeQuadraticCurve(3.0));
  aGenerator.SetFirstTangentSection(makeQuadraticCurveWithMiddleKnot(1.0));
  aGenerator.SetFirstCurvatureSection(makeQuadraticCurveWithMiddleKnot(2.0));
  aGenerator.SetLastTangentSection(makeQuadraticCurveWithMiddleKnot(2.0));
  aGenerator.SetLastCurvatureSection(makeQuadraticCurveWithMiddleKnot(1.0));
  aGenerator.SetFirstTangentSection(makeQuadraticCurve(0.8));
  aGenerator.SetFirstCurvatureSection(makeQuadraticCurve(1.9));
  aGenerator.SetLastTangentSection(makeQuadraticCurve(2.2));
  aGenerator.SetLastCurvatureSection(makeQuadraticCurve(1.1));
#ifndef No_Exception
  EXPECT_THROW((void)aGenerator.AddCurve(makeQuadraticCurve(4.0)), Standard_ConstructionError);
#endif
  aGenerator.Perform(Precision::PConfusion());

  int aNbPoles = 0, aNbKnots = 0, aDegree = 0, aNbPoles2d = 0;
  aGenerator.GetShape(aNbPoles, aNbKnots, aDegree, aNbPoles2d);
  EXPECT_EQ(aNbPoles, 3);
  EXPECT_EQ(aNbKnots, 2);

  NCollection_Array1<gp_Pnt>   aPoles(1, 3);
  NCollection_Array1<gp_Vec>   aD1(1, 3), aD2(1, 3);
  NCollection_Array1<gp_Pnt2d> aPoles2d(1, 1);
  NCollection_Array1<gp_Vec2d> aD1Poles2d(1, 1), aD2Poles2d(1, 1);
  NCollection_Array1<double>   aWeights(1, 3), aD1Weights(1, 3), aD2Weights(1, 3);
  EXPECT_TRUE(aGenerator.SectionWithCurvature(1,
                                              aPoles,
                                              aD1,
                                              aD2,
                                              aPoles2d,
                                              aD1Poles2d,
                                              aD2Poles2d,
                                              aWeights,
                                              aD1Weights,
                                              aD2Weights));
  EXPECT_TRUE(aGenerator.SectionWithCurvature(2,
                                              aPoles,
                                              aD1,
                                              aD2,
                                              aPoles2d,
                                              aD1Poles2d,
                                              aD2Poles2d,
                                              aWeights,
                                              aD1Weights,
                                              aD2Weights));
#ifndef No_Exception
  EXPECT_THROW((void)aGenerator.SetFirstTangent(gp_Vec(0.0, 0.0, 1.0)), Standard_ConstructionError);
#endif

  GeomFill_SectionGenerator aConstantGenerator;
  aConstantGenerator.AddCurve(makeQuadraticCurve(0.0));
  aConstantGenerator.AddCurve(makeQuadraticCurve(3.0));
  aConstantGenerator.SetFirstTangentSection(makeQuadraticCurveWithMiddleKnot(1.0));
  aConstantGenerator.SetFirstCurvatureSection(makeQuadraticCurveWithMiddleKnot(2.0));
  aConstantGenerator.SetLastTangentSection(makeQuadraticCurveWithMiddleKnot(2.0));
  aConstantGenerator.SetLastCurvatureSection(makeQuadraticCurveWithMiddleKnot(1.0));
  aConstantGenerator.SetFirstTangent(gp_Vec(0.0, 0.0, 2.0));
  aConstantGenerator.SetLastTangent(gp_Vec(0.0, 0.0, -2.0));
  aConstantGenerator.Perform(Precision::PConfusion());
  EXPECT_FALSE(aConstantGenerator.SectionWithCurvature(1,
                                                       aPoles,
                                                       aD1,
                                                       aD2,
                                                       aPoles2d,
                                                       aD1Poles2d,
                                                       aD2Poles2d,
                                                       aWeights,
                                                       aD1Weights,
                                                       aD2Weights));
  ASSERT_TRUE(
    aConstantGenerator.Section(1, aPoles, aD1, aPoles2d, aD1Poles2d, aWeights, aD1Weights));
  for (const gp_Vec& aDerivative : aD1)
  {
    EXPECT_LE(aDerivative.Subtracted(gp_Vec(0.0, 0.0, 2.0)).Magnitude(), 1.0e-12);
  }
  EXPECT_FALSE(aConstantGenerator.SectionWithCurvature(2,
                                                       aPoles,
                                                       aD1,
                                                       aD2,
                                                       aPoles2d,
                                                       aD1Poles2d,
                                                       aD2Poles2d,
                                                       aWeights,
                                                       aD1Weights,
                                                       aD2Weights));
  ASSERT_TRUE(
    aConstantGenerator.Section(2, aPoles, aD1, aPoles2d, aD1Poles2d, aWeights, aD1Weights));
  for (const gp_Vec& aDerivative : aD1)
  {
    EXPECT_LE(aDerivative.Subtracted(gp_Vec(0.0, 0.0, -2.0)).Magnitude(), 1.0e-12);
  }
}

TEST(GeomFill_SectionGenerator_Test, BaseClassAddCurveAfterConstraint_PerformRejects)
{
  GeomFill_SectionGenerator aGenerator;
  aGenerator.AddCurve(makeQuadraticCurve(0.0));
  aGenerator.AddCurve(makeQuadraticCurve(3.0));
  aGenerator.SetFirstTangentSection(makeQuadraticCurve(1.0));

  GeomFill_Profiler& aProfiler = aGenerator;
  aProfiler.AddCurve(makeQuadraticCurve(4.0));
#ifndef No_Exception
  EXPECT_THROW((void)aProfiler.Perform(Precision::PConfusion()), Standard_ConstructionError);
#endif
}
