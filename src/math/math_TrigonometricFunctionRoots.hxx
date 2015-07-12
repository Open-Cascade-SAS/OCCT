// Created on: 1991-09-03
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

#ifndef _math_TrigonometricFunctionRoots_HeaderFile
#define _math_TrigonometricFunctionRoots_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <Standard_Integer.hxx>
#include <TColStd_Array1OfReal.hxx>
#include <Standard_Boolean.hxx>
#include <Standard_Real.hxx>
#include <Standard_OStream.hxx>
class StdFail_NotDone;
class Standard_OutOfRange;
class Standard_RangeError;


//! This class implements the solutions of the equation
//! a*Cos(x)*Cos(x) + 2*b*Cos(x)*Sin(x) + c*Cos(x) + d*Sin(x) + e
//! The degree of this equation can be 4, 3 or 2.
class math_TrigonometricFunctionRoots 
{
public:

  DEFINE_STANDARD_ALLOC

  
  //! Given coefficients a, b, c, d , e, this constructor
  //! performs the resolution of the equation above.
  //! The solutions must be contained in [InfBound, SupBound].
  //! InfBound and SupBound can be set by default to 0 and 2*PI.
  Standard_EXPORT math_TrigonometricFunctionRoots(const Standard_Real A, const Standard_Real B, const Standard_Real C, const Standard_Real D, const Standard_Real E, const Standard_Real InfBound, const Standard_Real SupBound);
  
  //! Given the two coefficients d and e, it performs
  //! the resolution of d*sin(x) + e = 0.
  //! The solutions must be contained in [InfBound, SupBound].
  //! InfBound and SupBound can be set by default to 0 and 2*PI.
  Standard_EXPORT math_TrigonometricFunctionRoots(const Standard_Real D, const Standard_Real E, const Standard_Real InfBound, const Standard_Real SupBound);
  
  //! Given the three coefficients c, d and e, it performs
  //! the resolution of 2*b*cos(x)*sin(x) + d*sin(x) + e = 0.
  //! The solutions must be contained in [InfBound, SupBound].
  //! InfBound and SupBound can be set by default to 0 and 2*PI.
  Standard_EXPORT math_TrigonometricFunctionRoots(const Standard_Real C, const Standard_Real D, const Standard_Real E, const Standard_Real InfBound, const Standard_Real SupBound);
  
  //! Returns true if the computations are successful, otherwise returns false.
    Standard_Boolean IsDone() const;
  
  //! Returns true if there is an infinity of roots, otherwise returns false.
    Standard_Boolean InfiniteRoots() const;
  
  //! Returns the solution of range Index.
  //! An exception is raised if NotDone.
  //! An exception is raised if Index>NbSolutions.
  //! An exception is raised if there is an infinity of solutions.
    Standard_Real Value (const Standard_Integer Index) const;
  
  //! Returns the number of solutions found.
  //! An exception is raised if NotDone.
  //! An exception is raised if there is an infinity of solutions.
    Standard_Integer NbSolutions() const;
  
  //! Prints information on the current state of the object.
  Standard_EXPORT void Dump (Standard_OStream& o) const;




protected:

  
  //! is used by the constructors above.
  Standard_EXPORT void Perform (const Standard_Real A, const Standard_Real B, const Standard_Real C, const Standard_Real D, const Standard_Real E, const Standard_Real InfBound, const Standard_Real SupBound);




private:



  Standard_Integer NbSol;
  TColStd_Array1OfReal Sol;
  Standard_Boolean InfiniteStatus;
  Standard_Boolean Done;


};


#include <math_TrigonometricFunctionRoots.lxx>





#endif // _math_TrigonometricFunctionRoots_HeaderFile
