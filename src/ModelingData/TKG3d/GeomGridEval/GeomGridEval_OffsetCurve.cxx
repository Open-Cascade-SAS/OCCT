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

#include <GeomGridEval_OffsetCurve.hxx>

#include <GeomAdaptor_Curve.hxx>
#include <GeomGridEval_Curve.hxx>

namespace
{
//! Tolerance for detecting singular offset direction.
constexpr double THE_OFFSET_TOL = 1e-10;
} // namespace

//==================================================================================================

void GeomGridEval_OffsetCurve::SetParams(const TColStd_Array1OfReal& theParams)
{
  const int aNbParams = theParams.Size();

  myParams.Resize(1, aNbParams, false);
  for (int i = 1; i <= aNbParams; ++i)
  {
    myParams.SetValue(i, theParams.Value(theParams.Lower() + i - 1));
  }
}

//==================================================================================================

NCollection_Array1<gp_Pnt> GeomGridEval_OffsetCurve::EvaluateGrid() const
{
  if (myBasis.IsNull() || myParams.IsEmpty())
  {
    return NCollection_Array1<gp_Pnt>();
  }

  // Offset D0 requires basis D1 to compute offset direction
  GeomGridEval_Curve aBasisEval;
  aBasisEval.Initialize(myBasis);
  aBasisEval.SetParams(myParams);

  NCollection_Array1<GeomGridEval::CurveD1> aBasisD1 = aBasisEval.EvaluateGridD1();
  if (aBasisD1.IsEmpty())
  {
    return NCollection_Array1<gp_Pnt>();
  }

  const int                  aNbParams = myParams.Size();
  NCollection_Array1<gp_Pnt> aResult(1, aNbParams);

  const gp_XYZ aDirXYZ = myDirection.XYZ();

  for (int i = 1; i <= aNbParams; ++i)
  {
    const GeomGridEval::CurveD1& aBasis = aBasisD1.Value(i);

    // Compute offset direction: N = D1 ^ Direction
    gp_Vec aN    = aBasis.D1.Crossed(gp_Vec(aDirXYZ));
    double aNMag = aN.Magnitude();
    gp_Pnt aP    = aBasis.Point;

    if (aNMag > THE_OFFSET_TOL)
    {
      aP.SetXYZ(aP.XYZ() + (myOffset / aNMag) * aN.XYZ());
    }
    aResult.SetValue(i, aP);
  }

  return aResult;
}

//==================================================================================================

NCollection_Array1<GeomGridEval::CurveD1> GeomGridEval_OffsetCurve::EvaluateGridD1() const
{
  if (myGeom.IsNull() || myParams.IsEmpty())
  {
    return NCollection_Array1<GeomGridEval::CurveD1>();
  }

  // Use adaptor for D1 evaluation (offset D1 requires basis D2)
  const int                                aNbParams = myParams.Size();
  NCollection_Array1<GeomGridEval::CurveD1> aResult(1, aNbParams);

  GeomAdaptor_Curve anAdaptor(myGeom);

  for (int i = 1; i <= aNbParams; ++i)
  {
    gp_Pnt aPoint;
    gp_Vec aD1;
    anAdaptor.D1(myParams.Value(i), aPoint, aD1);
    aResult.ChangeValue(i) = {aPoint, aD1};
  }

  return aResult;
}

//==================================================================================================

NCollection_Array1<GeomGridEval::CurveD2> GeomGridEval_OffsetCurve::EvaluateGridD2() const
{
  if (myGeom.IsNull() || myParams.IsEmpty())
  {
    return NCollection_Array1<GeomGridEval::CurveD2>();
  }

  // Use adaptor for D2 evaluation (offset D2 requires basis D3)
  const int                                aNbParams = myParams.Size();
  NCollection_Array1<GeomGridEval::CurveD2> aResult(1, aNbParams);

  GeomAdaptor_Curve anAdaptor(myGeom);

  for (int i = 1; i <= aNbParams; ++i)
  {
    gp_Pnt aPoint;
    gp_Vec aD1, aD2;
    anAdaptor.D2(myParams.Value(i), aPoint, aD1, aD2);
    aResult.ChangeValue(i) = {aPoint, aD1, aD2};
  }

  return aResult;
}

//==================================================================================================

NCollection_Array1<GeomGridEval::CurveD3> GeomGridEval_OffsetCurve::EvaluateGridD3() const
{
  if (myGeom.IsNull() || myParams.IsEmpty())
  {
    return NCollection_Array1<GeomGridEval::CurveD3>();
  }

  // Use adaptor for D3 evaluation
  const int                                aNbParams = myParams.Size();
  NCollection_Array1<GeomGridEval::CurveD3> aResult(1, aNbParams);

  GeomAdaptor_Curve anAdaptor(myGeom);

  for (int i = 1; i <= aNbParams; ++i)
  {
    gp_Pnt aPoint;
    gp_Vec aD1, aD2, aD3;
    anAdaptor.D3(myParams.Value(i), aPoint, aD1, aD2, aD3);
    aResult.ChangeValue(i) = {aPoint, aD1, aD2, aD3};
  }

  return aResult;
}
