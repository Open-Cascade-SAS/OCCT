// Created on: 1992-03-26
// Created by: Laurent BUCHARD
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

#ifndef _IntCurve_PConicTool_HeaderFile
#define _IntCurve_PConicTool_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <Standard_Real.hxx>
#include <Standard_Integer.hxx>
class IntCurve_PConic;
class gp_Pnt2d;
class gp_Vec2d;


//! Implementation of the ParTool from IntImpParGen
//! for conics of gp, using the class PConic from IntCurve.
class IntCurve_PConicTool 
{
public:

  DEFINE_STANDARD_ALLOC

  
  Standard_EXPORT static Standard_Real EpsX (const IntCurve_PConic& C);
  
  Standard_EXPORT static Standard_Integer NbSamples (const IntCurve_PConic& C);
  
  Standard_EXPORT static Standard_Integer NbSamples (const IntCurve_PConic& C, const Standard_Real U0, const Standard_Real U1);
  
  Standard_EXPORT static gp_Pnt2d Value (const IntCurve_PConic& C, const Standard_Real X);
  
  Standard_EXPORT static void D1 (const IntCurve_PConic& C, const Standard_Real U, gp_Pnt2d& P, gp_Vec2d& T);
  
  Standard_EXPORT static void D2 (const IntCurve_PConic& C, const Standard_Real U, gp_Pnt2d& P, gp_Vec2d& T, gp_Vec2d& N);




protected:





private:





};







#endif // _IntCurve_PConicTool_HeaderFile
