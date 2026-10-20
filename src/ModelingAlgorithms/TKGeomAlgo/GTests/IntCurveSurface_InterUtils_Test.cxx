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

#include <IntCurveSurface_InterUtils.pxx>
#include <Adaptor3d_Curve.hxx>
#include <Adaptor3d_Surface.hxx>
#include <Adaptor3d_HSurfaceTool.hxx>
#include <Intf_SectionPoint.hxx>
#include <gp_Pnt.hxx>
#include <gtest/gtest.h>
#include <GeomAdaptor_Curve.hxx>
#include <GeomAdaptor_Surface.hxx>
#include <Geom_BSplineCurve.hxx>
#include <Geom_BSplineSurface.hxx>
#include <Geom_Plane.hxx>
#include <Geom_Line.hxx>
#include <IntCurveSurface_TheHCurveTool.hxx>
#include <IntCurveSurface_HInter.hxx>
#include <NCollection_Array1.hxx>
#include <NCollection_Array2.hxx>
#include <Precision.hxx>
#include <gp_Dir.hxx>
#include <gp_Pln.hxx>
#include <gp_Vec.hxx>
#include <array>
#include <cmath>
#include <initializer_list>
#include <tuple>

namespace
{

class TestPolyhedron
{
public:
  TestPolyhedron()
  {
    myPoints[0] = gp_Pnt(0.0, 0.0, 0.0);
    myPoints[1] = gp_Pnt(2.0, 0.0, 0.0);
    myPoints[2] = gp_Pnt(2.0, 0.0, 0.0);

    myU[0] = 0.0;
    myU[1] = 1.0;
    myU[2] = 2.0;

    myV[0] = 0.0;
    myV[1] = 0.0;
    myV[2] = 0.0;
  }

  void Triangle(const int, int& theP1, int& theP2, int& theP3) const
  {
    theP1 = 1;
    theP2 = 2;
    theP3 = 3;
  }

  const gp_Pnt& Point(const int theIndex) const { return myPoints[theIndex - 1]; }

  void Parameters(const int theIndex, double& theU, double& theV) const
  {
    theU = myU[theIndex - 1];
    theV = myV[theIndex - 1];
  }

private:
  gp_Pnt myPoints[3];
  double myU[3];
  double myV[3];
};

class TestPolygon
{
public:
  double ApproxParamOnCurve(const int theIndex, const double theParamOnLine) const
  {
    return 10.0 * theIndex + theParamOnLine;
  }
};

} // namespace

TEST(IntCurveSurface_InterUtils, SectionPointToParameters_DegenerateFaceFallsBackToLongestEdge)
{
  const TestPolyhedron aPolyhedron;
  const TestPolygon    aPolygon;

  const Intf_SectionPoint
    aSectionPoint(gp_Pnt(0.5, 0.0, 0.0), Intf_EDGE, 0, 2, 0.4, Intf_FACE, 1, 0, 0.0, 0.0);

  double aU = 0.0;
  double aV = 0.0;
  double aW = 0.0;
  IntCurveSurface_InterUtils::SectionPointToParameters(aSectionPoint,
                                                       aPolyhedron,
                                                       aPolygon,
                                                       aU,
                                                       aV,
                                                       aW);

  EXPECT_NEAR(aU, 0.25, 1.0e-12);
  EXPECT_NEAR(aV, 0.0, 1.0e-12);
  EXPECT_NEAR(aW, 20.4, 1.0e-12);
}

//==================================================================================================

TEST(IntCurveSurface_InterUtils, SortingPreservesDistinctParameterTriples)
{
  // Parameter proximity does not imply geometric proximity: a reparameterized
  // curve or surface can travel an arbitrary distance over these intervals.
  IntCurveSurface_InterUtils::SortedStartPoints aPoints;
  aPoints.Append({3.0e-12, 3.0, 2.0e-12});
  aPoints.Append({2.0e-12, 2.0, 1.0e-12});
  aPoints.Append({1.0e-12, 1.0, 0.0});
  IntCurveSurface_InterUtils::SortStartPoints(aPoints);
  ASSERT_EQ(aPoints.Size(), 3);
  for (int aPointIndex = 0; aPointIndex < 3; ++aPointIndex)
  {
    EXPECT_DOUBLE_EQ(aPoints.Value(aPointIndex).U, (aPointIndex + 1) * 1.0e-12);
    EXPECT_DOUBLE_EQ(aPoints.Value(aPointIndex).V, aPointIndex + 1);
    EXPECT_DOUBLE_EQ(aPoints.Value(aPointIndex).W, aPointIndex * 1.0e-12);
  }
}

//==================================================================================================

TEST(IntCurveSurface_InterUtilsTest, TransitionIsIndependentOfParameterScale)
{
  // The same vertical segment crosses the same planar square in every chart.
  // Stretching a knot range changes derivative magnitude, not crossing sense.
  NCollection_Array1<gp_Pnt> aCurvePoles(size_t(2));
  aCurvePoles.ChangeAt(0) = gp_Pnt(0, 0, -1);
  aCurvePoles.ChangeAt(1) = gp_Pnt(0, 0, 1);
  NCollection_Array2<gp_Pnt> aSurfacePoles(size_t(2), size_t(2));
  aSurfacePoles.ChangeAt(0, 0) = gp_Pnt(-1, -1, 0);
  aSurfacePoles.ChangeAt(1, 0) = gp_Pnt(1, -1, 0);
  aSurfacePoles.ChangeAt(0, 1) = gp_Pnt(-1, 1, 0);
  aSurfacePoles.ChangeAt(1, 1) = gp_Pnt(1, 1, 0);
  NCollection_Array1<double> aCurveKnots(size_t(2));
  NCollection_Array1<double> aSurfaceKnots(size_t(2));
  NCollection_Array1<int>    aMultiplicities(size_t(2));
  aMultiplicities.Init(2);
  aCurveKnots.ChangeAt(0)   = 0;
  aSurfaceKnots.ChangeAt(0) = 0;
  for (const double aCurveRange : {1.0, 1.0e9})
  {
    aCurveKnots.ChangeAt(1) = aCurveRange;
    const occ::handle<Geom_BSplineCurve> aCurve =
      new Geom_BSplineCurve(aCurvePoles, aCurveKnots, aMultiplicities, 1);
    const occ::handle<Adaptor3d_Curve> aCurveAdaptor = new GeomAdaptor_Curve(aCurve);
    for (const double aSurfaceRange : {1.0, 1.0e9})
    {
      aSurfaceKnots.ChangeAt(1)                       = aSurfaceRange;
      const occ::handle<Geom_BSplineSurface> aSurface = new Geom_BSplineSurface(aSurfacePoles,
                                                                                aSurfaceKnots,
                                                                                aSurfaceKnots,
                                                                                aMultiplicities,
                                                                                aMultiplicities,
                                                                                1,
                                                                                1);
      const occ::handle<Adaptor3d_Surface>   aSurfaceAdaptor = new GeomAdaptor_Surface(aSurface);
      IntCurveSurface_TransitionOnCurve      aTransition     = IntCurveSurface_Tangent;
      IntCurveSurface_InterUtils::ComputeTransitions<occ::handle<Adaptor3d_Curve>,
                                                     IntCurveSurface_TheHCurveTool,
                                                     occ::handle<Adaptor3d_Surface>,
                                                     Adaptor3d_HSurfaceTool>(aCurveAdaptor,
                                                                             aCurveRange / 2,
                                                                             aTransition,
                                                                             aSurfaceAdaptor,
                                                                             aSurfaceRange / 2,
                                                                             aSurfaceRange / 2);
      EXPECT_EQ(aTransition, IntCurveSurface_Out)
        << "curve range " << aCurveRange << ", surface range " << aSurfaceRange;
    }
  }
}

//==================================================================================================

TEST(IntCurveSurface_InterUtilsTest, TransitionPreservesAngularTangency)
{
  const occ::handle<Adaptor3d_Surface> aSurface =
    new GeomAdaptor_Surface(new Geom_Plane(gp_Pln(gp_Pnt(), gp_Dir(0, 0, 1))));
  for (const double aSlope : {0.0, 0.5e-12, -0.5e-12, 1.0e-12, -1.0e-12, 2.0e-12, -2.0e-12})
  {
    const occ::handle<Adaptor3d_Curve> aCurve =
      new GeomAdaptor_Curve(new Geom_Line(gp_Pnt(), gp_Dir(1, 0, aSlope)));
    IntCurveSurface_TransitionOnCurve aTransition;
    IntCurveSurface_InterUtils::ComputeTransitions<occ::handle<Adaptor3d_Curve>,
                                                   IntCurveSurface_TheHCurveTool,
                                                   occ::handle<Adaptor3d_Surface>,
                                                   Adaptor3d_HSurfaceTool>(aCurve,
                                                                           0,
                                                                           aTransition,
                                                                           aSurface,
                                                                           0,
                                                                           0);
    const IntCurveSurface_TransitionOnCurve anExpected =
      std::abs(aSlope) <= 1.0e-12 ? IntCurveSurface_Tangent
                                  : (aSlope > 0 ? IntCurveSurface_Out : IntCurveSurface_In);
    EXPECT_EQ(aTransition, anExpected);
  }
}

//==================================================================================================

TEST(IntCurveSurface_InterUtils, SortingPreservesEqualComponentsAndDuplicates)
{
  IntCurveSurface_InterUtils::SortedStartPoints aPoints;
  IntCurveSurface_InterUtils::SortStartPoints(aPoints);
  EXPECT_EQ(aPoints.Size(), 0);
  aPoints.Append({1, 2, 1});
  aPoints.Append({0, 7, 1});
  aPoints.Append({1, 2, 1});
  aPoints.Append({1, 1, 1});
  aPoints.Append({9, 9, 0});
  const std::array<std::tuple<double, double, double>, 5> anExpected = {
    {{9, 9, 0}, {0, 7, 1}, {1, 1, 1}, {1, 2, 1}, {1, 2, 1}}};
  for (int anIteration = 0; anIteration < 2; ++anIteration)
  {
    IntCurveSurface_InterUtils::SortStartPoints(aPoints);
    ASSERT_EQ(aPoints.Size(), 5);
    for (size_t anIndex = 0; anIndex < anExpected.size(); ++anIndex)
    {
      const auto& [aU, aV, aW] = anExpected[anIndex];
      EXPECT_EQ(aPoints.Value(anIndex).U, aU);
      EXPECT_EQ(aPoints.Value(anIndex).V, aV);
      EXPECT_EQ(aPoints.Value(anIndex).W, aW);
    }
  }
}

//==================================================================================================

TEST(IntCurveSurface_InterUtils, SortingPreservesLargeSeedSets)
{
  IntCurveSurface_InterUtils::SortedStartPoints aPoints;
  constexpr size_t                              aCount = 2048;
  aPoints.Reserve(aCount);
  for (size_t anIndex = aCount; anIndex > 0; --anIndex)
  {
    const double aKey = static_cast<double>((anIndex * 73) % aCount);
    aPoints.Append({aKey * 0.5, -aKey, aKey});
  }
  IntCurveSurface_InterUtils::SortStartPoints(aPoints);
  ASSERT_EQ(aPoints.Size(), aCount);
  for (size_t anIndex = 0; anIndex < aCount; ++anIndex)
  {
    const double aKey = static_cast<double>(anIndex);
    EXPECT_EQ(aPoints.Value(anIndex).U, aKey * 0.5);
    EXPECT_EQ(aPoints.Value(anIndex).V, -aKey);
    EXPECT_EQ(aPoints.Value(anIndex).W, aKey);
  }
  aPoints.Clear(false);
  aPoints.Append({2, 3, 1});
  IntCurveSurface_InterUtils::SortStartPoints(aPoints);
  ASSERT_EQ(aPoints.Size(), 1u);
  EXPECT_EQ(aPoints.First().U, 2);
  EXPECT_EQ(aPoints.First().V, 3);
  EXPECT_EQ(aPoints.First().W, 1);
}

namespace
{
//! Supplies derivatives directly to test direction scaling and exact degeneracy.
struct IntCurveSurface_TestTransitionTools
{
  static void D1(const std::array<gp_Vec, 3>& theDerivatives, double, gp_Pnt&, gp_Vec& theTangent);
  static void D1(const std::array<gp_Vec, 3>& theDerivatives,
                 double,
                 double,
                 gp_Pnt&,
                 gp_Vec& theDU,
                 gp_Vec& theDV);
};

//==================================================================================================

void IntCurveSurface_TestTransitionTools::D1(const std::array<gp_Vec, 3>& theDerivatives,
                                             double,
                                             gp_Pnt&,
                                             gp_Vec& theTangent)
{
  theTangent = theDerivatives[2];
}

//==================================================================================================

void IntCurveSurface_TestTransitionTools::D1(const std::array<gp_Vec, 3>& theDerivatives,
                                             double,
                                             double,
                                             gp_Pnt&,
                                             gp_Vec& theDU,
                                             gp_Vec& theDV)
{
  theDU = theDerivatives[0];
  theDV = theDerivatives[1];
}
} // namespace

//==================================================================================================

TEST(IntCurveSurface_InterUtilsTest, TransitionHandlesExtremeAndZeroDerivatives)
{
  for (const double aScale : std::array<double, 5>{{1.e-200, 1.e-9, 1.0, 1.e150, 1.e200}})
  {
    SCOPED_TRACE(aScale);
    std::array<gp_Vec, 3> aDerivatives = {
      {gp_Vec(aScale, 0, 0), gp_Vec(0, aScale, 0), gp_Vec(0, 0, aScale)}};
    const auto aTransition = [&]() {
      IntCurveSurface_TransitionOnCurve aResult;
      IntCurveSurface_InterUtils::ComputeTransitions<std::array<gp_Vec, 3>,
                                                     IntCurveSurface_TestTransitionTools,
                                                     std::array<gp_Vec, 3>,
                                                     IntCurveSurface_TestTransitionTools>(
        aDerivatives,
        0,
        aResult,
        aDerivatives,
        0,
        0);
      return aResult;
    };
    EXPECT_EQ(aTransition(), IntCurveSurface_Out);
    aDerivatives[2].Reverse();
    EXPECT_EQ(aTransition(), IntCurveSurface_In);
    aDerivatives[1] = aDerivatives[0];
    EXPECT_EQ(aTransition(), IntCurveSurface_Tangent);
    aDerivatives[1] = gp_Vec(0, aScale, 0);
    aDerivatives[2] = gp_Vec();
    EXPECT_EQ(aTransition(), IntCurveSurface_Tangent);
  }
}

//==================================================================================================

TEST(IntCurveSurface_InterUtilsTest, TransitionHandlesObliqueAndNearlyParallelDerivatives)
{
  const std::array<double, 6> aScales = {
    {std::nextafter(0., 1.), 1.e-200, 1.e-30, 1., 1.e30, 1.e200}};
  for (const double aSurfaceScale : aScales)
  {
    for (const double aCurveScale : aScales)
    {
      SCOPED_TRACE(aSurfaceScale);
      SCOPED_TRACE(aCurveScale);
      std::array<gp_Vec, 3> aDerivatives = {{gp_Vec(1, 2, 3) * aSurfaceScale,
                                             gp_Vec(3, 1, 2) * aSurfaceScale,
                                             gp_Vec(1, 7, -5) * aCurveScale}};
      for (const bool isReversed : {false, true})
      {
        if (isReversed)
        {
          aDerivatives[2].Reverse();
        }
        IntCurveSurface_TransitionOnCurve aTransition;
        IntCurveSurface_InterUtils::ComputeTransitions<std::array<gp_Vec, 3>,
                                                       IntCurveSurface_TestTransitionTools,
                                                       std::array<gp_Vec, 3>,
                                                       IntCurveSurface_TestTransitionTools>(
          aDerivatives,
          0,
          aTransition,
          aDerivatives,
          0,
          0);
        EXPECT_EQ(aTransition, isReversed ? IntCurveSurface_In : IntCurveSurface_Out);
      }
    }
  }
  // The surface normal remains defined when the chart directions almost agree.
  const std::array<gp_Vec, 3> aDerivatives = {
    {gp_Vec(1, 1, 1), gp_Vec(1, 1 + 1.e-12, 1), gp_Vec(-1, 0, 1)}};
  IntCurveSurface_TransitionOnCurve aTransition;
  IntCurveSurface_InterUtils::ComputeTransitions<std::array<gp_Vec, 3>,
                                                 IntCurveSurface_TestTransitionTools,
                                                 std::array<gp_Vec, 3>,
                                                 IntCurveSurface_TestTransitionTools>(aDerivatives,
                                                                                      0,
                                                                                      aTransition,
                                                                                      aDerivatives,
                                                                                      0,
                                                                                      0);
  EXPECT_EQ(aTransition, IntCurveSurface_Out);
}

//==================================================================================================

TEST(IntCurveSurface_InterUtilsTest, TransitionPreservesObliqueAngularTangencyAcrossScales)
{
  for (const double aSurfaceScale : {1.e-200, 1., 1.e30, 1.e200})
  {
    for (const double aCurveScale : {1.e-200, 1., 1.e30, 1.e200})
    {
      for (const double aSlope : {0., 0.25e-12, -0.25e-12, 2.e-12, -2.e-12})
      {
        SCOPED_TRACE(aSurfaceScale);
        SCOPED_TRACE(aCurveScale);
        SCOPED_TRACE(aSlope);
        const gp_Vec                aNormal(1, 7, -5);
        const std::array<gp_Vec, 3> aDerivatives = {
          {gp_Vec(1, 2, 3) * aSurfaceScale,
           gp_Vec(3, 1, 2) * aSurfaceScale,
           (gp_Vec(7, -1, 0) + aNormal * aSlope) * aCurveScale}};
        IntCurveSurface_TransitionOnCurve aTransition;
        IntCurveSurface_InterUtils::ComputeTransitions<std::array<gp_Vec, 3>,
                                                       IntCurveSurface_TestTransitionTools,
                                                       std::array<gp_Vec, 3>,
                                                       IntCurveSurface_TestTransitionTools>(
          aDerivatives,
          0,
          aTransition,
          aDerivatives,
          0,
          0);
        // The sine of the crossing angle is approximately slope * sqrt(75 / 50).
        const IntCurveSurface_TransitionOnCurve anExpected =
          std::abs(aSlope) < 1.e-12 ? IntCurveSurface_Tangent
                                    : (aSlope < 0 ? IntCurveSurface_In : IntCurveSurface_Out);
        EXPECT_EQ(aTransition, anExpected);
      }
    }
  }
}

//==================================================================================================

TEST(IntCurveSurface_InterUtilsTest, SlowCurvePlaneCrossingRetainsTransition)
{
  NCollection_Array1<gp_Pnt> aPoles(size_t{2});
  aPoles.ChangeAt(0) = gp_Pnt(0, 0, -1);
  aPoles.ChangeAt(1) = gp_Pnt(0, 0, 1);
  NCollection_Array1<double> aKnots(size_t{2});
  aKnots.ChangeAt(0) = 0;
  aKnots.ChangeAt(1) = 1.e9;
  NCollection_Array1<int> aMultiplicities(size_t{2});
  aMultiplicities.Init(2);
  const occ::handle<Geom_BSplineCurve> aCurve =
    new Geom_BSplineCurve(aPoles, aKnots, aMultiplicities, 1);
  const occ::handle<Adaptor3d_Surface> aSurface =
    new GeomAdaptor_Surface(new Geom_Plane(gp_Pln(gp_Pnt(), gp_Dir(0, 0, 1))));
  IntCurveSurface_HInter anIntersector;
  for (int anIteration = 0; anIteration < 2; ++anIteration)
  {
    const occ::handle<Adaptor3d_Curve> anAdaptor = new GeomAdaptor_Curve(aCurve);
    anIntersector.Perform(anAdaptor, aSurface);
    ASSERT_TRUE(anIntersector.IsDone());
    ASSERT_EQ(anIntersector.NbPoints(), 1);
    EXPECT_NEAR(anIntersector.Point(1).Pnt().Distance(gp_Pnt()), 0, Precision::Confusion());
    // The knot range is 1e9, so this parameter check allows a few double ULPs.
    EXPECT_NEAR(anIntersector.Point(1).W(), 5.e8, 1.e-6);
    EXPECT_EQ(anIntersector.Point(1).Transition(),
              anIteration == 0 ? IntCurveSurface_Out : IntCurveSurface_In);
    aCurve->Reverse();
  }
}
