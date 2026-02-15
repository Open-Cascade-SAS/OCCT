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

#include <Geom2dGridEval_OtherCurve.hxx>

//==================================================================================================

NCollection_Array1<gp_Pnt2d> Geom2dGridEval_OtherCurve::EvaluateGrid(
  const NCollection_Array1<double>& theParams) const
{
  if (theParams.IsEmpty())
  {
    return NCollection_Array1<gp_Pnt2d>();
  }

  const int                    aNb = theParams.Size();
  NCollection_Array1<gp_Pnt2d> aResult(1, aNb);

  for (int i = theParams.Lower(); i <= theParams.Upper(); ++i)
  {
    const gp_Pnt2d aP = myCurve.get().EvalD0(theParams.Value(i));
    aResult.SetValue(i - theParams.Lower() + 1, aP);
  }

  return aResult;
}

//==================================================================================================

NCollection_Array1<Geom2dGridEval::CurveD1> Geom2dGridEval_OtherCurve::EvaluateGridD1(
  const NCollection_Array1<double>& theParams) const
{
  if (theParams.IsEmpty())
  {
    return NCollection_Array1<Geom2dGridEval::CurveD1>();
  }

  const int                                   aNb = theParams.Size();
  NCollection_Array1<Geom2dGridEval::CurveD1> aResult(1, aNb);

  for (int i = theParams.Lower(); i <= theParams.Upper(); ++i)
  {
    const Geom2d_Curve::ResD1 aD1                  = myCurve.get().EvalD1(theParams.Value(i));
    aResult.ChangeValue(i - theParams.Lower() + 1) = {aD1.Point, aD1.D1};
  }

  return aResult;
}

//==================================================================================================

NCollection_Array1<Geom2dGridEval::CurveD2> Geom2dGridEval_OtherCurve::EvaluateGridD2(
  const NCollection_Array1<double>& theParams) const
{
  if (theParams.IsEmpty())
  {
    return NCollection_Array1<Geom2dGridEval::CurveD2>();
  }

  const int                                   aNb = theParams.Size();
  NCollection_Array1<Geom2dGridEval::CurveD2> aResult(1, aNb);

  for (int i = theParams.Lower(); i <= theParams.Upper(); ++i)
  {
    const Geom2d_Curve::ResD2 aD2                  = myCurve.get().EvalD2(theParams.Value(i));
    aResult.ChangeValue(i - theParams.Lower() + 1) = {aD2.Point, aD2.D1, aD2.D2};
  }

  return aResult;
}

//==================================================================================================

NCollection_Array1<Geom2dGridEval::CurveD3> Geom2dGridEval_OtherCurve::EvaluateGridD3(
  const NCollection_Array1<double>& theParams) const
{
  if (theParams.IsEmpty())
  {
    return NCollection_Array1<Geom2dGridEval::CurveD3>();
  }

  const int                                   aNb = theParams.Size();
  NCollection_Array1<Geom2dGridEval::CurveD3> aResult(1, aNb);

  for (int i = theParams.Lower(); i <= theParams.Upper(); ++i)
  {
    const Geom2d_Curve::ResD3 aD3                  = myCurve.get().EvalD3(theParams.Value(i));
    aResult.ChangeValue(i - theParams.Lower() + 1) = {aD3.Point, aD3.D1, aD3.D2, aD3.D3};
  }

  return aResult;
}

//==================================================================================================

NCollection_Array1<gp_Vec2d> Geom2dGridEval_OtherCurve::EvaluateGridDN(
  const NCollection_Array1<double>& theParams,
  int                               theN) const
{
  if (theParams.IsEmpty() || theN < 1)
  {
    return NCollection_Array1<gp_Vec2d>();
  }

  const int                    aNb = theParams.Size();
  NCollection_Array1<gp_Vec2d> aResult(1, aNb);

  // Reuse existing grid evaluators for orders 1-3
  if (theN == 1)
  {
    NCollection_Array1<Geom2dGridEval::CurveD1> aD1Grid = EvaluateGridD1(theParams);
    if (aD1Grid.IsEmpty())
    {
      return NCollection_Array1<gp_Vec2d>();
    }
    for (int i = 1; i <= aNb; ++i)
    {
      aResult.SetValue(i, aD1Grid.Value(i).D1);
    }
  }
  else if (theN == 2)
  {
    NCollection_Array1<Geom2dGridEval::CurveD2> aD2Grid = EvaluateGridD2(theParams);
    if (aD2Grid.IsEmpty())
    {
      return NCollection_Array1<gp_Vec2d>();
    }
    for (int i = 1; i <= aNb; ++i)
    {
      aResult.SetValue(i, aD2Grid.Value(i).D2);
    }
  }
  else if (theN == 3)
  {
    NCollection_Array1<Geom2dGridEval::CurveD3> aD3Grid = EvaluateGridD3(theParams);
    if (aD3Grid.IsEmpty())
    {
      return NCollection_Array1<gp_Vec2d>();
    }
    for (int i = 1; i <= aNb; ++i)
    {
      aResult.SetValue(i, aD3Grid.Value(i).D3);
    }
  }
  else
  {
    // For orders > 3, use adaptor EvalDN method
    for (int i = theParams.Lower(); i <= theParams.Upper(); ++i)
    {
      const gp_Vec2d aDN = myCurve.get().EvalDN(theParams.Value(i), theN);
      aResult.SetValue(i - theParams.Lower() + 1, aDN);
    }
  }
  return aResult;
}
