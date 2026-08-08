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

#include <GccAna_Circ2d3Tan.hxx>
#include <GccEnt_QualifiedCirc.hxx>
#include <GccEnt_QualifiedLin.hxx>
#include <GCPnts_AbscissaPoint.hxx>
#include <Geom2dAdaptor_Curve.hxx>
#include <Geom2d_Circle.hxx>
#include <gp_Ax2d.hxx>
#include <gp_Circ2d.hxx>
#include <gp_Dir2d.hxx>
#include <gp_Lin2d.hxx>
#include <gp_Pnt2d.hxx>
#include <NCollection_Array1.hxx>
#include <Precision.hxx>

#include <gtest/gtest.h>

namespace
{
gp_Circ2d makeCircle(const double theX, const double theY, const double theRadius)
{
  return gp_Circ2d(gp_Ax2d(gp_Pnt2d(theX, theY), gp_Dir2d(1.0, 0.0)), theRadius);
}

GccEnt_QualifiedCirc makeQualifiedCircle(const double theX,
                                         const double theY,
                                         const double theRadius)
{
  return GccEnt_QualifiedCirc(makeCircle(theX, theY, theRadius), GccEnt_unqualified);
}

GccEnt_QualifiedLin makeQualifiedLine(const double theX,
                                      const double theY,
                                      const double theDX,
                                      const double theDY)
{
  return GccEnt_QualifiedLin(gp_Lin2d(gp_Pnt2d(theX, theY), gp_Dir2d(theDX, theDY)),
                             GccEnt_unqualified);
}

double circleLength(const gp_Circ2d& theCircle)
{
  const occ::handle<Geom2d_Circle> aCurve = new Geom2d_Circle(theCircle);
  Geom2dAdaptor_Curve              anAdaptor(aCurve);
  return GCPnts_AbscissaPoint::Length(anAdaptor);
}

NCollection_Array1<double> makeExpectedLengths(const double* theValues, const int theLength)
{
  NCollection_Array1<double> anExpected(1, theLength);
  for (int anIndex = 1; anIndex <= theLength; ++anIndex)
  {
    anExpected.SetValue(anIndex, theValues[anIndex - 1]);
  }
  return anExpected;
}

void verifySolutionLengths(GccAna_Circ2d3Tan&              theSolver,
                           const NCollection_Array1<double>& theExpected)
{
  ASSERT_TRUE(theSolver.IsDone());
  ASSERT_GE(theSolver.NbSolutions(), theExpected.Length());

  for (int anIndex = 1; anIndex <= theExpected.Length(); ++anIndex)
  {
    const double anExpectedLength = theExpected.Value(anIndex);
    const double aTolerance       = 1.e-7 * (anExpectedLength > 1.0 ? anExpectedLength : 1.0);
    const gp_Circ2d aSolution     = theSolver.ThisSolution(anIndex);
    EXPECT_GT(aSolution.Radius(), 0.0);
    EXPECT_NEAR(circleLength(aSolution), anExpectedLength, aTolerance)
      << "Unexpected circumference for solution " << anIndex;
  }
}

void verifySolverDoesNotFail(GccAna_Circ2d3Tan& theSolver)
{
  ASSERT_TRUE(theSolver.IsDone());
  for (int anIndex = 1; anIndex <= theSolver.NbSolutions(); ++anIndex)
  {
    EXPECT_GT(theSolver.ThisSolution(anIndex).Radius(), 0.0)
      << "Invalid zero-radius solution " << anIndex;
  }
}
} // namespace

// Migrated from tests/geometry/circ2d3Tan/CircleCircleCircle_11.
TEST(GccAna_Circ2d3TanTest, CircleCircleCircle_11)
{
  const GccEnt_QualifiedCirc aCircle1 = makeQualifiedCircle(0.0, 0.0, 50.0);
  const GccEnt_QualifiedCirc aCircle2 = makeQualifiedCircle(20.0, 0.0, 10.0);
  const GccEnt_QualifiedCirc aCircle3 = makeQualifiedCircle(0.0, 20.0, 10.0);
  GccAna_Circ2d3Tan aSolver(aCircle1, aCircle2, aCircle3, Precision::Confusion());

  const double anExpectedValues[] = {168.36566348025758,
                                     244.52937099154383,
                                     131.42863607625242,
                                     182.73062928272694,
                                     182.7306292827268,
                                     131.42863607625236,
                                     94.936311385359318,
                                     178.56704904481091};
  const NCollection_Array1<double> anExpected = makeExpectedLengths(anExpectedValues, 8);
  verifySolutionLengths(aSolver, anExpected);
}

// Migrated from tests/geometry/circ2d3Tan/CircleCirclePoint_11.
TEST(GccAna_Circ2d3TanTest, CircleCirclePoint_11)
{
  const GccEnt_QualifiedCirc aCircle1 = makeQualifiedCircle(0.0, 0.0, 10.0);
  const GccEnt_QualifiedCirc aCircle2 = makeQualifiedCircle(30.0, 0.0, 20.0);
  GccAna_Circ2d3Tan aSolver(aCircle1,
                            aCircle2,
                            gp_Pnt2d(10.0, 10.0),
                            Precision::Confusion());

  const double anExpectedValues[] = {13.802267767659149, 80.445511840034683};
  const NCollection_Array1<double> anExpected = makeExpectedLengths(anExpectedValues, 2);
  verifySolutionLengths(aSolver, anExpected);
}

// Migrated from tests/geometry/circ2d3Tan/CircleCirclePoint_12.
TEST(GccAna_Circ2d3TanTest, CircleCirclePoint_12)
{
  const GccEnt_QualifiedCirc aCircle1 = makeQualifiedCircle(0.0, 0.0, 10.0);
  const GccEnt_QualifiedCirc aCircle2 = makeQualifiedCircle(30.0, 0.0, 20.0);
  GccAna_Circ2d3Tan aSolver(aCircle1,
                            aCircle2,
                            gp_Pnt2d(7.0, 30.0),
                            Precision::Confusion());

  const double anExpectedValues[] = {951.90257403770727, 66.177052461460107, 190.20396243259333};
  const NCollection_Array1<double> anExpected = makeExpectedLengths(anExpectedValues, 3);
  verifySolutionLengths(aSolver, anExpected);
}

// Migrated from tests/geometry/circ2d3Tan/CircleLinLin_11.
TEST(GccAna_Circ2d3TanTest, CircleLinLin_11)
{
  const GccEnt_QualifiedCirc aCircle = makeQualifiedCircle(0.0, 120.0, 20.0);
  const GccEnt_QualifiedLin aLine1 = makeQualifiedLine(0.0, 0.0, 10.0, 20.0);
  const GccEnt_QualifiedLin aLine2 = makeQualifiedLine(0.0, 0.0, 10.0, -40.0);
  GccAna_Circ2d3Tan aSolver(aCircle, aLine1, aLine2, Precision::Confusion());

  const double anExpectedValues[] = {461.86006847878718,
                                     163.75801021417183,
                                     321.80336707682847,
                                     235.02950419226329};
  const NCollection_Array1<double> anExpected = makeExpectedLengths(anExpectedValues, 4);
  verifySolutionLengths(aSolver, anExpected);
}

// Migrated from tests/geometry/circ2d3Tan/CirclePointPoint_11.
TEST(GccAna_Circ2d3TanTest, CirclePointPoint_11)
{
  const GccEnt_QualifiedCirc aCircle = makeQualifiedCircle(0.0, 0.0, 10.0);
  GccAna_Circ2d3Tan aSolver(aCircle,
                            gp_Pnt2d(20.0, 0.0),
                            gp_Pnt2d(15.0, 5.0),
                            Precision::Confusion());

  const double anExpectedValues[] = {157.07963267948966, 31.415926535897931};
  const NCollection_Array1<double> anExpected = makeExpectedLengths(anExpectedValues, 2);
  verifySolutionLengths(aSolver, anExpected);
}

// Migrated from tests/geometry/circ2d3Tan/CirclePointPoint_21.
TEST(GccAna_Circ2d3TanTest, CirclePointPoint_21)
{
  const GccEnt_QualifiedCirc aCircle = makeQualifiedCircle(0.0, 0.0, 10.0);
  GccAna_Circ2d3Tan aSolver(aCircle,
                            gp_Pnt2d(5.0, 0.0),
                            gp_Pnt2d(0.0, 5.0),
                            Precision::Confusion());

  const double anExpectedValues[] = {45.424379479936995, 26.383452602115419};
  const NCollection_Array1<double> anExpected = makeExpectedLengths(anExpectedValues, 2);
  verifySolutionLengths(aSolver, anExpected);
}

// Migrated from tests/geometry/circ2d3Tan/LinLinLin_11.
TEST(GccAna_Circ2d3TanTest, LinLinLin_11)
{
  const GccEnt_QualifiedLin aLine1 = makeQualifiedLine(0.0, 0.0, 10.0, 20.0);
  const GccEnt_QualifiedLin aLine2 = makeQualifiedLine(0.0, 0.0, 10.0, -40.0);
  const GccEnt_QualifiedLin aLine3 = makeQualifiedLine(160.0, 0.0, -40.0, 10.0);
  GccAna_Circ2d3Tan aSolver(aLine1, aLine2, aLine3, Precision::Confusion());

  const double anExpectedValues[] = {213.09795279419643,
                                     284.90187851033369,
                                     131.38343888467227,
                                     63.235238531994284};
  const NCollection_Array1<double> anExpected = makeExpectedLengths(anExpectedValues, 4);
  verifySolutionLengths(aSolver, anExpected);
}

// Migrated from tests/geometry/circ2d3Tan/LinLinPoint_11.
TEST(GccAna_Circ2d3TanTest, LinLinPoint_11)
{
  const GccEnt_QualifiedLin aLine1 = makeQualifiedLine(0.0, 0.0, 10.0, 20.0);
  const GccEnt_QualifiedLin aLine2 = makeQualifiedLine(0.0, 0.0, 10.0, -40.0);
  GccAna_Circ2d3Tan aSolver(aLine1,
                            aLine2,
                            gp_Pnt2d(10.0, 80.0),
                            Precision::Confusion());

  const double anExpectedValues[] = {269.03484941268533, 130.52381207643296};
  const NCollection_Array1<double> anExpected = makeExpectedLengths(anExpectedValues, 2);
  verifySolutionLengths(aSolver, anExpected);
}

// Migrated from tests/geometry/circ2d3Tan/LinPointPoint_11.
TEST(GccAna_Circ2d3TanTest, LinPointPoint_11)
{
  const GccEnt_QualifiedLin aLine = makeQualifiedLine(0.0, 0.0, 10.0, 20.0);
  GccAna_Circ2d3Tan aSolver(aLine,
                            gp_Pnt2d(10.0, 10.0),
                            gp_Pnt2d(100.0, 10.0),
                            Precision::Confusion());

  const double anExpectedValues[] = {419.71016104587477, 282.77131205819785};
  const NCollection_Array1<double> anExpected = makeExpectedLengths(anExpectedValues, 2);
  verifySolutionLengths(aSolver, anExpected);
}

// Migrated from tests/geometry/circ2d3Tan/CircleCirclePoint_14.
TEST(GccAna_Circ2d3TanTest, CircleCirclePoint_14)
{
  const GccEnt_QualifiedCirc aCircle1 = makeQualifiedCircle(0.0, 0.0, 10.0);
  const GccEnt_QualifiedCirc aCircle2 = makeQualifiedCircle(30.0, 0.0, 20.0);
  GccAna_Circ2d3Tan aSolver(aCircle1,
                            aCircle2,
                            gp_Pnt2d(30.0, -20.0),
                            Precision::Confusion());

  const double anExpectedValues[] = {125.6637061435916,
                                     376.99111854338514,
                                     376.99111854338514};
  const NCollection_Array1<double> anExpected = makeExpectedLengths(anExpectedValues, 3);
  verifySolutionLengths(aSolver, anExpected);
}

// Migrated from tests/geometry/circ2d3Tan/CirclePointPoint_12.
TEST(GccAna_Circ2d3TanTest, CirclePointPoint_12)
{
  const GccEnt_QualifiedCirc aCircle = makeQualifiedCircle(0.0, 0.0, 10.0);
  GccAna_Circ2d3Tan aSolver(aCircle,
                            gp_Pnt2d(20.0, 0.0),
                            gp_Pnt2d(10.0, 0.0),
                            Precision::Confusion());
  verifySolverDoesNotFail(aSolver);
}

// Migrated from tests/geometry/circ2d3Tan/CirclePointPoint_13.
TEST(GccAna_Circ2d3TanTest, CirclePointPoint_13)
{
  const GccEnt_QualifiedCirc aCircle = makeQualifiedCircle(0.0, 0.0, 10.0);
  GccAna_Circ2d3Tan aSolver(aCircle,
                            gp_Pnt2d(20.0, 0.0),
                            gp_Pnt2d(0.0, 10.0),
                            Precision::Confusion());

  const double anExpectedValues[] = {157.07963267948978, 157.07963267948978};
  const NCollection_Array1<double> anExpected = makeExpectedLengths(anExpectedValues, 2);
  verifySolutionLengths(aSolver, anExpected);
}

// Migrated from tests/geometry/circ2d3Tan/CirclePointPoint_14.
TEST(GccAna_Circ2d3TanTest, CirclePointPoint_14)
{
  const GccEnt_QualifiedCirc aCircle = makeQualifiedCircle(0.0, 0.0, 10.0);
  GccAna_Circ2d3Tan aSolver(aCircle,
                            gp_Pnt2d(20.0, 0.0),
                            gp_Pnt2d(5.0, 5.0),
                            Precision::Confusion());
  verifySolverDoesNotFail(aSolver);
}

// Migrated from tests/geometry/circ2d3Tan/CirclePointPoint_15.
TEST(GccAna_Circ2d3TanTest, CirclePointPoint_15)
{
  const GccEnt_QualifiedCirc aCircle = makeQualifiedCircle(0.0, 0.0, 10.0);
  GccAna_Circ2d3Tan aSolver(aCircle,
                            gp_Pnt2d(20.0, 0.0),
                            gp_Pnt2d(-20.0, 0.0),
                            Precision::Confusion());

  const double anExpectedValues[] = {157.07963267948966, 157.07963267948966};
  const NCollection_Array1<double> anExpected = makeExpectedLengths(anExpectedValues, 2);
  verifySolutionLengths(aSolver, anExpected);
}

// Migrated from tests/geometry/circ2d3Tan/CirclePointPoint_22.
TEST(GccAna_Circ2d3TanTest, CirclePointPoint_22)
{
  const GccEnt_QualifiedCirc aCircle = makeQualifiedCircle(0.0, 0.0, 10.0);
  GccAna_Circ2d3Tan aSolver(aCircle,
                            gp_Pnt2d(5.0, 0.0),
                            gp_Pnt2d(0.0, 0.0),
                            Precision::Confusion());

  const double anExpectedValues[] = {31.415926535897931, 31.415926535897931};
  const NCollection_Array1<double> anExpected = makeExpectedLengths(anExpectedValues, 2);
  verifySolutionLengths(aSolver, anExpected);
}

// Migrated from tests/geometry/circ2d3Tan/CirclePointPoint_24.
TEST(GccAna_Circ2d3TanTest, CirclePointPoint_24)
{
  const GccEnt_QualifiedCirc aCircle = makeQualifiedCircle(0.0, 0.0, 10.0);
  GccAna_Circ2d3Tan aSolver(aCircle,
                            gp_Pnt2d(5.0, 0.0),
                            gp_Pnt2d(0.0, 10.0),
                            Precision::Confusion());

  const double anExpectedValues[] = {39.269908169872416};
  const NCollection_Array1<double> anExpected = makeExpectedLengths(anExpectedValues, 1);
  verifySolutionLengths(aSolver, anExpected);
}

// Migrated from tests/geometry/circ2d3Tan/CirclePointPoint_31.
TEST(GccAna_Circ2d3TanTest, CirclePointPoint_31)
{
  const GccEnt_QualifiedCirc aCircle = makeQualifiedCircle(0.0, 0.0, 10.0);
  GccAna_Circ2d3Tan aSolver(aCircle,
                            gp_Pnt2d(10.0, 0.0),
                            gp_Pnt2d(0.0, 10.0),
                            Precision::Confusion());

  const double anExpectedValues[] = {62.831853071795862};
  const NCollection_Array1<double> anExpected = makeExpectedLengths(anExpectedValues, 1);
  verifySolutionLengths(aSolver, anExpected);
}

// Migrated from tests/geometry/circ2d3Tan/PointPointPoint_11.
TEST(GccAna_Circ2d3TanTest, PointPointPoint_11)
{
  GccAna_Circ2d3Tan aSolver(gp_Pnt2d(0.0, 50.0),
                            gp_Pnt2d(30.0, 20.0),
                            gp_Pnt2d(150.0, 150.0),
                            Precision::Confusion());

  const double anExpectedValues[] = {566.81157580298293};
  const NCollection_Array1<double> anExpected = makeExpectedLengths(anExpectedValues, 1);
  verifySolutionLengths(aSolver, anExpected);
}
