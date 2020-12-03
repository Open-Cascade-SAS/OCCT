// Created on: 1993-04-07
// Created by: Laurent BUCHARD
// Copyright (c) 1993-1999 Matra Datavision
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

#ifndef _IntCurveSurface_TheQuadCurvFuncOfTheQuadCurvExactHInter_HeaderFile
#define _IntCurveSurface_TheQuadCurvFuncOfTheQuadCurvExactHInter_HeaderFile

#include <Adaptor3d_Curve.hxx>
#include <IntSurf_Quadric.hxx>
#include <math_FunctionWithDerivative.hxx>

class IntSurf_Quadric;
class IntCurveSurface_TheHCurveTool;

class IntCurveSurface_TheQuadCurvFuncOfTheQuadCurvExactHInter  : public math_FunctionWithDerivative
{
public:

  DEFINE_STANDARD_ALLOC

  
  //! Create the function.
  Standard_EXPORT IntCurveSurface_TheQuadCurvFuncOfTheQuadCurvExactHInter(const IntSurf_Quadric& Q, const Handle(Adaptor3d_Curve)& C);
  
  //! Computes the value of the signed  distance between
  //! the  implicit surface and  the point  at parameter
  //! Param on the parametrised curve.
  //! Value always returns True.
  Standard_EXPORT Standard_Boolean Value (const Standard_Real Param, Standard_Real& F) Standard_OVERRIDE;
  
  //! Computes the derivative of the previous function at
  //! parameter Param.
  //! Derivative always returns True.
  Standard_EXPORT Standard_Boolean Derivative (const Standard_Real Param, Standard_Real& D) Standard_OVERRIDE;
  
  //! Computes the value and the derivative of the function.
  //! returns True.
  Standard_EXPORT Standard_Boolean Values (const Standard_Real Param, Standard_Real& F, Standard_Real& D) Standard_OVERRIDE;




protected:





private:



  IntSurf_Quadric myQuadric;
  Handle(Adaptor3d_Curve) myCurve;


};







#endif // _IntCurveSurface_TheQuadCurvFuncOfTheQuadCurvExactHInter_HeaderFile
