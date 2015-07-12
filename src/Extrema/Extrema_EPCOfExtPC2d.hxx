// Created on: 1991-02-26
// Created by: Isabelle GRIGNON
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

#ifndef _Extrema_EPCOfExtPC2d_HeaderFile
#define _Extrema_EPCOfExtPC2d_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <Standard_Boolean.hxx>
#include <Standard_Integer.hxx>
#include <Standard_Real.hxx>
#include <Extrema_PCFOfEPCOfExtPC2d.hxx>
class StdFail_NotDone;
class Standard_OutOfRange;
class Standard_TypeMismatch;
class Adaptor2d_Curve2d;
class Extrema_Curve2dTool;
class Extrema_POnCurv2d;
class gp_Pnt2d;
class gp_Vec2d;
class Extrema_PCFOfEPCOfExtPC2d;
class Extrema_SeqPCOfPCFOfEPCOfExtPC2d;



class Extrema_EPCOfExtPC2d 
{
public:

  DEFINE_STANDARD_ALLOC

  
  Standard_EXPORT Extrema_EPCOfExtPC2d();
  
  Standard_EXPORT Extrema_EPCOfExtPC2d(const gp_Pnt2d& P, const Adaptor2d_Curve2d& C, const Standard_Integer NbU, const Standard_Real TolU, const Standard_Real TolF);
  
  Standard_EXPORT Extrema_EPCOfExtPC2d(const gp_Pnt2d& P, const Adaptor2d_Curve2d& C, const Standard_Integer NbU, const Standard_Real Umin, const Standard_Real Usup, const Standard_Real TolU, const Standard_Real TolF);
  
  Standard_EXPORT void Initialize (const Adaptor2d_Curve2d& C, const Standard_Integer NbU, const Standard_Real TolU, const Standard_Real TolF);
  
  Standard_EXPORT void Initialize (const Adaptor2d_Curve2d& C, const Standard_Integer NbU, const Standard_Real Umin, const Standard_Real Usup, const Standard_Real TolU, const Standard_Real TolF);
  
  Standard_EXPORT void Initialize (const Adaptor2d_Curve2d& C);
  
  Standard_EXPORT void Initialize (const Standard_Integer NbU, const Standard_Real Umin, const Standard_Real Usup, const Standard_Real TolU, const Standard_Real TolF);
  
  Standard_EXPORT void Perform (const gp_Pnt2d& P);
  
  Standard_EXPORT Standard_Boolean IsDone() const;
  
  Standard_EXPORT Standard_Integer NbExt() const;
  
  Standard_EXPORT Standard_Real SquareDistance (const Standard_Integer N) const;
  
  Standard_EXPORT Standard_Boolean IsMin (const Standard_Integer N) const;
  
  Standard_EXPORT const Extrema_POnCurv2d& Point (const Standard_Integer N) const;




protected:





private:



  Standard_Boolean myDone;
  Standard_Boolean myInit;
  Standard_Integer mynbsample;
  Standard_Real myumin;
  Standard_Real myusup;
  Standard_Real mytolu;
  Standard_Real mytolF;
  Extrema_PCFOfEPCOfExtPC2d myF;


};







#endif // _Extrema_EPCOfExtPC2d_HeaderFile
