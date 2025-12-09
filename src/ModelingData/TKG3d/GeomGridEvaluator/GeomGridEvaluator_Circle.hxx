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

#ifndef _GeomGridEvaluator_Circle_HeaderFile
#define _GeomGridEvaluator_Circle_HeaderFile

#include <Geom_Circle.hxx>
#include <gp_Pnt.hxx>
#include <NCollection_Array1.hxx>
#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <TColStd_Array1OfReal.hxx>

//! @brief Efficient batch evaluator for circle grid points.
//!
//! Uses analytical formula: P(u) = Center + R * (cos(u) * XDir + sin(u) * YDir)
//!
//! Usage:
//! @code
//!   GeomGridEvaluator_Circle anEvaluator(myGeomCircle);
//!   anEvaluator.SetParams(myParams);
//!   NCollection_Array1<gp_Pnt> aGrid = anEvaluator.EvaluateGrid();
//! @endcode
class GeomGridEvaluator_Circle
{
public:
  DEFINE_STANDARD_ALLOC

  //! Constructor with geometry.
  //! @param theCircle the circle geometry to evaluate
  GeomGridEvaluator_Circle(const Handle(Geom_Circle)& theCircle)
      : myGeom(theCircle)
  {
  }

  //! Set parameters for grid evaluation (by const reference).
  //! @param theParams array of parameter values (angles in radians)
  void SetParams(const TColStd_Array1OfReal& theParams);

  //! Set parameters for grid evaluation (by move).
  //! @param theParams array of parameter values to move
  void SetParams(NCollection_Array1<double>&& theParams) { myParams = std::move(theParams); }

  //! Returns the geometry handle.
  const Handle(Geom_Circle)& Geometry() const { return myGeom; }

  //! Returns number of parameters.
  int NbParams() const { return myParams.Size(); }

  //! Evaluate all grid points.
  //! @return array of evaluated points (1-based indexing),
  //!         or empty array if geometry is null or no parameters set
  Standard_EXPORT NCollection_Array1<gp_Pnt> EvaluateGrid() const;

private:
  Handle(Geom_Circle)        myGeom;
  NCollection_Array1<double> myParams;
};

#endif // _GeomGridEvaluator_Circle_HeaderFile
