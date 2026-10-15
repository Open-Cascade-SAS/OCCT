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

#include <Geom2dAPI_InterCurveCurve.hxx>
#include <Geom2d_BSplineCurve.hxx>
#include <Geom2d_BezierCurve.hxx>
#include <Geom2d_Ellipse.hxx>
#include <Geom2d_Line.hxx>
#include <Geom2d_TrimmedCurve.hxx>
#include <NCollection_Array1.hxx>
#include <gp_Elips2d.hxx>
#include <gp_Ax2d.hxx>
#include <gp_Pnt2d.hxx>
#include <gp_Dir2d.hxx>
#include <math_NewtonFunctionRoot.hxx>
#include <math_TrigonometricEquationFunction.hxx>
#include <Standard_NullObject.hxx>
#include <Standard_OutOfRange.hxx>
#include <Precision.hxx>
#include <Standard_Real.hxx>
#include <IntRes2d_IntersectionPoint.hxx>
#include <IntRes2d_Position.hxx>
#include <IntRes2d_Transition.hxx>
#include <IntRes2d_IntersectionSegment.hxx>

#include <algorithm>
#include <gtest/gtest.h>

TEST(Geom2dAPI_InterCurveCurve_Test, TangentRationalEndpointHasOneConnectedZone)
{
  NCollection_Array1<double> aKnots(1, 2);
  aKnots(1) = 0.0;
  aKnots(2) = 1.0;
  NCollection_Array1<int> aMults(1, 2);
  aMults.Init(4);
  NCollection_Array1<gp_Pnt2d> aPoles1(1, 4), aPoles2(1, 4);
  aPoles1(1) = gp_Pnt2d(-29.0, 0.0);
  aPoles1(2) = gp_Pnt2d(-29.0, 36.62477607);
  aPoles1(3) = gp_Pnt2d(6.91354767, 43.8074856);
  aPoles1(4) = gp_Pnt2d(21.0, 10.0);
  aPoles2(1) = gp_Pnt2d(27.22131518, 10.0);
  aPoles2(2) = gp_Pnt2d(12.63802245, 49.69764077);
  aPoles2(3) = gp_Pnt2d(-29.0, 42.29154892);
  aPoles2(4) = gp_Pnt2d(-29.0, 0.0);
  NCollection_Array1<double> aWeights1(1, 4), aWeights2(1, 4);
  aWeights1(1) = aWeights1(4) = aWeights2(1) = aWeights2(4) = 1.0;
  aWeights1(2) = aWeights1(3) = 0.46407742343;
  aWeights2(2) = aWeights2(3) = 0.45007993394;
  const occ::handle<Geom2d_BSplineCurve> aCurve1 =
    new Geom2d_BSplineCurve(aPoles1, aWeights1, aKnots, aMults, 3);
  const occ::handle<Geom2d_BSplineCurve> aCurve2 =
    new Geom2d_BSplineCurve(aPoles2, aWeights2, aKnots, aMults, 3);
  for (const bool isReversed : {false, true})
  {
    if (isReversed)
    {
      aCurve1->Reverse();
      aCurve2->Reverse();
    }
    for (const bool isSwapped : {false, true})
    {
      SCOPED_TRACE(isReversed);
      SCOPED_TRACE(isSwapped);
      constexpr double aTolerance = 1.0e-10;
      const Geom2dAPI_InterCurveCurve anIntersection(isSwapped ? aCurve2 : aCurve1,
                                                     isSwapped ? aCurve1 : aCurve2,
                                                     aTolerance);
      ASSERT_TRUE(anIntersection.Intersector().IsDone());
      EXPECT_GT(anIntersection.NbPoints() + anIntersection.NbSegments(), 0);
      EXPECT_LE(anIntersection.NbSegments(), 1);
      const gp_Pnt2d aJoint(-29.0, 0.0);
      for (int anIndex = 1; anIndex <= anIntersection.NbPoints(); ++anIndex)
      {
        const IntRes2d_IntersectionPoint& aPoint = anIntersection.Intersector().Point(anIndex);
        const gp_Pnt2d aPoint1 = (isSwapped ? aCurve2 : aCurve1)->EvalD0(aPoint.ParamOnFirst());
        const gp_Pnt2d aPoint2 = (isSwapped ? aCurve1 : aCurve2)->EvalD0(aPoint.ParamOnSecond());
        EXPECT_LE(aPoint1.Distance(aPoint2), aTolerance);
      }
      for (int anIndex = 1; anIndex <= anIntersection.NbSegments(); ++anIndex)
      {
        const IntRes2d_IntersectionSegment& aSegment =
          anIntersection.Intersector().Segment(anIndex);
        ASSERT_TRUE(aSegment.HasFirstPoint());
        ASSERT_TRUE(aSegment.HasLastPoint());
        EXPECT_LE(std::min(aSegment.FirstPoint().Value().Distance(aJoint),
                           aSegment.LastPoint().Value().Distance(aJoint)),
                  Precision::Confusion());
      }
    }
  }
}

TEST(Geom2dAPI_InterCurveCurve_Test, StationaryEndpointUsesSecondDerivative)
{
  for (const bool isReversed : {false, true})
  {
    SCOPED_TRACE(isReversed);
    NCollection_Array1<gp_Pnt2d> aPoles(1, 3);
    aPoles.ChangeAt(0)                           = gp_Pnt2d(0.0, 0.0);
    aPoles.ChangeAt(1)                           = gp_Pnt2d(-RealEpsilon(), RealEpsilon());
    aPoles.ChangeAt(2)                           = gp_Pnt2d(1.0, 0.0);
    const occ::handle<Geom2d_BezierCurve> aCurve = new Geom2d_BezierCurve(aPoles);
    if (isReversed)
    {
      aCurve->Reverse();
    }
    NCollection_Array1<gp_Pnt2d> aLinePoles(1, 2);
    aLinePoles.ChangeAt(0)                      = gp_Pnt2d(-1.0, -1.0);
    aLinePoles.ChangeAt(1)                      = gp_Pnt2d(1.0, 1.0);
    const occ::handle<Geom2d_BezierCurve> aLine = new Geom2d_BezierCurve(aLinePoles);
    const Geom2dAPI_InterCurveCurve       anIntersection(aCurve, aLine, Precision::Confusion());
    ASSERT_EQ(anIntersection.NbPoints(), 1);
    const auto& aPoint = anIntersection.Intersector().Point(1);
    EXPECT_LE(aPoint.Value().Distance(gp_Pnt2d()), Precision::Confusion());
    EXPECT_EQ(aPoint.TransitionOfFirst().PositionOnCurve(),
              isReversed ? IntRes2d_End : IntRes2d_Head);
    EXPECT_EQ(aPoint.TransitionOfFirst().TransitionType(), IntRes2d_Out);
    EXPECT_EQ(aPoint.TransitionOfSecond().TransitionType(), IntRes2d_In);
  }
}

namespace
{
occ::handle<Geom2d_BSplineCurve> createOCC23706Curve1()
{
  NCollection_Array1<gp_Pnt2d> aPoles(1, 6);
  aPoles(1) = gp_Pnt2d(2.0, 0.0);
  aPoles(2) = gp_Pnt2d(3.0, -1.0);
  aPoles(3) = gp_Pnt2d(5.0, 5.0);
  aPoles(4) = gp_Pnt2d(5.0, 5.0);
  aPoles(5) = gp_Pnt2d(6.0, 8.0);
  aPoles(6) = gp_Pnt2d(4.0, 7.0);

  NCollection_Array1<double> aKnots(1, 5);
  aKnots(1) = 0.0;
  aKnots(2) = 0.2;
  aKnots(3) = 0.3;
  aKnots(4) = 0.4;
  aKnots(5) = 0.5;

  NCollection_Array1<int> aMultiplicities(1, 5);
  aMultiplicities(1) = 3;
  aMultiplicities(2) = 1;
  aMultiplicities(3) = 1;
  aMultiplicities(4) = 1;
  aMultiplicities(5) = 3;

  NCollection_Array1<double> aWeights(1, 6);
  for (int anIndex = aWeights.Lower(); anIndex <= aWeights.Upper(); ++anIndex)
  {
    aWeights(anIndex) = 1.0;
  }
  return new Geom2d_BSplineCurve(aPoles, aWeights, aKnots, aMultiplicities, 2);
}

occ::handle<Geom2d_BSplineCurve> createOCC23706Curve2(const bool thePerturbed)
{
  NCollection_Array1<gp_Pnt2d> aPoles(1, 5);
  aPoles(1) = gp_Pnt2d(6.0, 3.0);
  aPoles(2) = gp_Pnt2d(thePerturbed ? 5.001 : 5.0, thePerturbed ? 5.01 : 5.0);
  aPoles(3) = aPoles(2);
  aPoles(4) = gp_Pnt2d(3.0, 9.0);
  aPoles(5) = gp_Pnt2d(2.0, 11.0);

  NCollection_Array1<double> aKnots(1, 4);
  aKnots(1) = 1.0;
  aKnots(2) = 2.0;
  aKnots(3) = 3.0;
  aKnots(4) = 5.0;

  NCollection_Array1<int> aMultiplicities(1, 4);
  aMultiplicities(1) = 3;
  aMultiplicities(2) = 1;
  aMultiplicities(3) = 1;
  aMultiplicities(4) = 3;

  NCollection_Array1<double> aWeights(1, 5);
  for (int anIndex = aWeights.Lower(); anIndex <= aWeights.Upper(); ++anIndex)
  {
    aWeights(anIndex) = 1.0;
  }
  return new Geom2d_BSplineCurve(aPoles, aWeights, aKnots, aMultiplicities, 2);
}
} // namespace

TEST(Geom2dAPI_InterCurveCurve_Test, OCC29289_EllipseIntersectionNewtonRoot)
{
  // Create two ellipses
  gp_Elips2d                  e1(gp_Ax2d(gp_Pnt2d(0., 0.), gp_Dir2d(gp_Dir2d::D::X)), 2., 1.);
  occ::handle<Geom2d_Ellipse> Ge1 = new Geom2d_Ellipse(e1);
  gp_Elips2d                  e2(gp_Ax2d(gp_Pnt2d(0.5, 0.5), gp_Dir2d(1., 1.)), 2., 1.);
  occ::handle<Geom2d_Ellipse> Ge2 = new Geom2d_Ellipse(e2);

  // Find intersection points
  Geom2dAPI_InterCurveCurve Intersector;
  Intersector.Init(Ge1, Ge2, 1.e-7);
  EXPECT_GT(Intersector.NbPoints(), 0) << "Error: intersector found no points";

  // Setup trigonometric equation: A*std::cos(x) + B*Sin(x) + C*std::cos(2*x) + D*Sin(2*x) + E
  double A, B, C, D, E;
  A = 1.875;
  B = -.75;
  C = -.5;
  D = -.25;
  E = -.25;
  math_TrigonometricEquationFunction MyF(A, B, C, D, E);

  double Tol1  = 1.e-15;
  double Eps   = 1.5e-12;
  int    Nit[] = {5, 6, 7, 6};

  // For each intersection point, verify Newton root finding
  double TetaPrev = 0.;
  int    i;
  for (i = 1; i <= Intersector.NbPoints(); i++)
  {
    double Teta = Intersector.Intersector().Point(i).ParamOnFirst();
    double X    = Teta - 0.1 * (Teta - TetaPrev);
    TetaPrev    = Teta;

    math_NewtonFunctionRoot Resol(MyF, X, Tol1, Eps, Nit[i - 1]);
    ASSERT_TRUE(Resol.IsDone()) << "Error: Newton is not done for " << Teta;

    double TetaNewton = Resol.Root();
    EXPECT_LE(std::abs(Teta - TetaNewton), 1.e-7) << "Error: Newton root is wrong for " << Teta;
  }
}

TEST(Geom2dAPI_InterCurveCurve_Test, PointRejectsZeroIndex)
{
  gp_Elips2d                  anEllipse1(gp_Ax2d(gp_Pnt2d(0.0, 0.0), gp_Dir2d(1.0, 0.0)), 2.0, 1.0);
  occ::handle<Geom2d_Ellipse> aCurve1 = new Geom2d_Ellipse(anEllipse1);
  gp_Elips2d                  anEllipse2(gp_Ax2d(gp_Pnt2d(0.5, 0.5), gp_Dir2d(1.0, 1.0)), 2.0, 1.0);
  occ::handle<Geom2d_Ellipse> aCurve2 = new Geom2d_Ellipse(anEllipse2);

  Geom2dAPI_InterCurveCurve anIntersector(aCurve1, aCurve2, 1.0e-7);
  ASSERT_GT(anIntersector.NbPoints(), 0);

#ifndef No_Exception
  EXPECT_THROW((void)anIntersector.Point(0), Standard_OutOfRange);
#endif
}

// Regression test for bug #11: Init with null handle must raise Standard_NullObject.
TEST(Geom2dAPI_InterCurveCurve_Test, Init_NullHandle_RaisesException)
{
#ifndef No_Exception
  occ::handle<Geom2d_Curve> aNullCurve;
  gp_Elips2d                anEllipse(gp_Ax2d(gp_Pnt2d(0., 0.), gp_Dir2d(1., 0.)), 2., 1.);
  occ::handle<Geom2d_Curve> aValidCurve = new Geom2d_Ellipse(anEllipse);

  // Both null.
  EXPECT_THROW(Geom2dAPI_InterCurveCurve(aNullCurve, aNullCurve, 1.0e-7), Standard_NullObject);

  // First null.
  EXPECT_THROW(Geom2dAPI_InterCurveCurve(aNullCurve, aValidCurve, 1.0e-7), Standard_NullObject);

  // Second null.
  EXPECT_THROW(Geom2dAPI_InterCurveCurve(aValidCurve, aNullCurve, 1.0e-7), Standard_NullObject);

  // Single-curve Init with null.
  Geom2dAPI_InterCurveCurve anInter;
  EXPECT_THROW(anInter.Init(aNullCurve, 1.0e-7), Standard_NullObject);
#endif
}

// Migrated from tests/lowalgos/2dinter/bug23706_4.  Preserve the rounded
// four-decimal coordinate checks used by the DRAW regression.
TEST(Geom2dAPI_InterCurveCurve_Test, OCC23706_4_BSplineIntersectionCoordinates)
{
  const occ::handle<Geom2d_BSplineCurve> aCurve1 = createOCC23706Curve1();
  const occ::handle<Geom2d_BSplineCurve> aCurve2 = createOCC23706Curve2(true);
  Geom2dAPI_InterCurveCurve              anIntersector(aCurve1, aCurve2, 0.001);

  ASSERT_EQ(anIntersector.NbPoints(), 2);
  const gp_Pnt2d aPoint1 = anIntersector.Point(1);
  const gp_Pnt2d aPoint2 = anIntersector.Point(2);
  EXPECT_NEAR(aPoint1.X(), 5.0024, 1.e-4);
  EXPECT_NEAR(aPoint1.Y(), 5.0072, 1.e-4);
  EXPECT_NEAR(aPoint2.X(), 4.0024, 1.e-4);
  EXPECT_NEAR(aPoint2.Y(), 7.0012, 1.e-4);
}

// Migrated from tests/lowalgos/2dinter/bug23706_5.  The original script used
// integer truncation for the second point, so retain that intent with ranges.
TEST(Geom2dAPI_InterCurveCurve_Test, OCC23706_5_BSplineIntersectionCoordinates)
{
  const occ::handle<Geom2d_BSplineCurve> aCurve1 = createOCC23706Curve1();
  const occ::handle<Geom2d_BSplineCurve> aCurve2 = createOCC23706Curve2(false);
  Geom2dAPI_InterCurveCurve              anIntersector(aCurve1, aCurve2, 0.001);

  ASSERT_EQ(anIntersector.NbPoints(), 2);
  const gp_Pnt2d aPoint1 = anIntersector.Point(1);
  const gp_Pnt2d aPoint2 = anIntersector.Point(2);
  EXPECT_NEAR(aPoint1.X(), 5.0, 1.e-7);
  EXPECT_NEAR(aPoint1.Y(), 5.0, 1.e-7);
  EXPECT_GE(aPoint2.X(), 4.0);
  EXPECT_LT(aPoint2.X(), 5.0);
  EXPECT_GE(aPoint2.Y(), 7.0);
  EXPECT_LT(aPoint2.Y(), 8.0);
}

// Migrated from tests/lowalgos/2dinter/bug29162.  The DRAW command uses its
// default 1.e-3 intersection tolerance.
TEST(Geom2dAPI_InterCurveCurve_Test, OCC29162_TrimmedEllipseLineIntersection)
{
  const occ::handle<Geom2d_Ellipse> anEllipse =
    new Geom2d_Ellipse(gp_Ax22d(gp_Pnt2d(-610.348096534595, -710.720096056787),
                                gp_Dir2d(0.999999902285153, 0.000442074298181498)),
                       15.0066332711999,
                       0.291884102212871);
  const occ::handle<Geom2d_TrimmedCurve> aTrimmedEllipse =
    new Geom2d_TrimmedCurve(anEllipse, 3.09462291909258, 9.37780822627216);

  const occ::handle<Geom2d_Line> aLine =
    new Geom2d_Line(gp_Pnt2d(-625.34430362036, -680.713463264921),
                    gp_Dir2d(-0.000252749178714602, -0.999999968058926));
  const occ::handle<Geom2d_TrimmedCurve> aTrimmedLine =
    new Geom2d_TrimmedCurve(aLine, 0.0, 30.0132665523925);

  Geom2dAPI_InterCurveCurve anIntersector(aTrimmedEllipse, aTrimmedLine, 0.001);
  ASSERT_EQ(anIntersector.NbPoints(), 1);
  const gp_Pnt2d aPoint = anIntersector.Point(1);
  EXPECT_NEAR(aPoint.X(), -625.35188801291508, 1.e-7);
  EXPECT_NEAR(aPoint.Y(), -710.72104765746838, 1.e-7);
}
