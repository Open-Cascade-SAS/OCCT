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

#ifndef _GeomGridEval_Torus_HeaderFile
#define _GeomGridEval_Torus_HeaderFile

#include <Geom_ToroidalSurface.hxx>
#include <GeomGridEval.hxx>
#include <gp_Pnt2d.hxx>
#include <NCollection_Array1.hxx>
#include <NCollection_Array2.hxx>
#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>

//! @brief Efficient batch evaluator for torus grid points.
//!
//! Uses analytical formula:
//! P(u,v) = Location + (MajorRadius + MinorRadius * cos(v)) * (cos(u) * XDir + sin(u) * YDir) +
//! MinorRadius * sin(v) * ZDir
//!
//! Where U is major angle (0 to 2*PI) and V is minor angle (0 to 2*PI).
//!
//! Usage:
//! @code
//!   GeomGridEval_Torus anEvaluator(myGeomTorus);
//!   NCollection_Array2<gp_Pnt> aGrid = anEvaluator.EvaluateGrid(myUParams, myVParams);
//! @endcode
class GeomGridEval_Torus
{
public:
  DEFINE_STANDARD_ALLOC

  //! Constructor with geometry.
  //! @param theTorus the toroidal surface geometry to evaluate
  GeomGridEval_Torus(const occ::handle<Geom_ToroidalSurface>& theTorus)
      : myGeom(theTorus)
  {
  }

  //! Non-copyable and non-movable.
  GeomGridEval_Torus(const GeomGridEval_Torus&)            = delete;
  GeomGridEval_Torus& operator=(const GeomGridEval_Torus&) = delete;
  GeomGridEval_Torus(GeomGridEval_Torus&&)                 = delete;
  GeomGridEval_Torus& operator=(GeomGridEval_Torus&&)      = delete;

  //! Returns the geometry handle.
  const occ::handle<Geom_ToroidalSurface>& Geometry() const { return myGeom; }

  //! Evaluate grid points at Cartesian product of U and V parameters.
  //! @param theUParams array of U parameter values (major angle)
  //! @param theVParams array of V parameter values (minor angle)
  //! @return 2D array of evaluated points (1-based indexing)
  Standard_EXPORT NCollection_Array2<gp_Pnt> EvaluateGrid(
    const NCollection_Array1<double>& theUParams,
    const NCollection_Array1<double>& theVParams) const;

  //! Evaluate grid points with first partial derivatives.
  //! @param theUParams array of U parameter values (major angle)
  //! @param theVParams array of V parameter values (minor angle)
  //! @return 2D array of SurfD1 (1-based indexing)
  Standard_EXPORT NCollection_Array2<GeomGridEval::SurfD1> EvaluateGridD1(
    const NCollection_Array1<double>& theUParams,
    const NCollection_Array1<double>& theVParams) const;

  //! Evaluate grid points with first and second partial derivatives.
  //! @param theUParams array of U parameter values (major angle)
  //! @param theVParams array of V parameter values (minor angle)
  //! @return 2D array of SurfD2 (1-based indexing)
  Standard_EXPORT NCollection_Array2<GeomGridEval::SurfD2> EvaluateGridD2(
    const NCollection_Array1<double>& theUParams,
    const NCollection_Array1<double>& theVParams) const;

  //! Evaluate grid points with derivatives up to third order.
  //! @param theUParams array of U parameter values (major angle)
  //! @param theVParams array of V parameter values (minor angle)
  //! @return 2D array of SurfD3 (1-based indexing)
  Standard_EXPORT NCollection_Array2<GeomGridEval::SurfD3> EvaluateGridD3(
    const NCollection_Array1<double>& theUParams,
    const NCollection_Array1<double>& theVParams) const;

  //! Evaluate partial derivative d^(NU+NV)S/(dU^NU dV^NV) at all grid points.
  //! For orders 1-3, reuses EvaluateGridD1/D2/D3.
  //! For orders > 3, uses geometry DN method.
  //! @param theUParams array of U parameter values (major angle)
  //! @param theVParams array of V parameter values (minor angle)
  //! @param theNU derivative order in U direction
  //! @param theNV derivative order in V direction
  //! @return 2D array of derivative vectors (1-based indexing)
  Standard_EXPORT NCollection_Array2<gp_Vec> EvaluateGridDN(
    const NCollection_Array1<double>& theUParams,
    const NCollection_Array1<double>& theVParams,
    int                               theNU,
    int                               theNV) const;

private:
  //! Pre-extracted torus data for efficient evaluation.
  struct Data
  {
    double CX, CY, CZ;  //!< Center coordinates
    double XX, XY, XZ;  //!< XDir coordinates
    double YX, YY, YZ;  //!< YDir coordinates
    double ZX, ZY, ZZ;  //!< ZDir coordinates
    double MajorRadius; //!< Major radius
    double MinorRadius; //!< Minor radius
  };

  //! Pre-computed U-dependent values for optimized grid evaluation.
  struct UContext
  {
    double cosU, sinU;
    double dirUX, dirUY, dirUZ;    //!< DirU = cosU*XDir + sinU*YDir
    double dDirUX, dDirUY, dDirUZ; //!< DerivDirU = -sinU*XDir + cosU*YDir
  };

  //! Extract torus data for evaluation.
  Data extractData() const;

  //! Pre-compute U-dependent values.
  static UContext computeUContext(const Data& theData, double theU);

  //! Compute point with pre-computed U context.
  static gp_Pnt computeD0(const Data& theData, const UContext& theUCtx, double theV);

  //! Compute point with D1 with pre-computed U context.
  static GeomGridEval::SurfD1 computeD1(const Data& theData, const UContext& theUCtx, double theV);

  //! Compute point with D2 with pre-computed U context.
  static GeomGridEval::SurfD2 computeD2(const Data& theData, const UContext& theUCtx, double theV);

  //! Compute point with D3 with pre-computed U context.
  static GeomGridEval::SurfD3 computeD3(const Data& theData, const UContext& theUCtx, double theV);

  //! Compute DN with pre-computed U context.
  static gp_Vec computeDN(const Data&     theData,
                          const UContext& theUCtx,
                          double          theV,
                          int             theNU,
                          int             theNV);

  occ::handle<Geom_ToroidalSurface> myGeom;
};

#endif // _GeomGridEval_Torus_HeaderFile
