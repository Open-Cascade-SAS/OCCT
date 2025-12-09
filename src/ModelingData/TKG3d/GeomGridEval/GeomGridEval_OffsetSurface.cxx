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

      // Compute normal N = D1U ^ D1V
      gp_Vec  aN    = aBasis.D1U.Crossed(aBasis.D1V);
      double  aNMag = aN.Magnitude();
      gp_Pnt  aP    = aBasis.Point;

      if (aNMag > THE_NORMAL_TOL)
      {
        aP.SetXYZ(aP.XYZ() + (myOffset / aNMag) * aN.XYZ());
      }
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

      // Compute N = D1U ^ D1V
      gp_Vec aN    = aBasis.D1U.Crossed(aBasis.D1V);
      double aNMag = aN.Magnitude();

      gp_Pnt aP   = aBasis.Point;
      gp_Vec aD1U = aBasis.D1U;
      gp_Vec aD1V = aBasis.D1V;

      if (aNMag > THE_NORMAL_TOL)
      {
        gp_XYZ aNorm = aN.XYZ() / aNMag;
        aP.SetXYZ(aP.XYZ() + myOffset * aNorm);

        // Compute dN/du and dN/dv (non-normalized cross product derivatives)
        // Nu = D2U ^ D1V + D1U ^ D2UV
        // Nv = D2UV ^ D1V + D1U ^ D2V
        gp_Vec aNu = aBasis.D2U.Crossed(aBasis.D1V) + aBasis.D1U.Crossed(aBasis.D2UV);
        gp_Vec aNv = aBasis.D2UV.Crossed(aBasis.D1V) + aBasis.D1U.Crossed(aBasis.D2V);

        // Compute dn/du and dn/dv (unit normal derivatives)
        // dn/du = (Nu - n * (n . Nu)) / |N|
        double nDotNu = aNorm.Dot(aNu.XYZ());
        double nDotNv = aNorm.Dot(aNv.XYZ());

        gp_Vec aDnDu((aNu.XYZ() - aNorm * nDotNu) / aNMag);
        gp_Vec aDnDv((aNv.XYZ() - aNorm * nDotNv) / aNMag);

        aD1U += myOffset * aDnDu;
        aD1V += myOffset * aDnDv;
      }

      aResult.ChangeValue(i, j) = {aP, aD1U, aD1V};
    }
  }

  return aResult;
}

//==================================================================================================

NCollection_Array2<GeomGridEval::SurfD2> GeomGridEval_OffsetSurface::EvaluateGridD2() const
{
  if (myGeom.IsNull() || myUParams.IsEmpty() || myVParams.IsEmpty())
  {
    return NCollection_Array2<GeomGridEval::SurfD2>();
  }

  // Offset D2 requires basis D3, which is not available in batch form.
  // Use adaptor for more efficient repeated evaluations.
  const int                                aNbU = myUParams.Size();
  const int                                aNbV = myVParams.Size();
  NCollection_Array2<GeomGridEval::SurfD2> aResult(1, aNbU, 1, aNbV);

  GeomAdaptor_Surface anAdaptor(myGeom);

  for (int i = 1; i <= aNbU; ++i)
  {
    const double aU = myUParams.Value(i);
    for (int j = 1; j <= aNbV; ++j)
    {
      gp_Pnt aPoint;
      gp_Vec aD1U, aD1V, aD2U, aD2V, aD2UV;
      anAdaptor.D2(aU, myVParams.Value(j), aPoint, aD1U, aD1V, aD2U, aD2V, aD2UV);
      aResult.ChangeValue(i, j) = {aPoint, aD1U, aD1V, aD2U, aD2V, aD2UV};
    }
  }

  return aResult;
}
