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

#ifndef _GeomGridEval_OffsetCurve_HeaderFile
#define _GeomGridEval_OffsetCurve_HeaderFile

#include <Geom_Curve.hxx>
#include <Geom_OffsetCurve.hxx>
#include <GeomGridEval.hxx>
#include <NCollection_Array1.hxx>
#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <NCollection_Array1.hxx>

//! @brief Batch evaluator for offset curve grid points.
//!
//! Evaluates the offset curve formula:
//! P(u) = C(u) + Offset * (D1(u) ^ Direction) / |D1(u) ^ Direction|
//!
//! Uses GeomGridEval_Curve for batch evaluation of the basis curve,
//! then applies offset transformation.
//!
//! Usage:
//! @code
//!   GeomGridEval_OffsetCurve anEvaluator(myGeomOffset);
//!   NCollection_Array1<gp_Pnt> aGrid = anEvaluator.EvaluateGrid(myParams);
//! @endcode
class GeomGridEval_OffsetCurve
{
public:
  DEFINE_STANDARD_ALLOC

  //! Constructor with geometry.
  //! @param theOffset the offset curve geometry to evaluate
  GeomGridEval_OffsetCurve(const occ::handle<Geom_OffsetCurve>& theOffset)
      : myGeom(theOffset),
        myOffset(0.0)
  {
    if (!myGeom.IsNull())
    {
      myOffset    = myGeom->Offset();
      myBasis     = myGeom->BasisCurve();
      myDirection = myGeom->Direction();
    }
  }

  //! Non-copyable and non-movable.
  GeomGridEval_OffsetCurve(const GeomGridEval_OffsetCurve&)            = delete;
  GeomGridEval_OffsetCurve& operator=(const GeomGridEval_OffsetCurve&) = delete;
  GeomGridEval_OffsetCurve(GeomGridEval_OffsetCurve&&)                 = delete;
  GeomGridEval_OffsetCurve& operator=(GeomGridEval_OffsetCurve&&)      = delete;

  //! Returns the geometry handle.
  const occ::handle<Geom_OffsetCurve>& Geometry() const { return myGeom; }

  //! Evaluate all grid points.
  //! @param theParams array of parameter values
  //! @return array of evaluated points (1-based indexing),
  //!         or empty array if geometry is null or no parameters
  Standard_EXPORT NCollection_Array1<gp_Pnt> EvaluateGrid(
    const NCollection_Array1<double>& theParams) const;

  //! Evaluate all grid points with first derivative.
  //! @param theParams array of parameter values
  //! @return array of CurveD1 (1-based indexing),
  //!         or empty array if geometry is null or no parameters
  Standard_EXPORT NCollection_Array1<GeomGridEval::CurveD1> EvaluateGridD1(
    const NCollection_Array1<double>& theParams) const;

  //! Evaluate all grid points with first and second derivatives.
  //! @param theParams array of parameter values
  //! @return array of CurveD2 (1-based indexing),
  //!         or empty array if geometry is null or no parameters
  Standard_EXPORT NCollection_Array1<GeomGridEval::CurveD2> EvaluateGridD2(
    const NCollection_Array1<double>& theParams) const;

  //! Evaluate all grid points with derivatives up to third order.
  //! Uses GeomAdaptor_Curve::D3 for evaluation.
  //! @param theParams array of parameter values
  //! @return array of CurveD3 (1-based indexing),
  //!         or empty array if geometry is null or no parameters
  Standard_EXPORT NCollection_Array1<GeomGridEval::CurveD3> EvaluateGridD3(
    const NCollection_Array1<double>& theParams) const;

  //! Evaluate Nth derivative at all grid points.
  //! For orders 1-3, reuses EvaluateGridD1/D2/D3.
  //! For orders > 3, uses geometry DN method.
  //! @param theParams array of parameter values
  //! @param theN derivative order (N >= 1)
  //! @return array of derivative vectors (1-based indexing)
  Standard_EXPORT NCollection_Array1<gp_Vec> EvaluateGridDN(const NCollection_Array1<double>& theParams,
                                                            int                         theN) const;

private:
  occ::handle<Geom_OffsetCurve> myGeom;
  occ::handle<Geom_Curve>       myBasis;
  double                   myOffset;
  gp_Dir                   myDirection;
};

#endif // _GeomGridEval_OffsetCurve_HeaderFile
