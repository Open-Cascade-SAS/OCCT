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

#include <Geom_ToroidalSurface.hxx>
#include <gp_Ax3.hxx>
#include <gp_Dir.hxx>
#include <gp_Pnt.hxx>
#include <gp_Torus.hxx>
#include <NCollection_Array1.hxx>
#include <Precision.hxx>

#include <cmath>

// Test OCC26746: gp_Torus::Coefficients() computes correct implicit equation coefficients.
// Verifies that evaluating the 35-coefficient polynomial at many points on the torus surface
// gives values close to zero within tolerance.
TEST(gp_TorusTest, OCC26746_CoefficientsCorrect)
{
  // Torus constructed from the original Draw script:
  // torus tr 55.52514413 2.070076585 73.83409062
  //          0.37231784651136368 0.58886674834874120 0.71736697293527607
  //          0.80682335496555135 0.17666016102759910 -0.56376170618524390
  //          87.08479625 23.14682176
  const gp_Ax3 anAx3(gp_Pnt(55.52514413, 2.070076585, 73.83409062),
                     gp_Dir(0.37231784651136368, 0.58886674834874120, 0.71736697293527607),
                     gp_Dir(0.80682335496555135, 0.17666016102759910, -0.56376170618524390));

  Handle(Geom_ToroidalSurface) aTorus = new Geom_ToroidalSurface(anAx3, 87.08479625, 23.14682176);

  const double aTolerance = 3.0e-7;
  const int    aNbPtsMax  = 5;
  const int    aLowIndex  = 5;
  const double aStep      = 2.0 * M_PI / aNbPtsMax;

  NCollection_Array1<double> aCoeffs(aLowIndex, aLowIndex + 34);
  aTorus->Torus().Coefficients(aCoeffs);

  double aUPar = 0.0;
  for (int aUind = 0; aUind <= aNbPtsMax; aUind++)
  {
    double aVPar = 0.0;
    for (int aVind = 0; aVind <= aNbPtsMax; aVind++)
    {
      const gp_Pnt aPt = aTorus->Value(aUPar, aVPar);
      const double aX1 = aPt.X();
      const double aX2 = aX1 * aX1;
      const double aX3 = aX2 * aX1;
      const double aX4 = aX2 * aX2;
      const double aY1 = aPt.Y();
      const double aY2 = aY1 * aY1;
      const double aY3 = aY2 * aY1;
      const double aY4 = aY2 * aY2;
      const double aZ1 = aPt.Z();
      const double aZ2 = aZ1 * aZ1;
      const double aZ3 = aZ2 * aZ1;
      const double aZ4 = aZ2 * aZ2;

      int i = aLowIndex;

      double aDelta = aCoeffs(i++) * aX4;
      aDelta += aCoeffs(i++) * aY4;
      aDelta += aCoeffs(i++) * aZ4;
      aDelta += aCoeffs(i++) * aX3 * aY1;
      aDelta += aCoeffs(i++) * aX3 * aZ1;
      aDelta += aCoeffs(i++) * aY3 * aX1;
      aDelta += aCoeffs(i++) * aY3 * aZ1;
      aDelta += aCoeffs(i++) * aZ3 * aX1;
      aDelta += aCoeffs(i++) * aZ3 * aY1;
      aDelta += aCoeffs(i++) * aX2 * aY2;
      aDelta += aCoeffs(i++) * aX2 * aZ2;
      aDelta += aCoeffs(i++) * aY2 * aZ2;
      aDelta += aCoeffs(i++) * aX2 * aY1 * aZ1;
      aDelta += aCoeffs(i++) * aX1 * aY2 * aZ1;
      aDelta += aCoeffs(i++) * aX1 * aY1 * aZ2;
      aDelta += aCoeffs(i++) * aX3;
      aDelta += aCoeffs(i++) * aY3;
      aDelta += aCoeffs(i++) * aZ3;
      aDelta += aCoeffs(i++) * aX2 * aY1;
      aDelta += aCoeffs(i++) * aX2 * aZ1;
      aDelta += aCoeffs(i++) * aY2 * aX1;
      aDelta += aCoeffs(i++) * aY2 * aZ1;
      aDelta += aCoeffs(i++) * aZ2 * aX1;
      aDelta += aCoeffs(i++) * aZ2 * aY1;
      aDelta += aCoeffs(i++) * aX1 * aY1 * aZ1;
      aDelta += aCoeffs(i++) * aX2;
      aDelta += aCoeffs(i++) * aY2;
      aDelta += aCoeffs(i++) * aZ2;
      aDelta += aCoeffs(i++) * aX1 * aY1;
      aDelta += aCoeffs(i++) * aX1 * aZ1;
      aDelta += aCoeffs(i++) * aY1 * aZ1;
      aDelta += aCoeffs(i++) * aX1;
      aDelta += aCoeffs(i++) * aY1;
      aDelta += aCoeffs(i++) * aZ1;
      aDelta += aCoeffs(i++);

      EXPECT_NEAR(aDelta, 0.0, aTolerance)
        << "Torus coefficient equation not satisfied at (u=" << aUPar << ", v=" << aVPar
        << "), delta=" << aDelta;

      aVPar = (aVind == aNbPtsMax) ? 2.0 * M_PI : aVPar + aStep;
    }

    aVPar = 0.0;
    aUPar = (aUind == aNbPtsMax) ? 2.0 * M_PI : aUPar + aStep;
  }
}
