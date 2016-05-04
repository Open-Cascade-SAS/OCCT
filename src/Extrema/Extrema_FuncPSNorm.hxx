// Created on: 1991-07-24
// Created by: Michel CHAUVAT
// Copyright (c) 1991-1999 Matra Datavision
// Copyright (c) 1999-2014 OPEN CASCADE SAS
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

#ifndef _Extrema_FunctPSNorm_HeaderFile
#define _Extrema_FunctPSNorm_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>

#include <gp_Pnt.hxx>
#include <Adaptor3d_SurfacePtr.hxx>
#include <Standard_Real.hxx>
#include <TColStd_SequenceOfReal.hxx>
#include <Extrema_SequenceOfPOnSurf.hxx>
#include <Standard_Boolean.hxx>
#include <math_FunctionSetWithDerivatives.hxx>
#include <Standard_Integer.hxx>
#include <math_Vector.hxx>
class Standard_OutOfRange;
class gp_Pnt;
class Adaptor3d_Surface;
class math_Matrix;
class Extrema_POnSurf;



//! Functional for search of extremum of the distance between point P and
//! surface S, starting from approximate solution (u0, v0).
//!
//! The class inherits math_FunctionSetWithDerivatives and thus is intended
//! for use in math_FunctionSetRoot algorithm .
//!
//! Denoting derivatives of the surface S(u,v) by u and v, respectively, as
//! Su and Sv, the two functions to be nullified are:
//!
//! F1(u,v) = (S - P) * Su
//! F2(u,v) = (S - P) * Sv
//!
//! The derivatives of the functional are:
//!
//! Duf1(u,v) = Su^2    + (S-P) * Suu;
//! Dvf1(u,v) = Su * Sv + (S-P) * Suv
//! Duf2(u,v) = Sv * Su + (S-P) * Suv = Dvf1
//! Dvf2(u,v) = Sv^2    + (S-P) * Svv
//!
//! Here * denotes scalar product, and ^2 is square power.
class Extrema_FuncPSNorm  : public math_FunctionSetWithDerivatives
{
public:

  DEFINE_STANDARD_ALLOC

  
  Standard_EXPORT Extrema_FuncPSNorm();
  
  Standard_EXPORT Extrema_FuncPSNorm(const gp_Pnt& P, const Adaptor3d_Surface& S);
  
  //! sets the field mysurf of the function.
  Standard_EXPORT void Initialize (const Adaptor3d_Surface& S);
  
  //! sets the field mysurf of the function.
  Standard_EXPORT void SetPoint (const gp_Pnt& P);
  
  Standard_EXPORT Standard_Integer NbVariables() const Standard_OVERRIDE;
  
  Standard_EXPORT Standard_Integer NbEquations() const Standard_OVERRIDE;
  
  //! Calculate Fi(U,V).
  Standard_EXPORT Standard_Boolean Value (const math_Vector& UV, math_Vector& F) Standard_OVERRIDE;
  
  //! Calculate Fi'(U,V).
  Standard_EXPORT Standard_Boolean Derivatives (const math_Vector& UV, math_Matrix& DF) Standard_OVERRIDE;
  
  //! Calculate Fi(U,V) and Fi'(U,V).
  Standard_EXPORT Standard_Boolean Values (const math_Vector& UV, math_Vector& F, math_Matrix& DF) Standard_OVERRIDE;
  
  //! Save the found extremum.
  Standard_EXPORT virtual Standard_Integer GetStateNumber() Standard_OVERRIDE;
  
  //! Return the number of found extrema.
  Standard_EXPORT Standard_Integer NbExt() const;
  
  //! Return the value of the Nth distance.
  Standard_EXPORT Standard_Real SquareDistance (const Standard_Integer N) const;
  
  //! Returns the Nth extremum.
  Standard_EXPORT const Extrema_POnSurf& Point (const Standard_Integer N) const;

private:

  gp_Pnt myP;
  Adaptor3d_SurfacePtr myS;
  Standard_Real myU;
  Standard_Real myV;
  gp_Pnt myPs;
  TColStd_SequenceOfReal mySqDist;
  Extrema_SequenceOfPOnSurf myPoint;
  Standard_Boolean myPinit;
  Standard_Boolean mySinit;
};
#endif // _Extrema_FunctPSNorm_HeaderFile
