// Created on: 1994-02-14
// Created by: Jacques GOUSSARD
// Copyright (c) 1994-1999 Matra Datavision
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

#ifndef _IntPatch_CSFunction_HeaderFile
#define _IntPatch_CSFunction_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <Standard_Address.hxx>
#include <gp_Pnt.hxx>
#include <Standard_Real.hxx>
#include <math_FunctionSetWithDerivatives.hxx>
#include <Standard_Integer.hxx>
#include <Standard_Boolean.hxx>
#include <math_Vector.hxx>
class Adaptor3d_HSurface;
class Adaptor2d_HCurve2d;
class math_Matrix;
class gp_Pnt;


//! this function is associated to the intersection between
//! a curve on surface and a surface  .
class IntPatch_CSFunction  : public math_FunctionSetWithDerivatives
{
public:

  DEFINE_STANDARD_ALLOC

  
  //! S1 is the surface on which the intersection is searched.
  //! C is a curve on the surface S2.
  Standard_EXPORT IntPatch_CSFunction(const Handle(Adaptor3d_HSurface)& S1, const Handle(Adaptor2d_HCurve2d)& C, const Handle(Adaptor3d_HSurface)& S2);
  
  Standard_EXPORT Standard_Integer NbVariables() const;
  
  Standard_EXPORT Standard_Integer NbEquations() const;
  
  Standard_EXPORT Standard_Boolean Value (const math_Vector& X, math_Vector& F);
  
  Standard_EXPORT Standard_Boolean Derivatives (const math_Vector& X, math_Matrix& D);
  
  Standard_EXPORT Standard_Boolean Values (const math_Vector& X, math_Vector& F, math_Matrix& D);
  
  Standard_EXPORT const gp_Pnt& Point() const;
  
  Standard_EXPORT Standard_Real Root() const;
  
  Standard_EXPORT const Handle(Adaptor3d_HSurface)& AuxillarSurface() const;
  
  Standard_EXPORT const Handle(Adaptor2d_HCurve2d)& AuxillarCurve() const;




protected:





private:



  Standard_Address curve;
  Standard_Address surface1;
  Standard_Address surface2;
  gp_Pnt p;
  Standard_Real f;


};







#endif // _IntPatch_CSFunction_HeaderFile
