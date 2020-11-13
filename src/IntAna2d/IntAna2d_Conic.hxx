// Created on: 1992-02-11
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

#ifndef _IntAna2d_Conic_HeaderFile
#define _IntAna2d_Conic_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <Standard_Real.hxx>
class gp_Circ2d;
class gp_Lin2d;
class gp_Parab2d;
class gp_Hypr2d;
class gp_Elips2d;
class gp_XY;
class gp_Ax2d;


//! Definition of a conic by its implicit quadaratic equation:
//! A.X**2 + B.Y**2 + 2.C.X*Y + 2.D.X + 2.E.Y + F = 0.
class IntAna2d_Conic 
{
public:

  DEFINE_STANDARD_ALLOC

  
  Standard_EXPORT IntAna2d_Conic(const gp_Circ2d& C);
  
  Standard_EXPORT IntAna2d_Conic(const gp_Lin2d& C);
  
  Standard_EXPORT IntAna2d_Conic(const gp_Parab2d& C);
  
  Standard_EXPORT IntAna2d_Conic(const gp_Hypr2d& C);
  
  Standard_EXPORT IntAna2d_Conic(const gp_Elips2d& C);
  
  //! value of the function F at the point X,Y.
  Standard_EXPORT Standard_Real Value (const Standard_Real X, const Standard_Real Y) const;
  
  //! returns the value of the gradient of F at the point X,Y.
  Standard_EXPORT gp_XY Grad (const Standard_Real X, const Standard_Real Y) const;
  
  //! Returns the value of the function and its gradient at
  //! the point X,Y.
  Standard_EXPORT void ValAndGrad (const Standard_Real X, const Standard_Real Y, Standard_Real& Val, gp_XY& Grd) const;
  
  //! returns the coefficients of the polynomial equation
  //! which defines the conic:
  //! A.X**2 + B.Y**2 + 2.C.X*Y + 2.D.X + 2.E.Y + F = 0.
  Standard_EXPORT void Coefficients (Standard_Real& A, Standard_Real& B, Standard_Real& C, Standard_Real& D, Standard_Real& E, Standard_Real& F) const;
  
  //! Returns the coefficients of the polynomial equation
  //! ( written in the natural coordinates system )
  //! A x x + B y y + 2 C x y + 2 D x + 2 E y + F
  //! in the local coordinates system defined by Axis
  Standard_EXPORT void NewCoefficients (Standard_Real& A, Standard_Real& B, Standard_Real& C, Standard_Real& D, Standard_Real& E, Standard_Real& F, const gp_Ax2d& Axis) const;




protected:





private:



  Standard_Real a;
  Standard_Real b;
  Standard_Real c;
  Standard_Real d;
  Standard_Real e;
  Standard_Real f;


};







#endif // _IntAna2d_Conic_HeaderFile
