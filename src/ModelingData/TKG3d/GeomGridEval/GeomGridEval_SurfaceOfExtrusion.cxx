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

#include <Geom_ExtrusionUtils.pxx>

//==================================================================================================

GeomGridEval_SurfaceOfExtrusion::GeomGridEval_SurfaceOfExtrusion(
  const occ::handle<Geom_SurfaceOfLinearExtrusion>& theExtrusion)
    : myGeom(theExtrusion)
{
  if (!myGeom.IsNull())
  {
    myBasisCurve = myGeom->BasisCurve();
    myDirection  = myGeom->Direction();
  }
}

//==================================================================================================

NCollection_Array2<gp_Pnt> GeomGridEval_SurfaceOfExtrusion::EvaluateGrid(
  const NCollection_Array1<double>& theUParams,
  const NCollection_Array1<double>& theVParams) const
{
  if (myBasisCurve.IsNull() || theUParams.IsEmpty() || theVParams.IsEmpty())
  {
    return NCollection_Array2<gp_Pnt>();
  }

  const int aNbU = theUParams.Size();
  const int aNbV = theVParams.Size();

  // Batch evaluate curve points using optimized curve evaluator
  GeomGridEval_Curve aCurveEval;
  aCurveEval.Initialize(myBasisCurve);

  NCollection_Array1<gp_Pnt> aCurvePoints = aCurveEval.EvaluateGrid(theUParams);
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
    const gp_Pnt& aCurvePt = aCurvePoints.Value(i);
    for (int j = 1; j <= aNbV; ++j)
    {
      const double aV = theVParams.Value(theVParams.Lower() + j - 1);
      gp_Pnt       aP;
      Geom_ExtrusionUtils::CalculateD0(aCurvePt, aV, aDirXYZ, aP);
      aResult.SetValue(i, j, aP);
    }
  }

  return aResult;
}

//==================================================================================================

NCollection_Array2<GeomGridEval::SurfD1> GeomGridEval_SurfaceOfExtrusion::EvaluateGridD1(
  const NCollection_Array1<double>& theUParams,
  const NCollection_Array1<double>& theVParams) const
{
  if (myBasisCurve.IsNull() || theUParams.IsEmpty() || theVParams.IsEmpty())
  {
    return NCollection_Array2<GeomGridEval::SurfD1>();
  }

  const int aNbU = theUParams.Size();
  const int aNbV = theVParams.Size();

  // Batch evaluate curve D1 using optimized curve evaluator
  GeomGridEval_Curve aCurveEval;
  aCurveEval.Initialize(myBasisCurve);

  NCollection_Array1<GeomGridEval::CurveD1> aCurveD1 = aCurveEval.EvaluateGridD1(theUParams);
  if (aCurveD1.IsEmpty())
  {
    return NCollection_Array2<GeomGridEval::SurfD1>();
  }

  // For extrusion: D1U = C'(u), D1V = Direction (constant)
  const gp_XYZ                             aDirXYZ = myDirection.XYZ();
  NCollection_Array2<GeomGridEval::SurfD1> aResult(1, aNbU, 1, aNbV);

  for (int i = 1; i <= aNbU; ++i)
  {
    const GeomGridEval::CurveD1& aCurveData = aCurveD1.Value(i);

    for (int j = 1; j <= aNbV; ++j)
    {
      const double aV = theVParams.Value(theVParams.Lower() + j - 1);
      gp_Pnt       aP;
      gp_Vec       aD1U, aD1V;
      Geom_ExtrusionUtils::CalculateD1(aCurveData.Point,
                                       aCurveData.D1,
                                       aV,
                                       aDirXYZ,
                                       aP,
                                       aD1U,
                                       aD1V);
      aResult.ChangeValue(i, j) = {aP, aD1U, aD1V};
    }
  }

  return aResult;
}

//==================================================================================================

NCollection_Array2<GeomGridEval::SurfD2> GeomGridEval_SurfaceOfExtrusion::EvaluateGridD2(
  const NCollection_Array1<double>& theUParams,
  const NCollection_Array1<double>& theVParams) const
{
  if (myBasisCurve.IsNull() || theUParams.IsEmpty() || theVParams.IsEmpty())
  {
    return NCollection_Array2<GeomGridEval::SurfD2>();
  }

  const int aNbU = theUParams.Size();
  const int aNbV = theVParams.Size();

  // Batch evaluate curve D2 using optimized curve evaluator
  GeomGridEval_Curve aCurveEval;
  aCurveEval.Initialize(myBasisCurve);

  NCollection_Array1<GeomGridEval::CurveD2> aCurveD2 = aCurveEval.EvaluateGridD2(theUParams);
  if (aCurveD2.IsEmpty())
  {
    return NCollection_Array2<GeomGridEval::SurfD2>();
  }

  // For extrusion: D1U = C'(u), D1V = Direction
  // D2U = C''(u), D2V = 0, D2UV = 0
  const gp_XYZ                             aDirXYZ = myDirection.XYZ();
  NCollection_Array2<GeomGridEval::SurfD2> aResult(1, aNbU, 1, aNbV);

  for (int i = 1; i <= aNbU; ++i)
  {
    const GeomGridEval::CurveD2& aCurveData = aCurveD2.Value(i);

    for (int j = 1; j <= aNbV; ++j)
    {
      const double aV = theVParams.Value(theVParams.Lower() + j - 1);
      gp_Pnt       aP;
      gp_Vec       aD1U, aD1V, aD2U, aD2V, aD2UV;
      Geom_ExtrusionUtils::CalculateD2(aCurveData.Point,
                                       aCurveData.D1,
                                       aCurveData.D2,
                                       aV,
                                       aDirXYZ,
                                       aP,
                                       aD1U,
                                       aD1V,
                                       aD2U,
                                       aD2V,
                                       aD2UV);
      aResult.ChangeValue(i, j) = {aP, aD1U, aD1V, aD2U, aD2V, aD2UV};
    }
  }

  return aResult;
}

//==================================================================================================

NCollection_Array2<GeomGridEval::SurfD3> GeomGridEval_SurfaceOfExtrusion::EvaluateGridD3(
  const NCollection_Array1<double>& theUParams,
  const NCollection_Array1<double>& theVParams) const
{
  if (myBasisCurve.IsNull() || theUParams.IsEmpty() || theVParams.IsEmpty())
  {
    return NCollection_Array2<GeomGridEval::SurfD3>();
  }

  const int aNbU = theUParams.Size();
  const int aNbV = theVParams.Size();

  // Batch evaluate curve D3 using optimized curve evaluator
  GeomGridEval_Curve aCurveEval;
  aCurveEval.Initialize(myBasisCurve);

  NCollection_Array1<GeomGridEval::CurveD3> aCurveD3 = aCurveEval.EvaluateGridD3(theUParams);
  if (aCurveD3.IsEmpty())
  {
    return NCollection_Array2<GeomGridEval::SurfD3>();
  }

  // For extrusion: D1U = C'(u), D1V = Direction
  // D2U = C''(u), D2V = 0, D2UV = 0
  // D3U = C'''(u), D3V = 0, D3UUV = 0, D3UVV = 0
  const gp_XYZ                             aDirXYZ = myDirection.XYZ();
  NCollection_Array2<GeomGridEval::SurfD3> aResult(1, aNbU, 1, aNbV);

  for (int i = 1; i <= aNbU; ++i)
  {
    const GeomGridEval::CurveD3& aCurveData = aCurveD3.Value(i);

    for (int j = 1; j <= aNbV; ++j)
    {
      const double aV = theVParams.Value(theVParams.Lower() + j - 1);
      gp_Pnt       aP;
      gp_Vec       aD1U, aD1V, aD2U, aD2V, aD2UV, aD3U, aD3V, aD3UUV, aD3UVV;
      Geom_ExtrusionUtils::CalculateD3(aCurveData.Point,
                                       aCurveData.D1,
                                       aCurveData.D2,
                                       aCurveData.D3,
                                       aV,
                                       aDirXYZ,
                                       aP,
                                       aD1U,
                                       aD1V,
                                       aD2U,
                                       aD2V,
                                       aD2UV,
                                       aD3U,
                                       aD3V,
                                       aD3UUV,
                                       aD3UVV);
      aResult.ChangeValue(i, j) = {aP, aD1U, aD1V, aD2U, aD2V, aD2UV, aD3U, aD3V, aD3UUV, aD3UVV};
    }
  }

  return aResult;
}

//==================================================================================================

NCollection_Array2<gp_Vec> GeomGridEval_SurfaceOfExtrusion::EvaluateGridDN(
  const NCollection_Array1<double>& theUParams,
  const NCollection_Array1<double>& theVParams,
  int                               theNU,
  int                               theNV) const
{
  if (myBasisCurve.IsNull() || theUParams.IsEmpty() || theVParams.IsEmpty() || theNU < 0
      || theNV < 0 || (theNU + theNV) < 1)
  {
    return NCollection_Array2<gp_Vec>();
  }

  const int aNbU = theUParams.Size();
  const int aNbV = theVParams.Size();

  NCollection_Array2<gp_Vec> aResult(1, aNbU, 1, aNbV);

  const gp_XYZ aDirXYZ = myDirection.XYZ();

  // For theNV == 0: need curve derivative, otherwise use CalculateDN directly
  if (theNV == 0)
  {
    // Pure U derivative = curve derivative
    GeomGridEval_Curve aCurveEval;
    aCurveEval.Initialize(myBasisCurve);

    NCollection_Array1<gp_Vec> aCurveDN = aCurveEval.EvaluateGridDN(theUParams, theNU);

    for (int i = 1; i <= aNbU; ++i)
    {
      const gp_Vec aDN = Geom_ExtrusionUtils::CalculateDN(aCurveDN.Value(i), aDirXYZ, theNU, theNV);
      for (int j = 1; j <= aNbV; ++j)
      {
        aResult.SetValue(i, j, aDN);
      }
    }
  }
  else
  {
    // For theNV >= 1: result doesn't depend on curve, use CalculateDN with zero vector
    const gp_Vec aDN = Geom_ExtrusionUtils::CalculateDN(gp_Vec(), aDirXYZ, theNU, theNV);
    for (int i = 1; i <= aNbU; ++i)
    {
      for (int j = 1; j <= aNbV; ++j)
      {
        aResult.SetValue(i, j, aDN);
      }
    }
  }

  return aResult;
}
