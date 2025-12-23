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

#include <gp_Trsf.hxx>
#include <Precision.hxx>

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

  NCollection_Array2<gp_Pnt> aResult(1, aNbU, 1, aNbV);

  for (int i = 1; i <= aNbU; ++i)
  {
    gp_Trsf aRotation;
    aRotation.SetRotation(myAxis, myUParams.Value(i));

    for (int j = 1; j <= aNbV; ++j)
    {
      gp_Pnt aP = aCurvePoints.Value(j);
      aP.Transform(aRotation);
      aResult.SetValue(i, j, aP);
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

  const gp_XYZ&                            aAxisDir = myAxisDirection.XYZ();
  const gp_XYZ&                            aAxisLoc = myAxisLocation.XYZ();
  NCollection_Array2<GeomGridEval::SurfD1> aResult(1, aNbU, 1, aNbV);

  for (int i = 1; i <= aNbU; ++i)
  {
    gp_Trsf aRotation;
    aRotation.SetRotation(myAxis, myUParams.Value(i));

    for (int j = 1; j <= aNbV; ++j)
    {
      const GeomGridEval::CurveD1& aCurveData = aCurveD1.Value(j);
      const gp_XYZ                 aCQ        = aCurveData.Point.XYZ() - aAxisLoc;

      // D1U (before rotation) = Axis Cross (P - AxisLoc)
      gp_Vec aD1U(aAxisDir.Crossed(aCQ));
      if (aD1U.SquareMagnitude() < Precision::SquareConfusion())
      {
        aD1U.SetCoord(0.0, 0.0, 0.0);
      }

      // Rotate point and vectors
      gp_Pnt aP = aCurveData.Point;
      aP.Transform(aRotation);
      aD1U.Transform(aRotation);
      gp_Vec aD1V = aCurveData.D1;
      aD1V.Transform(aRotation);

      aResult.ChangeValue(i, j) = {aP, aD1U, aD1V};
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

  const gp_XYZ&                            aAxisDir = myAxisDirection.XYZ();
  const gp_XYZ&                            aAxisLoc = myAxisLocation.XYZ();
  NCollection_Array2<GeomGridEval::SurfD2> aResult(1, aNbU, 1, aNbV);

  for (int i = 1; i <= aNbU; ++i)
  {
    gp_Trsf aRotation;
    aRotation.SetRotation(myAxis, myUParams.Value(i));

    for (int j = 1; j <= aNbV; ++j)
    {
      const GeomGridEval::CurveD2& aCurveData = aCurveD2.Value(j);
      const gp_XYZ                 aCQ        = aCurveData.Point.XYZ() - aAxisLoc;

      // D1U (before rotation) = Axis Cross (P - AxisLoc)
      gp_Vec aD1U(aAxisDir.Crossed(aCQ));
      if (aD1U.SquareMagnitude() < Precision::SquareConfusion())
      {
        aD1U.SetCoord(0.0, 0.0, 0.0);
      }

      // D2U (before rotation) = (Axis Dot CQ) * Axis - CQ
      gp_Vec aD2U(aAxisDir.Dot(aCQ) * aAxisDir - aCQ);

      // D2UV (before rotation) = Axis Cross C'(v)
      gp_Vec aD2UV(aAxisDir.Crossed(aCurveData.D1.XYZ()));

      // Rotate point and all vectors
      gp_Pnt aP = aCurveData.Point;
      aP.Transform(aRotation);
      aD1U.Transform(aRotation);
      gp_Vec aD1V = aCurveData.D1;
      aD1V.Transform(aRotation);
      aD2U.Transform(aRotation);
      gp_Vec aD2V = aCurveData.D2;
      aD2V.Transform(aRotation);
      aD2UV.Transform(aRotation);

      aResult.ChangeValue(i, j) = {aP, aD1U, aD1V, aD2U, aD2V, aD2UV};
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

  const gp_XYZ&                            aAxisDir = myAxisDirection.XYZ();
  const gp_XYZ&                            aAxisLoc = myAxisLocation.XYZ();
  NCollection_Array2<GeomGridEval::SurfD3> aResult(1, aNbU, 1, aNbV);

  for (int i = 1; i <= aNbU; ++i)
  {
    gp_Trsf aRotation;
    aRotation.SetRotation(myAxis, myUParams.Value(i));

    for (int j = 1; j <= aNbV; ++j)
    {
      const GeomGridEval::CurveD3& aCurveData = aCurveD3.Value(j);
      const gp_XYZ                 aCQ        = aCurveData.Point.XYZ() - aAxisLoc;

      // D1U (before rotation) = Axis Cross (P - AxisLoc)
      gp_Vec aD1U(aAxisDir.Crossed(aCQ));
      if (aD1U.SquareMagnitude() < Precision::SquareConfusion())
      {
        aD1U.SetCoord(0.0, 0.0, 0.0);
      }

      // D2U (before rotation) = (Axis Dot CQ) * Axis - CQ
      gp_Vec aD2U(aAxisDir.Dot(aCQ) * aAxisDir - aCQ);

      // D2UV (before rotation) = Axis Cross C'(v)
      gp_Vec aD2UV(aAxisDir.Crossed(aCurveData.D1.XYZ()));

      // D3U (before rotation) = -D1U
      gp_Vec aD3U = -aD1U;

      // D3UUV (before rotation) = (Axis Dot D1V) * Axis - D1V
      gp_Vec aD3UUV(aAxisDir.Dot(aCurveData.D1.XYZ()) * aAxisDir - aCurveData.D1.XYZ());

      // D3UVV (before rotation) = Axis Cross D2V
      gp_Vec aD3UVV(aAxisDir.Crossed(aCurveData.D2.XYZ()));

      // Rotate point and all vectors
      gp_Pnt aP = aCurveData.Point;
      aP.Transform(aRotation);
      aD1U.Transform(aRotation);
      gp_Vec aD1V = aCurveData.D1;
      aD1V.Transform(aRotation);
      aD2U.Transform(aRotation);
      gp_Vec aD2V = aCurveData.D2;
      aD2V.Transform(aRotation);
      aD2UV.Transform(aRotation);
      aD3U.Transform(aRotation);
      gp_Vec aD3V = aCurveData.D3;
      aD3V.Transform(aRotation);
      aD3UUV.Transform(aRotation);
      aD3UVV.Transform(aRotation);

      aResult.ChangeValue(i, j) = {aP, aD1U, aD1V, aD2U, aD2V, aD2UV, aD3U, aD3V, aD3UUV, aD3UVV};
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

    for (int i = 1; i <= aNbU; ++i)
    {
      gp_Trsf aRotation;
      aRotation.SetRotation(myAxis, myUParams.Value(i));

      for (int j = 1; j <= aNbV; ++j)
      {
        gp_Vec aDV = aCurveDN.Value(j);
        aDV.Transform(aRotation);
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

    for (int i = 1; i <= aNbU; ++i)
    {
      gp_Trsf aRotation;
      aRotation.SetRotation(myAxis, myUParams.Value(i));

      for (int j = 1; j <= aNbV; ++j)
      {
        gp_Vec aDV;
        if (theNV == 0)
        {
          // For pure U derivative, the "vector" is the position from axis
          aDV = gp_Vec(aCurvePts.Value(j).XYZ() - aAxisLoc);
        }
        else
        {
          aDV = aCurveDV.Value(j);
        }

        // Apply U derivative formula based on GeomEvaluator pattern:
        // theNU % 4 == 1: Axis Cross DV
        // theNU % 4 == 2: (Axis Dot DV) * Axis - DV
        // theNU % 4 == 3: -(Axis Cross DV)
        // theNU % 4 == 0: DV - (Axis Dot DV) * Axis
        gp_Vec    aDN;
        const int aModU = theNU % 4;
        if (aModU == 1)
        {
          aDN = gp_Vec(aAxisDir.Crossed(aDV.XYZ()));
        }
        else if (aModU == 2)
        {
          aDN = gp_Vec(aAxisDir.Dot(aDV.XYZ()) * aAxisDir - aDV.XYZ());
        }
        else if (aModU == 3)
        {
          aDN = gp_Vec(aAxisDir.Crossed(aDV.XYZ())) * (-1.0);
        }
        else // aModU == 0
        {
          aDN = gp_Vec(aDV.XYZ() - aAxisDir.Dot(aDV.XYZ()) * aAxisDir);
        }

        aDN.Transform(aRotation);
        aResult.SetValue(i, j, aDN);
      }
    }
  }

  return aResult;
}
