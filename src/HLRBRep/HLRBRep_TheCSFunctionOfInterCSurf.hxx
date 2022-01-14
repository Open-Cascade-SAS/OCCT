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
class HLRBRep_SurfaceTool;
class gp_Lin;
class HLRBRep_LineTool;
class math_Matrix;
class gp_Pnt;



class HLRBRep_TheCSFunctionOfInterCSurf  : public math_FunctionSetWithDerivatives
{
public:

  DEFINE_STANDARD_ALLOC

  
  Standard_EXPORT HLRBRep_TheCSFunctionOfInterCSurf(const Standard_Address& S, const gp_Lin& C);
  
  Standard_EXPORT Standard_Integer NbVariables() const;
  
  Standard_EXPORT Standard_Integer NbEquations() const;
  
  Standard_EXPORT Standard_Boolean Value (const math_Vector& X, math_Vector& F);
  
  Standard_EXPORT Standard_Boolean Derivatives (const math_Vector& X, math_Matrix& D);
  
  Standard_EXPORT Standard_Boolean Values (const math_Vector& X, math_Vector& F, math_Matrix& D);
  
  Standard_EXPORT const gp_Pnt& Point() const;
  
  Standard_EXPORT Standard_Real Root() const;
  
  Standard_EXPORT const Standard_Address& AuxillarSurface() const;
  
  Standard_EXPORT const gp_Lin& AuxillarCurve() const;




protected:





private:



  Standard_Address surface;
  gp_Lin curve;
  gp_Pnt p;
  Standard_Real f;


};







#endif // _HLRBRep_TheCSFunctionOfInterCSurf_HeaderFile
