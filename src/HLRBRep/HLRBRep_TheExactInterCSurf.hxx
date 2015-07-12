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

#ifndef _HLRBRep_TheExactInterCSurf_HeaderFile
#define _HLRBRep_TheExactInterCSurf_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <Standard_Boolean.hxx>
#include <HLRBRep_TheCSFunctionOfInterCSurf.hxx>
#include <Standard_Real.hxx>
#include <Standard_Address.hxx>
class StdFail_NotDone;
class Standard_DomainError;
class HLRBRep_SurfaceTool;
class gp_Lin;
class HLRBRep_LineTool;
class HLRBRep_TheCSFunctionOfInterCSurf;
class math_FunctionSetRoot;
class gp_Pnt;



class HLRBRep_TheExactInterCSurf 
{
public:

  DEFINE_STANDARD_ALLOC

  
  Standard_EXPORT HLRBRep_TheExactInterCSurf(const Standard_Real U, const Standard_Real V, const Standard_Real W, const HLRBRep_TheCSFunctionOfInterCSurf& F, const Standard_Real TolTangency, const Standard_Real MarginCoef = 0.0);
  
  Standard_EXPORT HLRBRep_TheExactInterCSurf(const HLRBRep_TheCSFunctionOfInterCSurf& F, const Standard_Real TolTangency);
  
  Standard_EXPORT void Perform (const Standard_Real U, const Standard_Real V, const Standard_Real W, math_FunctionSetRoot& Rsnld, const Standard_Real u0, const Standard_Real v0, const Standard_Real u1, const Standard_Real v1, const Standard_Real w0, const Standard_Real w1);
  
  Standard_EXPORT Standard_Boolean IsDone() const;
  
  Standard_EXPORT Standard_Boolean IsEmpty() const;
  
  Standard_EXPORT const gp_Pnt& Point() const;
  
  Standard_EXPORT Standard_Real ParameterOnCurve() const;
  
  Standard_EXPORT void ParameterOnSurface (Standard_Real& U, Standard_Real& V) const;
  
  Standard_EXPORT HLRBRep_TheCSFunctionOfInterCSurf& Function();




protected:





private:



  Standard_Boolean done;
  Standard_Boolean empty;
  HLRBRep_TheCSFunctionOfInterCSurf myFunction;
  Standard_Real w;
  Standard_Real u;
  Standard_Real v;
  Standard_Real tol;


};







#endif // _HLRBRep_TheExactInterCSurf_HeaderFile
