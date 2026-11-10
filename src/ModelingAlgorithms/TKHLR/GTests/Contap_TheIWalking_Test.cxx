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

#include <Contap_SurfFunction.hxx>
#include <Contap_TheIWalking.hxx>
#include <Contap_TheIWLineOfTheIWalking.hxx>
#include <GeomAdaptor_Surface.hxx>
#include <Geom_ToroidalSurface.hxx>
#include <IntSurf_PathPoint.hxx>
#include <IntSurf_PntOn2S.hxx>
#include <NCollection_Sequence.hxx>
#include <Precision.hxx>
#include <gp_Ax3.hxx>
#include <gp_Dir.hxx>
#include <math_Vector.hxx>

#include <gtest/gtest.h>

namespace
{
class Contap_TestWalking : public Contap_TheIWalking
{
public:
  using Contap_TheIWalking::Cadrage;
  using Contap_TheIWalking::Contap_TheIWalking;
  using Contap_TheIWalking::OpenLine;
};
} // namespace

TEST(Contap_TheIWalkingTest, InteriorStepNearBoundaryKeepsParameters)
{
  const occ::handle<Geom_ToroidalSurface> aSurface  = new Geom_ToroidalSurface(gp_Ax3(), 2.0, 1.0);
  const occ::handle<GeomAdaptor_Surface>  anAdaptor = new GeomAdaptor_Surface(aSurface);
  const NCollection_Sequence<IntSurf_PathPoint> aNoStarts;
  for (const bool isConstantU : {false, true})
  {
    Contap_SurfFunction aFunction;
    aFunction.Set(anAdaptor);
    aFunction.Set(isConstantU ? gp_Dir(0.0, 1.0, 0.0) : gp_Dir(0.0, 0.0, 1.0));
    for (const bool isLower : {false, true})
    {
      SCOPED_TRACE(isConstantU);
      SCOPED_TRACE(isLower);
      Contap_TestWalking aWalking(Precision::Confusion(), 0.01, 0.1);
      IntSurf_PntOn2S    aPoint;
      aPoint.SetValue(aSurface->EvalD0(0.0, 0.0), false, 0.0, 0.0);
      const occ::handle<Contap_TheIWLineOfTheIWalking> aLine = new Contap_TheIWLineOfTheIWalking;
      aLine->AddPoint(aPoint);
      aWalking.OpenLine(0, aPoint, aNoStarts, aFunction, aLine);

      math_Vector aLower(1, 2), anUpper(1, 2), aNext(1, 2);
      aLower.Init(-1.0);
      anUpper.Init(1.0);
      const int    aConstantAxis     = isConstantU ? 1 : 2;
      const double aBoundaryDistance = 0.5 * Precision::PConfusion();
      (isLower ? aLower : anUpper)(aConstantAxis) =
        isLower ? -aBoundaryDistance : aBoundaryDistance;
      double aStep = 0.25;
      EXPECT_FALSE(aWalking.Cadrage(aLower, anUpper, aNext, aStep, 1));
      EXPECT_DOUBLE_EQ(aStep, 0.25);
      EXPECT_DOUBLE_EQ(aNext(1), isConstantU ? 0.0 : -0.25);
      EXPECT_DOUBLE_EQ(aNext(2), isConstantU ? 0.25 : 0.0);

      aLower.Init(-1.0);
      anUpper.Init(1.0);
      (isConstantU ? anUpper : aLower)(isConstantU ? 2 : 1) = isConstantU ? 0.125 : -0.125;
      aStep                                                 = 0.25;
      EXPECT_TRUE(aWalking.Cadrage(aLower, anUpper, aNext, aStep, 1));
      EXPECT_DOUBLE_EQ(aStep, 0.125);
      EXPECT_DOUBLE_EQ(aNext(1), isConstantU ? 0.0 : -0.125);
      EXPECT_DOUBLE_EQ(aNext(2), isConstantU ? 0.125 : 0.0);
    }
  }
}
