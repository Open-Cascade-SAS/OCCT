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

#ifndef _Geom2dInt_TheProjPCurOfGInter_HeaderFile
#define _Geom2dInt_TheProjPCurOfGInter_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <Standard_Real.hxx>
class Adaptor2d_Curve2d;
class Geom2dInt_Geom2dCurveTool;
class Geom2dInt_TheCurveLocatorOfTheProjPCurOfGInter;
class Geom2dInt_TheLocateExtPCOfTheProjPCurOfGInter;
class Geom2dInt_PCLocFOfTheLocateExtPCOfTheProjPCurOfGInter;
class gp_Pnt2d;



class Geom2dInt_TheProjPCurOfGInter 
{
public:

  DEFINE_STANDARD_ALLOC

  
  Standard_EXPORT static Standard_Real FindParameter (const Adaptor2d_Curve2d& C, const gp_Pnt2d& Pnt, const Standard_Real Tol);
  
  Standard_EXPORT static Standard_Real FindParameter (const Adaptor2d_Curve2d& C, const gp_Pnt2d& Pnt, const Standard_Real LowParameter, const Standard_Real HighParameter, const Standard_Real Tol);




protected:





private:





};







#endif // _Geom2dInt_TheProjPCurOfGInter_HeaderFile
