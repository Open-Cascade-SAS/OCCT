// Created on: 1997-05-13
// Created by: Stagiaire Francois DUMONT
// Copyright (c) 1997-1999 Matra Datavision
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

#ifndef _GeomLib_DenominatorMultiplier_HeaderFile
#define _GeomLib_DenominatorMultiplier_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <TColStd_Array1OfReal.hxx>
#include <Standard_Real.hxx>
class Geom_BSplineSurface;
class Standard_OutOfRange;
class Standard_ConstructionError;


//! this defines an evaluator for a function of 2 variables
//! that will be used by CancelDenominatorDerivative in one
//! direction.
class GeomLib_DenominatorMultiplier 
{
public:

  DEFINE_STANDARD_ALLOC

  
  //! if the surface is rational this will define the evaluator
  //! of a real function of 2 variables a(u,v) such that
  //! if we define a new surface by :
  //! a(u,v) * N(u,v)
  //! NewF(u,v) = ----------------
  //! a(u,v) * D(u,v)
  Standard_EXPORT GeomLib_DenominatorMultiplier(const Handle(Geom_BSplineSurface)& Surface, const TColStd_Array1OfReal& KnotVector);
  
  //! Returns the value of
  //! a(UParameter,VParameter)=
  //!
  //! H0(UParameter)/Denominator(Umin,Vparameter)
  //!
  //! D Denominator(Umin,Vparameter)
  //! - ------------------------------[H1(u)]/(Denominator(Umin,Vparameter)^2)
  //! D U
  //!
  //! + H3(UParameter)/Denominator(Umax,Vparameter)
  //!
  //! D Denominator(Umax,Vparameter)
  //! - ------------------------------[H2(u)]/(Denominator(Umax,Vparameter)^2)
  //! D U
  Standard_EXPORT Standard_Real Value (const Standard_Real UParameter, const Standard_Real VParameter) const;




protected:





private:



  Handle(Geom_BSplineSurface) mySurface;
  TColStd_Array1OfReal myKnotFlatVector;


};







#endif // _GeomLib_DenominatorMultiplier_HeaderFile
