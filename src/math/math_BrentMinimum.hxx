// Created on: 1991-05-14
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

#ifndef _math_BrentMinimum_HeaderFile
#define _math_BrentMinimum_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <Standard_Boolean.hxx>
#include <Standard_Real.hxx>
#include <Standard_Integer.hxx>
#include <Standard_OStream.hxx>
class StdFail_NotDone;
class math_Function;



//! This class implements the Brent's method to find the minimum of
//! a function of a single variable.
//! No knowledge of the derivative is required.
class math_BrentMinimum 
{
public:

  DEFINE_STANDARD_ALLOC

  

  //! This constructor should be used in a sub-class to initialize
  //! correctly all the fields of this class.
  Standard_EXPORT math_BrentMinimum(const Standard_Real TolX, const Standard_Integer NbIterations = 100, const Standard_Real ZEPS = 1.0e-12);
  

  //! This constructor should be used in a sub-class to initialize
  //! correctly all the fields of this class.
  //! It has to be used if F(Bx) is known.
  Standard_EXPORT math_BrentMinimum(const Standard_Real TolX, const Standard_Real Fbx, const Standard_Integer NbIterations = 100, const Standard_Real ZEPS = 1.0e-12);
  
  //! Destructor
  Standard_EXPORT virtual ~math_BrentMinimum();
  

  //! Brent minimization is performed on function F from a given
  //! bracketing triplet of abscissas Ax, Bx, Cx (such that Bx is
  //! between Ax and Cx, F(Bx) is less than both F(Bx) and F(Cx))
  //! The solution is found when: abs(Xi - Xi-1) <= TolX * abs(Xi) + ZEPS;
  Standard_EXPORT void Perform (math_Function& F, const Standard_Real Ax, const Standard_Real Bx, const Standard_Real Cx);
  

  //! This method is called at the end of each iteration to check if the
  //! solution is found.
  //! It can be redefined in a sub-class to implement a specific test to
  //! stop the iterations.
    virtual Standard_Boolean IsSolutionReached (math_Function& theFunction);
  
  //! Returns true if the computations are successful, otherwise returns false.
    Standard_Boolean IsDone() const;
  
  //! returns the location value of the minimum.
  //! Exception NotDone is raised if the minimum was not found.
    Standard_Real Location() const;
  
  //! returns the value of the minimum.
  //! Exception NotDone is raised if the minimum was not found.
    Standard_Real Minimum() const;
  
  //! returns the number of iterations really done during the
  //! computation of the minimum.
  //! Exception NotDone is raised if the minimum was not found.
    Standard_Integer NbIterations() const;
  
  //! Prints on the stream o information on the current state
  //! of the object.
  //! Is used to redefine the operator <<.
  Standard_EXPORT void Dump (Standard_OStream& o) const;




protected:



  Standard_Real a;
  Standard_Real b;
  Standard_Real x;
  Standard_Real fx;
  Standard_Real fv;
  Standard_Real fw;
  Standard_Real XTol;
  Standard_Real EPSZ;


private:



  Standard_Boolean Done;
  Standard_Integer iter;
  Standard_Integer Itermax;
  Standard_Boolean myF;


};


#include <math_BrentMinimum.lxx>





#endif // _math_BrentMinimum_HeaderFile
