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

#ifndef _GeomGridEval_OffsetSurface_HeaderFile
#define _GeomGridEval_OffsetSurface_HeaderFile

#include <Geom_OffsetSurface.hxx>
#include <Geom_Surface.hxx>
#include <GeomGridEval.hxx>
#include <gp_Pnt2d.hxx>
#include <NCollection_Array1.hxx>
#include <NCollection_Array2.hxx>
#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <TColStd_Array1OfReal.hxx>

//! @brief Batch evaluator for offset surface grid points.
//!
//! Evaluates the offset surface formula:
//! P(u,v) = S(u,v) + Offset * Normal(u,v)
//!
//! Uses GeomGridEval_Surface for batch evaluation of the basis surface,
//! then applies offset transformation.
//!
//! Usage:
//! @code
//!   GeomGridEval_OffsetSurface anEvaluator(myGeomOffset);
//!   NCollection_Array2<gp_Pnt> aGrid = anEvaluator.EvaluateGrid(myUParams, myVParams);
//! @endcode
class GeomGridEval_OffsetSurface
{
public:
  DEFINE_STANDARD_ALLOC

  //! Constructor with geometry.
  //! @param theOffset the offset surface geometry to evaluate
  GeomGridEval_OffsetSurface(const Handle(Geom_OffsetSurface)& theOffset)
      : myGeom(theOffset),
        myOffset(0.0)
  {
    if (!myGeom.IsNull())
    {
      myOffset = myGeom->Offset();
      myBasis  = myGeom->BasisSurface();
    }
  }

  //! Non-copyable and non-movable.
  GeomGridEval_OffsetSurface(const GeomGridEval_OffsetSurface&)            = delete;
  GeomGridEval_OffsetSurface& operator=(const GeomGridEval_OffsetSurface&) = delete;
  GeomGridEval_OffsetSurface(GeomGridEval_OffsetSurface&&)                 = delete;
  GeomGridEval_OffsetSurface& operator=(GeomGridEval_OffsetSurface&&)      = delete;

  //! Returns the geometry handle.
  const Handle(Geom_OffsetSurface)& Geometry() const { return myGeom; }

  //! Evaluate all grid points.
  //! @param[in] theUParams array of U parameter values
  //! @param[in] theVParams array of V parameter values
  //! @return 2D array of evaluated points (1-based indexing),
  //!         or empty array if geometry is null or no parameters set
  Standard_EXPORT NCollection_Array2<gp_Pnt> EvaluateGrid(
    const TColStd_Array1OfReal& theUParams,
    const TColStd_Array1OfReal& theVParams) const;

  //! Evaluate all grid points with first partial derivatives.
  //! @param[in] theUParams array of U parameter values
  //! @param[in] theVParams array of V parameter values
  //! @return 2D array of SurfD1 (1-based indexing),
  //!         or empty array if geometry is null or no parameters set
  Standard_EXPORT NCollection_Array2<GeomGridEval::SurfD1> EvaluateGridD1(
    const TColStd_Array1OfReal& theUParams,
    const TColStd_Array1OfReal& theVParams) const;

  //! Evaluate all grid points with first and second partial derivatives.
  //! @param[in] theUParams array of U parameter values
  //! @param[in] theVParams array of V parameter values
  //! @return 2D array of SurfD2 (1-based indexing),
  //!         or empty array if geometry is null or no parameters set
  Standard_EXPORT NCollection_Array2<GeomGridEval::SurfD2> EvaluateGridD2(
    const TColStd_Array1OfReal& theUParams,
    const TColStd_Array1OfReal& theVParams) const;

  //! Evaluate all grid points with derivatives up to third order.
  //! Uses GeomAdaptor_Surface::D3 for evaluation.
  //! @param[in] theUParams array of U parameter values
  //! @param[in] theVParams array of V parameter values
  //! @return 2D array of SurfD3 (1-based indexing),
  //!         or empty array if geometry is null or no parameters set
  Standard_EXPORT NCollection_Array2<GeomGridEval::SurfD3> EvaluateGridD3(
    const TColStd_Array1OfReal& theUParams,
    const TColStd_Array1OfReal& theVParams) const;

  //! Evaluate partial derivative d^(NU+NV)S/(dU^NU dV^NV) at all grid points.
  //! Uses geometry DN method.
  //! @param[in] theUParams array of U parameter values
  //! @param[in] theVParams array of V parameter values
  //! @param[in] theNU derivative order in U direction
  //! @param[in] theNV derivative order in V direction
  //! @return 2D array of derivative vectors (1-based indexing)
  Standard_EXPORT NCollection_Array2<gp_Vec> EvaluateGridDN(
    const TColStd_Array1OfReal& theUParams,
    const TColStd_Array1OfReal& theVParams,
    int                         theNU,
    int                         theNV) const;

  //! Evaluate points at arbitrary UV pairs.
  //! @param[in] theUVPairs array of UV coordinate pairs (U=X(), V=Y())
  //! @return array of evaluated points (1-based indexing),
  //!         or empty array if geometry is null or no parameters set
  Standard_EXPORT NCollection_Array1<gp_Pnt> EvaluatePoints(
    const NCollection_Array1<gp_Pnt2d>& theUVPairs) const;

  //! Evaluate points with first partial derivatives at arbitrary UV pairs.
  //! @param[in] theUVPairs array of UV coordinate pairs (U=X(), V=Y())
  //! @return array of SurfD1 (1-based indexing),
  //!         or empty array if geometry is null or no parameters set
  Standard_EXPORT NCollection_Array1<GeomGridEval::SurfD1> EvaluatePointsD1(
    const NCollection_Array1<gp_Pnt2d>& theUVPairs) const;

  //! Evaluate points with first and second partial derivatives at arbitrary UV pairs.
  //! @param[in] theUVPairs array of UV coordinate pairs (U=X(), V=Y())
  //! @return array of SurfD2 (1-based indexing),
  //!         or empty array if geometry is null or no parameters set
  Standard_EXPORT NCollection_Array1<GeomGridEval::SurfD2> EvaluatePointsD2(
    const NCollection_Array1<gp_Pnt2d>& theUVPairs) const;

  //! Evaluate points with derivatives up to third order at arbitrary UV pairs.
  //! @param[in] theUVPairs array of UV coordinate pairs (U=X(), V=Y())
  //! @return array of SurfD3 (1-based indexing),
  //!         or empty array if geometry is null or no parameters set
  Standard_EXPORT NCollection_Array1<GeomGridEval::SurfD3> EvaluatePointsD3(
    const NCollection_Array1<gp_Pnt2d>& theUVPairs) const;

  //! Evaluate partial derivative at arbitrary UV pairs.
  //! @param[in] theUVPairs array of UV coordinate pairs (U=X(), V=Y())
  //! @param[in] theNU derivative order in U direction
  //! @param[in] theNV derivative order in V direction
  //! @return array of derivative vectors (1-based indexing)
  Standard_EXPORT NCollection_Array1<gp_Vec> EvaluatePointsDN(
    const NCollection_Array1<gp_Pnt2d>& theUVPairs,
    int                                 theNU,
    int                                 theNV) const;

private:
  Handle(Geom_OffsetSurface) myGeom;
  Handle(Geom_Surface)       myBasis;
  double                     myOffset;
};

#endif // _GeomGridEval_OffsetSurface_HeaderFile
