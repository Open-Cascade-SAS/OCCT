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

#include <BlendFunc_ConstRadInv.hxx>
#include <BlendFunc_EvolRadInv.hxx>
#include <BlendFunc_ChamfInv.hxx>
#include <BlendFunc_ChAsymInv.hxx>
#include <Geom_BezierSurface.hxx>
#include <Geom_Line.hxx>
#include <Geom2d_Line.hxx>
#include <GeomAdaptor_Surface.hxx>
#include <GeomAdaptor_Curve.hxx>
#include <Geom2dAdaptor_Curve.hxx>
#include <Law_Constant.hxx>
#include <gtest/gtest.h>

namespace
{
occ::handle<Adaptor3d_Surface> scaledPlane(double theUScale, double theVScale)
{
  NCollection_Array2<gp_Pnt> aPoles(1, 2, 1, 2);
  for (int u = 1; u <= 2; ++u)
  {
    for (int v = 1; v <= 2; ++v)
    {
      aPoles(u, v) = gp_Pnt((u - 1) * theUScale, (v - 1) * theVScale, 0.0);
    }
  }
  return new GeomAdaptor_Surface(new Geom_BezierSurface(aPoles));
}
} // namespace

TEST(BlendFunc_InvTolerance, RestrictionUsesSpatialUnits)
{
  const occ::handle<Adaptor3d_Surface> aFirst  = scaledPlane(100.0, 200.0);
  const occ::handle<Adaptor3d_Surface> aSecond = scaledPlane(5.0, 20.0);
  const occ::handle<Adaptor3d_Curve>   aGuide =
    new GeomAdaptor_Curve(new Geom_Line(gp::Origin(), gp::DZ()));
  const occ::handle<Law_Function> aLaw = new Law_Constant;
  BlendFunc_ConstRadInv           aConstant(aFirst, aSecond, aGuide);
  BlendFunc_EvolRadInv            anEvolving(aFirst, aSecond, aGuide, aLaw);
  BlendFunc_ChamfInv              aChamfer(aFirst, aSecond, aGuide);
  BlendFunc_ChAsymInv             anAsymmetric(aFirst, aSecond, aGuide);
  Blend_FuncInv* aFunctions[]      = {&aConstant, &anEvolving, &aChamfer, &anAsymmetric};
  const double   aSpatialTolerance = 1.e-4;
  for (Blend_FuncInv* aFunction : aFunctions)
  {
    for (bool isFirst : {true, false})
    {
      const auto& aSurface = isFirst ? aFirst : aSecond;
      for (const gp_Dir2d& aDirection : {gp::DX2d(), gp::DY2d()})
      {
        const occ::handle<Adaptor2d_Curve2d> aRestriction =
          new Geom2dAdaptor_Curve(new Geom2d_Line(gp_Pnt2d(0, 0), aDirection));
        aFunction->Set(isFirst, aRestriction);
        math_Vector aTolerance(1, 4);
        aFunction->GetTolerance(aTolerance, aSpatialTolerance);
        ASSERT_GT(aTolerance(1), 0.0);
        const gp_Pnt2d aUV1 = aRestriction->EvalD0(0.5);
        const gp_Pnt2d aUV2 = aRestriction->EvalD0(0.5 + aTolerance(1));
        const double   aDistance =
          aSurface->EvalD0(aUV1.X(), aUV1.Y()).Distance(aSurface->EvalD0(aUV2.X(), aUV2.Y()));
        EXPECT_LE(aDistance, aSpatialTolerance * (1.0 + 1.e-7));
      }
    }
  }
}
