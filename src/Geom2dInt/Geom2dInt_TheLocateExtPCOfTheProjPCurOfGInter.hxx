// Created on: 1992-06-04
// Created by: Jacques GOUSSARD
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

#ifndef _Geom2dInt_TheLocateExtPCOfTheProjPCurOfGInter_HeaderFile
#define _Geom2dInt_TheLocateExtPCOfTheProjPCurOfGInter_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <Standard_Boolean.hxx>
#include <Standard_Real.hxx>
#include <Geom2dInt_PCLocFOfTheLocateExtPCOfTheProjPCurOfGInter.hxx>
class Standard_DomainError;
class Standard_TypeMismatch;
class StdFail_NotDone;
class Adaptor2d_Curve2d;
class Geom2dInt_Geom2dCurveTool;
class Extrema_POnCurv2d;
class gp_Pnt2d;
class gp_Vec2d;
class Geom2dInt_PCLocFOfTheLocateExtPCOfTheProjPCurOfGInter;
class Geom2dInt_SeqPCOfPCLocFOfTheLocateExtPCOfTheProjPCurOfGInter;



class Geom2dInt_TheLocateExtPCOfTheProjPCurOfGInter 
{
public:

  DEFINE_STANDARD_ALLOC

  
  Standard_EXPORT Geom2dInt_TheLocateExtPCOfTheProjPCurOfGInter();
  
  Standard_EXPORT Geom2dInt_TheLocateExtPCOfTheProjPCurOfGInter(const gp_Pnt2d& P, const Adaptor2d_Curve2d& C, const Standard_Real U0, const Standard_Real TolU);
  
  Standard_EXPORT Geom2dInt_TheLocateExtPCOfTheProjPCurOfGInter(const gp_Pnt2d& P, const Adaptor2d_Curve2d& C, const Standard_Real U0, const Standard_Real Umin, const Standard_Real Usup, const Standard_Real TolU);
  
  Standard_EXPORT void Initialize (const Adaptor2d_Curve2d& C, const Standard_Real Umin, const Standard_Real Usup, const Standard_Real TolU);
  
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
  Geom2dInt_PCLocFOfTheLocateExtPCOfTheProjPCurOfGInter myF;


};







#endif // _Geom2dInt_TheLocateExtPCOfTheProjPCurOfGInter_HeaderFile
