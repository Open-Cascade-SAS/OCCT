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
  // d²n/du² = (Nuu - n*(n.Nuu) - 2*(n.Nu)*(dn/du) - n*(dn/du.Nu)) / |N|
  // d²n/dv² = (Nvv - n*(n.Nvv) - 2*(n.Nv)*(dn/dv) - n*(dn/dv.Nv)) / |N|
  // d²n/dudv = (Nuv - n*(n.Nuv) - (n.Nu)*(dn/dv) - (n.Nv)*(dn/du) - n*(dn/du.Nv) - n*(dn/dv.Nu)) / |N|
  double nDotNuu = aNorm.Dot(aNuu.XYZ());
  double nDotNvv = aNorm.Dot(aNvv.XYZ());
  double nDotNuv = aNorm.Dot(aNuv.XYZ());

  double dnDuDotNu = aDnDu.Dot(aNu);
  double dnDvDotNv = aDnDv.Dot(aNv);
  double dnDuDotNv = aDnDu.Dot(aNv);
  double dnDvDotNu = aDnDv.Dot(aNu);

  gp_Vec aD2nDu2((aNuu.XYZ() - aNorm * nDotNuu - 2.0 * nDotNu * aDnDu.XYZ() - aNorm * dnDuDotNu) / aNMag);
  gp_Vec aD2nDv2((aNvv.XYZ() - aNorm * nDotNvv - 2.0 * nDotNv * aDnDv.XYZ() - aNorm * dnDvDotNv) / aNMag);
  gp_Vec aD2nDuDv(
    (aNuv.XYZ() - aNorm * nDotNuv - nDotNu * aDnDv.XYZ() - nDotNv * aDnDu.XYZ() - aNorm * dnDuDotNv - aNorm * dnDvDotNu)
    / aNMag);

  // Apply offset corrections
  theD1U += theOffset * aDnDu;
  theD1V += theOffset * aDnDv;
  theD2U += theOffset * aD2nDu2;
  theD2V += theOffset * aD2nDv2;
  theD2UV += theOffset * aD2nDuDv;
}

//! Compute offset D3 from basis D1, D2, D3, D4.
//! This extends computeOffsetD2 with third derivative computation.
//! @param theD4U    fourth derivative ∂⁴S/∂u⁴
//! @param theD4V    fourth derivative ∂⁴S/∂v⁴
//! @param theD4UUV  fourth derivative ∂⁴S/∂u³∂v (D3UUV derivative w.r.t. u)
//! @param theD4UVV  fourth derivative ∂⁴S/∂u∂v³ (D3UVV derivative w.r.t. v)
//! @param theD4UUVV fourth derivative ∂⁴S/∂u²∂v² (D3UUV derivative w.r.t. v or D3UVV w.r.t. u)
inline void computeOffsetD3(gp_Pnt&       theP,
                            gp_Vec&       theD1U,
                            gp_Vec&       theD1V,
                            gp_Vec&       theD2U,
                            gp_Vec&       theD2V,
                            gp_Vec&       theD2UV,
                            gp_Vec&       theD3U,
                            gp_Vec&       theD3V,
                            gp_Vec&       theD3UUV,
                            gp_Vec&       theD3UVV,
                            const gp_Vec& theD4U,
                            const gp_Vec& theD4V,
                            const gp_Vec& theD4UUV,
                            const gp_Vec& theD4UVV,
                            const gp_Vec& theD4UUVV,
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
  gp_Vec aNu = theD2U.Crossed(theD1V) + theD1U.Crossed(theD2UV);
  gp_Vec aNv = theD2UV.Crossed(theD1V) + theD1U.Crossed(theD2V);

  // Second derivatives of N
  gp_Vec aNuu = theD3U.Crossed(theD1V) + 2.0 * theD2U.Crossed(theD2UV) + theD1U.Crossed(theD3UUV);
  gp_Vec aNvv = theD3UVV.Crossed(theD1V) + 2.0 * theD2UV.Crossed(theD2V) + theD1U.Crossed(theD3V);
  gp_Vec aNuv =
    theD3UUV.Crossed(theD1V) + theD2U.Crossed(theD2V) + theD2UV.Crossed(theD2UV) + theD1U.Crossed(theD3UVV);

  // Third derivatives of N
  // Nuuu = D4U ^ D1V + 3*D3U ^ D2UV + 3*D2U ^ D3UUV + D1U ^ D4UUV
  // Nvvv = D4UVV ^ D1V + 3*D3UVV ^ D2V + 3*D2UV ^ D3V + D1U ^ D4V
  // Nuuv = D4UUV ^ D1V + 2*D3U ^ D2V + D3UUV ^ D2UV + 2*D2U ^ D3UVV + D2UV ^ D3UUV + D1U ^ D4UUVV
  // Nuvv = D4UUVV ^ D1V + D3UUV ^ D2V + 2*D3UVV ^ D2UV + D2U ^ D3V + 2*D2UV ^ D3UVV + D1U ^ D4UVV
  gp_Vec aNuuu = theD4U.Crossed(theD1V) + 3.0 * theD3U.Crossed(theD2UV) + 3.0 * theD2U.Crossed(theD3UUV)
                 + theD1U.Crossed(theD4UUV);
  gp_Vec aNvvv = theD4UVV.Crossed(theD1V) + 3.0 * theD3UVV.Crossed(theD2V) + 3.0 * theD2UV.Crossed(theD3V)
                 + theD1U.Crossed(theD4V);
  gp_Vec aNuuv = theD4UUV.Crossed(theD1V) + 2.0 * theD3U.Crossed(theD2V) + theD3UUV.Crossed(theD2UV)
                 + 2.0 * theD2U.Crossed(theD3UVV) + theD2UV.Crossed(theD3UUV) + theD1U.Crossed(theD4UUVV);
  gp_Vec aNuvv = theD4UUVV.Crossed(theD1V) + theD3UUV.Crossed(theD2V) + 2.0 * theD3UVV.Crossed(theD2UV)
                 + theD2U.Crossed(theD3V) + 2.0 * theD2UV.Crossed(theD3UVV) + theD1U.Crossed(theD4UVV);

  // Unit normal derivatives
  double nDotNu = aNorm.Dot(aNu.XYZ());
  double nDotNv = aNorm.Dot(aNv.XYZ());

  gp_Vec aDnDu((aNu.XYZ() - aNorm * nDotNu) / aNMag);
  gp_Vec aDnDv((aNv.XYZ() - aNorm * nDotNv) / aNMag);

  // Second derivatives of unit normal
  // d²n/du² = (Nuu - n*(n.Nuu) - 2*(n.Nu)*(dn/du) - n*(dn/du.Nu)) / |N|
  // d²n/dv² = (Nvv - n*(n.Nvv) - 2*(n.Nv)*(dn/dv) - n*(dn/dv.Nv)) / |N|
  // d²n/dudv = (Nuv - n*(n.Nuv) - (n.Nu)*(dn/dv) - (n.Nv)*(dn/du) - n*(dn/du.Nv) - n*(dn/dv.Nu)) / |N|
  double nDotNuu = aNorm.Dot(aNuu.XYZ());
  double nDotNvv = aNorm.Dot(aNvv.XYZ());
  double nDotNuv = aNorm.Dot(aNuv.XYZ());

  double dnDuDotNu = aDnDu.Dot(aNu);
  double dnDvDotNv = aDnDv.Dot(aNv);
  double dnDuDotNv = aDnDu.Dot(aNv);
  double dnDvDotNu = aDnDv.Dot(aNu);

  gp_Vec aD2nDu2((aNuu.XYZ() - aNorm * nDotNuu - 2.0 * nDotNu * aDnDu.XYZ() - aNorm * dnDuDotNu) / aNMag);
  gp_Vec aD2nDv2((aNvv.XYZ() - aNorm * nDotNvv - 2.0 * nDotNv * aDnDv.XYZ() - aNorm * dnDvDotNv) / aNMag);
  gp_Vec aD2nDuDv(
    (aNuv.XYZ() - aNorm * nDotNuv - nDotNu * aDnDv.XYZ() - nDotNv * aDnDu.XYZ() - aNorm * dnDuDotNv - aNorm * dnDvDotNu)
    / aNMag);

  // Third derivatives of unit normal
  // Full formula with all terms from the chain rule:
  // d³n/du³ = (Nuuu - n*(n.Nuuu) - 3*(n.Nuu)*(dn/du) - 3*(n.Nu)*(d²n/du²)
  //           - 3*n*(dn/du.Nuu) - 3*n*(d²n/du².Nu)) / |N|
  double nDotNuuu = aNorm.Dot(aNuuu.XYZ());
  double nDotNvvv = aNorm.Dot(aNvvv.XYZ());
  double nDotNuuv = aNorm.Dot(aNuuv.XYZ());
  double nDotNuvv = aNorm.Dot(aNuvv.XYZ());

  double dnDuDotNuu  = aDnDu.Dot(aNuu);
  double dnDvDotNvv  = aDnDv.Dot(aNvv);
  double d2nDu2DotNu = aD2nDu2.Dot(aNu);
  double d2nDv2DotNv = aD2nDv2.Dot(aNv);

  // Mixed terms for third order mixed derivatives
  double dnDuDotNuv   = aDnDu.Dot(aNuv);
  double dnDvDotNuu   = aDnDv.Dot(aNuu);
  double dnDuDotNvv   = aDnDu.Dot(aNvv);
  double dnDvDotNuv   = aDnDv.Dot(aNuv);
  double d2nDuDvDotNu = aD2nDuDv.Dot(aNu);
  double d2nDuDvDotNv = aD2nDuDv.Dot(aNv);
  double d2nDu2DotNv  = aD2nDu2.Dot(aNv);
  double d2nDv2DotNu  = aD2nDv2.Dot(aNu);

  // d³n/du³: includes term -3*(dn/du.Nu)*dn/du from differentiating 2*(n.Nu)*dn/du
  gp_Vec aD3nDu3((aNuuu.XYZ() - aNorm * nDotNuuu - 3.0 * nDotNuu * aDnDu.XYZ() - 3.0 * nDotNu * aD2nDu2.XYZ()
                  - 3.0 * dnDuDotNu * aDnDu.XYZ() - 3.0 * aNorm * dnDuDotNuu - 3.0 * aNorm * d2nDu2DotNu)
                 / aNMag);
  gp_Vec aD3nDv3((aNvvv.XYZ() - aNorm * nDotNvvv - 3.0 * nDotNvv * aDnDv.XYZ() - 3.0 * nDotNv * aD2nDv2.XYZ()
                  - 3.0 * dnDvDotNv * aDnDv.XYZ() - 3.0 * aNorm * dnDvDotNvv - 3.0 * aNorm * d2nDv2DotNv)
                 / aNMag);
  gp_Vec aD3nDu2Dv((aNuuv.XYZ() - aNorm * nDotNuuv - 2.0 * nDotNuv * aDnDu.XYZ() - nDotNuu * aDnDv.XYZ()
                    - 2.0 * nDotNu * aD2nDuDv.XYZ() - nDotNv * aD2nDu2.XYZ() - 2.0 * aNorm * dnDuDotNuv
                    - aNorm * dnDvDotNuu - 2.0 * aNorm * d2nDuDvDotNu - aNorm * d2nDu2DotNv)
                   / aNMag);
  gp_Vec aD3nDuDv2((aNuvv.XYZ() - aNorm * nDotNuvv - nDotNvv * aDnDu.XYZ() - 2.0 * nDotNuv * aDnDv.XYZ()
                    - nDotNu * aD2nDv2.XYZ() - 2.0 * nDotNv * aD2nDuDv.XYZ() - aNorm * dnDuDotNvv
                    - 2.0 * aNorm * dnDvDotNuv - aNorm * d2nDv2DotNu - 2.0 * aNorm * d2nDuDvDotNv)
                   / aNMag);

  // Apply offset corrections
  theD1U += theOffset * aDnDu;
  theD1V += theOffset * aDnDv;
  theD2U += theOffset * aD2nDu2;
  theD2V += theOffset * aD2nDv2;
  theD2UV += theOffset * aD2nDuDv;
  theD3U += theOffset * aD3nDu3;
  theD3V += theOffset * aD3nDv3;
  theD3UUV += theOffset * aD3nDu2Dv;
  theD3UVV += theOffset * aD3nDuDv2;
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
  if (myBasis.IsNull() || myUParams.IsEmpty() || myVParams.IsEmpty())
  {
    return NCollection_Array2<GeomGridEval::SurfD3>();
  }

  // Offset D3 requires basis D3 + D4 derivatives
  GeomGridEval_Surface aBasisEval;
  aBasisEval.Initialize(myBasis);
  aBasisEval.SetUVParams(myUParams, myVParams);

  // Get basis D3
  NCollection_Array2<GeomGridEval::SurfD3> aBasisD3 = aBasisEval.EvaluateGridD3();
  if (aBasisD3.IsEmpty())
  {
    return NCollection_Array2<GeomGridEval::SurfD3>();
  }

  // Get fourth-order derivatives from basis surface
  NCollection_Array2<gp_Vec> aD4U    = aBasisEval.EvaluateGridDN(4, 0); // ∂⁴S/∂u⁴
  NCollection_Array2<gp_Vec> aD4V    = aBasisEval.EvaluateGridDN(0, 4); // ∂⁴S/∂v⁴
  NCollection_Array2<gp_Vec> aD4UUV  = aBasisEval.EvaluateGridDN(3, 1); // ∂⁴S/∂u³∂v
  NCollection_Array2<gp_Vec> aD4UVV  = aBasisEval.EvaluateGridDN(1, 3); // ∂⁴S/∂u∂v³
  NCollection_Array2<gp_Vec> aD4UUVV = aBasisEval.EvaluateGridDN(2, 2); // ∂⁴S/∂u²∂v²

  const int                                aNbU = myUParams.Size();
  const int                                aNbV = myVParams.Size();
  NCollection_Array2<GeomGridEval::SurfD3> aResult(1, aNbU, 1, aNbV);

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
      gp_Vec                      aD3U   = aBasis.D3U;
      gp_Vec                      aD3V   = aBasis.D3V;
      gp_Vec                      aD3UUV = aBasis.D3UUV;
      gp_Vec                      aD3UVV = aBasis.D3UVV;

      computeOffsetD3(aP,
                      aD1U,
                      aD1V,
                      aD2U,
                      aD2V,
                      aD2UV,
                      aD3U,
                      aD3V,
                      aD3UUV,
                      aD3UVV,
                      aD4U.Value(i, j),
                      aD4V.Value(i, j),
                      aD4UUV.Value(i, j),
                      aD4UVV.Value(i, j),
                      aD4UUVV.Value(i, j),
                      myOffset);
      aResult.ChangeValue(i, j) = {aP, aD1U, aD1V, aD2U, aD2V, aD2UV, aD3U, aD3V, aD3UUV, aD3UVV};
    }
  }

  return aResult;
}

//==================================================================================================

NCollection_Array2<gp_Vec> GeomGridEval_OffsetSurface::EvaluateGridDN(int theNU, int theNV) const
{
  if (myBasis.IsNull() || myUParams.IsEmpty() || myVParams.IsEmpty() || theNU < 0 || theNV < 0
      || (theNU + theNV) < 1)
  {
    return NCollection_Array2<gp_Vec>();
  }

  const int aNbU   = myUParams.Size();
  const int aNbV   = myVParams.Size();
  const int aOrder = theNU + theNV;

  NCollection_Array2<gp_Vec> aResult(1, aNbU, 1, aNbV);

  // For common low orders, use the optimized grid evaluations
  if (aOrder == 1)
  {
    // Reuse D1 evaluation which handles offset computation optimally
    NCollection_Array2<GeomGridEval::SurfD1> aD1Grid = EvaluateGridD1();
    for (int i = 1; i <= aNbU; ++i)
    {
      for (int j = 1; j <= aNbV; ++j)
      {
        const GeomGridEval::SurfD1& aD1 = aD1Grid.Value(i, j);
        aResult.SetValue(i, j, (theNU == 1) ? gp_Vec(aD1.D1U.XYZ()) : gp_Vec(aD1.D1V.XYZ()));
      }
    }
  }
  else if (aOrder == 2)
  {
    // Reuse D2 evaluation
    NCollection_Array2<GeomGridEval::SurfD2> aD2Grid = EvaluateGridD2();
    for (int i = 1; i <= aNbU; ++i)
    {
      for (int j = 1; j <= aNbV; ++j)
      {
        const GeomGridEval::SurfD2& aD2 = aD2Grid.Value(i, j);
        if (theNU == 2)
          aResult.SetValue(i, j, aD2.D2U);
        else if (theNV == 2)
          aResult.SetValue(i, j, aD2.D2V);
        else
          aResult.SetValue(i, j, aD2.D2UV);
      }
    }
  }
  else if (aOrder == 3)
  {
    // Reuse D3 evaluation
    NCollection_Array2<GeomGridEval::SurfD3> aD3Grid = EvaluateGridD3();
    for (int i = 1; i <= aNbU; ++i)
    {
      for (int j = 1; j <= aNbV; ++j)
      {
        const GeomGridEval::SurfD3& aD3 = aD3Grid.Value(i, j);
        if (theNU == 3)
          aResult.SetValue(i, j, aD3.D3U);
        else if (theNV == 3)
          aResult.SetValue(i, j, aD3.D3V);
        else if (theNU == 2)
          aResult.SetValue(i, j, aD3.D3UUV);
        else
          aResult.SetValue(i, j, aD3.D3UVV);
      }
    }
  }
  else
  {
    // For higher orders, compute offset DN from basis derivatives
    // Offset P = S + offset * n, so DN(P) = DN(S) + offset * DN(n)
    // DN(n) requires basis derivatives up to order N+1

    // Get basis surface derivatives needed for offset computation
    GeomGridEval_Surface aBasisEval;
    aBasisEval.Initialize(myBasis);
    aBasisEval.SetUVParams(myUParams, myVParams);

    // Get the required basis DN
    NCollection_Array2<gp_Vec> aBasisDN = aBasisEval.EvaluateGridDN(theNU, theNV);

    // Get basis D1 for normal computation
    NCollection_Array2<GeomGridEval::SurfD1> aBasisD1 = aBasisEval.EvaluateGridD1();

    // Get higher-order derivatives needed for normal derivative
    // For offset DN of order N, we need basis derivatives up to order N+1
    NCollection_Array2<gp_Vec> aBasisDNPlusU = aBasisEval.EvaluateGridDN(theNU + 1, theNV);
    NCollection_Array2<gp_Vec> aBasisDNPlusV = aBasisEval.EvaluateGridDN(theNU, theNV + 1);

    for (int i = 1; i <= aNbU; ++i)
    {
      for (int j = 1; j <= aNbV; ++j)
      {
        const GeomGridEval::SurfD1& aD1 = aBasisD1.Value(i, j);

        // N = D1U ^ D1V
        gp_Vec aN    = aD1.D1U.Crossed(aD1.D1V);
        double aNMag = aN.Magnitude();

        if (aNMag <= THE_NORMAL_TOL)
        {
          // Singular point - use basis derivative only
          aResult.SetValue(i, j, aBasisDN.Value(i, j));
          continue;
        }

        // For high-order derivatives, the general formula is complex
        // Use a simplified approximation: DN(P) ≈ DN(S) + offset * (projection of DN+1(S) onto normal
        // tangent space) This is an approximation for orders > 3
        gp_XYZ aNorm = aN.XYZ() / aNMag;

        // Compute simplified offset correction using the Nth derivative of normal
        // dn_Nu_Nv = (D(Nu+1,Nv) ^ D1V + D1U ^ D(Nu,Nv+1)) / |N| projected perpendicular to n
        gp_Vec aDnContribU = aBasisDNPlusU.Value(i, j).Crossed(aD1.D1V);
        gp_Vec aDnContribV = aD1.D1U.Crossed(aBasisDNPlusV.Value(i, j));
        gp_Vec aDnApprox   = aDnContribU + aDnContribV;

        // Project perpendicular to normal
        double nDotDn = aNorm.Dot(aDnApprox.XYZ());
        gp_Vec aDnPerp((aDnApprox.XYZ() - aNorm * nDotDn) / aNMag);

        // Final result
        gp_Vec aOffsetDN = aBasisDN.Value(i, j) + myOffset * aDnPerp;
        aResult.SetValue(i, j, aOffsetDN);
      }
    }
  }

  return aResult;
}
