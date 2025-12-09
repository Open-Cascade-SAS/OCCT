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

#ifndef _GeomGridEval_Ellipse_HeaderFile
#define _GeomGridEval_Ellipse_HeaderFile

#include <Geom_Ellipse.hxx>
#include <GeomGridEval.hxx>
#include <NCollection_Array1.hxx>
#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <TColStd_Array1OfReal.hxx>

//! @brief Efficient batch evaluator for ellipse grid points.
//!
//! Uses analytical formula:
//! P(u) = Center + MajorRadius * cos(u) * XDir + MinorRadius * sin(u) * YDir
//!
//! Usage:
//! @code
//!   GeomGridEval_Ellipse anEvaluator(myGeomEllipse);
//!   anEvaluator.SetParams(myParams);
//!   NCollection_Array1<gp_Pnt> aGrid = anEvaluator.EvaluateGrid();
//! @endcode
class GeomGridEval_Ellipse
{
public:
  DEFINE_STANDARD_ALLOC

  //! Constructor with geometry.
  //! @param theEllipse the ellipse geometry to evaluate
  GeomGridEval_Ellipse(const Handle(Geom_Ellipse)& theEllipse)
      : myGeom(theEllipse)
  {
  }

  //! Set parameter values.
  //! @param theParams array of parameter values
  Standard_EXPORT void SetParams(const TColStd_Array1OfReal& theParams);

  //! Returns the geometry handle.
  const Handle(Geom_Ellipse)& Geometry() const { return myGeom; }

  //! Returns number of parameters.
  int NbParams() const { return myParams.Size(); }

  //! Evaluate all grid points.
  //! @return array of evaluated points (1-based indexing),
  //!         or empty array if geometry is null or no parameters set
  Standard_EXPORT NCollection_Array1<gp_Pnt> EvaluateGrid() const;

  //! Evaluate all grid points with first derivative.
  //! @return array of CurveD1 (1-based indexing)
  Standard_EXPORT NCollection_Array1<GeomGridEval::CurveD1> EvaluateGridD1() const;

  //! Evaluate all grid points with first and second derivatives.
  //! @return array of CurveD2 (1-based indexing)
  Standard_EXPORT NCollection_Array1<GeomGridEval::CurveD2> EvaluateGridD2() const;

  //! Evaluate all grid points with first, second, and third derivatives.
  //! @return array of CurveD3 (1-based indexing)
  Standard_EXPORT NCollection_Array1<GeomGridEval::CurveD3> EvaluateGridD3() const;

private:
  Handle(Geom_Ellipse)       myGeom;
  NCollection_Array1<double> myParams;
};

#endif // _GeomGridEval_Ellipse_HeaderFile
