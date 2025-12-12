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

#include <GeomGridEval_SurfaceOfExtrusion.hxx>

//==================================================================================================

GeomGridEval_SurfaceOfExtrusion::GeomGridEval_SurfaceOfExtrusion(
  const Handle(Geom_SurfaceOfLinearExtrusion)& theExtrusion)
    : myGeom(theExtrusion)
{
  if (!myGeom.IsNull())
  {
    myBasisCurve = myGeom->BasisCurve();
    myDirection  = myGeom->Direction();
  }
}

//==================================================================================================

void GeomGridEval_SurfaceOfExtrusion::SetUVParams(const TColStd_Array1OfReal& theUParams,
                                                  const TColStd_Array1OfReal& theVParams)
{
  const int aNbU = theUParams.Size();
  const int aNbV = theVParams.Size();

  myUParams.Resize(1, aNbU, false);
  for (int i = 1; i <= aNbU; ++i)
  {
    myUParams.SetValue(i, theUParams.Value(theUParams.Lower() + i - 1));
  }

  myVParams.Resize(1, aNbV, false);
  for (int j = 1; j <= aNbV; ++j)
  {
    myVParams.SetValue(j, theVParams.Value(theVParams.Lower() + j - 1));
  }
}

//==================================================================================================

NCollection_Array2<gp_Pnt> GeomGridEval_SurfaceOfExtrusion::EvaluateGrid() const
{
  if (myBasisCurve.IsNull() || myUParams.IsEmpty() || myVParams.IsEmpty())
  {
    return NCollection_Array2<gp_Pnt>();
  }

  const int aNbU = myUParams.Size();
  const int aNbV = myVParams.Size();

  // Batch evaluate curve points using optimized curve evaluator
  GeomGridEval_Curve aCurveEval;
  aCurveEval.Initialize(myBasisCurve);
  aCurveEval.SetParams(myUParams);

  NCollection_Array1<gp_Pnt> aCurvePoints = aCurveEval.EvaluateGrid();
  if (aCurvePoints.IsEmpty())
  {
    return NCollection_Array2<gp_Pnt>();
  }

  // For each curve point, compute surface points for all V values
  // S(u, v) = C(u) + v * Direction
  const gp_XYZ               aDirXYZ = myDirection.XYZ();
  NCollection_Array2<gp_Pnt> aResult(1, aNbU, 1, aNbV);

  for (int i = 1; i <= aNbU; ++i)
  {
    const gp_XYZ& aCurvePt = aCurvePoints.Value(i).XYZ();
    for (int j = 1; j <= aNbV; ++j)
    {
      const double aV = myVParams.Value(j);
      aResult.SetValue(i, j, gp_Pnt(aCurvePt + aV * aDirXYZ));
    }
  }

  return aResult;
}

//==================================================================================================

NCollection_Array2<GeomGridEval::SurfD1> GeomGridEval_SurfaceOfExtrusion::EvaluateGridD1() const
{
  if (myBasisCurve.IsNull() || myUParams.IsEmpty() || myVParams.IsEmpty())
  {
    return NCollection_Array2<GeomGridEval::SurfD1>();
  }

  const int aNbU = myUParams.Size();
  const int aNbV = myVParams.Size();

  // Batch evaluate curve D1 using optimized curve evaluator
  GeomGridEval_Curve aCurveEval;
  aCurveEval.Initialize(myBasisCurve);
  aCurveEval.SetParams(myUParams);

  NCollection_Array1<GeomGridEval::CurveD1> aCurveD1 = aCurveEval.EvaluateGridD1();
  if (aCurveD1.IsEmpty())
  {
    return NCollection_Array2<GeomGridEval::SurfD1>();
  }

  // For extrusion: D1U = C'(u), D1V = Direction (constant)
  const gp_XYZ                             aDirXYZ = myDirection.XYZ();
  const gp_Vec                             aD1V(myDirection);
  NCollection_Array2<GeomGridEval::SurfD1> aResult(1, aNbU, 1, aNbV);

  for (int i = 1; i <= aNbU; ++i)
  {
    const GeomGridEval::CurveD1& aCurveData = aCurveD1.Value(i);
    const gp_XYZ&                aCurvePt   = aCurveData.Point.XYZ();
    const gp_Vec&                aD1U       = aCurveData.D1;

    for (int j = 1; j <= aNbV; ++j)
    {
      const double aV           = myVParams.Value(j);
      aResult.ChangeValue(i, j) = {gp_Pnt(aCurvePt + aV * aDirXYZ), aD1U, aD1V};
    }
  }

  return aResult;
}

//==================================================================================================

NCollection_Array2<GeomGridEval::SurfD2> GeomGridEval_SurfaceOfExtrusion::EvaluateGridD2() const
{
  if (myBasisCurve.IsNull() || myUParams.IsEmpty() || myVParams.IsEmpty())
  {
    return NCollection_Array2<GeomGridEval::SurfD2>();
  }

  const int aNbU = myUParams.Size();
  const int aNbV = myVParams.Size();

  // Batch evaluate curve D2 using optimized curve evaluator
  GeomGridEval_Curve aCurveEval;
  aCurveEval.Initialize(myBasisCurve);
  aCurveEval.SetParams(myUParams);

  NCollection_Array1<GeomGridEval::CurveD2> aCurveD2 = aCurveEval.EvaluateGridD2();
  if (aCurveD2.IsEmpty())
  {
    return NCollection_Array2<GeomGridEval::SurfD2>();
  }

  // For extrusion: D1U = C'(u), D1V = Direction
  // D2U = C''(u), D2V = 0, D2UV = 0
  const gp_XYZ                             aDirXYZ = myDirection.XYZ();
  const gp_Vec                             aD1V(myDirection);
  const gp_Vec                             aZero(0.0, 0.0, 0.0);
  NCollection_Array2<GeomGridEval::SurfD2> aResult(1, aNbU, 1, aNbV);

  for (int i = 1; i <= aNbU; ++i)
  {
    const GeomGridEval::CurveD2& aCurveData = aCurveD2.Value(i);
    const gp_XYZ&                aCurvePt   = aCurveData.Point.XYZ();
    const gp_Vec&                aD1U       = aCurveData.D1;
    const gp_Vec&                aD2U       = aCurveData.D2;

    for (int j = 1; j <= aNbV; ++j)
    {
      const double aV           = myVParams.Value(j);
      aResult.ChangeValue(i, j) = {gp_Pnt(aCurvePt + aV * aDirXYZ), aD1U, aD1V, aD2U, aZero, aZero};
    }
  }

  return aResult;
}

//==================================================================================================

NCollection_Array2<GeomGridEval::SurfD3> GeomGridEval_SurfaceOfExtrusion::EvaluateGridD3() const
{
  if (myBasisCurve.IsNull() || myUParams.IsEmpty() || myVParams.IsEmpty())
  {
    return NCollection_Array2<GeomGridEval::SurfD3>();
  }

  const int aNbU = myUParams.Size();
  const int aNbV = myVParams.Size();

  // Batch evaluate curve D3 using optimized curve evaluator
  GeomGridEval_Curve aCurveEval;
  aCurveEval.Initialize(myBasisCurve);
  aCurveEval.SetParams(myUParams);

  NCollection_Array1<GeomGridEval::CurveD3> aCurveD3 = aCurveEval.EvaluateGridD3();
  if (aCurveD3.IsEmpty())
  {
    return NCollection_Array2<GeomGridEval::SurfD3>();
  }

  // For extrusion: D1U = C'(u), D1V = Direction
  // D2U = C''(u), D2V = 0, D2UV = 0
  // D3U = C'''(u), D3V = 0, D3UUV = 0, D3UVV = 0
  const gp_XYZ                             aDirXYZ = myDirection.XYZ();
  const gp_Vec                             aD1V(myDirection);
  const gp_Vec                             aZero(0.0, 0.0, 0.0);
  NCollection_Array2<GeomGridEval::SurfD3> aResult(1, aNbU, 1, aNbV);

  for (int i = 1; i <= aNbU; ++i)
  {
    const GeomGridEval::CurveD3& aCurveData = aCurveD3.Value(i);
    const gp_XYZ&                aCurvePt   = aCurveData.Point.XYZ();
    const gp_Vec&                aD1U       = aCurveData.D1;
    const gp_Vec&                aD2U       = aCurveData.D2;
    const gp_Vec&                aD3U       = aCurveData.D3;

    for (int j = 1; j <= aNbV; ++j)
    {
      const double aV           = myVParams.Value(j);
      aResult.ChangeValue(i, j) = {gp_Pnt(aCurvePt + aV * aDirXYZ),
                                   aD1U,
                                   aD1V,
                                   aD2U,
                                   aZero,
                                   aZero,
                                   aD3U,
                                   aZero,
                                   aZero,
                                   aZero};
    }
  }

  return aResult;
}

//==================================================================================================

NCollection_Array2<gp_Vec> GeomGridEval_SurfaceOfExtrusion::EvaluateGridDN(int theNU,
                                                                           int theNV) const
{
  if (myBasisCurve.IsNull() || myUParams.IsEmpty() || myVParams.IsEmpty() || theNU < 0 || theNV < 0
      || (theNU + theNV) < 1)
  {
    return NCollection_Array2<gp_Vec>();
  }

  const int aNbU = myUParams.Size();
  const int aNbV = myVParams.Size();

  NCollection_Array2<gp_Vec> aResult(1, aNbU, 1, aNbV);

  // For extrusion surface:
  // - DN with theNV >= 2: always zero (direction is constant)
  // - DN with theNV == 1 and theNU == 0: Direction
  // - DN with theNV == 0: curve derivative C^(theNU)(u)
  if (theNV >= 2)
  {
    // All higher V derivatives are zero
    const gp_Vec aZero(0.0, 0.0, 0.0);
    for (int i = 1; i <= aNbU; ++i)
    {
      for (int j = 1; j <= aNbV; ++j)
      {
        aResult.SetValue(i, j, aZero);
      }
    }
  }
  else if (theNV == 1 && theNU == 0)
  {
    // D0V = Direction
    const gp_Vec aDir(myDirection);
    for (int i = 1; i <= aNbU; ++i)
    {
      for (int j = 1; j <= aNbV; ++j)
      {
        aResult.SetValue(i, j, aDir);
      }
    }
  }
  else if (theNV == 1)
  {
    // Mixed derivatives with theNV == 1 are all zero (direction is constant)
    const gp_Vec aZero(0.0, 0.0, 0.0);
    for (int i = 1; i <= aNbU; ++i)
    {
      for (int j = 1; j <= aNbV; ++j)
      {
        aResult.SetValue(i, j, aZero);
      }
    }
  }
  else
  {
    // theNV == 0: pure U derivative = curve derivative
    GeomGridEval_Curve aCurveEval;
    aCurveEval.Initialize(myBasisCurve);
    aCurveEval.SetParams(myUParams);

    NCollection_Array1<gp_Vec> aCurveDN = aCurveEval.EvaluateGridDN(theNU);

    for (int i = 1; i <= aNbU; ++i)
    {
      const gp_Vec& aDN = aCurveDN.Value(i);
      for (int j = 1; j <= aNbV; ++j)
      {
        aResult.SetValue(i, j, aDN);
      }
    }
  }

  return aResult;
}
