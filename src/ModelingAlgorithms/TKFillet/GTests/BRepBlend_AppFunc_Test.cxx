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

#include <BRepBlend_AppFunc.hxx>
#include <BRepBlend_Line.hxx>
#include <Blend_Point.hxx>
#include <BlendFunc_ConstRad.hxx>
#include <Adaptor3d_Curve.hxx>
#include <Adaptor3d_Surface.hxx>
#include <GeomAdaptor_Curve.hxx>
#include <GeomAdaptor_Surface.hxx>
#include <Geom_Line.hxx>
#include <Geom_Plane.hxx>
#include <gp.hxx>
#include <gp_Ax3.hxx>
#include <gp_Pnt.hxx>
#include <gp_Pnt2d.hxx>
#include <NCollection_Array1.hxx>
#include <Precision.hxx>
#include <gtest/gtest.h>

namespace
{
constexpr double THE_BLEND_RADIUS = 1.0;
constexpr int    THE_BLEND_CHOICE = 1;

//==================================================================================================

Blend_Point sectionPoint(double theParameter)
{
  return Blend_Point(gp_Pnt(THE_BLEND_RADIUS, theParameter, 0.0),
                     gp_Pnt(0.0, theParameter, THE_BLEND_RADIUS),
                     theParameter,
                     THE_BLEND_RADIUS,
                     theParameter,
                     theParameter,
                     THE_BLEND_RADIUS);
}

struct BRepBlend_PlaneBlend
{
  // First contact plane.
  occ::handle<Adaptor3d_Surface> First = new GeomAdaptor_Surface(new Geom_Plane(gp_Ax3()));
  // Second contact plane.
  occ::handle<Adaptor3d_Surface> Second =
    new GeomAdaptor_Surface(new Geom_Plane(gp_Ax3(gp::Origin(), gp::DX(), gp::DY())));
  // Straight guide.
  occ::handle<Adaptor3d_Curve> Guide = new GeomAdaptor_Curve(new Geom_Line(gp::Origin(), gp::DY()));
  // Blend section function.
  BlendFunc_ConstRad Function{First, Second, Guide};

  BRepBlend_PlaneBlend();
};

//==================================================================================================

BRepBlend_PlaneBlend::BRepBlend_PlaneBlend()
{
  Function.Set(THE_BLEND_RADIUS, THE_BLEND_CHOICE);
}

//==================================================================================================

bool evaluateSection(BRepBlend_AppFunc&  theApproximation,
                     BlendFunc_ConstRad& theFunction,
                     double              theParameter)
{
  int aNbPoles, aNbKnots, aDegree, aNbPoles2d;
  theFunction.GetShape(aNbPoles, aNbKnots, aDegree, aNbPoles2d);
  NCollection_Array1<gp_Pnt>   aPoles(1, aNbPoles);
  NCollection_Array1<gp_Pnt2d> aPoles2d(1, aNbPoles2d);
  NCollection_Array1<double>   aWeights(1, aNbPoles);
  if (!theApproximation.D0(theParameter, theParameter, theParameter, aPoles, aPoles2d, aWeights))
  {
    return false;
  }
  for (const gp_Pnt& aPole : aPoles)
  {
    EXPECT_NEAR(aPole.Y(), theParameter, Precision::Confusion());
  }
  return true;
}
} // namespace

//==================================================================================================

TEST(BRepBlend_AppFuncTest, LongWalkingLinePreservesSections)
{
  BRepBlend_PlaneBlend        aBlend;
  occ::handle<BRepBlend_Line> aLine          = new BRepBlend_Line;
  constexpr int               aNbSteps       = 1024;
  constexpr double            aStep          = 1.0 / 16.0;
  constexpr double            aLastParameter = aNbSteps * aStep;
  for (int anIndex = 0; anIndex <= aNbSteps; ++anIndex)
  {
    aLine->Append(sectionPoint(anIndex * aStep));
  }
  BRepBlend_AppFunc aApproximation(aLine,
                                   aBlend.Function,
                                   Precision::Confusion(),
                                   Precision::Confusion());
  for (double aParameter : {0.0,
                            aLastParameter,
                            0.5 * (aLastParameter + aStep),
                            0.5 * aStep,
                            aLastParameter - 0.5 * aStep,
                            2.0 * aStep})
  {
    EXPECT_TRUE(evaluateSection(aApproximation, aBlend.Function, aParameter));
  }
}

//==================================================================================================

TEST(BRepBlend_AppFuncTest, EditedWalkingLinePreservesSections)
{
  BRepBlend_PlaneBlend        aBlend;
  occ::handle<BRepBlend_Line> aLine = new BRepBlend_Line;
  aLine->Append(sectionPoint(0.0));
  aLine->Append(sectionPoint(1.0));
  BRepBlend_AppFunc aApproximation(aLine,
                                   aBlend.Function,
                                   Precision::Confusion(),
                                   Precision::Confusion());
  ASSERT_TRUE(evaluateSection(aApproximation, aBlend.Function, 0.5));

  aLine->Remove(1, 1);
  aLine->Append(sectionPoint(2.0));
  EXPECT_TRUE(evaluateSection(aApproximation, aBlend.Function, 1.5));

  aLine->Prepend(sectionPoint(0.0));
  aLine->InsertBefore(2, sectionPoint(0.5));
  EXPECT_TRUE(evaluateSection(aApproximation, aBlend.Function, 0.25));
  EXPECT_TRUE(evaluateSection(aApproximation, aBlend.Function, 0.5));
}

//==================================================================================================

TEST(BRepBlend_AppFuncTest, ClearedAndAssignedWalkingLinePreservesSections)
{
  BRepBlend_PlaneBlend        aBlend;
  occ::handle<BRepBlend_Line> aLine = new BRepBlend_Line;
  aLine->Append(sectionPoint(0.0));
  aLine->Append(sectionPoint(1.0));
  BRepBlend_AppFunc aApproximation(aLine,
                                   aBlend.Function,
                                   Precision::Confusion(),
                                   Precision::Confusion());
  ASSERT_TRUE(evaluateSection(aApproximation, aBlend.Function, 0.5));

  aLine->Clear();
  EXPECT_FALSE(evaluateSection(aApproximation, aBlend.Function, 0.5));
  aLine->Append(sectionPoint(2.0));
  aLine->Append(sectionPoint(3.0));
  ASSERT_TRUE(evaluateSection(aApproximation, aBlend.Function, 2.5));

  BRepBlend_Line aReplacement;
  aReplacement.Append(sectionPoint(4.0));
  aReplacement.Append(sectionPoint(5.0));
  *aLine = aReplacement;
  EXPECT_TRUE(evaluateSection(aApproximation, aBlend.Function, 4.5));
  EXPECT_FALSE(evaluateSection(aApproximation, aBlend.Function, 2.5));
}
