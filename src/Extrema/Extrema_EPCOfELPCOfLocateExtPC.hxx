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

#ifndef _Extrema_EPCOfELPCOfLocateExtPC_HeaderFile
#define _Extrema_EPCOfELPCOfLocateExtPC_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <Standard_Boolean.hxx>
#include <Standard_Integer.hxx>
#include <Standard_Real.hxx>
#include <Extrema_PCFOfEPCOfELPCOfLocateExtPC.hxx>
class StdFail_NotDone;
class Standard_OutOfRange;
class Standard_TypeMismatch;
class Adaptor3d_Curve;
class Extrema_CurveTool;
class Extrema_POnCurv;
class gp_Pnt;
class gp_Vec;
class Extrema_PCFOfEPCOfELPCOfLocateExtPC;
class Extrema_SeqPCOfPCFOfEPCOfELPCOfLocateExtPC;



class Extrema_EPCOfELPCOfLocateExtPC 
{
public:

  DEFINE_STANDARD_ALLOC

  
  Standard_EXPORT Extrema_EPCOfELPCOfLocateExtPC();
  
  Standard_EXPORT Extrema_EPCOfELPCOfLocateExtPC(const gp_Pnt& P, const Adaptor3d_Curve& C, const Standard_Integer NbU, const Standard_Real TolU, const Standard_Real TolF);
  
  Standard_EXPORT Extrema_EPCOfELPCOfLocateExtPC(const gp_Pnt& P, const Adaptor3d_Curve& C, const Standard_Integer NbU, const Standard_Real Umin, const Standard_Real Usup, const Standard_Real TolU, const Standard_Real TolF);
  
  Standard_EXPORT void Initialize (const Adaptor3d_Curve& C, const Standard_Integer NbU, const Standard_Real TolU, const Standard_Real TolF);
  
  Standard_EXPORT void Initialize (const Adaptor3d_Curve& C, const Standard_Integer NbU, const Standard_Real Umin, const Standard_Real Usup, const Standard_Real TolU, const Standard_Real TolF);
  
  Standard_EXPORT void Initialize (const Adaptor3d_Curve& C);
  
  Standard_EXPORT void Initialize (const Standard_Integer NbU, const Standard_Real Umin, const Standard_Real Usup, const Standard_Real TolU, const Standard_Real TolF);
  
  Standard_EXPORT void Perform (const gp_Pnt& P);
  
  Standard_EXPORT Standard_Boolean IsDone() const;
  
  Standard_EXPORT Standard_Integer NbExt() const;
  
  Standard_EXPORT Standard_Real SquareDistance (const Standard_Integer N) const;
  
  Standard_EXPORT Standard_Boolean IsMin (const Standard_Integer N) const;
  
  Standard_EXPORT const Extrema_POnCurv& Point (const Standard_Integer N) const;




protected:





private:



  Standard_Boolean myDone;
  Standard_Boolean myInit;
  Standard_Integer mynbsample;
  Standard_Real myumin;
  Standard_Real myusup;
  Standard_Real mytolu;
  Standard_Real mytolF;
  Extrema_PCFOfEPCOfELPCOfLocateExtPC myF;


};







#endif // _Extrema_EPCOfELPCOfLocateExtPC_HeaderFile
