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

#include <GCPnts_QuasiUniformDeflection.hxx>
#include <GeomAdaptor_Curve.hxx>
#include <Geom_BSplineCurve.hxx>
#include <NCollection_Array1.hxx>
#include <gp_Pnt.hxx>

namespace
{
occ::handle<Geom_BSplineCurve> makeOCC33828Curve()
{
  NCollection_Array1<double> aKnots(1, 7);
  aKnots(1) = 0.0;
  aKnots(2) = 0.17;
  aKnots(3) = 0.33;
  aKnots(4) = 0.5;
  aKnots(5) = 0.67;
  aKnots(6) = 0.83;
  aKnots(7) = 1.0;

  NCollection_Array1<int> aMultiplicities(1, 7);
  aMultiplicities(1) = 4;
  aMultiplicities(2) = 2;
  aMultiplicities(3) = 2;
  aMultiplicities(4) = 2;
  aMultiplicities(5) = 2;
  aMultiplicities(6) = 2;
  aMultiplicities(7) = 4;

  const double aPoleCoordinates[14][3] = {{0.163, 0.233, 0.0},
                                          {0.158, 0.204, 0.0},
                                          {0.139, 0.180, 0.0},
                                          {0.086, 0.159, 0.0},
                                          {0.055, 0.163, 0.0},
                                          {0.009, 0.196, 0.0},
                                          {-0.004, 0.225, 0.0},
                                          {0.002, 0.281, 0.0},
                                          {0.019, 0.307, 0.0},
                                          {0.070, 0.332, 0.0},
                                          {0.101, 0.331, 0.0},
                                          {0.149, 0.301, 0.0},
                                          {0.164, 0.274, 0.0},
                                          {0.163, 0.246, 0.0}};

  NCollection_Array1<gp_Pnt> aPoles(1, 14);
  for (int anIndex = 0; anIndex < 14; ++anIndex)
  {
    aPoles(anIndex + 1) = gp_Pnt(aPoleCoordinates[anIndex][0],
                                 aPoleCoordinates[anIndex][1],
                                 aPoleCoordinates[anIndex][2]);
  }

  return new Geom_BSplineCurve(aPoles, aKnots, aMultiplicities, 3);
}

void checkOCC33828Deflection(const GeomAdaptor_Curve& theCurve,
                             const double             theDeflection,
                             const int                theExpectedNbPoints)
{
  const GCPnts_QuasiUniformDeflection anAlgo(theCurve, theDeflection);
  ASSERT_TRUE(anAlgo.IsDone());
  EXPECT_EQ(anAlgo.NbPoints(), theExpectedNbPoints);
  EXPECT_LE(anAlgo.Deflection(), theDeflection);
}
} // namespace

// Migrated from tests/bugs/moddata_3/bug33828. Small changes in requested
// deflection must produce the stable point counts recorded by the DRAW test.
TEST(GCPnts_QuasiUniformDeflectionTest, ModDataBug_33828_StablePointCounts)
{
  const occ::handle<Geom_BSplineCurve> aCurve = makeOCC33828Curve();
  const GeomAdaptor_Curve              anAdaptor(aCurve);

  checkOCC33828Deflection(anAdaptor, 0.5, 2);
  checkOCC33828Deflection(anAdaptor, 0.1, 3);
  checkOCC33828Deflection(anAdaptor, 0.05, 5);
  checkOCC33828Deflection(anAdaptor, 0.025, 5);
  checkOCC33828Deflection(anAdaptor, 0.007, 9);
  checkOCC33828Deflection(anAdaptor, 0.005, 17);
  checkOCC33828Deflection(anAdaptor, 0.0005, 33);
  checkOCC33828Deflection(anAdaptor, 0.0003, 65);
  checkOCC33828Deflection(anAdaptor, 0.0002, 65);
  checkOCC33828Deflection(anAdaptor, 0.0001, 73);
}
