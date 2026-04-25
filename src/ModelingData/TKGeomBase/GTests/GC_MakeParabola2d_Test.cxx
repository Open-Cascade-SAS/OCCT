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

#include <GC_MakeParabola2d.hxx>
#include <gp_Ax2d.hxx>
#include <gp_Dir2d.hxx>
#include <gp_Parab2d.hxx>
#include <gp_Pnt2d.hxx>

namespace
{
// Helper: verify a parabola built from directrix+focus against expected values.
void CheckParabola2d(const gp_Ax2d& theAxes,
                     const gp_Pnt2d& theFocus,
                     bool             theSense,
                     double           theExpectedFocal,
                     double           theExpectedVertX,
                     double           theExpectedVertY,
                     double           theExpectedParam,
                     const double     theExpectedCoeffs[6])
{
  const double aCompareTol = 1.0e-12;

  GC_MakeParabola2d aPrb(theAxes, theFocus, theSense);
  ASSERT_FALSE(aPrb.Value().IsNull()) << "GC_MakeParabola2d should produce a non-null result";

  const gp_Parab2d& aParab = aPrb.Value()->Parab2d();
  const gp_Pnt2d    aVert(aParab.Location());

  EXPECT_NEAR(aParab.Focal(), theExpectedFocal, aCompareTol) << "Wrong focal length";
  EXPECT_NEAR(aVert.X(), theExpectedVertX, aCompareTol) << "Wrong vertex X";
  EXPECT_NEAR(aVert.Y(), theExpectedVertY, aCompareTol) << "Wrong vertex Y";
  EXPECT_NEAR(aParab.Parameter(), theExpectedParam, aCompareTol) << "Wrong parameter";

  double aF[6];
  aParab.Coefficients(aF[0], aF[1], aF[2], aF[3], aF[4], aF[5]);
  for (int i = 0; i < 6; ++i)
  {
    EXPECT_NEAR(aF[i], theExpectedCoeffs[i], aCompareTol)
      << "Wrong coefficient [" << i << "]: got " << aF[i] << ", expected "
      << theExpectedCoeffs[i];
  }
}
} // namespace

// Test OCC26747 (case 1): parabola with vertex at (0.5, 3.0) opening in +X direction.
// Directrix Y-axis at x=0, y=3; focus at (1.0, 3.0); sense=true.
// Equation: (y-3)^2 = 2*(x-0.5), i.e. 1*Y^2 + 2*(-1)*X + 2*(-3)*Y + 10 = 0.
TEST(GC_MakeParabola2d_Test, OCC26747_1_ParabolaOpeningRight)
{
  const gp_Ax2d   anAxes(gp_Pnt2d(0.0, 3.0), gp_Dir2d(gp_Dir2d::D::Y));
  const gp_Pnt2d  aFocus(1.0, 3.0);
  const double    aCoeffs[6] = {0.0, 1.0, 0.0, -1.0, -3.0, 10.0};
  CheckParabola2d(anAxes, aFocus, true, 0.5, 0.5, 3.0, 1.0, aCoeffs);
}

// Test OCC26747 (case 2): parabola with vertex at (-0.5, 3.0) opening in -X direction.
// Directrix Y-axis at origin; focus at (-1.0, 3.0); sense=false.
// Equation (WCS): (y-3)^2 = 2*(-x-0.5), i.e. 1*Y^2 + 2*1*X + 2*(-3)*Y + 10 = 0.
TEST(GC_MakeParabola2d_Test, OCC26747_2_ParabolaOpeningLeft)
{
  const gp_Ax2d   anAxes(gp_Pnt2d(0.0, 0.0), gp_Dir2d(gp_Dir2d::D::Y));
  const gp_Pnt2d  aFocus(-1.0, 3.0);
  const double    aCoeffs[6] = {0.0, 1.0, 0.0, 1.0, -3.0, 10.0};
  CheckParabola2d(anAxes, aFocus, false, 0.5, -0.5, 3.0, 1.0, aCoeffs);
}

// Test OCC26747 (case 3): degenerate parabola where focus coincides with vertex.
// Directrix Y-axis at origin; focus at (0.0, 3.0); sense=false.
// Focal length = 0, parameter = 0. Equation: Y^2 + 2*(-3)*Y + 9 = 0 (line y=3).
TEST(GC_MakeParabola2d_Test, OCC26747_3_DegenerateParabola)
{
  const gp_Ax2d   anAxes(gp_Pnt2d(0.0, 0.0), gp_Dir2d(gp_Dir2d::D::Y));
  const gp_Pnt2d  aFocus(0.0, 3.0);
  const double    aCoeffs[6] = {0.0, 1.0, 0.0, 0.0, -3.0, 9.0};
  CheckParabola2d(anAxes, aFocus, false, 0.0, 0.0, 3.0, 0.0, aCoeffs);
}
