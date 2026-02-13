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
    aResult.SetValue(i - theParams.Lower() + 1, myCurve.get().Value(theParams.Value(i)));
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
    gp_Pnt2d aPoint;
    gp_Vec2d aD1;
    myCurve.get().D1(theParams.Value(i), aPoint, aD1);
    aResult.ChangeValue(i - theParams.Lower() + 1) = {aPoint, aD1};
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
    gp_Pnt2d aPoint;
    gp_Vec2d aD1, aD2;
    myCurve.get().D2(theParams.Value(i), aPoint, aD1, aD2);
    aResult.ChangeValue(i - theParams.Lower() + 1) = {aPoint, aD1, aD2};
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
    gp_Pnt2d aPoint;
    gp_Vec2d aD1, aD2, aD3;
    myCurve.get().D3(theParams.Value(i), aPoint, aD1, aD2, aD3);
    aResult.ChangeValue(i - theParams.Lower() + 1) = {aPoint, aD1, aD2, aD3};
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
    for (int i = 1; i <= aNb; ++i)
    {
      aResult.SetValue(i, aD1Grid.Value(i).D1);
    }
  }
  else if (theN == 2)
  {
    NCollection_Array1<Geom2dGridEval::CurveD2> aD2Grid = EvaluateGridD2(theParams);
    for (int i = 1; i <= aNb; ++i)
    {
      aResult.SetValue(i, aD2Grid.Value(i).D2);
    }
  }
  else if (theN == 3)
  {
    NCollection_Array1<Geom2dGridEval::CurveD3> aD3Grid = EvaluateGridD3(theParams);
    for (int i = 1; i <= aNb; ++i)
    {
      aResult.SetValue(i, aD3Grid.Value(i).D3);
    }
  }
  else
  {
    // For orders > 3, use adaptor DN method
    for (int i = theParams.Lower(); i <= theParams.Upper(); ++i)
    {
      aResult.SetValue(i - theParams.Lower() + 1, myCurve.get().DN(theParams.Value(i), theN));
    }
  }
  return aResult;
}
