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

#include <Geom2dGridEval_OffsetCurve.hxx>

#include <Geom2d_OffsetCurveUtils.pxx>
#include <Geom2dGridEval_Curve.hxx>

//==================================================================================================

NCollection_Array1<gp_Pnt2d> Geom2dGridEval_OffsetCurve::EvaluateGrid(
  const NCollection_Array1<double>& theParams) const
{
  if (myBasis.IsNull() || theParams.IsEmpty())
  {
    return NCollection_Array1<gp_Pnt2d>();
  }

  // Offset D0 requires basis D1 to compute offset normal direction
  Geom2dGridEval_Curve aBasisEval;
  aBasisEval.Initialize(myBasis);

  NCollection_Array1<Geom2dGridEval::CurveD1> aBasisD1 = aBasisEval.EvaluateGridD1(theParams);
  if (aBasisD1.IsEmpty())
  {
    return NCollection_Array1<gp_Pnt2d>();
  }

  const int                    aNbParams = theParams.Size();
  NCollection_Array1<gp_Pnt2d> aResult(1, aNbParams);

  for (int i = 1; i <= aNbParams; ++i)
  {
    const Geom2dGridEval::CurveD1& aBasis = aBasisD1.Value(i);
    gp_Pnt2d                       aP     = aBasis.Point;
    Geom2d_OffsetCurveUtils::CalculateD0(aP, aBasis.D1, myOffset);
    aResult.SetValue(i, aP);
  }

  return aResult;
}

//==================================================================================================

NCollection_Array1<Geom2dGridEval::CurveD1> Geom2dGridEval_OffsetCurve::EvaluateGridD1(
  const NCollection_Array1<double>& theParams) const
{
  if (myBasis.IsNull() || theParams.IsEmpty())
  {
    return NCollection_Array1<Geom2dGridEval::CurveD1>();
  }

  // Offset D1 requires basis D2
  Geom2dGridEval_Curve aBasisEval;
  aBasisEval.Initialize(myBasis);

  NCollection_Array1<Geom2dGridEval::CurveD2> aBasisD2 = aBasisEval.EvaluateGridD2(theParams);
  if (aBasisD2.IsEmpty())
  {
    return NCollection_Array1<Geom2dGridEval::CurveD1>();
  }

  const int                                   aNbParams = theParams.Size();
  NCollection_Array1<Geom2dGridEval::CurveD1> aResult(1, aNbParams);

  for (int i = 1; i <= aNbParams; ++i)
  {
    const Geom2dGridEval::CurveD2& aBasis = aBasisD2.Value(i);
    gp_Pnt2d                       aP     = aBasis.Point;
    gp_Vec2d                       aD1    = aBasis.D1;
    Geom2d_OffsetCurveUtils::CalculateD1(aP, aD1, aBasis.D2, myOffset);
    aResult.ChangeValue(i) = {aP, aD1};
  }

  return aResult;
}

//==================================================================================================

NCollection_Array1<Geom2dGridEval::CurveD2> Geom2dGridEval_OffsetCurve::EvaluateGridD2(
  const NCollection_Array1<double>& theParams) const
{
  if (myBasis.IsNull() || theParams.IsEmpty())
  {
    return NCollection_Array1<Geom2dGridEval::CurveD2>();
  }

  // Offset D2 requires basis D3
  Geom2dGridEval_Curve aBasisEval;
  aBasisEval.Initialize(myBasis);

  NCollection_Array1<Geom2dGridEval::CurveD3> aBasisD3 = aBasisEval.EvaluateGridD3(theParams);
  if (aBasisD3.IsEmpty())
  {
    return NCollection_Array1<Geom2dGridEval::CurveD2>();
  }

  const int                                   aNbParams = theParams.Size();
  NCollection_Array1<Geom2dGridEval::CurveD2> aResult(1, aNbParams);

  for (int i = 1; i <= aNbParams; ++i)
  {
    const Geom2dGridEval::CurveD3& aBasis = aBasisD3.Value(i);
    gp_Pnt2d                       aP     = aBasis.Point;
    gp_Vec2d                       aD1    = aBasis.D1;
    gp_Vec2d                       aD2    = aBasis.D2;
    gp_Vec2d                       aD3    = aBasis.D3;

    // Check for direction change at singular points
    bool isDirectionChange = false;
    if (aD1.SquareMagnitude() <= gp::Resolution())
    {
      gp_Vec2d aDummyD4;
      isDirectionChange =
        Geom2d_OffsetCurveUtils::AdjustDerivative(*myBasis,
                                                   3,
                                                   theParams.Value(theParams.Lower() + i - 1),
                                                   aD1,
                                                   aD2,
                                                   aD3,
                                                   aDummyD4);
    }

    Geom2d_OffsetCurveUtils::CalculateD2(aP, aD1, aD2, aD3, isDirectionChange, myOffset);
    aResult.ChangeValue(i) = {aP, aD1, aD2};
  }

  return aResult;
}

//==================================================================================================

NCollection_Array1<Geom2dGridEval::CurveD3> Geom2dGridEval_OffsetCurve::EvaluateGridD3(
  const NCollection_Array1<double>& theParams) const
{
  if (myBasis.IsNull() || theParams.IsEmpty())
  {
    return NCollection_Array1<Geom2dGridEval::CurveD3>();
  }

  // Offset D3 requires basis D3 + D4
  Geom2dGridEval_Curve aBasisEval;
  aBasisEval.Initialize(myBasis);

  NCollection_Array1<Geom2dGridEval::CurveD3> aBasisD3 = aBasisEval.EvaluateGridD3(theParams);
  if (aBasisD3.IsEmpty())
  {
    return NCollection_Array1<Geom2dGridEval::CurveD3>();
  }

  const int                                   aNbParams = theParams.Size();
  NCollection_Array1<Geom2dGridEval::CurveD3> aResult(1, aNbParams);

  for (int i = 1; i <= aNbParams; ++i)
  {
    const double aParam = theParams.Value(theParams.Lower() + i - 1);

    const Geom2dGridEval::CurveD3& aBasis = aBasisD3.Value(i);
    gp_Pnt2d                       aP     = aBasis.Point;
    gp_Vec2d                       aD1    = aBasis.D1;
    gp_Vec2d                       aD2    = aBasis.D2;
    gp_Vec2d                       aD3    = aBasis.D3;
    gp_Vec2d                       aD4    = myBasis->DN(aParam, 4);

    // Check for direction change at singular points
    bool isDirectionChange = false;
    if (aD1.SquareMagnitude() <= gp::Resolution())
    {
      isDirectionChange =
        Geom2d_OffsetCurveUtils::AdjustDerivative(*myBasis, 4, aParam, aD1, aD2, aD3, aD4);
    }

    Geom2d_OffsetCurveUtils::CalculateD3(aP,
                                          aD1,
                                          aD2,
                                          aD3,
                                          aD4,
                                          isDirectionChange,
                                          myOffset);
    aResult.ChangeValue(i) = {aP, aD1, aD2, aD3};
  }

  return aResult;
}

//==================================================================================================

NCollection_Array1<gp_Vec2d> Geom2dGridEval_OffsetCurve::EvaluateGridDN(
  const NCollection_Array1<double>& theParams,
  int                               theN) const
{
  if (myBasis.IsNull() || theParams.IsEmpty() || theN < 1)
  {
    return NCollection_Array1<gp_Vec2d>();
  }

  const int aNbParams = theParams.Size();

  // Reuse optimized grid evaluators for orders 1-3
  if (theN == 1)
  {
    NCollection_Array1<Geom2dGridEval::CurveD1> aD1Grid = EvaluateGridD1(theParams);
    NCollection_Array1<gp_Vec2d>                aResult(1, aNbParams);
    for (int i = 1; i <= aNbParams; ++i)
    {
      aResult.SetValue(i, aD1Grid.Value(i).D1);
    }
    return aResult;
  }
  else if (theN == 2)
  {
    NCollection_Array1<Geom2dGridEval::CurveD2> aD2Grid = EvaluateGridD2(theParams);
    NCollection_Array1<gp_Vec2d>                aResult(1, aNbParams);
    for (int i = 1; i <= aNbParams; ++i)
    {
      aResult.SetValue(i, aD2Grid.Value(i).D2);
    }
    return aResult;
  }
  else if (theN == 3)
  {
    NCollection_Array1<Geom2dGridEval::CurveD3> aD3Grid = EvaluateGridD3(theParams);
    NCollection_Array1<gp_Vec2d>                aResult(1, aNbParams);
    for (int i = 1; i <= aNbParams; ++i)
    {
      aResult.SetValue(i, aD3Grid.Value(i).D3);
    }
    return aResult;
  }
  else
  {
    // For orders > 3, batch evaluate basis curve DN
    Geom2dGridEval_Curve aBasisEval;
    aBasisEval.Initialize(myBasis);
    return aBasisEval.EvaluateGridDN(theParams, theN);
  }
}
