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

#include <GeomGridEval_BezierCurve.hxx>

#include <BSplCLib.hxx>
#include <gp_Pnt.hxx>
#include <TColgp_Array1OfPnt.hxx>
#include <TColStd_Array1OfReal.hxx>

//==================================================================================================

void GeomGridEval_BezierCurve::SetParams(const TColStd_Array1OfReal& theParams)
{
  const int aNb = theParams.Size();
  myParams.Resize(1, aNb, false);
  for (int i = 1; i <= aNb; ++i)
  {
    myParams.SetValue(i, theParams.Value(theParams.Lower() + i - 1));
  }
}

//==================================================================================================

NCollection_Array1<gp_Pnt> GeomGridEval_BezierCurve::EvaluateGrid() const
{
  if (myGeom.IsNull() || myParams.IsEmpty())
  {
    return NCollection_Array1<gp_Pnt>();
  }

  const int                  aNb = myParams.Size();
  NCollection_Array1<gp_Pnt> aResult(1, aNb);

  const TColgp_Array1OfPnt&   aPoles = myGeom->Poles();
  const TColStd_Array1OfReal* aW     = myGeom->Weights();

  for (int i = 1; i <= aNb; ++i)
  {
    gp_Pnt aP;
    BSplCLib::BezierD0(myParams.Value(i), aPoles, aW, aP);
    aResult.SetValue(i, aP);
  }
  return aResult;
}

//==================================================================================================

NCollection_Array1<GeomGridEval::CurveD1> GeomGridEval_BezierCurve::EvaluateGridD1() const
{
  if (myGeom.IsNull() || myParams.IsEmpty())
  {
    return NCollection_Array1<GeomGridEval::CurveD1>();
  }

  const int                                   aNb = myParams.Size();
  NCollection_Array1<GeomGridEval::CurveD1> aResult(1, aNb);

  const TColgp_Array1OfPnt&   aPoles = myGeom->Poles();
  const TColStd_Array1OfReal* aW     = myGeom->Weights();

  for (int i = 1; i <= aNb; ++i)
  {
    gp_Pnt aP;
    gp_Vec aD1;
    BSplCLib::BezierD1(myParams.Value(i), aPoles, aW, aP, aD1);
    aResult.ChangeValue(i) = {aP, aD1};
  }
  return aResult;
}

//==================================================================================================

NCollection_Array1<GeomGridEval::CurveD2> GeomGridEval_BezierCurve::EvaluateGridD2() const
{
  if (myGeom.IsNull() || myParams.IsEmpty())
  {
    return NCollection_Array1<GeomGridEval::CurveD2>();
  }

  const int                                   aNb = myParams.Size();
  NCollection_Array1<GeomGridEval::CurveD2> aResult(1, aNb);

  const TColgp_Array1OfPnt&   aPoles = myGeom->Poles();
  const TColStd_Array1OfReal* aW     = myGeom->Weights();

  for (int i = 1; i <= aNb; ++i)
  {
    gp_Pnt aP;
    gp_Vec aD1, aD2;
    BSplCLib::BezierD2(myParams.Value(i), aPoles, aW, aP, aD1, aD2);
    aResult.ChangeValue(i) = {aP, aD1, aD2};
  }
  return aResult;
}

//==================================================================================================

NCollection_Array1<GeomGridEval::CurveD3> GeomGridEval_BezierCurve::EvaluateGridD3() const
{
  if (myGeom.IsNull() || myParams.IsEmpty())
  {
    return NCollection_Array1<GeomGridEval::CurveD3>();
  }

  const int                                   aNb = myParams.Size();
  NCollection_Array1<GeomGridEval::CurveD3> aResult(1, aNb);

  const TColgp_Array1OfPnt&   aPoles = myGeom->Poles();
  const TColStd_Array1OfReal* aW     = myGeom->Weights();

  for (int i = 1; i <= aNb; ++i)
  {
    gp_Pnt aP;
    gp_Vec aD1, aD2, aD3;
    BSplCLib::BezierD3(myParams.Value(i), aPoles, aW, aP, aD1, aD2, aD3);
    aResult.ChangeValue(i) = {aP, aD1, aD2, aD3};
  }
  return aResult;
}
