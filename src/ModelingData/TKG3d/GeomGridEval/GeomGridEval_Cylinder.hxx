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

#ifndef _GeomGridEval_Cylinder_HeaderFile
#define _GeomGridEval_Cylinder_HeaderFile

#include <Geom_CylindricalSurface.hxx>
#include <GeomGridEval.hxx>
#include <NCollection_Array1.hxx>
#include <NCollection_Array2.hxx>
#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>

//! @brief Efficient batch evaluator for cylinder grid points.
//!
//! Uses analytical formula:
//! P(u,v) = Location + R * (cos(u) * XDir + sin(u) * YDir) + v * ZDir
//!
//! Where U is angle (0 to 2*PI) and V is height.
//!
//! Usage:
//! @code
//!   GeomGridEval_Cylinder anEvaluator(myGeomCylinder);
//!   NCollection_Array2<gp_Pnt> aGrid = anEvaluator.EvaluateGrid(myUParams, myVParams);
//! @endcode
class GeomGridEval_Cylinder
{
public:
  DEFINE_STANDARD_ALLOC

  //! Constructor with geometry.
  //! @param theCylinder the cylindrical surface geometry to evaluate
  GeomGridEval_Cylinder(const occ::handle<Geom_CylindricalSurface>& theCylinder)
      : myGeom(theCylinder)
  {
  }

  //! Non-copyable and non-movable.
  GeomGridEval_Cylinder(const GeomGridEval_Cylinder&)            = delete;
  GeomGridEval_Cylinder& operator=(const GeomGridEval_Cylinder&) = delete;
  GeomGridEval_Cylinder(GeomGridEval_Cylinder&&)                 = delete;
  GeomGridEval_Cylinder& operator=(GeomGridEval_Cylinder&&)      = delete;

  //! Returns the geometry handle.
  const occ::handle<Geom_CylindricalSurface>& Geometry() const { return myGeom; }

  //! Evaluate grid points at Cartesian product of U and V parameters.
  //! @param theUParams array of U parameter values (angle)
  //! @param theVParams array of V parameter values (height)
  //! @return 2D array of evaluated points (1-based indexing)
  Standard_EXPORT NCollection_Array2<gp_Pnt> EvaluateGrid(
    const NCollection_Array1<double>& theUParams,
    const NCollection_Array1<double>& theVParams) const;

  //! Evaluate grid points with first partial derivatives.
  //! @param theUParams array of U parameter values (angle)
  //! @param theVParams array of V parameter values (height)
  //! @return 2D array of SurfD1 (1-based indexing)
  Standard_EXPORT NCollection_Array2<GeomGridEval::SurfD1> EvaluateGridD1(
    const NCollection_Array1<double>& theUParams,
    const NCollection_Array1<double>& theVParams) const;

  //! Evaluate grid points with first and second partial derivatives.
  //! @param theUParams array of U parameter values (angle)
  //! @param theVParams array of V parameter values (height)
  //! @return 2D array of SurfD2 (1-based indexing)
  Standard_EXPORT NCollection_Array2<GeomGridEval::SurfD2> EvaluateGridD2(
    const NCollection_Array1<double>& theUParams,
    const NCollection_Array1<double>& theVParams) const;

  //! Evaluate grid points with derivatives up to third order.
  //! @param theUParams array of U parameter values (angle)
  //! @param theVParams array of V parameter values (height)
  //! @return 2D array of SurfD3 (1-based indexing)
  Standard_EXPORT NCollection_Array2<GeomGridEval::SurfD3> EvaluateGridD3(
    const NCollection_Array1<double>& theUParams,
    const NCollection_Array1<double>& theVParams) const;

  //! Evaluate partial derivative d^(NU+NV)S/(dU^NU dV^NV) at all grid points.
  //! For a cylinder:
  //! - U derivatives are cyclic (period 4): D_nU = R * (cyclic trig)
  //! - V derivatives: D1V = ZDir, higher = 0
  //! - Mixed: D_{nu,nv} = 0 for nv > 1
  //! @param theUParams array of U parameter values (angle)
  //! @param theVParams array of V parameter values (height)
  //! @param theNU derivative order in U direction
  //! @param theNV derivative order in V direction
  //! @return 2D array of derivative vectors (1-based indexing)
  Standard_EXPORT NCollection_Array2<gp_Vec> EvaluateGridDN(
    const NCollection_Array1<double>& theUParams,
    const NCollection_Array1<double>& theVParams,
    int                               theNU,
    int                               theNV) const;

private:
  //! Pre-extracted cylinder data for efficient evaluation.
  struct Data
  {
    double CX, CY, CZ; //!< Center coordinates
    double XX, XY, XZ; //!< XDir coordinates
    double YX, YY, YZ; //!< YDir coordinates
    double ZX, ZY, ZZ; //!< ZDir coordinates
    double Radius;
  };

  //! Pre-computed U-dependent values for optimized grid evaluation.
  struct UContext
  {
    double cosU, sinU;
    double dirUX, dirUY, dirUZ;    //!< DirU = cosU*XDir + sinU*YDir (circle point direction)
    double dDirUX, dDirUY, dDirUZ; //!< DerivDirU = -sinU*XDir + cosU*YDir
  };

  //! Extract cylinder data for evaluation.
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

  occ::handle<Geom_CylindricalSurface> myGeom;
};

#endif // _GeomGridEval_Cylinder_HeaderFile
