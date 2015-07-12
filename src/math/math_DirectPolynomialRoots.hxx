// Created on: 1991-05-13
// Created by: Laurent PAINNOT
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

#ifndef _math_DirectPolynomialRoots_HeaderFile
#define _math_DirectPolynomialRoots_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <Standard_Boolean.hxx>
#include <Standard_Integer.hxx>
#include <Standard_Real.hxx>
#include <Standard_OStream.hxx>
class Standard_RangeError;
class StdFail_InfiniteSolutions;



//! This class implements the calculation of all the real roots of a real
//! polynomial of degree <= 4 using a direct method. Once found,
//! the roots are polished using the Newton method.
class math_DirectPolynomialRoots 
{
public:

  DEFINE_STANDARD_ALLOC

  

  //! computes all the real roots of the polynomial
  //! Ax4 + Bx3 + Cx2 + Dx + E using a direct method.
  Standard_EXPORT math_DirectPolynomialRoots(const Standard_Real A, const Standard_Real B, const Standard_Real C, const Standard_Real D, const Standard_Real E);
  

  //! computes all the real roots of the polynomial
  //! Ax3 + Bx2 + Cx + D using a direct method.
  Standard_EXPORT math_DirectPolynomialRoots(const Standard_Real A, const Standard_Real B, const Standard_Real C, const Standard_Real D);
  

  //! computes all the real roots of the polynomial
  //! Ax2 + Bx + C using a direct method.
  Standard_EXPORT math_DirectPolynomialRoots(const Standard_Real A, const Standard_Real B, const Standard_Real C);
  

  //! computes the real root of the polynomial Ax + B.
  Standard_EXPORT math_DirectPolynomialRoots(const Standard_Real A, const Standard_Real B);
  
  //! Returns true if the computations are successful, otherwise returns false.
    Standard_Boolean IsDone() const;
  
  //! Returns true if there is an infinity of roots, otherwise returns false.
    Standard_Boolean InfiniteRoots() const;
  
  //! returns the number of solutions.
  //! An exception is raised if there are an infinity of roots.
    Standard_Integer NbSolutions() const;
  
  //! returns the value of the Nieme root.
  //! An exception is raised if there are an infinity of roots.
  //! Exception RangeError is raised if Nieme is < 1
  //! or Nieme > NbSolutions.
    Standard_Real Value (const Standard_Integer Nieme) const;
  
  //! Prints on the stream o information on the current state
  //! of the object.
  //! Is used to redefine the operator <<.
  Standard_EXPORT void Dump (Standard_OStream& o) const;




protected:

  
  Standard_EXPORT void Solve (const Standard_Real A, const Standard_Real B, const Standard_Real C, const Standard_Real D, const Standard_Real E);
  
  Standard_EXPORT void Solve (const Standard_Real A, const Standard_Real B, const Standard_Real C, const Standard_Real D);
  
  Standard_EXPORT void Solve (const Standard_Real A, const Standard_Real B, const Standard_Real C);
  
  Standard_EXPORT void Solve (const Standard_Real A, const Standard_Real B);




private:



  Standard_Boolean Done;
  Standard_Boolean InfiniteStatus;
  Standard_Integer NbSol;
  Standard_Real TheRoots[4];


};


#include <math_DirectPolynomialRoots.lxx>





#endif // _math_DirectPolynomialRoots_HeaderFile
