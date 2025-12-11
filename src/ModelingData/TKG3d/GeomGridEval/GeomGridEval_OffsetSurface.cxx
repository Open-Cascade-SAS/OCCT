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

#include <GeomGridEval_OffsetSurface.hxx>

#include <GeomAdaptor_Surface.hxx>
#include <GeomGridEval_Surface.hxx>

namespace
{
//! Tolerance for detecting singular normal (degenerate surface point).
constexpr double THE_NORMAL_TOL = 1e-10;

//! Compute offset D0 from basis D1.
//! P(u,v) = S(u,v) + Offset * N / |N|, where N = D1U ^ D1V
inline void computeOffsetD0(gp_Pnt&       theP,
                            const gp_Vec& theD1U,
                            const gp_Vec& theD1V,
                            double        theOffset)
{
  gp_Vec aN    = theD1U.Crossed(theD1V);
  double aNMag = aN.Magnitude();
  if (aNMag > THE_NORMAL_TOL)
  {
    theP.SetXYZ(theP.XYZ() + (theOffset / aNMag) * aN.XYZ());
  }
}

//! Compute offset D1 from basis D1, D2.
//! Uses the formula: P'u = S'u + Offset * dN/du, P'v = S'v + Offset * dN/dv
//! where N is the unit normal and dN/du, dN/dv are its partial derivatives.
inline void computeOffsetD1(gp_Pnt&       theP,
                            gp_Vec&       theD1U,
                            gp_Vec&       theD1V,
                            const gp_Vec& theD2U,
                            const gp_Vec& theD2V,
                            const gp_Vec& theD2UV,
                            double        theOffset)
{
  // N = D1U ^ D1V (non-normalized)
  gp_Vec aN    = theD1U.Crossed(theD1V);
  double aNMag = aN.Magnitude();

  if (aNMag <= THE_NORMAL_TOL)
  {
    // Singular point - just return basis values
    return;
  }

  gp_XYZ aNorm = aN.XYZ() / aNMag;
  theP.SetXYZ(theP.XYZ() + theOffset * aNorm);

  // Compute dN/du and dN/dv (non-normalized cross product derivatives)
  // Nu = D2U ^ D1V + D1U ^ D2UV
  // Nv = D2UV ^ D1V + D1U ^ D2V
  gp_Vec aNu = theD2U.Crossed(theD1V) + theD1U.Crossed(theD2UV);
  gp_Vec aNv = theD2UV.Crossed(theD1V) + theD1U.Crossed(theD2V);

  // Compute dn/du and dn/dv (unit normal derivatives)
  // dn/du = (Nu - n * (n . Nu)) / |N|
  double nDotNu = aNorm.Dot(aNu.XYZ());
  double nDotNv = aNorm.Dot(aNv.XYZ());

  gp_Vec aDnDu((aNu.XYZ() - aNorm * nDotNu) / aNMag);
  gp_Vec aDnDv((aNv.XYZ() - aNorm * nDotNv) / aNMag);

  theD1U += theOffset * aDnDu;
  theD1V += theOffset * aDnDv;
}

//! Compute offset D2 from basis D1, D2, D3.
//! This extends computeOffsetD1 with second derivative computation.
inline void computeOffsetD2(gp_Pnt&       theP,
                            gp_Vec&       theD1U,
                            gp_Vec&       theD1V,
                            gp_Vec&       theD2U,
                            gp_Vec&       theD2V,
                            gp_Vec&       theD2UV,
                            const gp_Vec& theD3U,
                            const gp_Vec& theD3V,
                            const gp_Vec& theD3UUV,
                            const gp_Vec& theD3UVV,
                            double        theOffset)
{
  // N = D1U ^ D1V (non-normalized)
  gp_Vec aN    = theD1U.Crossed(theD1V);
  double aNMag = aN.Magnitude();

  if (aNMag <= THE_NORMAL_TOL)
  {
    // Singular point - just return basis values
    return;
  }

  gp_XYZ aNorm = aN.XYZ() / aNMag;
  theP.SetXYZ(theP.XYZ() + theOffset * aNorm);

  // First derivatives of N (non-normalized cross product)
  // Nu = D2U ^ D1V + D1U ^ D2UV
  // Nv = D2UV ^ D1V + D1U ^ D2V
  gp_Vec aNu = theD2U.Crossed(theD1V) + theD1U.Crossed(theD2UV);
  gp_Vec aNv = theD2UV.Crossed(theD1V) + theD1U.Crossed(theD2V);

  // Second derivatives of N (non-normalized cross product)
  // Nuu = D3U ^ D1V + 2 * D2U ^ D2UV + D1U ^ D3UUV
  // Nvv = D3UVV ^ D1V + 2 * D2UV ^ D2V + D1U ^ D3V
  // Nuv = D3UUV ^ D1V + D2U ^ D2V + D2UV ^ D2UV + D1U ^ D3UVV
  gp_Vec aNuu = theD3U.Crossed(theD1V) + 2.0 * theD2U.Crossed(theD2UV) + theD1U.Crossed(theD3UUV);
  gp_Vec aNvv = theD3UVV.Crossed(theD1V) + 2.0 * theD2UV.Crossed(theD2V) + theD1U.Crossed(theD3V);
  gp_Vec aNuv =
    theD3UUV.Crossed(theD1V) + theD2U.Crossed(theD2V) + theD2UV.Crossed(theD2UV) + theD1U.Crossed(theD3UVV);

  // Unit normal derivatives
  double nDotNu = aNorm.Dot(aNu.XYZ());
  double nDotNv = aNorm.Dot(aNv.XYZ());

  gp_Vec aDnDu((aNu.XYZ() - aNorm * nDotNu) / aNMag);
  gp_Vec aDnDv((aNv.XYZ() - aNorm * nDotNv) / aNMag);

  // Second derivatives of unit normal
  // d²n/du² = (Nuu - n*(n.Nuu) - 2*(dn/du)*(n.Nu)) / |N|
  // d²n/dv² = (Nvv - n*(n.Nvv) - 2*(dn/dv)*(n.Nv)) / |N|
  // d²n/dudv = (Nuv - n*(n.Nuv) - (dn/du)*(n.Nv) - (dn/dv)*(n.Nu)) / |N|
  double nDotNuu = aNorm.Dot(aNuu.XYZ());
  double nDotNvv = aNorm.Dot(aNvv.XYZ());
  double nDotNuv = aNorm.Dot(aNuv.XYZ());

  gp_Vec aD2nDu2((aNuu.XYZ() - aNorm * nDotNuu - 2.0 * nDotNu * aDnDu.XYZ()) / aNMag);
  gp_Vec aD2nDv2((aNvv.XYZ() - aNorm * nDotNvv - 2.0 * nDotNv * aDnDv.XYZ()) / aNMag);
  gp_Vec aD2nDuDv((aNuv.XYZ() - aNorm * nDotNuv - nDotNv * aDnDu.XYZ() - nDotNu * aDnDv.XYZ()) / aNMag);

  // Apply offset corrections
  theD1U += theOffset * aDnDu;
  theD1V += theOffset * aDnDv;
  theD2U += theOffset * aD2nDu2;
  theD2V += theOffset * aD2nDv2;
  theD2UV += theOffset * aD2nDuDv;
}

} // namespace

//==================================================================================================

void GeomGridEval_OffsetSurface::SetUVParams(const TColStd_Array1OfReal& theUParams,
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

NCollection_Array2<gp_Pnt> GeomGridEval_OffsetSurface::EvaluateGrid() const
{
  if (myBasis.IsNull() || myUParams.IsEmpty() || myVParams.IsEmpty())
  {
    return NCollection_Array2<gp_Pnt>();
  }

  // Offset D0 requires basis D1 to compute normal
  GeomGridEval_Surface aBasisEval;
  aBasisEval.Initialize(myBasis);
  aBasisEval.SetUVParams(myUParams, myVParams);

  NCollection_Array2<GeomGridEval::SurfD1> aBasisD1 = aBasisEval.EvaluateGridD1();
  if (aBasisD1.IsEmpty())
  {
    return NCollection_Array2<gp_Pnt>();
  }

  const int                  aNbU = myUParams.Size();
  const int                  aNbV = myVParams.Size();
  NCollection_Array2<gp_Pnt> aResult(1, aNbU, 1, aNbV);

  for (int i = 1; i <= aNbU; ++i)
  {
    for (int j = 1; j <= aNbV; ++j)
    {
      const GeomGridEval::SurfD1& aBasis = aBasisD1.Value(i, j);
      gp_Pnt                      aP     = aBasis.Point;
      computeOffsetD0(aP, aBasis.D1U, aBasis.D1V, myOffset);
      aResult.SetValue(i, j, aP);
    }
  }

  return aResult;
}

//==================================================================================================

NCollection_Array2<GeomGridEval::SurfD1> GeomGridEval_OffsetSurface::EvaluateGridD1() const
{
  if (myBasis.IsNull() || myUParams.IsEmpty() || myVParams.IsEmpty())
  {
    return NCollection_Array2<GeomGridEval::SurfD1>();
  }

  // Offset D1 requires basis D2 to compute derivative of normal
  GeomGridEval_Surface aBasisEval;
  aBasisEval.Initialize(myBasis);
  aBasisEval.SetUVParams(myUParams, myVParams);

  NCollection_Array2<GeomGridEval::SurfD2> aBasisD2 = aBasisEval.EvaluateGridD2();
  if (aBasisD2.IsEmpty())
  {
    return NCollection_Array2<GeomGridEval::SurfD1>();
  }

  const int                                aNbU = myUParams.Size();
  const int                                aNbV = myVParams.Size();
  NCollection_Array2<GeomGridEval::SurfD1> aResult(1, aNbU, 1, aNbV);

  for (int i = 1; i <= aNbU; ++i)
  {
    for (int j = 1; j <= aNbV; ++j)
    {
      const GeomGridEval::SurfD2& aBasis = aBasisD2.Value(i, j);
      gp_Pnt                      aP     = aBasis.Point;
      gp_Vec                      aD1U   = aBasis.D1U;
      gp_Vec                      aD1V   = aBasis.D1V;
      computeOffsetD1(aP, aD1U, aD1V, aBasis.D2U, aBasis.D2V, aBasis.D2UV, myOffset);
      aResult.ChangeValue(i, j) = {aP, aD1U, aD1V};
    }
  }

  return aResult;
}

//==================================================================================================

NCollection_Array2<GeomGridEval::SurfD2> GeomGridEval_OffsetSurface::EvaluateGridD2() const
{
  if (myBasis.IsNull() || myUParams.IsEmpty() || myVParams.IsEmpty())
  {
    return NCollection_Array2<GeomGridEval::SurfD2>();
  }

  // Offset D2 requires basis D3
  GeomGridEval_Surface aBasisEval;
  aBasisEval.Initialize(myBasis);
  aBasisEval.SetUVParams(myUParams, myVParams);

  NCollection_Array2<GeomGridEval::SurfD3> aBasisD3 = aBasisEval.EvaluateGridD3();
  if (aBasisD3.IsEmpty())
  {
    return NCollection_Array2<GeomGridEval::SurfD2>();
  }

  const int                                aNbU = myUParams.Size();
  const int                                aNbV = myVParams.Size();
  NCollection_Array2<GeomGridEval::SurfD2> aResult(1, aNbU, 1, aNbV);

  for (int i = 1; i <= aNbU; ++i)
  {
    for (int j = 1; j <= aNbV; ++j)
    {
      const GeomGridEval::SurfD3& aBasis = aBasisD3.Value(i, j);
      gp_Pnt                      aP     = aBasis.Point;
      gp_Vec                      aD1U   = aBasis.D1U;
      gp_Vec                      aD1V   = aBasis.D1V;
      gp_Vec                      aD2U   = aBasis.D2U;
      gp_Vec                      aD2V   = aBasis.D2V;
      gp_Vec                      aD2UV  = aBasis.D2UV;
      computeOffsetD2(aP,
                      aD1U,
                      aD1V,
                      aD2U,
                      aD2V,
                      aD2UV,
                      aBasis.D3U,
                      aBasis.D3V,
                      aBasis.D3UUV,
                      aBasis.D3UVV,
                      myOffset);
      aResult.ChangeValue(i, j) = {aP, aD1U, aD1V, aD2U, aD2V, aD2UV};
    }
  }

  return aResult;
}

//==================================================================================================

NCollection_Array2<GeomGridEval::SurfD3> GeomGridEval_OffsetSurface::EvaluateGridD3() const
{
  if (myGeom.IsNull() || myUParams.IsEmpty() || myVParams.IsEmpty())
  {
    return NCollection_Array2<GeomGridEval::SurfD3>();
  }

  // Offset D3 requires basis D4, which is not available in batch form.
  // Use adaptor for repeated evaluations.
  const int                                aNbU = myUParams.Size();
  const int                                aNbV = myVParams.Size();
  NCollection_Array2<GeomGridEval::SurfD3> aResult(1, aNbU, 1, aNbV);

  GeomAdaptor_Surface anAdaptor(myGeom);

  for (int i = 1; i <= aNbU; ++i)
  {
    const double aU = myUParams.Value(i);
    for (int j = 1; j <= aNbV; ++j)
    {
      gp_Pnt aPoint;
      gp_Vec aD1U, aD1V, aD2U, aD2V, aD2UV, aD3U, aD3V, aD3UUV, aD3UVV;
      anAdaptor.D3(aU, myVParams.Value(j), aPoint, aD1U, aD1V, aD2U, aD2V, aD2UV, aD3U, aD3V, aD3UUV, aD3UVV);
      aResult.ChangeValue(i, j) = {aPoint, aD1U, aD1V, aD2U, aD2V, aD2UV, aD3U, aD3V, aD3UUV, aD3UVV};
    }
  }

  return aResult;
}
