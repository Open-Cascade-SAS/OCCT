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

#include <GeomGridEval_Curve.hxx>

namespace
{
//! Tolerance for detecting singular offset direction.
constexpr double THE_OFFSET_TOL = 1e-10;

//! Compute offset D0 from basis D1.
//! P(u) = p(u) + Offset * Ndir / R
//! with R = ||p' ^ V|| and Ndir = p' ^ direction
inline void computeOffsetD0(gp_Pnt&       theP,
                            const gp_Vec& theD1,
                            const gp_XYZ& theDirXYZ,
                            double        theOffset)
{
  gp_XYZ aNdir = theD1.XYZ().Crossed(theDirXYZ);
  double aR    = aNdir.Modulus();
  if (aR > THE_OFFSET_TOL)
  {
    theP.SetXYZ(theP.XYZ() + (theOffset / aR) * aNdir);
  }
}

//! Compute offset D1 from basis D1, D2.
//! P'(u) = p'(u) + (Offset / R^2) * (DNdir * R - Ndir * (Dr/R))
inline void computeOffsetD1(gp_Pnt&       theP,
                            gp_Vec&       theD1,
                            const gp_Vec& theD2,
                            const gp_XYZ& theDirXYZ,
                            double        theOffset)
{
  gp_XYZ aNdir  = theD1.XYZ().Crossed(theDirXYZ);
  gp_XYZ aDNdir = theD2.XYZ().Crossed(theDirXYZ);
  double aR2    = aNdir.SquareModulus();
  double aR     = std::sqrt(aR2);
  double aR3    = aR * aR2;
  double aDr    = aNdir.Dot(aDNdir);

  if (aR3 <= THE_OFFSET_TOL)
  {
    if (aR2 <= THE_OFFSET_TOL)
    {
      // Singular point - just return basis values
      return;
    }
    // Alternative computation for stability
    aDNdir.Multiply(aR);
    aDNdir.Subtract(aNdir.Multiplied(aDr / aR));
    aDNdir.Multiply(theOffset / aR2);
  }
  else
  {
    // Same computation as IICURV in EUCLID-IS for better stability
    aDNdir.Multiply(theOffset / aR);
    aDNdir.Subtract(aNdir.Multiplied(theOffset * aDr / aR3));
  }

  aNdir.Multiply(theOffset / aR);
  theP.ChangeCoord().Add(aNdir);
  theD1.Add(gp_Vec(aDNdir));
}

//! Compute offset D2 from basis D1, D2, D3.
//! P"(u) = p"(u) + (Offset / R) * (D2Ndir - DNdir * (2 * Dr / R^2) +
//!         Ndir * ((3 * Dr^2 / R^4) - (D2r / R^2)))
inline void computeOffsetD2(gp_Pnt&       theP,
                            gp_Vec&       theD1,
                            gp_Vec&       theD2,
                            const gp_Vec& theD3,
                            const gp_XYZ& theDirXYZ,
                            double        theOffset)
{
  gp_XYZ aNdir   = theD1.XYZ().Crossed(theDirXYZ);
  gp_XYZ aDNdir  = theD2.XYZ().Crossed(theDirXYZ);
  gp_XYZ aD2Ndir = theD3.XYZ().Crossed(theDirXYZ);
  double aR2     = aNdir.SquareModulus();
  double aR      = std::sqrt(aR2);
  double aR3     = aR2 * aR;
  double aR4     = aR2 * aR2;
  double aR5     = aR3 * aR2;
  double aDr     = aNdir.Dot(aDNdir);
  double aD2r    = aNdir.Dot(aD2Ndir) + aDNdir.Dot(aDNdir);

  if (aR5 <= THE_OFFSET_TOL)
  {
    if (aR4 <= THE_OFFSET_TOL)
    {
      // Singular point - just return basis values
      return;
    }
    // Alternative computation for stability
    aR4 = aR2 * aR2;
    aD2Ndir.Subtract(aDNdir.Multiplied(2.0 * aDr / aR2));
    aD2Ndir.Add(aNdir.Multiplied(((3.0 * aDr * aDr) / aR4) - (aD2r / aR2)));
    aD2Ndir.Multiply(theOffset / aR);

    aDNdir.Multiply(aR);
    aDNdir.Subtract(aNdir.Multiplied(aDr / aR));
    aDNdir.Multiply(theOffset / aR2);
  }
  else
  {
    // Same computation as IICURV in EUCLID-IS for better stability
    aD2Ndir.Multiply(theOffset / aR);
    aD2Ndir.Subtract(aDNdir.Multiplied(2.0 * theOffset * aDr / aR3));
    aD2Ndir.Add(aNdir.Multiplied(theOffset * (((3.0 * aDr * aDr) / aR5) - (aD2r / aR3))));

    aDNdir.Multiply(theOffset / aR);
    aDNdir.Subtract(aNdir.Multiplied(theOffset * aDr / aR3));
  }

  aNdir.Multiply(theOffset / aR);
  theP.ChangeCoord().Add(aNdir);
  theD1.Add(gp_Vec(aDNdir));
  theD2.Add(gp_Vec(aD2Ndir));
}

//! Compute offset D3 from basis D1, D2, D3, D4.
//! P"'(u) = p"'(u) + (Offset / R) * (D3Ndir - (3 * Dr / R^2) * D2Ndir -
//!          (3 * D2r / R^2) * DNdir + (3 * Dr^2 / R^4) * DNdir -
//!          (D3r / R^2) * Ndir + (6 * Dr^2 / R^4) * Ndir +
//!          (6 * Dr * D2r / R^4) * Ndir - (15 * Dr^3 / R^6) * Ndir
inline void computeOffsetD3(gp_Pnt&       theP,
                            gp_Vec&       theD1,
                            gp_Vec&       theD2,
                            gp_Vec&       theD3,
                            const gp_Vec& theD4,
                            const gp_XYZ& theDirXYZ,
                            double        theOffset)
{
  gp_XYZ aNdir   = theD1.XYZ().Crossed(theDirXYZ);
  gp_XYZ aDNdir  = theD2.XYZ().Crossed(theDirXYZ);
  gp_XYZ aD2Ndir = theD3.XYZ().Crossed(theDirXYZ);
  gp_XYZ aD3Ndir = theD4.XYZ().Crossed(theDirXYZ);
  double aR2     = aNdir.SquareModulus();
  double aR      = std::sqrt(aR2);
  double aR3     = aR2 * aR;
  double aR4     = aR2 * aR2;
  double aR5     = aR3 * aR2;
  double aR6     = aR3 * aR3;
  double aR7     = aR5 * aR2;
  double aDr     = aNdir.Dot(aDNdir);
  double aD2r    = aNdir.Dot(aD2Ndir) + aDNdir.Dot(aDNdir);
  double aD3r    = aNdir.Dot(aD3Ndir) + 3.0 * aDNdir.Dot(aD2Ndir);

  if (aR7 <= THE_OFFSET_TOL)
  {
    if (aR6 <= THE_OFFSET_TOL)
    {
      // Singular point - just return basis values
      return;
    }
    // Alternative computation for stability
    aD3Ndir.Subtract(aD2Ndir.Multiplied(3.0 * aDr / aR2));
    aD3Ndir.Subtract(aDNdir.Multiplied(3.0 * ((aD2r / aR2) + (aDr * aDr / aR4))));
    aD3Ndir.Add(aNdir.Multiplied(6.0 * aDr * aDr / aR4 + 6.0 * aDr * aD2r / aR4
                                 - 15.0 * aDr * aDr * aDr / aR6 - aD3r));
    aD3Ndir.Multiply(theOffset / aR);

    aR4 = aR2 * aR2;
    aD2Ndir.Subtract(aDNdir.Multiplied(2.0 * aDr / aR2));
    aD2Ndir.Subtract(aNdir.Multiplied((3.0 * aDr * aDr / aR4) - (aD2r / aR2)));
    aD2Ndir.Multiply(theOffset / aR);

    aDNdir.Multiply(aR);
    aDNdir.Subtract(aNdir.Multiplied(aDr / aR));
    aDNdir.Multiply(theOffset / aR2);
  }
  else
  {
    // Same computation as IICURV in EUCLID-IS for better stability
    aD3Ndir.Divide(aR);
    aD3Ndir.Subtract(aD2Ndir.Multiplied(3.0 * aDr / aR3));
    aD3Ndir.Subtract(aDNdir.Multiplied((3.0 * ((aD2r / aR3) + (aDr * aDr) / aR5))));
    aD3Ndir.Add(aNdir.Multiplied(6.0 * aDr * aDr / aR5 + 6.0 * aDr * aD2r / aR5
                                 - 15.0 * aDr * aDr * aDr / aR7 - aD3r));
    aD3Ndir.Multiply(theOffset);

    aD2Ndir.Divide(aR);
    aD2Ndir.Subtract(aDNdir.Multiplied(2.0 * aDr / aR3));
    aD2Ndir.Subtract(aNdir.Multiplied((3.0 * aDr * aDr / aR5) - (aD2r / aR3)));
    aD2Ndir.Multiply(theOffset);

    aDNdir.Multiply(theOffset / aR);
    aDNdir.Subtract(aNdir.Multiplied(theOffset * aDr / aR3));
  }

  aNdir.Multiply(theOffset / aR);
  theP.ChangeCoord().Add(aNdir);
  theD1.Add(gp_Vec(aDNdir));
  theD2.Add(gp_Vec(aD2Ndir));
  theD3.Add(gp_Vec(aD3Ndir));
}

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
    gp_Pnt                       aP     = aBasis.Point;
    computeOffsetD0(aP, aBasis.D1, aDirXYZ, myOffset);
    aResult.SetValue(i, aP);
  }

  return aResult;
}

//==================================================================================================

NCollection_Array1<GeomGridEval::CurveD1> GeomGridEval_OffsetCurve::EvaluateGridD1() const
{
  if (myBasis.IsNull() || myParams.IsEmpty())
  {
    return NCollection_Array1<GeomGridEval::CurveD1>();
  }

  // Offset D1 requires basis D2
  GeomGridEval_Curve aBasisEval;
  aBasisEval.Initialize(myBasis);
  aBasisEval.SetParams(myParams);

  NCollection_Array1<GeomGridEval::CurveD2> aBasisD2 = aBasisEval.EvaluateGridD2();
  if (aBasisD2.IsEmpty())
  {
    return NCollection_Array1<GeomGridEval::CurveD1>();
  }

  const int                                 aNbParams = myParams.Size();
  NCollection_Array1<GeomGridEval::CurveD1> aResult(1, aNbParams);

  const gp_XYZ aDirXYZ = myDirection.XYZ();

  for (int i = 1; i <= aNbParams; ++i)
  {
    const GeomGridEval::CurveD2& aBasis = aBasisD2.Value(i);
    gp_Pnt                       aP     = aBasis.Point;
    gp_Vec                       aD1    = aBasis.D1;
    computeOffsetD1(aP, aD1, aBasis.D2, aDirXYZ, myOffset);
    aResult.ChangeValue(i) = {aP, aD1};
  }

  return aResult;
}

//==================================================================================================

NCollection_Array1<GeomGridEval::CurveD2> GeomGridEval_OffsetCurve::EvaluateGridD2() const
{
  if (myBasis.IsNull() || myParams.IsEmpty())
  {
    return NCollection_Array1<GeomGridEval::CurveD2>();
  }

  // Offset D2 requires basis D3
  GeomGridEval_Curve aBasisEval;
  aBasisEval.Initialize(myBasis);
  aBasisEval.SetParams(myParams);

  NCollection_Array1<GeomGridEval::CurveD3> aBasisD3 = aBasisEval.EvaluateGridD3();
  if (aBasisD3.IsEmpty())
  {
    return NCollection_Array1<GeomGridEval::CurveD2>();
  }

  const int                                 aNbParams = myParams.Size();
  NCollection_Array1<GeomGridEval::CurveD2> aResult(1, aNbParams);

  const gp_XYZ aDirXYZ = myDirection.XYZ();

  for (int i = 1; i <= aNbParams; ++i)
  {
    const GeomGridEval::CurveD3& aBasis = aBasisD3.Value(i);
    gp_Pnt                       aP     = aBasis.Point;
    gp_Vec                       aD1    = aBasis.D1;
    gp_Vec                       aD2    = aBasis.D2;
    computeOffsetD2(aP, aD1, aD2, aBasis.D3, aDirXYZ, myOffset);
    aResult.ChangeValue(i) = {aP, aD1, aD2};
  }

  return aResult;
}

//==================================================================================================

NCollection_Array1<GeomGridEval::CurveD3> GeomGridEval_OffsetCurve::EvaluateGridD3() const
{
  if (myBasis.IsNull() || myParams.IsEmpty())
  {
    return NCollection_Array1<GeomGridEval::CurveD3>();
  }

  // Offset D3 requires basis D4 - we need to use adaptor for D4 since we don't have batch D4
  // But we can still batch evaluate D3 from basis and compute D4 individually
  GeomGridEval_Curve aBasisEval;
  aBasisEval.Initialize(myBasis);
  aBasisEval.SetParams(myParams);

  NCollection_Array1<GeomGridEval::CurveD3> aBasisD3 = aBasisEval.EvaluateGridD3();
  if (aBasisD3.IsEmpty())
  {
    return NCollection_Array1<GeomGridEval::CurveD3>();
  }

  const int                                 aNbParams = myParams.Size();
  NCollection_Array1<GeomGridEval::CurveD3> aResult(1, aNbParams);

  const gp_XYZ aDirXYZ = myDirection.XYZ();

  for (int i = 1; i <= aNbParams; ++i)
  {
    const GeomGridEval::CurveD3& aBasis = aBasisD3.Value(i);
    gp_Pnt                       aP     = aBasis.Point;
    gp_Vec                       aD1    = aBasis.D1;
    gp_Vec                       aD2    = aBasis.D2;
    gp_Vec                       aD3    = aBasis.D3;

    // Get D4 from basis curve for this parameter
    gp_Vec aD4 = myBasis->DN(myParams.Value(i), 4);

    computeOffsetD3(aP, aD1, aD2, aD3, aD4, aDirXYZ, myOffset);
    aResult.ChangeValue(i) = {aP, aD1, aD2, aD3};
  }

  return aResult;
}

//==================================================================================================

NCollection_Array1<gp_Vec> GeomGridEval_OffsetCurve::EvaluateGridDN(int theN) const
{
  if (myGeom.IsNull() || myParams.IsEmpty() || theN < 1)
  {
    return NCollection_Array1<gp_Vec>();
  }

  const int                  aNbParams = myParams.Size();
  NCollection_Array1<gp_Vec> aResult(1, aNbParams);

  // Reuse existing grid evaluators for orders 1-3
  if (theN == 1)
  {
    NCollection_Array1<GeomGridEval::CurveD1> aD1Grid = EvaluateGridD1();
    for (int i = 1; i <= aNbParams; ++i)
    {
      aResult.SetValue(i, aD1Grid.Value(i).D1);
    }
  }
  else if (theN == 2)
  {
    NCollection_Array1<GeomGridEval::CurveD2> aD2Grid = EvaluateGridD2();
    for (int i = 1; i <= aNbParams; ++i)
    {
      aResult.SetValue(i, aD2Grid.Value(i).D2);
    }
  }
  else if (theN == 3)
  {
    NCollection_Array1<GeomGridEval::CurveD3> aD3Grid = EvaluateGridD3();
    for (int i = 1; i <= aNbParams; ++i)
    {
      aResult.SetValue(i, aD3Grid.Value(i).D3);
    }
  }
  else
  {
    // For orders > 3, use geometry DN method directly
    for (int i = 1; i <= aNbParams; ++i)
    {
      aResult.SetValue(i, myGeom->DN(myParams.Value(i), theN));
    }
  }
  return aResult;
}
