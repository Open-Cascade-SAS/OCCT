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

#include <GeomGridEval_SurfaceOfRevolution.hxx>

#include <Precision.hxx>

#include <cmath>

namespace
{
//! Rotate a point around an axis using Rodrigues' rotation formula.
//! More efficient than gp_Trsf for single point rotation.
//! @param thePoint point to rotate (relative to axis location)
//! @param theAxisDir axis direction (unit vector)
//! @param theCos cosine of rotation angle
//! @param theSin sine of rotation angle
//! @return rotated point coordinates
inline gp_XYZ rotatePoint(const gp_XYZ& thePoint,
                          const gp_XYZ& theAxisDir,
                          double        theCos,
                          double        theSin)
{
  // Rodrigues' rotation formula:
  // v' = v*cos(θ) + (k×v)*sin(θ) + k*(k·v)*(1-cos(θ))
  const double aKDotV   = theAxisDir.Dot(thePoint);
  gp_XYZ       aKCrossV = theAxisDir.Crossed(thePoint);
  return thePoint * theCos + aKCrossV * theSin + theAxisDir * aKDotV * (1.0 - theCos);
}

//! Rotate a vector around an axis using Rodrigues' rotation formula.
inline gp_Vec rotateVec(const gp_Vec& theVec,
                        const gp_XYZ& theAxisDir,
                        double        theCos,
                        double        theSin)
{
  return gp_Vec(rotatePoint(theVec.XYZ(), theAxisDir, theCos, theSin));
}

} // namespace

//==================================================================================================

GeomGridEval_SurfaceOfRevolution::GeomGridEval_SurfaceOfRevolution(
  const Handle(Geom_SurfaceOfRevolution)& theRevolution)
    : myGeom(theRevolution)
{
  if (!myGeom.IsNull())
  {
    myBasisCurve    = myGeom->BasisCurve();
    myAxis          = myGeom->Axis();
    myAxisLocation  = myAxis.Location();
    myAxisDirection = myAxis.Direction();
  }
}

//==================================================================================================

void GeomGridEval_SurfaceOfRevolution::SetUVParams(const TColStd_Array1OfReal& theUParams,
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

NCollection_Array2<gp_Pnt> GeomGridEval_SurfaceOfRevolution::EvaluateGrid() const
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
  aCurveEval.SetParams(myVParams);

  NCollection_Array1<gp_Pnt> aCurvePoints = aCurveEval.EvaluateGrid();
  if (aCurvePoints.IsEmpty())
  {
    return NCollection_Array2<gp_Pnt>();
  }

  // Precompute sin/cos for all U values
  NCollection_Array1<double> aCosU(1, aNbU);
  NCollection_Array1<double> aSinU(1, aNbU);
  for (int i = 1; i <= aNbU; ++i)
  {
    const double aU = myUParams.Value(i);
    aCosU.SetValue(i, std::cos(aU));
    aSinU.SetValue(i, std::sin(aU));
  }

  const gp_XYZ&              aAxisDir = myAxisDirection.XYZ();
  const gp_XYZ&              aAxisLoc = myAxisLocation.XYZ();
  NCollection_Array2<gp_Pnt> aResult(1, aNbU, 1, aNbV);

  for (int j = 1; j <= aNbV; ++j)
  {
    // Vector from axis location to curve point (before rotation)
    const gp_XYZ aCQ = aCurvePoints.Value(j).XYZ() - aAxisLoc;

    for (int i = 1; i <= aNbU; ++i)
    {
      // Rotate the point around the axis
      const gp_XYZ aRotatedPt = rotatePoint(aCQ, aAxisDir, aCosU.Value(i), aSinU.Value(i));
      aResult.SetValue(i, j, gp_Pnt(aRotatedPt + aAxisLoc));
    }
  }

  return aResult;
}

//==================================================================================================

NCollection_Array2<GeomGridEval::SurfD1> GeomGridEval_SurfaceOfRevolution::EvaluateGridD1() const
{
  if (myBasisCurve.IsNull() || myUParams.IsEmpty() || myVParams.IsEmpty())
  {
    return NCollection_Array2<GeomGridEval::SurfD1>();
  }

  const int aNbU = myUParams.Size();
  const int aNbV = myVParams.Size();

  // Batch evaluate curve D1
  GeomGridEval_Curve aCurveEval;
  aCurveEval.Initialize(myBasisCurve);
  aCurveEval.SetParams(myVParams);

  NCollection_Array1<GeomGridEval::CurveD1> aCurveD1 = aCurveEval.EvaluateGridD1();
  if (aCurveD1.IsEmpty())
  {
    return NCollection_Array2<GeomGridEval::SurfD1>();
  }

  // Precompute sin/cos
  NCollection_Array1<double> aCosU(1, aNbU);
  NCollection_Array1<double> aSinU(1, aNbU);
  for (int i = 1; i <= aNbU; ++i)
  {
    const double aU = myUParams.Value(i);
    aCosU.SetValue(i, std::cos(aU));
    aSinU.SetValue(i, std::sin(aU));
  }

  const gp_XYZ&                            aAxisDir = myAxisDirection.XYZ();
  const gp_XYZ&                            aAxisLoc = myAxisLocation.XYZ();
  NCollection_Array2<GeomGridEval::SurfD1> aResult(1, aNbU, 1, aNbV);

  for (int j = 1; j <= aNbV; ++j)
  {
    const GeomGridEval::CurveD1& aCurveData = aCurveD1.Value(j);
    const gp_XYZ                 aCQ        = aCurveData.Point.XYZ() - aAxisLoc;

    // D1U (before rotation) = Axis × (P - AxisLoc)
    gp_Vec aD1U_unrot(aAxisDir.Crossed(aCQ));
    if (aD1U_unrot.SquareMagnitude() < Precision::SquareConfusion())
    {
      aD1U_unrot.SetCoord(0.0, 0.0, 0.0);
    }

    // D1V (before rotation) = C'(v)
    const gp_Vec& aD1V_unrot = aCurveData.D1;

    for (int i = 1; i <= aNbU; ++i)
    {
      const double aCos = aCosU.Value(i);
      const double aSin = aSinU.Value(i);

      // Rotate all vectors
      const gp_XYZ aRotatedPt = rotatePoint(aCQ, aAxisDir, aCos, aSin);
      const gp_Vec aD1U       = rotateVec(aD1U_unrot, aAxisDir, aCos, aSin);
      const gp_Vec aD1V       = rotateVec(aD1V_unrot, aAxisDir, aCos, aSin);

      aResult.ChangeValue(i, j) = {gp_Pnt(aRotatedPt + aAxisLoc), aD1U, aD1V};
    }
  }

  return aResult;
}

//==================================================================================================

NCollection_Array2<GeomGridEval::SurfD2> GeomGridEval_SurfaceOfRevolution::EvaluateGridD2() const
{
  if (myBasisCurve.IsNull() || myUParams.IsEmpty() || myVParams.IsEmpty())
  {
    return NCollection_Array2<GeomGridEval::SurfD2>();
  }

  const int aNbU = myUParams.Size();
  const int aNbV = myVParams.Size();

  // Batch evaluate curve D2
  GeomGridEval_Curve aCurveEval;
  aCurveEval.Initialize(myBasisCurve);
  aCurveEval.SetParams(myVParams);

  NCollection_Array1<GeomGridEval::CurveD2> aCurveD2 = aCurveEval.EvaluateGridD2();
  if (aCurveD2.IsEmpty())
  {
    return NCollection_Array2<GeomGridEval::SurfD2>();
  }

  // Precompute sin/cos
  NCollection_Array1<double> aCosU(1, aNbU);
  NCollection_Array1<double> aSinU(1, aNbU);
  for (int i = 1; i <= aNbU; ++i)
  {
    const double aU = myUParams.Value(i);
    aCosU.SetValue(i, std::cos(aU));
    aSinU.SetValue(i, std::sin(aU));
  }

  const gp_XYZ&                            aAxisDir = myAxisDirection.XYZ();
  const gp_XYZ&                            aAxisLoc = myAxisLocation.XYZ();
  NCollection_Array2<GeomGridEval::SurfD2> aResult(1, aNbU, 1, aNbV);

  for (int j = 1; j <= aNbV; ++j)
  {
    const GeomGridEval::CurveD2& aCurveData = aCurveD2.Value(j);
    const gp_XYZ                 aCQ        = aCurveData.Point.XYZ() - aAxisLoc;

    // D1U (before rotation) = Axis × (P - AxisLoc)
    gp_Vec aD1U_unrot(aAxisDir.Crossed(aCQ));
    if (aD1U_unrot.SquareMagnitude() < Precision::SquareConfusion())
    {
      aD1U_unrot.SetCoord(0.0, 0.0, 0.0);
    }

    // D1V (before rotation) = C'(v)
    const gp_Vec& aD1V_unrot = aCurveData.D1;

    // D2U (before rotation) = (Axis · CQ) * Axis - CQ
    const gp_Vec aD2U_unrot(aAxisDir.Dot(aCQ) * aAxisDir - aCQ);

    // D2UV (before rotation) = Axis × C'(v)
    const gp_Vec aD2UV_unrot(aAxisDir.Crossed(aD1V_unrot.XYZ()));

    // D2V (before rotation) = C''(v)
    const gp_Vec& aD2V_unrot = aCurveData.D2;

    for (int i = 1; i <= aNbU; ++i)
    {
      const double aCos = aCosU.Value(i);
      const double aSin = aSinU.Value(i);

      // Rotate all vectors
      const gp_XYZ aRotatedPt = rotatePoint(aCQ, aAxisDir, aCos, aSin);
      const gp_Vec aD1U       = rotateVec(aD1U_unrot, aAxisDir, aCos, aSin);
      const gp_Vec aD1V       = rotateVec(aD1V_unrot, aAxisDir, aCos, aSin);
      const gp_Vec aD2U       = rotateVec(aD2U_unrot, aAxisDir, aCos, aSin);
      const gp_Vec aD2V       = rotateVec(aD2V_unrot, aAxisDir, aCos, aSin);
      const gp_Vec aD2UV      = rotateVec(aD2UV_unrot, aAxisDir, aCos, aSin);

      aResult.ChangeValue(i, j) = {gp_Pnt(aRotatedPt + aAxisLoc), aD1U, aD1V, aD2U, aD2V, aD2UV};
    }
  }

  return aResult;
}

//==================================================================================================

NCollection_Array2<GeomGridEval::SurfD3> GeomGridEval_SurfaceOfRevolution::EvaluateGridD3() const
{
  if (myBasisCurve.IsNull() || myUParams.IsEmpty() || myVParams.IsEmpty())
  {
    return NCollection_Array2<GeomGridEval::SurfD3>();
  }

  const int aNbU = myUParams.Size();
  const int aNbV = myVParams.Size();

  // Batch evaluate curve D3
  GeomGridEval_Curve aCurveEval;
  aCurveEval.Initialize(myBasisCurve);
  aCurveEval.SetParams(myVParams);

  NCollection_Array1<GeomGridEval::CurveD3> aCurveD3 = aCurveEval.EvaluateGridD3();
  if (aCurveD3.IsEmpty())
  {
    return NCollection_Array2<GeomGridEval::SurfD3>();
  }

  // Precompute sin/cos
  NCollection_Array1<double> aCosU(1, aNbU);
  NCollection_Array1<double> aSinU(1, aNbU);
  for (int i = 1; i <= aNbU; ++i)
  {
    const double aU = myUParams.Value(i);
    aCosU.SetValue(i, std::cos(aU));
    aSinU.SetValue(i, std::sin(aU));
  }

  const gp_XYZ&                            aAxisDir = myAxisDirection.XYZ();
  const gp_XYZ&                            aAxisLoc = myAxisLocation.XYZ();
  NCollection_Array2<GeomGridEval::SurfD3> aResult(1, aNbU, 1, aNbV);

  for (int j = 1; j <= aNbV; ++j)
  {
    const GeomGridEval::CurveD3& aCurveData = aCurveD3.Value(j);
    const gp_XYZ                 aCQ        = aCurveData.Point.XYZ() - aAxisLoc;

    // D1U (before rotation) = Axis × (P - AxisLoc)
    gp_Vec aD1U_unrot(aAxisDir.Crossed(aCQ));
    if (aD1U_unrot.SquareMagnitude() < Precision::SquareConfusion())
    {
      aD1U_unrot.SetCoord(0.0, 0.0, 0.0);
    }

    const gp_Vec& aD1V_unrot = aCurveData.D1;
    const gp_Vec  aD2U_unrot(aAxisDir.Dot(aCQ) * aAxisDir - aCQ);
    const gp_Vec  aD2UV_unrot(aAxisDir.Crossed(aD1V_unrot.XYZ()));
    const gp_Vec& aD2V_unrot = aCurveData.D2;

    // D3U (before rotation) = -D1U
    const gp_Vec aD3U_unrot = -aD1U_unrot;

    // D3UUV (before rotation) = (Axis · D1V) * Axis - D1V
    const gp_Vec aD3UUV_unrot(aAxisDir.Dot(aD1V_unrot.XYZ()) * aAxisDir - aD1V_unrot.XYZ());

    // D3UVV (before rotation) = Axis × D2V
    const gp_Vec aD3UVV_unrot(aAxisDir.Crossed(aD2V_unrot.XYZ()));

    // D3V (before rotation) = C'''(v)
    const gp_Vec& aD3V_unrot = aCurveData.D3;

    for (int i = 1; i <= aNbU; ++i)
    {
      const double aCos = aCosU.Value(i);
      const double aSin = aSinU.Value(i);

      // Rotate all vectors
      const gp_XYZ aRotatedPt = rotatePoint(aCQ, aAxisDir, aCos, aSin);
      const gp_Vec aD1U       = rotateVec(aD1U_unrot, aAxisDir, aCos, aSin);
      const gp_Vec aD1V       = rotateVec(aD1V_unrot, aAxisDir, aCos, aSin);
      const gp_Vec aD2U       = rotateVec(aD2U_unrot, aAxisDir, aCos, aSin);
      const gp_Vec aD2V       = rotateVec(aD2V_unrot, aAxisDir, aCos, aSin);
      const gp_Vec aD2UV      = rotateVec(aD2UV_unrot, aAxisDir, aCos, aSin);
      const gp_Vec aD3U       = rotateVec(aD3U_unrot, aAxisDir, aCos, aSin);
      const gp_Vec aD3V       = rotateVec(aD3V_unrot, aAxisDir, aCos, aSin);
      const gp_Vec aD3UUV     = rotateVec(aD3UUV_unrot, aAxisDir, aCos, aSin);
      const gp_Vec aD3UVV     = rotateVec(aD3UVV_unrot, aAxisDir, aCos, aSin);

      aResult.ChangeValue(i, j) =
        {gp_Pnt(aRotatedPt + aAxisLoc), aD1U, aD1V, aD2U, aD2V, aD2UV, aD3U, aD3V, aD3UUV, aD3UVV};
    }
  }

  return aResult;
}

//==================================================================================================

NCollection_Array2<gp_Vec> GeomGridEval_SurfaceOfRevolution::EvaluateGridDN(int theNU,
                                                                            int theNV) const
{
  if (myBasisCurve.IsNull() || myUParams.IsEmpty() || myVParams.IsEmpty() || theNU < 0 || theNV < 0
      || (theNU + theNV) < 1)
  {
    return NCollection_Array2<gp_Vec>();
  }

  const int aNbU = myUParams.Size();
  const int aNbV = myVParams.Size();

  // Precompute sin/cos
  NCollection_Array1<double> aCosU(1, aNbU);
  NCollection_Array1<double> aSinU(1, aNbU);
  for (int i = 1; i <= aNbU; ++i)
  {
    const double aU = myUParams.Value(i);
    aCosU.SetValue(i, std::cos(aU));
    aSinU.SetValue(i, std::sin(aU));
  }

  const gp_XYZ&              aAxisDir = myAxisDirection.XYZ();
  const gp_XYZ&              aAxisLoc = myAxisLocation.XYZ();
  NCollection_Array2<gp_Vec> aResult(1, aNbU, 1, aNbV);

  // Get curve data
  GeomGridEval_Curve aCurveEval;
  aCurveEval.Initialize(myBasisCurve);
  aCurveEval.SetParams(myVParams);

  if (theNU == 0)
  {
    // Pure V derivative = curve derivative, rotated
    NCollection_Array1<gp_Vec> aCurveDN = aCurveEval.EvaluateGridDN(theNV);

    for (int j = 1; j <= aNbV; ++j)
    {
      const gp_Vec& aDV_unrot = aCurveDN.Value(j);
      for (int i = 1; i <= aNbU; ++i)
      {
        const gp_Vec aDV = rotateVec(aDV_unrot, aAxisDir, aCosU.Value(i), aSinU.Value(i));
        aResult.SetValue(i, j, aDV);
      }
    }
  }
  else
  {
    // Mixed or pure U derivative
    // Get curve point or derivative depending on theNV
    NCollection_Array1<gp_Vec> aCurveDV;
    NCollection_Array1<gp_Pnt> aCurvePts;

    if (theNV == 0)
    {
      aCurvePts = aCurveEval.EvaluateGrid();
    }
    else
    {
      aCurveDV = aCurveEval.EvaluateGridDN(theNV);
    }

    for (int j = 1; j <= aNbV; ++j)
    {
      gp_Vec aDV_unrot;
      if (theNV == 0)
      {
        // For pure U derivative, the "vector" is the position from axis
        aDV_unrot = gp_Vec(aCurvePts.Value(j).XYZ() - aAxisLoc);
      }
      else
      {
        aDV_unrot = aCurveDV.Value(j);
      }

      // Apply U derivative formula based on GeomEvaluator pattern:
      // theNU % 4 == 1: Axis × DV
      // theNU % 4 == 2: (Axis · DV) * Axis - DV
      // theNU % 4 == 3: -(Axis × DV)
      // theNU % 4 == 0: DV - (Axis · DV) * Axis
      gp_Vec    aDN_unrot;
      const int aModU = theNU % 4;
      if (aModU == 1)
      {
        aDN_unrot = gp_Vec(aAxisDir.Crossed(aDV_unrot.XYZ()));
      }
      else if (aModU == 2)
      {
        aDN_unrot = gp_Vec(aAxisDir.Dot(aDV_unrot.XYZ()) * aAxisDir - aDV_unrot.XYZ());
      }
      else if (aModU == 3)
      {
        aDN_unrot = gp_Vec(aAxisDir.Crossed(aDV_unrot.XYZ())) * (-1.0);
      }
      else // aModU == 0
      {
        aDN_unrot = gp_Vec(aDV_unrot.XYZ() - aAxisDir.Dot(aDV_unrot.XYZ()) * aAxisDir);
      }

      for (int i = 1; i <= aNbU; ++i)
      {
        const gp_Vec aDN = rotateVec(aDN_unrot, aAxisDir, aCosU.Value(i), aSinU.Value(i));
        aResult.SetValue(i, j, aDN);
      }
    }
  }

  return aResult;
}
