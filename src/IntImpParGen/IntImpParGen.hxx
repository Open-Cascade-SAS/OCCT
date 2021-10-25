// Created on: 1992-03-02
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

#ifndef _IntImpParGen_HeaderFile
#define _IntImpParGen_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <IntRes2d_Position.hxx>
#include <Standard_Real.hxx>
#include <Standard_Boolean.hxx>
class gp_Vec2d;
class IntRes2d_Transition;
class IntRes2d_Domain;
class gp_Pnt2d;


//! Gives a generic algorithm to intersect Implicit Curves
//! and Bounded Parametric Curves.
//!
//! Level: Internal
//!
//! All the methods of all the classes are Internal.
class IntImpParGen 
{
public:

  DEFINE_STANDARD_ALLOC

  
  //! Template class for an implicit  curve.
  //! Math function, instantiated inside the Intersector.
  //! Tool used by the package IntCurve and IntImpParGen
  Standard_EXPORT static void DetermineTransition (const IntRes2d_Position Pos1, gp_Vec2d& Tan1, const gp_Vec2d& Norm1, IntRes2d_Transition& Trans1, const IntRes2d_Position Pos2, gp_Vec2d& Tan2, const gp_Vec2d& Norm2, IntRes2d_Transition& Trans2, const Standard_Real Tol);
  
  Standard_EXPORT static Standard_Boolean DetermineTransition (const IntRes2d_Position Pos1, gp_Vec2d& Tan1, IntRes2d_Transition& Trans1, const IntRes2d_Position Pos2, gp_Vec2d& Tan2, IntRes2d_Transition& Trans2, const Standard_Real Tol);
  
  Standard_EXPORT static void DeterminePosition (IntRes2d_Position& Pos1, const IntRes2d_Domain& Dom1, const gp_Pnt2d& P1, const Standard_Real Tol);
  
  Standard_EXPORT static Standard_Real NormalizeOnDomain (Standard_Real& Par1, const IntRes2d_Domain& Dom1);




protected:





private:




friend class IntImpParGen_ImpTool;
friend class IntImpParGen_ImpParTool;
friend class IntImpParGen_Intersector;
friend class IntImpParGen_MyImpParTool;

};







#endif // _IntImpParGen_HeaderFile
