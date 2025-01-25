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

#ifndef _IntCurveSurface_TheCSFunctionOfHInter_HeaderFile
#define _IntCurveSurface_TheCSFunctionOfHInter_HeaderFile

#include <Adaptor3d_Curve.hxx>
#include <Adaptor3d_Surface.hxx>
#include <gp_Pnt.hxx>
#include <math_FunctionSetWithDerivatives.hxx>
#include <math_Vector.hxx>

class Adaptor3d_HSurfaceTool;
class IntCurveSurface_TheHCurveTool;
class math_Matrix;

class IntCurveSurface_TheCSFunctionOfHInter : public math_FunctionSetWithDerivatives
{
public:
  DEFINE_STANDARD_ALLOC

  Standard_EXPORT IntCurveSurface_TheCSFunctionOfHInter(const Handle(Adaptor3d_Surface)& S,
                                                        const Handle(Adaptor3d_Curve)&   C);

  Standard_EXPORT Standard_Integer NbVariables() const;

  Standard_EXPORT Standard_Integer NbEquations() const;

  Standard_EXPORT Standard_Boolean Value(const math_Vector& X, math_Vector& F);

  Standard_EXPORT Standard_Boolean Derivatives(const math_Vector& X, math_Matrix& D);

  Standard_EXPORT Standard_Boolean Values(const math_Vector& X, math_Vector& F, math_Matrix& D);

  Standard_EXPORT const gp_Pnt& Point() const;

  Standard_EXPORT Standard_Real Root() const;

  Standard_EXPORT const Handle(Adaptor3d_Surface)& AuxillarSurface() const;

  Standard_EXPORT const Handle(Adaptor3d_Curve)& AuxillarCurve() const;

protected:
private:
  Handle(Adaptor3d_Surface) surface;
  Handle(Adaptor3d_Curve)   curve;
  gp_Pnt                    p;
  Standard_Real             f;
};

#endif // _IntCurveSurface_TheCSFunctionOfHInter_HeaderFile
