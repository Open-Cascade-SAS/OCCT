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
#include <GeomFill_Generator.hxx>
#include <Geom_BSplineCurve.hxx>
#include <Geom_BSplineSurface.hxx>
#include <GeomFill_Profiler.hxx>
#include <GeomFill_SectionGenerator.hxx>
#include <NCollection_Array1.hxx>
#include <NCollection_BaseAllocator.hxx>
#include <NCollection_IncAllocator.hxx>
#include <gp_Pnt.hxx>
#include <gp_Pnt2d.hxx>
#include <initializer_list>
#include <array>
#include <limits>

namespace
{
//! Generator with a caller-selected allocator for section blocks.
class GeomFill_TestGenerator : public GeomFill_Generator
{
public:
  //! Use the given allocator for section blocks.
  explicit GeomFill_TestGenerator(const occ::handle<NCollection_BaseAllocator>& theAllocator);
  //! Return the allocator used by the section array.
  const occ::handle<NCollection_BaseAllocator>& Allocator() const;
};

//==================================================================================================

GeomFill_TestGenerator::GeomFill_TestGenerator(
  const occ::handle<NCollection_BaseAllocator>& theAllocator)
{
  myCurves = NCollection_DynamicArray<occ::handle<Geom_Curve>>(8, theAllocator);
}

//==================================================================================================

const occ::handle<NCollection_BaseAllocator>& GeomFill_TestGenerator::Allocator() const
{
  return myCurves.Allocator();
}

//==================================================================================================

void checkSectionGeometry(const double theScale)
{
  GeomFill_Generator                            aGenerator;
  std::array<occ::handle<Geom_BSplineCurve>, 2> aCurves;
  NCollection_Array1<double>                    aWeights(size_t{3}), aKnots(size_t{2});
  aWeights.ChangeAt(0) = theScale;
  aWeights.ChangeAt(1) = theScale / 2;
  aWeights.ChangeAt(2) = theScale;
  aKnots.ChangeAt(0)   = 0;
  aKnots.ChangeAt(1)   = 1;
  NCollection_Array1<int> aMults(size_t{2});
  aMults.Init(3);
  NCollection_Array1<gp_Pnt> aPoles(size_t{3});
  for (size_t aCurveIndex = 0; aCurveIndex < aCurves.size(); ++aCurveIndex)
  {
    aPoles.ChangeAt(0)   = gp_Pnt(-1, 0, static_cast<double>(aCurveIndex));
    aPoles.ChangeAt(1)   = gp_Pnt(0, 1, static_cast<double>(aCurveIndex));
    aPoles.ChangeAt(2)   = gp_Pnt(1, 0, static_cast<double>(aCurveIndex));
    aCurves[aCurveIndex] = new Geom_BSplineCurve(aPoles, aWeights, aKnots, aMults, 2);
    aGenerator.AddCurve(aCurves[aCurveIndex]);
  }
  for (int anIteration = 0; anIteration < 2; ++anIteration)
  {
    aGenerator.Perform(0);
    const occ::handle<Geom_BSplineSurface> aSurface =
      occ::down_cast<Geom_BSplineSurface>(aGenerator.Surface());
    ASSERT_FALSE(aSurface.IsNull());
    for (size_t aCurveIndex = 0; aCurveIndex < aCurves.size(); ++aCurveIndex)
    {
      EXPECT_EQ(aCurves[aCurveIndex]->Weight(1), theScale);
      EXPECT_EQ(aCurves[aCurveIndex]->Weight(2), theScale / 2);
      EXPECT_EQ(aCurves[aCurveIndex]->Weight(3), theScale);
      for (const double anU : {0., .2, .5, .8, 1.})
      {
        // The unscaled rational row has numerator (2*u-1,u*(1-u),height*W).
        const double aW = 1 - anU + anU * anU;
        const gp_Pnt anExpected((2 * anU - 1) / aW,
                                anU * (1 - anU) / aW,
                                static_cast<double>(aCurveIndex));
        EXPECT_NEAR(aSurface->EvalD0(anU, static_cast<double>(aCurveIndex)).Distance(anExpected),
                    0,
                    1.e-12);
      }
    }
  }
}
} // namespace

//==================================================================================================

TEST(GeomFill_ProfilerTest, FiniteLargeWeightScaleKeepsSectionGeometry)
{
  checkSectionGeometry(std::numeric_limits<double>::max() / 2);
}

//==================================================================================================

TEST(GeomFill_ProfilerTest, SmallWeightScaleKeepsSectionGeometry)
{
  checkSectionGeometry(1.e-200);
}

//==================================================================================================

TEST(GeomFill_ProfilerTest, OrdinaryWeightScaleKeepsSectionGeometry)
{
  checkSectionGeometry(1);
}

//==================================================================================================

TEST(GeomFill_ProfilerTest, SectionOrderPreservedAcrossGrowthAndRepeatedPerform)
{
  const occ::handle<NCollection_IncAllocator> anAllocator = new NCollection_IncAllocator;
  GeomFill_TestGenerator                      aGenerator(anAllocator);
  GeomFill_SectionGenerator                   aSectionGenerator;
  const occ::handle<Geom_Curve>*              aFirstCurve = nullptr;
  NCollection_Array1<gp_Pnt>                  aPoles(size_t{3});
  NCollection_Array1<double>                  aKnots(size_t{2});
  aKnots.ChangeAt(0) = 0;
  aKnots.ChangeAt(1) = 1;
  NCollection_Array1<int> aMultiplicities(size_t{2});
  aMultiplicities.Init(3);
  for (int aSectionIndex = 0; aSectionIndex < 33; ++aSectionIndex)
  {
    aPoles.ChangeAt(0) = gp_Pnt(-1, 0, aSectionIndex);
    aPoles.ChangeAt(1) = gp_Pnt(0, 1, aSectionIndex);
    aPoles.ChangeAt(2) = gp_Pnt(1, 0, aSectionIndex);
    const occ::handle<Geom_BSplineCurve> aCurve =
      new Geom_BSplineCurve(aPoles, aKnots, aMultiplicities, 2);
    if (aSectionIndex % 2 != 0)
    {
      aCurve->InsertKnot(0.5, 1, 0);
    }
    aGenerator.AddCurve(aCurve);
    if (aSectionIndex == 0)
    {
      aFirstCurve = &aGenerator.Curve(1);
    }
    EXPECT_EQ(aFirstCurve, &aGenerator.Curve(1));
    aSectionGenerator.AddCurve(aCurve);
  }
  NCollection_Array1<gp_Pnt>   aSectionPoles(size_t{4});
  NCollection_Array1<double>   aSectionWeights(size_t{4});
  NCollection_Array1<gp_Pnt2d> aSectionPoles2d;
  for (int anIteration = 0; anIteration < 2; ++anIteration)
  {
    aGenerator.Perform(0);
    aSectionGenerator.Perform(0);
    EXPECT_EQ(aGenerator.Allocator(), anAllocator);
    EXPECT_EQ(aGenerator.NbKnots(), 3);
    EXPECT_EQ(aGenerator.NbPoles(), 4);
    const occ::handle<Geom_BSplineSurface> aSurface =
      occ::down_cast<Geom_BSplineSurface>(aGenerator.Surface());
    ASSERT_FALSE(aSurface.IsNull());
    for (int aSectionIndex = 1; aSectionIndex <= 33; ++aSectionIndex)
    {
      const double aHeight = static_cast<double>(aSectionIndex - 1);
      EXPECT_EQ(aGenerator.Curve(aSectionIndex)->EvalD0(0).Z(), aHeight);
      aSectionGenerator.Section(aSectionIndex, aSectionPoles, aSectionPoles2d, aSectionWeights);
      EXPECT_EQ(aSectionPoles.First().Z(), aHeight);
      EXPECT_EQ(aSectionPoles.Last().Z(), aHeight);
      aGenerator.Poles(aSectionIndex, aSectionPoles);
      aGenerator.Weights(aSectionIndex, aSectionWeights);
      EXPECT_EQ(aSectionPoles.First().Z(), aHeight);
      for (const double aWeight : aSectionWeights)
      {
        EXPECT_EQ(aWeight, 1.);
      }
      for (const double aU : {0., 0.25, 0.5, 0.75, 1.})
      {
        EXPECT_NEAR(
          aSurface->EvalD0(aU, aHeight).Distance(gp_Pnt(2 * aU - 1, 2 * aU * (1 - aU), aHeight)),
          0,
          1.e-14);
      }
    }
  }
}

//==================================================================================================

TEST(GeomFill_ProfilerTest, WeightNormalizationPreservesMeanAcrossUniformIntermediateWeights)
{
  const std::array<std::array<double, 3>, 4> aWeightSets = {
    {{3, 1.5, 1.5}, {0.5, 0.75, 0.75}, {1.5, 0.75, 0.75}, {1.5, 3, 1.5}}};
  NCollection_Array1<gp_Pnt> aPoles(size_t{3});
  aPoles.ChangeAt(0) = gp_Pnt(-1, 0, 0);
  aPoles.ChangeAt(1) = gp_Pnt(0, 1, 0);
  aPoles.ChangeAt(2) = gp_Pnt(1, 0, 0);
  NCollection_Array1<double> aKnots(size_t{2});
  aKnots.ChangeAt(0) = 0;
  aKnots.ChangeAt(1) = 1;
  NCollection_Array1<int> aMultiplicities(size_t{2});
  aMultiplicities.Init(3);
  NCollection_Array1<double> aScaledWeights(size_t{3});
  NCollection_Array1<gp_Pnt> aRaisedPoles(size_t{3});
  aRaisedPoles.ChangeAt(0) = gp_Pnt(-1, 0, 1);
  aRaisedPoles.ChangeAt(1) = gp_Pnt(0, 1, 1);
  aRaisedPoles.ChangeAt(2) = gp_Pnt(1, 0, 1);
  for (const auto& aWeights : aWeightSets)
  {
    const double                         aMean = (aWeights[0] + aWeights[1] + aWeights[2]) / 3;
    const NCollection_Array1<double>     aWeightArray(aWeights[0], 1, 3);
    const occ::handle<Geom_BSplineCurve> aSource =
      new Geom_BSplineCurve(aPoles, aWeightArray, aKnots, aMultiplicities, 2);
    GeomFill_Profiler aProfiler;
    aProfiler.AddCurve(aSource);
    aProfiler.Perform(0);
    const occ::handle<Geom_BSplineCurve> aResult =
      occ::down_cast<Geom_BSplineCurve>(aProfiler.Curve(1));
    ASSERT_FALSE(aResult.IsNull());
    for (size_t aPoleIndex = 0; aPoleIndex < aWeights.size(); ++aPoleIndex)
    {
      EXPECT_NEAR(aResult->WeightsArray().At(aPoleIndex), aWeights[aPoleIndex] / aMean, 1.e-14);
    }
    for (const double aU : {0., 0.25, 0.5, 0.75, 1.})
    {
      EXPECT_NEAR(aResult->EvalD0(aU).Distance(aSource->EvalD0(aU)), 0, 1.e-14);
    }
    GeomFill_Generator aGenerator;
    aGenerator.AddCurve(aSource);
    for (size_t aPoleIndex = 0; aPoleIndex < aWeights.size(); ++aPoleIndex)
    {
      aScaledWeights.ChangeAt(aPoleIndex) = 2 * aWeights[aPoleIndex];
    }
    aGenerator.AddCurve(
      new Geom_BSplineCurve(aRaisedPoles, aScaledWeights, aKnots, aMultiplicities, 2));
    aGenerator.Perform(0);
    const occ::handle<Geom_BSplineSurface> aSurface =
      occ::down_cast<Geom_BSplineSurface>(aGenerator.Surface());
    ASSERT_FALSE(aSurface.IsNull());
    for (const double aU : {0., 0.25, 0.5, 0.75, 1.})
    {
      const gp_Pnt aPoint = aSource->EvalD0(aU);
      // A common weight scale must not change the height between matching sections.
      EXPECT_NEAR(aSurface->EvalD0(aU, 0.5).Distance(gp_Pnt(aPoint.X(), aPoint.Y(), 0.5)),
                  0,
                  1.e-14);
    }
  }
}
