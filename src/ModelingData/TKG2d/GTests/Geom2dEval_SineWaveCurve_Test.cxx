// Copyright (c) 2025 OPEN CASCADE SAS
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

#include <Geom2dEval_SineWaveCurve.hxx>
#include <gp_Ax2d.hxx>
#include <gp_Pnt2d.hxx>
#include <gp_Trsf2d.hxx>
#include <gp_Vec2d.hxx>
#include <Precision.hxx>
#include <Standard_ConstructionError.hxx>
#include <Standard_SStream.hxx>

#include <gtest/gtest.h>

#include <cmath>

namespace
{
constexpr double THE_FD_TOL = 1e-5;
} // namespace

TEST(Geom2dEval_SineWaveCurveTest, Construction_ValidParams)
{
  gp_Ax2d anAx2d;
  Geom2dEval_SineWaveCurve aCurve(anAx2d, 2.0, 3.0, 0.0);
  EXPECT_NEAR(aCurve.Amplitude(), 2.0, Precision::Confusion());
  EXPECT_NEAR(aCurve.Omega(), 3.0, Precision::Confusion());
}

TEST(Geom2dEval_SineWaveCurveTest, Construction_InvalidParams_Throws)
{
  gp_Ax2d anAx2d;
  EXPECT_THROW(Geom2dEval_SineWaveCurve(anAx2d, 0.0, 1.0), Standard_ConstructionError);
  EXPECT_THROW(Geom2dEval_SineWaveCurve(anAx2d, 1.0, 0.0), Standard_ConstructionError);
}

TEST(Geom2dEval_SineWaveCurveTest, EvalD0_AtKnownPoints)
{
  gp_Ax2d anAx2d;
  const double aA = 2.0, aOm = 3.0;
  Geom2dEval_SineWaveCurve aCurve(anAx2d, aA, aOm, 0.0);

  // t=0: C(0) = (0, 0)
  gp_Pnt2d aP0 = aCurve.EvalD0(0.0);
  EXPECT_NEAR(aP0.X(), 0.0, Precision::Confusion());
  EXPECT_NEAR(aP0.Y(), 0.0, Precision::Confusion());

  // t = Pi/(2*omega): C = (Pi/(2*omega), A)
  const double aT1 = M_PI / (2.0 * aOm);
  gp_Pnt2d aP1 = aCurve.EvalD0(aT1);
  EXPECT_NEAR(aP1.X(), aT1, Precision::Confusion());
  EXPECT_NEAR(aP1.Y(), aA, Precision::Confusion());
}

TEST(Geom2dEval_SineWaveCurveTest, IsPeriodic)
{
  gp_Ax2d anAx2d;
  Geom2dEval_SineWaveCurve aCurve(anAx2d, 2.0, 3.0, 0.0);
  EXPECT_TRUE(aCurve.IsPeriodic());
  EXPECT_NEAR(aCurve.Period(), 2.0 * M_PI / 3.0, Precision::Confusion());
}

TEST(Geom2dEval_SineWaveCurveTest, EvalD1_ConsistentWithD0)
{
  gp_Ax2d anAx2d;
  Geom2dEval_SineWaveCurve aCurve(anAx2d, 2.0, 3.0, 0.5);
  const double aT = 1.0;

  Geom2d_Curve::ResD1 aD1 = aCurve.EvalD1(aT);
  gp_Pnt2d aP1 = aCurve.EvalD0(aT + Precision::Confusion());
  gp_Pnt2d aP2 = aCurve.EvalD0(aT - Precision::Confusion());
  gp_Vec2d aFD((aP1.XY() - aP2.XY()) / (2.0 * Precision::Confusion()));

  EXPECT_NEAR(aD1.D1.X(), aFD.X(), THE_FD_TOL);
  EXPECT_NEAR(aD1.D1.Y(), aFD.Y(), THE_FD_TOL);
}

TEST(Geom2dEval_SineWaveCurveTest, Copy_Independent)
{
  gp_Ax2d anAx2d;
  Geom2dEval_SineWaveCurve aCurve(anAx2d, 2.0, 3.0, 0.5);
  occ::handle<Geom2d_Geometry> aCopy = aCurve.Copy();
  EXPECT_FALSE(aCopy.IsNull());
  const Geom2dEval_SineWaveCurve* aCopyCurve =
    dynamic_cast<const Geom2dEval_SineWaveCurve*>(aCopy.get());
  EXPECT_TRUE(aCopyCurve != nullptr);
  EXPECT_NEAR(aCopyCurve->Amplitude(), 2.0, Precision::Confusion());
}

TEST(Geom2dEval_SineWaveCurveTest, DumpJson_NoCrash)
{
  gp_Ax2d anAx2d;
  Geom2dEval_SineWaveCurve aCurve(anAx2d, 2.0, 3.0, 0.0);
  Standard_SStream aSS;
  EXPECT_NO_THROW(aCurve.DumpJson(aSS));
}
