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

#include <Geom_Line.hxx>
#include <GeomEval_SineWaveCurve.hxx>
#include <gp_Ax1.hxx>
#include <gp_Ax2.hxx>
#include <gp_Pnt.hxx>
#include <gp_Trsf.hxx>
#include <gp_Vec.hxx>
#include <Precision.hxx>
#include <Standard_ConstructionError.hxx>
#include <Standard_NoSuchObject.hxx>
#include <Standard_NotImplemented.hxx>
#include <Standard_SStream.hxx>

#include <gtest/gtest.h>

#include <cmath>

namespace
{
constexpr double THE_FD_TOL = 1e-5;
} // namespace

TEST(GeomEval_SineWaveCurveTest, Construction_ValidParams)
{
  gp_Ax2                 anAx2;
  GeomEval_SineWaveCurve aCurve(anAx2, 2.0, 3.0, 0.0);
  EXPECT_NEAR(aCurve.Amplitude(), 2.0, Precision::Confusion());
  EXPECT_NEAR(aCurve.Omega(), 3.0, Precision::Confusion());
  EXPECT_NEAR(aCurve.Phase(), 0.0, Precision::Confusion());
}

TEST(GeomEval_SineWaveCurveTest, Construction_InvalidParams_Throws)
{
  gp_Ax2 anAx2;
  EXPECT_THROW(GeomEval_SineWaveCurve(anAx2, 0.0, 1.0), Standard_ConstructionError);
  EXPECT_THROW(GeomEval_SineWaveCurve(anAx2, -1.0, 1.0), Standard_ConstructionError);
  EXPECT_THROW(GeomEval_SineWaveCurve(anAx2, 1.0, 0.0), Standard_ConstructionError);
  EXPECT_THROW(GeomEval_SineWaveCurve(anAx2, 1.0, -1.0), Standard_ConstructionError);
}

TEST(GeomEval_SineWaveCurveTest, EvalD0_AtKnownPoints)
{
  gp_Ax2                 anAx2;
  const double           aA = 2.0, aOm = 3.0;
  GeomEval_SineWaveCurve aCurve(anAx2, aA, aOm, 0.0);

  // t=0: C(0) = (0, 0, 0) since sin(0)=0
  gp_Pnt aP0 = aCurve.EvalD0(0.0);
  EXPECT_NEAR(aP0.X(), 0.0, Precision::Confusion());
  EXPECT_NEAR(aP0.Y(), 0.0, Precision::Confusion());
  EXPECT_NEAR(aP0.Z(), 0.0, Precision::Confusion());

  // t = Pi/(2*omega): C = (Pi/(2*omega), A, 0) since sin(Pi/2)=1
  const double aT1 = M_PI / (2.0 * aOm);
  gp_Pnt       aP1 = aCurve.EvalD0(aT1);
  EXPECT_NEAR(aP1.X(), aT1, Precision::Confusion());
  EXPECT_NEAR(aP1.Y(), aA, Precision::Confusion());
  EXPECT_NEAR(aP1.Z(), 0.0, Precision::Confusion());
}

TEST(GeomEval_SineWaveCurveTest, IsPeriodic)
{
  gp_Ax2                 anAx2;
  GeomEval_SineWaveCurve aCurve(anAx2, 2.0, 3.0, 0.0);
  EXPECT_FALSE(aCurve.IsPeriodic());
#ifndef No_Exception
  EXPECT_THROW(aCurve.Period(), Standard_NoSuchObject);
#endif
  EXPECT_FALSE(aCurve.IsClosed());
}

TEST(GeomEval_SineWaveCurveTest, Reverse_NotImplemented)
{
  gp_Ax2                 anAx2;
  GeomEval_SineWaveCurve aCurve(anAx2, 2.0, 3.0, 0.0);
  EXPECT_THROW(aCurve.Reverse(), Standard_NotImplemented);
  EXPECT_THROW(aCurve.ReversedParameter(0.5), Standard_NotImplemented);
}

TEST(GeomEval_SineWaveCurveTest, EvalD1_ConsistentWithD0)
{
  gp_Ax2                 anAx2;
  GeomEval_SineWaveCurve aCurve(anAx2, 2.0, 3.0, 0.5);
  const double           aT = 1.0;

  Geom_Curve::ResD1 aD1 = aCurve.EvalD1(aT);
  gp_Pnt            aP1 = aCurve.EvalD0(aT + Precision::Confusion());
  gp_Pnt            aP2 = aCurve.EvalD0(aT - Precision::Confusion());
  gp_Vec            aFD((aP1.XYZ() - aP2.XYZ()) / (2.0 * Precision::Confusion()));

  EXPECT_NEAR(aD1.D1.X(), aFD.X(), THE_FD_TOL);
  EXPECT_NEAR(aD1.D1.Y(), aFD.Y(), THE_FD_TOL);
  EXPECT_NEAR(aD1.D1.Z(), aFD.Z(), THE_FD_TOL);
}

TEST(GeomEval_SineWaveCurveTest, EvalD2_Analytical)
{
  gp_Ax2                 anAx2;
  const double           aA = 2.0, aOm = 3.0, aPhi = 0.5;
  GeomEval_SineWaveCurve aCurve(anAx2, aA, aOm, aPhi);
  const double           aT = 1.0;

  Geom_Curve::ResD2 aD2 = aCurve.EvalD2(aT);

  // D2 = -A*omega^2*sin(omega*t+phi)*YDir = (0, -A*om^2*sin(...), 0)
  const double aExpY = -aA * aOm * aOm * std::sin(aOm * aT + aPhi);
  EXPECT_NEAR(aD2.D2.X(), 0.0, Precision::Confusion());
  EXPECT_NEAR(aD2.D2.Y(), aExpY, Precision::Confusion());
  EXPECT_NEAR(aD2.D2.Z(), 0.0, Precision::Confusion());
}

TEST(GeomEval_SineWaveCurveTest, ComparisonWithLine_ZeroAmplitude_Like)
{
  // When amplitude approaches zero, D0 approaches a line.
  // We test D2 and D3 are near zero for very small amplitude.
  gp_Ax2                 anAx2;
  GeomEval_SineWaveCurve aCurve(anAx2, 1e-10, 1.0, 0.0);
  const double           aT = 1.0;

  Geom_Curve::ResD3 aD3 = aCurve.EvalD3(aT);
  EXPECT_NEAR(aD3.D2.Magnitude(), 0.0, 1e-5);
  EXPECT_NEAR(aD3.D3.Magnitude(), 0.0, 1e-5);
}

TEST(GeomEval_SineWaveCurveTest, Transform_NotImplemented)
{
  gp_Ax2                 anAx2;
  GeomEval_SineWaveCurve aCurve(anAx2, 2.0, 3.0, 0.5);
  gp_Trsf                aTrsf;
  aTrsf.SetTranslation(gp_Vec(1.0, 2.0, 3.0));

  EXPECT_THROW(aCurve.Transform(aTrsf), Standard_NotImplemented);
  EXPECT_THROW((void)aCurve.Transformed(aTrsf), Standard_NotImplemented);
}

TEST(GeomEval_SineWaveCurveTest, Copy_Independent)
{
  gp_Ax2                     anAx2;
  GeomEval_SineWaveCurve     aCurve(anAx2, 2.0, 3.0, 0.5);
  occ::handle<Geom_Geometry> aCopy = aCurve.Copy();
  EXPECT_FALSE(aCopy.IsNull());
  const GeomEval_SineWaveCurve* aCopyCurve =
    dynamic_cast<const GeomEval_SineWaveCurve*>(aCopy.get());
  EXPECT_TRUE(aCopyCurve != nullptr);
  EXPECT_NEAR(aCopyCurve->Amplitude(), 2.0, Precision::Confusion());
  EXPECT_NEAR(aCopyCurve->Omega(), 3.0, Precision::Confusion());
  EXPECT_NEAR(aCopyCurve->Phase(), 0.5, Precision::Confusion());
}

TEST(GeomEval_SineWaveCurveTest, DumpJson_NoCrash)
{
  gp_Ax2                 anAx2;
  GeomEval_SineWaveCurve aCurve(anAx2, 2.0, 3.0, 0.0);
  Standard_SStream       aSS;
  EXPECT_NO_THROW(aCurve.DumpJson(aSS));
}
