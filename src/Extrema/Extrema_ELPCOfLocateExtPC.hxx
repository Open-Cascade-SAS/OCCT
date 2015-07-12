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

#ifndef _Extrema_ELPCOfLocateExtPC_HeaderFile
#define _Extrema_ELPCOfLocateExtPC_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <Standard_Address.hxx>
#include <gp_Pnt.hxx>
#include <Extrema_ExtPElC.hxx>
#include <Extrema_SequenceOfPOnCurv.hxx>
#include <Standard_Boolean.hxx>
#include <Standard_Real.hxx>
#include <Extrema_EPCOfELPCOfLocateExtPC.hxx>
#include <Standard_Integer.hxx>
#include <GeomAbs_CurveType.hxx>
#include <TColStd_SequenceOfBoolean.hxx>
#include <TColStd_SequenceOfReal.hxx>
class StdFail_NotDone;
class Standard_OutOfRange;
class Standard_TypeMismatch;
class Adaptor3d_Curve;
class Extrema_CurveTool;
class Extrema_ExtPElC;
class gp_Pnt;
class gp_Vec;
class Extrema_POnCurv;
class Extrema_EPCOfELPCOfLocateExtPC;
class Extrema_PCFOfEPCOfELPCOfLocateExtPC;



class Extrema_ELPCOfLocateExtPC 
{
public:

  DEFINE_STANDARD_ALLOC

  
  Standard_EXPORT Extrema_ELPCOfLocateExtPC();
  
  Standard_EXPORT Extrema_ELPCOfLocateExtPC(const gp_Pnt& P, const Adaptor3d_Curve& C, const Standard_Real Uinf, const Standard_Real Usup, const Standard_Real TolF = 1.0e-10);
  
  Standard_EXPORT Extrema_ELPCOfLocateExtPC(const gp_Pnt& P, const Adaptor3d_Curve& C, const Standard_Real TolF = 1.0e-10);
  
  Standard_EXPORT void Initialize (const Adaptor3d_Curve& C, const Standard_Real Uinf, const Standard_Real Usup, const Standard_Real TolF = 1.0e-10);
  
  Standard_EXPORT void Perform (const gp_Pnt& P);
  
  Standard_EXPORT Standard_Boolean IsDone() const;
  
  Standard_EXPORT Standard_Real SquareDistance (const Standard_Integer N) const;
  
  Standard_EXPORT Standard_Integer NbExt() const;
  
  Standard_EXPORT Standard_Boolean IsMin (const Standard_Integer N) const;
  
  Standard_EXPORT const Extrema_POnCurv& Point (const Standard_Integer N) const;
  
  Standard_EXPORT void TrimmedSquareDistances (Standard_Real& dist1, Standard_Real& dist2, gp_Pnt& P1, gp_Pnt& P2) const;




protected:

  
  Standard_EXPORT void IntervalPerform (const gp_Pnt& P);




private:



  Standard_Address myC;
  gp_Pnt Pf;
  gp_Pnt Pl;
  Extrema_ExtPElC myExtPElC;
  Extrema_SequenceOfPOnCurv mypoint;
  Standard_Boolean mydone;
  Standard_Real mydist1;
  Standard_Real mydist2;
  Extrema_EPCOfELPCOfLocateExtPC myExtPC;
  Standard_Real mytolu;
  Standard_Real mytolf;
  Standard_Integer mysample;
  Standard_Real myintuinf;
  Standard_Real myintusup;
  Standard_Real myuinf;
  Standard_Real myusup;
  GeomAbs_CurveType type;
  TColStd_SequenceOfBoolean myismin;
  TColStd_SequenceOfReal mySqDist;


};







#endif // _Extrema_ELPCOfLocateExtPC_HeaderFile
