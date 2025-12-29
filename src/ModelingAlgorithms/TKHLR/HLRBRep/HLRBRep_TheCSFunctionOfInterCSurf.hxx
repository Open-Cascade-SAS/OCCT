// Created on: 1992-10-14
// Created by: Christophe MARION
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

#ifndef _HLRBRep_TheCSFunctionOfInterCSurf_HeaderFile
#define _HLRBRep_TheCSFunctionOfInterCSurf_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <gp_Lin.hxx>
#include <gp_Pnt.hxx>
#include <math_FunctionSetWithDerivatives.hxx>
#include <Standard_Boolean.hxx>
#include <math_Vector.hxx>
class HLRBRep_Surface;
class HLRBRep_SurfaceTool;
class gp_Lin;
class HLRBRep_LineTool;
class math_Matrix;
class gp_Pnt;

class HLRBRep_TheCSFunctionOfInterCSurf : public math_FunctionSetWithDerivatives
{
public:
  DEFINE_STANDARD_ALLOC

  Standard_EXPORT HLRBRep_TheCSFunctionOfInterCSurf(HLRBRep_Surface* const& S, const gp_Lin& C);

  Standard_EXPORT int NbVariables() const override;

  Standard_EXPORT int NbEquations() const override;

  Standard_EXPORT bool Value(const math_Vector& X, math_Vector& F) override;

  Standard_EXPORT bool Derivatives(const math_Vector& X, math_Matrix& D) override;

  Standard_EXPORT bool Values(const math_Vector& X, math_Vector& F, math_Matrix& D) override;

  Standard_EXPORT const gp_Pnt& Point() const;

  Standard_EXPORT double Root() const;

  Standard_EXPORT HLRBRep_Surface* const& AuxillarSurface() const;

  Standard_EXPORT const gp_Lin& AuxillarCurve() const;

private:
  HLRBRep_Surface* surface;
  gp_Lin           curve;
  gp_Pnt           p;
  double           f;
};

#endif // _HLRBRep_TheCSFunctionOfInterCSurf_HeaderFile
