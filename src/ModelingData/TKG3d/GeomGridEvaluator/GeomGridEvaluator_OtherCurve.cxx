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

#include <GeomGridEvaluator_OtherCurve.hxx>

//==================================================================================================

void GeomGridEvaluator_OtherCurve::SetParams(const TColStd_Array1OfReal& theParams)
{
  myParams.Resize(theParams.Lower(), theParams.Upper(), false);
  for (int i = theParams.Lower(); i <= theParams.Upper(); ++i)
  {
    myParams.SetValue(i, theParams.Value(i));
  }
}

//==================================================================================================

NCollection_Array1<gp_Pnt> GeomGridEvaluator_OtherCurve::EvaluateGrid() const
{
  if (myCurve.IsNull() || myParams.IsEmpty())
  {
    return NCollection_Array1<gp_Pnt>();
  }

  NCollection_Array1<gp_Pnt> aResult(myParams.Lower(), myParams.Upper());

  for (int i = myParams.Lower(); i <= myParams.Upper(); ++i)
  {
    aResult.SetValue(i, myCurve->Value(myParams.Value(i)));
  }

  return aResult;
}

//==================================================================================================

NCollection_Array1<GeomGridEval::CurveD1> GeomGridEvaluator_OtherCurve::EvaluateGridD1() const
{
  if (myCurve.IsNull() || myParams.IsEmpty())
  {
    return NCollection_Array1<GeomGridEval::CurveD1>();
  }

  NCollection_Array1<GeomGridEval::CurveD1> aResult(myParams.Lower(), myParams.Upper());

  for (int i = myParams.Lower(); i <= myParams.Upper(); ++i)
  {
    gp_Pnt aPoint;
    gp_Vec aD1;
    myCurve->D1(myParams.Value(i), aPoint, aD1);
    aResult.ChangeValue(i) = {aPoint, aD1};
  }

  return aResult;
}

//==================================================================================================

NCollection_Array1<GeomGridEval::CurveD2> GeomGridEvaluator_OtherCurve::EvaluateGridD2() const
{
  if (myCurve.IsNull() || myParams.IsEmpty())
  {
    return NCollection_Array1<GeomGridEval::CurveD2>();
  }

  NCollection_Array1<GeomGridEval::CurveD2> aResult(myParams.Lower(), myParams.Upper());

  for (int i = myParams.Lower(); i <= myParams.Upper(); ++i)
  {
    gp_Pnt aPoint;
    gp_Vec aD1, aD2;
    myCurve->D2(myParams.Value(i), aPoint, aD1, aD2);
    aResult.ChangeValue(i) = {aPoint, aD1, aD2};
  }

  return aResult;
}

//==================================================================================================

NCollection_Array1<GeomGridEval::CurveD3> GeomGridEvaluator_OtherCurve::EvaluateGridD3() const
{
  if (myCurve.IsNull() || myParams.IsEmpty())
  {
    return NCollection_Array1<GeomGridEval::CurveD3>();
  }

  NCollection_Array1<GeomGridEval::CurveD3> aResult(myParams.Lower(), myParams.Upper());

  for (int i = myParams.Lower(); i <= myParams.Upper(); ++i)
  {
    gp_Pnt aPoint;
    gp_Vec aD1, aD2, aD3;
    myCurve->D3(myParams.Value(i), aPoint, aD1, aD2, aD3);
    aResult.ChangeValue(i) = {aPoint, aD1, aD2, aD3};
  }

  return aResult;
}
