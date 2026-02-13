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

#ifndef _GeomGridEval_HeaderFile
#define _GeomGridEval_HeaderFile

#include <Geom_Curve.hxx>
#include <Geom_Surface.hxx>
#include <gp_Pnt2d.hxx>
#include <NCollection_Array1.hxx>
#include <NCollection_Array2.hxx>

//! @brief Namespace containing type aliases and template helpers for grid evaluators.
//!
//! Provides type aliases to common evaluation result structures defined in Geom_Curve and
//! Geom_Surface and template helpers for iterating over parameter grids.
namespace GeomGridEval
{

using CurveD1 = Geom_Curve::ResD1;
using CurveD2 = Geom_Curve::ResD2;
using CurveD3 = Geom_Curve::ResD3;
using SurfD1  = Geom_Surface::ResD1;
using SurfD2  = Geom_Surface::ResD2;
using SurfD3  = Geom_Surface::ResD3;

//==================================================================================================
// Template helpers for parametric surface evaluation.
// These provide the iteration pattern, while the actual computation is delegated to a functor.
//==================================================================================================

//! Evaluate grid points using a point evaluator functor.
//! @tparam Evaluator functor type with operator()(double theU, double theV) -> gp_Pnt
//! @param theUParams array of U parameter values
//! @param theVParams array of V parameter values
//! @param theEval evaluator functor
//! @return 2D array of evaluated points (1-based indexing)
template <typename Evaluator>
NCollection_Array2<gp_Pnt> EvaluateGridHelper(const NCollection_Array1<double>& theUParams,
                                              const NCollection_Array1<double>& theVParams,
                                              Evaluator                         theEval)
{
  const int aNbU = theUParams.Size();
  const int aNbV = theVParams.Size();
  if (aNbU == 0 || aNbV == 0)
  {
    return NCollection_Array2<gp_Pnt>();
  }

  NCollection_Array2<gp_Pnt> aResult(1, aNbU, 1, aNbV);
  for (int iU = 1; iU <= aNbU; ++iU)
  {
    const double aU = theUParams.Value(theUParams.Lower() + iU - 1);
    for (int iV = 1; iV <= aNbV; ++iV)
    {
      const double aV = theVParams.Value(theVParams.Lower() + iV - 1);
      aResult.SetValue(iU, iV, theEval(aU, aV));
    }
  }
  return aResult;
}

//! Evaluate grid points with D1 using an evaluator functor.
//! @tparam Evaluator functor type with operator()(double theU, double theV) -> SurfD1
template <typename Evaluator>
NCollection_Array2<SurfD1> EvaluateGridD1Helper(const NCollection_Array1<double>& theUParams,
                                                const NCollection_Array1<double>& theVParams,
                                                Evaluator                         theEval)
{
  const int aNbU = theUParams.Size();
  const int aNbV = theVParams.Size();
  if (aNbU == 0 || aNbV == 0)
  {
    return NCollection_Array2<SurfD1>();
  }

  NCollection_Array2<SurfD1> aResult(1, aNbU, 1, aNbV);
  for (int iU = 1; iU <= aNbU; ++iU)
  {
    const double aU = theUParams.Value(theUParams.Lower() + iU - 1);
    for (int iV = 1; iV <= aNbV; ++iV)
    {
      const double aV             = theVParams.Value(theVParams.Lower() + iV - 1);
      aResult.ChangeValue(iU, iV) = theEval(aU, aV);
    }
  }
  return aResult;
}

//! Evaluate grid points with D2 using an evaluator functor.
//! @tparam Evaluator functor type with operator()(double theU, double theV) -> SurfD2
template <typename Evaluator>
NCollection_Array2<SurfD2> EvaluateGridD2Helper(const NCollection_Array1<double>& theUParams,
                                                const NCollection_Array1<double>& theVParams,
                                                Evaluator                         theEval)
{
  const int aNbU = theUParams.Size();
  const int aNbV = theVParams.Size();
  if (aNbU == 0 || aNbV == 0)
  {
    return NCollection_Array2<SurfD2>();
  }

  NCollection_Array2<SurfD2> aResult(1, aNbU, 1, aNbV);
  for (int iU = 1; iU <= aNbU; ++iU)
  {
    const double aU = theUParams.Value(theUParams.Lower() + iU - 1);
    for (int iV = 1; iV <= aNbV; ++iV)
    {
      const double aV             = theVParams.Value(theVParams.Lower() + iV - 1);
      aResult.ChangeValue(iU, iV) = theEval(aU, aV);
    }
  }
  return aResult;
}

//! Evaluate grid points with D3 using an evaluator functor.
//! @tparam Evaluator functor type with operator()(double theU, double theV) -> SurfD3
template <typename Evaluator>
NCollection_Array2<SurfD3> EvaluateGridD3Helper(const NCollection_Array1<double>& theUParams,
                                                const NCollection_Array1<double>& theVParams,
                                                Evaluator                         theEval)
{
  const int aNbU = theUParams.Size();
  const int aNbV = theVParams.Size();
  if (aNbU == 0 || aNbV == 0)
  {
    return NCollection_Array2<SurfD3>();
  }

  NCollection_Array2<SurfD3> aResult(1, aNbU, 1, aNbV);
  for (int iU = 1; iU <= aNbU; ++iU)
  {
    const double aU = theUParams.Value(theUParams.Lower() + iU - 1);
    for (int iV = 1; iV <= aNbV; ++iV)
    {
      const double aV             = theVParams.Value(theVParams.Lower() + iV - 1);
      aResult.ChangeValue(iU, iV) = theEval(aU, aV);
    }
  }
  return aResult;
}

//! Evaluate grid DN using an evaluator functor.
//! @tparam Evaluator functor type with operator()(double theU, double theV) -> gp_Vec
template <typename Evaluator>
NCollection_Array2<gp_Vec> EvaluateGridDNHelper(const NCollection_Array1<double>& theUParams,
                                                const NCollection_Array1<double>& theVParams,
                                                Evaluator                         theEval)
{
  const int aNbU = theUParams.Size();
  const int aNbV = theVParams.Size();
  if (aNbU == 0 || aNbV == 0)
  {
    return NCollection_Array2<gp_Vec>();
  }

  NCollection_Array2<gp_Vec> aResult(1, aNbU, 1, aNbV);
  for (int iU = 1; iU <= aNbU; ++iU)
  {
    const double aU = theUParams.Value(theUParams.Lower() + iU - 1);
    for (int iV = 1; iV <= aNbV; ++iV)
    {
      const double aV = theVParams.Value(theVParams.Lower() + iV - 1);
      aResult.SetValue(iU, iV, theEval(aU, aV));
    }
  }
  return aResult;
}

} // namespace GeomGridEval

#endif // _GeomGridEval_HeaderFile
