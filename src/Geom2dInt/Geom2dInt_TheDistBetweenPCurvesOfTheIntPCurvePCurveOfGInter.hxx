// Created on: 1992-06-04
// Created by: Jacques GOUSSARD
// Copyright (c) 1992-1999 Matra Datavision
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

#ifndef _Geom2dInt_TheDistBetweenPCurvesOfTheIntPCurvePCurveOfGInter_HeaderFile
#define _Geom2dInt_TheDistBetweenPCurvesOfTheIntPCurvePCurveOfGInter_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <Standard_Address.hxx>
#include <math_FunctionSetWithDerivatives.hxx>
#include <Standard_Integer.hxx>
#include <Standard_Boolean.hxx>
#include <math_Vector.hxx>
class Adaptor2d_Curve2d;
class Geom2dInt_Geom2dCurveTool;
class math_Matrix;



class Geom2dInt_TheDistBetweenPCurvesOfTheIntPCurvePCurveOfGInter  : public math_FunctionSetWithDerivatives
{
public:

  DEFINE_STANDARD_ALLOC

  
  Standard_EXPORT Geom2dInt_TheDistBetweenPCurvesOfTheIntPCurvePCurveOfGInter(const Adaptor2d_Curve2d& curve1, const Adaptor2d_Curve2d& curve2);
  
  //! returns 2.
  Standard_EXPORT Standard_Integer NbVariables() const;
  
  //! returns 2.
  Standard_EXPORT Standard_Integer NbEquations() const;
  
  //! computes the values <F> of the Functions for the
  //! variable <X>.
  //! returns True if the computation was done successfully,
  //! False otherwise.
  Standard_EXPORT Standard_Boolean Value (const math_Vector& X, math_Vector& F);
  
  //! returns the values <D> of the derivatives for the
  //! variable <X>.
  //! returns True if the computation was done successfully,
  //! False otherwise.
  Standard_EXPORT Standard_Boolean Derivatives (const math_Vector& X, math_Matrix& D);
  
  //! returns the values <F> of the functions and the derivatives
  //! <D> for the variable <X>.
  //! returns True if the computation was done successfully,
  //! False otherwise.
  Standard_EXPORT Standard_Boolean Values (const math_Vector& X, math_Vector& F, math_Matrix& D);




protected:





private:



  Standard_Address thecurve1;
  Standard_Address thecurve2;


};







#endif // _Geom2dInt_TheDistBetweenPCurvesOfTheIntPCurvePCurveOfGInter_HeaderFile
