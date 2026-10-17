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

#include <Geom2dInt_GInter.hxx>
#include <Geom2dAdaptor_Curve.hxx>
#include <Geom2d_BSplineCurve.hxx>
#include <IntRes2d_IntersectionPoint.hxx>
#include <IntRes2d_Domain.hxx>
#include <NCollection_Array1.hxx>
#include <Precision.hxx>
#include <gp_Pnt2d.hxx>

#include <array>

namespace
{
//! Counts polygon sampling requests for a spline adaptor.
class Geom2dInt_CountingCurve : public Geom2dAdaptor_Curve
{
public:
  explicit Geom2dInt_CountingCurve(const occ::handle<Geom2d_BSplineCurve>& theCurve);
  int    NbSamples() const override;
  size_t SampleRequests() const;

private:
  mutable size_t mySampleRequests = 0; // Number of polygon sampling requests.
};

//==================================================================================================

Geom2dInt_CountingCurve::Geom2dInt_CountingCurve(const occ::handle<Geom2d_BSplineCurve>& theCurve)
    : Geom2dAdaptor_Curve(theCurve)
{
}

//==================================================================================================

int Geom2dInt_CountingCurve::NbSamples() const
{
  ++mySampleRequests;
  return Geom2dAdaptor_Curve::NbSamples();
}

//==================================================================================================

size_t Geom2dInt_CountingCurve::SampleRequests() const
{
  return mySampleRequests;
}

//==================================================================================================

occ::handle<Geom2d_BSplineCurve> makeZigzag(int theNbSpans, double theOffset, bool theRational)
{
  NCollection_Array1<gp_Pnt2d> aPoles(1, theNbSpans + 1);
  NCollection_Array1<double>   aWeights(1, theNbSpans + 1);
  NCollection_Array1<double>   aKnots(1, theNbSpans + 1);
  NCollection_Array1<int>      aMults(1, theNbSpans + 1);
  aMults.Init(1);
  aMults.ChangeAt(0) = aMults.ChangeAt(theNbSpans) = 2;
  for (int i = 0; i <= theNbSpans; ++i)
  {
    aPoles.ChangeAt(i)   = gp_Pnt2d(i, theOffset + (i % 2));
    aWeights.ChangeAt(i) = 1.0 + (i % 2);
    aKnots.ChangeAt(i)   = i;
  }
  return theRational ? new Geom2d_BSplineCurve(aPoles, aWeights, aKnots, aMults, 1)
                     : new Geom2d_BSplineCurve(aPoles, aKnots, aMults, 1);
}
} // namespace

//==================================================================================================

TEST(Geom2dInt_GInterTest, DisjointCompositeCurvesAvoidQuadraticSampling)
{
  std::array<size_t, 3> aRequests;
  size_t                anIndex = 0;
  for (const int aNbSpans : {8, 16, 32})
  {
    SCOPED_TRACE(aNbSpans);
    Geom2dInt_CountingCurve aCurve1(makeZigzag(aNbSpans, 0.0, false));
    Geom2dInt_CountingCurve aCurve2(makeZigzag(aNbSpans, 0.25, false));
    Geom2dInt_GInter        anIntersection(aCurve1,
                                    aCurve2,
                                    Precision::Confusion(),
                                    Precision::Confusion());
    ASSERT_TRUE(anIntersection.IsDone());
    EXPECT_EQ(anIntersection.NbPoints(), 0);
    EXPECT_EQ(anIntersection.NbSegments(), 0);
    aRequests[anIndex++] = aCurve1.SampleRequests() + aCurve2.SampleRequests();
  }
  // Doubling the span count must not quadruple polygon sampling work.
  EXPECT_LE(aRequests[1], 3 * aRequests[0]);
  EXPECT_LE(aRequests[2], 3 * aRequests[1]);
}

//==================================================================================================

TEST(Geom2dInt_GInterTest, CompositeSplineCrossings)
{
  constexpr int                aNbSpans = 16;
  NCollection_Array1<gp_Pnt2d> aPoles(1, 2);
  aPoles.ChangeAt(0)                             = gp_Pnt2d(-1.0, 0.5);
  aPoles.ChangeAt(1)                             = gp_Pnt2d(aNbSpans + 1.0, 0.5);
  const double                     aKnotValues[] = {0.0, 1.0};
  const NCollection_Array1<double> aKnots(aKnotValues[0], 1, 2);
  NCollection_Array1<int>          aMults(1, 2);
  aMults.Init(2);
  Geom2dAdaptor_Curve aCurve2(new Geom2d_BSplineCurve(aPoles, aKnots, aMults, 1));
  for (const bool isRational : {false, true})
  {
    SCOPED_TRACE(isRational);
    Geom2dAdaptor_Curve aCurve1(makeZigzag(aNbSpans, 0.0, isRational));
    Geom2dInt_GInter    anIntersection(aCurve1,
                                    aCurve2,
                                    Precision::Confusion(),
                                    Precision::Confusion());
    ASSERT_TRUE(anIntersection.IsDone());
    ASSERT_EQ(anIntersection.NbPoints(), aNbSpans);
    EXPECT_EQ(anIntersection.NbSegments(), 0);
    for (int i = 1; i <= anIntersection.NbPoints(); ++i)
    {
      const IntRes2d_IntersectionPoint& aPoint = anIntersection.Point(i);
      EXPECT_NEAR(aPoint.Value().Y(), 0.5, Precision::Confusion());
      EXPECT_LE(
        aCurve1.EvalD0(aPoint.ParamOnFirst()).Distance(aCurve2.EvalD0(aPoint.ParamOnSecond())),
        Precision::Confusion());
    }
    Geom2dAdaptor_Curve aTrimmed(aCurve1.Curve(), 3.25, 6.75);
    Geom2dInt_GInter    aTrimmedIntersection(aTrimmed,
                                          aCurve2,
                                          Precision::Confusion(),
                                          Precision::Confusion());
    ASSERT_TRUE(aTrimmedIntersection.IsDone());
    EXPECT_EQ(aTrimmedIntersection.NbPoints(), 4);
    EXPECT_EQ(aTrimmedIntersection.NbSegments(), 0);

    NCollection_Array1<gp_Pnt2d> aWidePoles(1, 4);
    aWidePoles.ChangeAt(0)                             = gp_Pnt2d(-1.0, 0.5);
    aWidePoles.ChangeAt(1)                             = gp_Pnt2d(1.0, 0.5);
    aWidePoles.ChangeAt(2)                             = gp_Pnt2d(13.0, 0.5);
    aWidePoles.ChangeAt(3)                             = gp_Pnt2d(17.0, 0.5);
    const double                     aWideKnotValues[] = {0.0, 1.0, 2.0};
    const int                        aWideMultValues[] = {3, 1, 3};
    const NCollection_Array1<double> aWideKnots(aWideKnotValues[0], 1, 3);
    const NCollection_Array1<int>    aWideMults(aWideMultValues[0], 1, 3);
    Geom2dAdaptor_Curve   aWide(new Geom2d_BSplineCurve(aWidePoles, aWideKnots, aWideMults, 2));
    Geom2dAdaptor_Curve   aNarrow(aWide.Curve(), 0.0, 0.25);
    const IntRes2d_Domain aDomain1 = anIntersection.ComputeDomain(aCurve1, Precision::Confusion());
    const IntRes2d_Domain aDomain2 = anIntersection.ComputeDomain(aWide, Precision::Confusion());
    // An explicit domain can extend beyond the adaptor's stored range.
    Geom2dInt_GInter anExplicitDomainIntersection(aCurve1,
                                                  aDomain1,
                                                  aNarrow,
                                                  aDomain2,
                                                  Precision::Confusion(),
                                                  Precision::Confusion());
    ASSERT_TRUE(anExplicitDomainIntersection.IsDone());
    EXPECT_EQ(anExplicitDomainIntersection.NbPoints(), aNbSpans);
  }
  NCollection_Array1<gp_Pnt2d> aPeriodicPoles(1, 3);
  aPeriodicPoles.ChangeAt(0)                             = gp_Pnt2d(0.0, 0.0);
  aPeriodicPoles.ChangeAt(1)                             = gp_Pnt2d(1.0, 1.0);
  aPeriodicPoles.ChangeAt(2)                             = gp_Pnt2d(2.0, 0.0);
  const double                     aPeriodicKnotValues[] = {0.0, 0.1, 0.9, 1.0};
  const NCollection_Array1<double> aPeriodicKnots(aPeriodicKnotValues[0], 1, 4);
  NCollection_Array1<int>          aPeriodicMults(1, 4);
  aPeriodicMults.Init(1);
  Geom2dAdaptor_Curve aPeriodicCurve(
    new Geom2d_BSplineCurve(aPeriodicPoles, aPeriodicKnots, aPeriodicMults, 1, true));
  Geom2dInt_GInter aPeriodicIntersection(aPeriodicCurve,
                                         aCurve2,
                                         Precision::Confusion(),
                                         Precision::Confusion());
  ASSERT_TRUE(aPeriodicIntersection.IsDone());
  EXPECT_EQ(aPeriodicIntersection.NbPoints(), 2);
  EXPECT_EQ(aPeriodicIntersection.NbSegments(), 0);
}

//==================================================================================================

TEST(Geom2dInt_GInterTest, CompositeSplineEndpointTolerance)
{
  const double                 aTolerance = Precision::Confusion();
  NCollection_Array1<gp_Pnt2d> aPoles1(1, 3), aPoles2(1, 3);
  aPoles1.ChangeAt(0)                            = gp_Pnt2d(0.0, 0.0);
  aPoles1.ChangeAt(1)                            = gp_Pnt2d(1.0, 0.0);
  aPoles1.ChangeAt(2)                            = gp_Pnt2d(2.0, 0.0);
  aPoles2.ChangeAt(0)                            = gp_Pnt2d(2.0 + 0.5 * aTolerance, 0.0);
  aPoles2.ChangeAt(1)                            = gp_Pnt2d(3.0, 1.0);
  aPoles2.ChangeAt(2)                            = gp_Pnt2d(4.0, 1.0);
  const double                     aKnotValues[] = {0.0, 1.0, 2.0};
  const int                        aMultValues[] = {2, 1, 2};
  const NCollection_Array1<double> aKnots(aKnotValues[0], 1, 3);
  const NCollection_Array1<int>    aMults(aMultValues[0], 1, 3);
  Geom2dAdaptor_Curve              aCurve1(new Geom2d_BSplineCurve(aPoles1, aKnots, aMults, 1));
  Geom2dAdaptor_Curve              aCurve2(new Geom2d_BSplineCurve(aPoles2, aKnots, aMults, 1));
  Geom2dInt_GInter                 anIntersection(aCurve1, aCurve2, aTolerance, aTolerance);
  ASSERT_TRUE(anIntersection.IsDone());
  EXPECT_EQ(anIntersection.NbPoints(), 1);
  EXPECT_EQ(anIntersection.NbSegments(), 0);
}
