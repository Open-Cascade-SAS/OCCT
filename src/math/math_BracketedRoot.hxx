// Created on: 1991-05-14
// Created by: Laurent Painnot
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

#ifndef _math_BracketedRoot_HeaderFile
#define _math_BracketedRoot_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <Standard_Real.hxx>
#include <Standard_OStream.hxx>
class math_Function;


//! This class implements the Brent method to find the root of a function
//! located within two bounds. No knowledge of the derivative is required.
class math_BracketedRoot 
{
public:

  DEFINE_STANDARD_ALLOC

  

  //! The Brent method is used to find the root of the function F between
  //! the bounds Bound1 and Bound2 on the function F.
  //! If F(Bound1)*F(Bound2) >0 the Brent method fails.
  //! The tolerance required for the root is given by Tolerance.
  //! The solution is found when :
  //! abs(Xi - Xi-1) <= Tolerance;
  //! The maximum number of iterations allowed is given by NbIterations.
  Standard_EXPORT math_BracketedRoot(math_Function& F, const Standard_Real Bound1, const Standard_Real Bound2, const Standard_Real Tolerance, const Standard_Integer NbIterations = 100, const Standard_Real ZEPS = 1.0e-12);
  
  //! Returns true if the computations are successful, otherwise returns false.
    Standard_Boolean IsDone() const;
  
  //! returns the value of the root.
  //! Exception NotDone is raised if the minimum was not found.
    Standard_Real Root() const;
  
  //! returns the value of the function at the root.
  //! Exception NotDone is raised if the minimum was not found.
    Standard_Real Value() const;
  
  //! returns the number of iterations really done during the
  //! computation of the Root.
  //! Exception NotDone is raised if the minimum was not found.
    Standard_Integer NbIterations() const;
  
  //! Prints on the stream o information on the current state
  //! of the object.
  Standard_EXPORT void Dump (Standard_OStream& o) const;




protected:





private:



  Standard_Boolean Done;
  Standard_Real TheRoot;
  Standard_Real TheError;
  Standard_Integer NbIter;


};


#include <math_BracketedRoot.lxx>





#endif // _math_BracketedRoot_HeaderFile
