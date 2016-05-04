// Created on: 2016-05-10
// Created by: Alexander MALYSHEV
// Copyright (c) 1991-1999 Matra Datavision
// Copyright (c) 1999-2016 OPEN CASCADE SAS
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

#ifndef _Extrema_FuncPSDsit_HeaderFile
#define _Extrema_FuncPSDsit_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>

#include <math_MultipleVarFunctionWithGradient.hxx>
#include <gp_Pnt.hxx>
#include <Adaptor3d_Surface.hxx>

#include <Standard_Real.hxx>
#include <Standard_Boolean.hxx>
#include <Standard_Integer.hxx>

class math_Vector;

//! Functional for search of extremum of the square Euclidean distance between point P and
//! surface S, starting from approximate solution (u0, v0).
//!
//! The class inherits math_MultipleVarFunctionWithGradient and thus is intended
//! for use in math_BFGS algorithm.
//!
//! The criteria is:
//! F = SquareDist(P, S(u, v)) - > min
//!
//! The first derivative are:
//! F1(u,v) = (S(u,v) - P) * Su
//! F2(u,v) = (S(u,v) - P) * Sv
//!
//! Su and Sv are first derivatives of the surface, * symbol means dot product.
class Extrema_FuncPSDist  : public math_MultipleVarFunctionWithGradient
{
public:

  DEFINE_STANDARD_ALLOC

  //! Constructor.
  Standard_EXPORT Extrema_FuncPSDist(const Adaptor3d_Surface& theS,
                                     const gp_Pnt& theP);

  //! Number of variables.
  Standard_EXPORT Standard_Integer NbVariables() const Standard_OVERRIDE;

  //! Value.
  Standard_EXPORT Standard_Boolean Value(const math_Vector& X,Standard_Real& F) Standard_OVERRIDE;

  //! Gradient.
  Standard_EXPORT Standard_Boolean Gradient(const math_Vector& X,math_Vector& G) Standard_OVERRIDE;

  //! Value and gradient.
  Standard_EXPORT Standard_Boolean Values(const math_Vector& X,Standard_Real& F,math_Vector& G) Standard_OVERRIDE;

private:


  //! Check point is inside of the surface parameter space.
  //! Returns true if inside and false otherwise.
  Standard_Boolean IsInside(const math_Vector& X);

  const Extrema_FuncPSDist& operator=(const Extrema_FuncPSDist&);
  Extrema_FuncPSDist(const Extrema_FuncPSDist&);

  const Adaptor3d_Surface &mySurf;
  const gp_Pnt &myP;
};
#endif // _Extrema_FuncPSDsit_HeaderFile
