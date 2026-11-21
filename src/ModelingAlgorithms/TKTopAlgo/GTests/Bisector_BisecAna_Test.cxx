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

#include <Bisector_BisecAna.hxx>
#include <Geom2d_Ellipse.hxx>
#include <Geom2d_TrimmedCurve.hxx>
#include <gp_Ax2d.hxx>
#include <gp_Dir2d.hxx>
#include <gp_Pnt2d.hxx>
#include <Precision.hxx>
#include <gtest/gtest.h>

TEST(Bisector_BisecAnaTest, CopyUsesNaturalPeriod)
{
  const occ::handle<Geom2d_Ellipse> anEllipse =
    new Geom2d_Ellipse(gp_Ax2d(gp_Pnt2d(0.0, 0.0), gp_Dir2d(1.0, 0.0)), 3.0, 2.0);
  for (const double aFirst : {-1.48, 1.53, -4.75})
  {
    const occ::handle<Geom2d_TrimmedCurve> aTrim =
      new Geom2d_TrimmedCurve(anEllipse, aFirst, aFirst + 2.0 * M_PI, true, false);
    Bisector_BisecAna aBisector;
    aBisector.Init(aTrim);
    const occ::handle<Bisector_BisecAna> aCopy =
      occ::down_cast<Bisector_BisecAna>(aBisector.Copy());
    ASSERT_FALSE(aCopy.IsNull());
    EXPECT_GE(aCopy->FirstParameter(), 0.0);
    EXPECT_LT(aCopy->FirstParameter(), 2.0 * M_PI);
    EXPECT_NEAR(aCopy->LastParameter() - aCopy->FirstParameter(),
                2.0 * M_PI,
                Precision::PConfusion());
    EXPECT_LE(aCopy->EvalD0(aCopy->FirstParameter()).Distance(aBisector.EvalD0(aFirst)),
              Precision::Confusion());
    EXPECT_DOUBLE_EQ(aBisector.FirstParameter(), aFirst);
  }
}
