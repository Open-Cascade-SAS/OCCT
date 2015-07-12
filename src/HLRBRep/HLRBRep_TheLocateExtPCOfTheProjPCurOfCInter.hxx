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

#ifndef _HLRBRep_TheLocateExtPCOfTheProjPCurOfCInter_HeaderFile
#define _HLRBRep_TheLocateExtPCOfTheProjPCurOfCInter_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <Standard_Boolean.hxx>
#include <Standard_Real.hxx>
#include <HLRBRep_PCLocFOfTheLocateExtPCOfTheProjPCurOfCInter.hxx>
#include <Standard_Address.hxx>
class Standard_DomainError;
class Standard_TypeMismatch;
class StdFail_NotDone;
class HLRBRep_CurveTool;
class Extrema_POnCurv2d;
class gp_Pnt2d;
class gp_Vec2d;
class HLRBRep_PCLocFOfTheLocateExtPCOfTheProjPCurOfCInter;
class HLRBRep_SeqPCOfPCLocFOfTheLocateExtPCOfTheProjPCurOfCInter;



class HLRBRep_TheLocateExtPCOfTheProjPCurOfCInter 
{
public:

  DEFINE_STANDARD_ALLOC

  
  Standard_EXPORT HLRBRep_TheLocateExtPCOfTheProjPCurOfCInter();
  
  Standard_EXPORT HLRBRep_TheLocateExtPCOfTheProjPCurOfCInter(const gp_Pnt2d& P, const Standard_Address& C, const Standard_Real U0, const Standard_Real TolU);
  
  Standard_EXPORT HLRBRep_TheLocateExtPCOfTheProjPCurOfCInter(const gp_Pnt2d& P, const Standard_Address& C, const Standard_Real U0, const Standard_Real Umin, const Standard_Real Usup, const Standard_Real TolU);
  
  Standard_EXPORT void Initialize (const Standard_Address& C, const Standard_Real Umin, const Standard_Real Usup, const Standard_Real TolU);
  
  Standard_EXPORT void Perform (const gp_Pnt2d& P, const Standard_Real U0);
  
  Standard_EXPORT Standard_Boolean IsDone() const;
  
  Standard_EXPORT Standard_Real SquareDistance() const;
  
  Standard_EXPORT Standard_Boolean IsMin() const;
  
  Standard_EXPORT const Extrema_POnCurv2d& Point() const;




protected:





private:



  Standard_Boolean myDone;
  Standard_Real mytolU;
  Standard_Real myumin;
  Standard_Real myusup;
  HLRBRep_PCLocFOfTheLocateExtPCOfTheProjPCurOfCInter myF;


};







#endif // _HLRBRep_TheLocateExtPCOfTheProjPCurOfCInter_HeaderFile
