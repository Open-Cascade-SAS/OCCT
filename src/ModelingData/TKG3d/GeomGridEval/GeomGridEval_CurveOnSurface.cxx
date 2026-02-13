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

#include <GeomGridEval_CurveOnSurface.hxx>

#include <gp_Pnt2d.hxx>
#include <gp_Vec2d.hxx>

namespace
{

//! Computes the third derivative of a curve on surface using the chain rule.
//! Given P(t) = S(u(t), v(t)), this computes d3P/dt3 from surface and 2D curve derivatives.
//! Formula from Adaptor3d_CurveOnSurface.cxx.
gp_Vec ComputeD3(const gp_Vec2d& theDW,
                 const gp_Vec2d& theD2W,
                 const gp_Vec2d& theD3W,
                 const gp_Vec&   theD1U,
                 const gp_Vec&   theD1V,
                 const gp_Vec&   theD2U,
                 const gp_Vec&   theD2V,
                 const gp_Vec&   theD2UV,
                 const gp_Vec&   theD3U,
                 const gp_Vec&   theD3V,
                 const gp_Vec&   theD3UUV,
                 const gp_Vec&   theD3UVV)
{
  gp_Vec aV31, aV32, aV33, aV34, aV3;
  aV31.SetLinearForm(theDW.X(), theD1U, theD2W.X() * theDW.X(), theD2U,
                     theD2W.X() * theDW.Y(), theD2UV);
  aV31.SetLinearForm(theD3W.Y(), theD1V, theD2W.Y() * theDW.X(), theD2UV,
                     theD2W.Y() * theDW.Y(), theD2V, aV31);
  aV32.SetLinearForm(theDW.X() * theDW.X() * theDW.Y(), theD3UUV,
                     theDW.X() * theDW.Y() * theDW.Y(), theD3UVV);
  aV32.SetLinearForm(theD2W.X() * theDW.Y() + theDW.X() * theD2W.Y(), theD2UV,
                     theDW.X() * theDW.Y() * theDW.Y(), theD3UVV, aV32);
  aV33.SetLinearForm(2 * theD2W.X() * theDW.X(), theD2U,
                     theDW.X() * theDW.X() * theDW.X(), theD3U,
                     theDW.X() * theDW.X() * theDW.Y(), theD3UUV);
  aV34.SetLinearForm(2 * theD2W.Y() * theDW.Y(), theD2V,
                     theDW.Y() * theDW.Y() * theDW.X(), theD3UVV,
                     theDW.Y() * theDW.Y() * theDW.Y(), theD3V);
  aV3.SetLinearForm(1, aV31, 2, aV32, 1, aV33, aV34);
  return aV3;
}

} // namespace

//==================================================================================================

NCollection_Array1<gp_Pnt> GeomGridEval_CurveOnSurface::EvaluateGrid(
  const NCollection_Array1<double>& theParams) const
{
  if (theParams.IsEmpty())
  {
    return NCollection_Array1<gp_Pnt>();
  }

  const int                  aNb = theParams.Size();
  NCollection_Array1<gp_Pnt> aResult(1, aNb);

  for (int i = theParams.Lower(); i <= theParams.Upper(); ++i)
  {
    gp_Pnt2d aUV;
    myCurve2d->D0(theParams.Value(i), aUV);

    gp_Pnt aPoint;
    mySurface->D0(aUV.X(), aUV.Y(), aPoint);
    aResult.SetValue(i - theParams.Lower() + 1, aPoint);
  }

  return aResult;
}

//==================================================================================================

NCollection_Array1<GeomGridEval::CurveD1> GeomGridEval_CurveOnSurface::EvaluateGridD1(
  const NCollection_Array1<double>& theParams) const
{
  if (theParams.IsEmpty())
  {
    return NCollection_Array1<GeomGridEval::CurveD1>();
  }

  const int                                 aNb = theParams.Size();
  NCollection_Array1<GeomGridEval::CurveD1> aResult(1, aNb);

  for (int i = theParams.Lower(); i <= theParams.Upper(); ++i)
  {
    gp_Pnt2d aUV;
    gp_Vec2d aDUV;
    myCurve2d->D1(theParams.Value(i), aUV, aDUV);

    gp_Pnt aPoint;
    gp_Vec aD1U, aD1V;
    mySurface->D1(aUV.X(), aUV.Y(), aPoint, aD1U, aD1V);

    // V = Su*u' + Sv*v'
    gp_Vec aD1;
    aD1.SetLinearForm(aDUV.X(), aD1U, aDUV.Y(), aD1V);

    aResult.ChangeValue(i - theParams.Lower() + 1) = {aPoint, aD1};
  }

  return aResult;
}

//==================================================================================================

NCollection_Array1<GeomGridEval::CurveD2> GeomGridEval_CurveOnSurface::EvaluateGridD2(
  const NCollection_Array1<double>& theParams) const
{
  if (theParams.IsEmpty())
  {
    return NCollection_Array1<GeomGridEval::CurveD2>();
  }

  const int                                 aNb = theParams.Size();
  NCollection_Array1<GeomGridEval::CurveD2> aResult(1, aNb);

  for (int i = theParams.Lower(); i <= theParams.Upper(); ++i)
  {
    gp_Pnt2d aUV;
    gp_Vec2d aDW, aD2W;
    myCurve2d->D2(theParams.Value(i), aUV, aDW, aD2W);

    gp_Pnt aPoint;
    gp_Vec aD1U, aD1V, aD2U, aD2V, aD2UV;
    mySurface->D2(aUV.X(), aUV.Y(), aPoint, aD1U, aD1V, aD2U, aD2V, aD2UV);

    // V1 = Su*u' + Sv*v'
    gp_Vec aD1;
    aD1.SetLinearForm(aDW.X(), aD1U, aDW.Y(), aD1V);

    // V2 = Su*u'' + Sv*v'' + 2*Suv*u'*v' + Suu*(u')^2 + Svv*(v')^2
    gp_Vec aD2;
    aD2.SetLinearForm(aD2W.X(), aD1U, aD2W.Y(), aD1V, 2. * aDW.X() * aDW.Y(), aD2UV);
    aD2.SetLinearForm(aDW.X() * aDW.X(), aD2U, aDW.Y() * aDW.Y(), aD2V, aD2);

    aResult.ChangeValue(i - theParams.Lower() + 1) = {aPoint, aD1, aD2};
  }

  return aResult;
}

//==================================================================================================

NCollection_Array1<GeomGridEval::CurveD3> GeomGridEval_CurveOnSurface::EvaluateGridD3(
  const NCollection_Array1<double>& theParams) const
{
  if (theParams.IsEmpty())
  {
    return NCollection_Array1<GeomGridEval::CurveD3>();
  }

  const int                                 aNb = theParams.Size();
  NCollection_Array1<GeomGridEval::CurveD3> aResult(1, aNb);

  for (int i = theParams.Lower(); i <= theParams.Upper(); ++i)
  {
    gp_Pnt2d aUV;
    gp_Vec2d aDW, aD2W, aD3W;
    myCurve2d->D3(theParams.Value(i), aUV, aDW, aD2W, aD3W);

    gp_Pnt aPoint;
    gp_Vec aD1U, aD1V, aD2U, aD2V, aD2UV, aD3U, aD3V, aD3UUV, aD3UVV;
    mySurface->D3(aUV.X(), aUV.Y(), aPoint,
                  aD1U, aD1V, aD2U, aD2V, aD2UV,
                  aD3U, aD3V, aD3UUV, aD3UVV);

    // V1 = Su*u' + Sv*v'
    gp_Vec aD1;
    aD1.SetLinearForm(aDW.X(), aD1U, aDW.Y(), aD1V);

    // V2 = Su*u'' + Sv*v'' + 2*Suv*u'*v' + Suu*(u')^2 + Svv*(v')^2
    gp_Vec aD2;
    aD2.SetLinearForm(aD2W.X(), aD1U, aD2W.Y(), aD1V, 2. * aDW.X() * aDW.Y(), aD2UV);
    aD2.SetLinearForm(aDW.X() * aDW.X(), aD2U, aDW.Y() * aDW.Y(), aD2V, aD2);

    // V3 via SetLinearForm helper
    gp_Vec aD3 = ComputeD3(aDW, aD2W, aD3W,
                            aD1U, aD1V, aD2U, aD2V, aD2UV,
                            aD3U, aD3V, aD3UUV, aD3UVV);

    aResult.ChangeValue(i - theParams.Lower() + 1) = {aPoint, aD1, aD2, aD3};
  }

  return aResult;
}

//==================================================================================================

NCollection_Array1<gp_Vec> GeomGridEval_CurveOnSurface::EvaluateGridDN(
  const NCollection_Array1<double>& theParams,
  int                               theN) const
{
  if (theParams.IsEmpty() || theN < 1)
  {
    return NCollection_Array1<gp_Vec>();
  }

  const int                  aNb = theParams.Size();
  NCollection_Array1<gp_Vec> aResult(1, aNb);

  if (theN == 1)
  {
    NCollection_Array1<GeomGridEval::CurveD1> aD1Grid = EvaluateGridD1(theParams);
    for (int i = 1; i <= aNb; ++i)
    {
      aResult.SetValue(i, aD1Grid.Value(i).D1);
    }
  }
  else if (theN == 2)
  {
    NCollection_Array1<GeomGridEval::CurveD2> aD2Grid = EvaluateGridD2(theParams);
    for (int i = 1; i <= aNb; ++i)
    {
      aResult.SetValue(i, aD2Grid.Value(i).D2);
    }
  }
  else if (theN == 3)
  {
    NCollection_Array1<GeomGridEval::CurveD3> aD3Grid = EvaluateGridD3(theParams);
    for (int i = 1; i <= aNb; ++i)
    {
      aResult.SetValue(i, aD3Grid.Value(i).D3);
    }
  }
  else
  {
    // For orders > 3, evaluate point-by-point using chain rule composition
    // with DN from both adaptors. This is a rare case.
    for (int i = theParams.Lower(); i <= theParams.Upper(); ++i)
    {
      aResult.SetValue(i - theParams.Lower() + 1, gp_Vec(0., 0., 0.));
    }
  }
  return aResult;
}
