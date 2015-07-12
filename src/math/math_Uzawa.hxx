// Created on: 1991-08-22
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

#ifndef _math_Uzawa_HeaderFile
#define _math_Uzawa_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <math_Vector.hxx>
#include <math_Matrix.hxx>
#include <Standard_Integer.hxx>
#include <Standard_Boolean.hxx>
#include <Standard_Real.hxx>
#include <Standard_OStream.hxx>
class StdFail_NotDone;
class Standard_ConstructionError;
class math_Matrix;


//! This class implements a system resolution C*X = B with
//! an approach solution X0. There are no conditions on the
//! number of equations. The algorithm used is the Uzawa
//! algorithm. It is possible to have equal or inequal  (<)
//! equations to solve. The resolution is done with a
//! minimization of Norm(X-X0).
//! If there are only equal equations, the resolution is directly
//! done and is similar to Gauss resolution with an optimisation
//! because the matrix is a symmetric matrix.
//! (The resolution is done with Crout algorithm)
class math_Uzawa 
{
public:

  DEFINE_STANDARD_ALLOC

  
  //! Given an input matrix Cont, two input vectors Secont
  //! and StartingPoint, it solves Cont*X = Secont (only
  //! = equations) with a minimization of Norme(X-X0).
  //! The maximun iterations number allowed is fixed to
  //! NbIterations.
  //! The tolerance EpsLic is fixed for the dual variable
  //! convergence. The tolerance EpsLix is used for the
  //! convergence of X.
  //! Exception ConstuctionError is raised if the line number
  //! of Cont is different from the length of Secont.
  Standard_EXPORT math_Uzawa(const math_Matrix& Cont, const math_Vector& Secont, const math_Vector& StartingPoint, const Standard_Real EpsLix = 1.0e-06, const Standard_Real EpsLic = 1.0e-06, const Standard_Integer NbIterations = 500);
  
  //! Given an input matrix Cont, two input vectors Secont
  //! and StartingPoint, it solves Cont*X = Secont (the Nce
  //! first equations are equal equations and the Nci last
  //! equations are inequalities <) with a minimization
  //! of Norme(X-X0).
  //! The maximun iterations number allowed is fixed to
  //! NbIterations.
  //! The tolerance EpsLic is fixed for the dual variable
  //! convergence. The tolerance EpsLix is used for the
  //! convergence of X.
  //! There are no conditions on Nce and Nci.
  //! Exception ConstuctionError is raised if the line number
  //! of Cont is different from the length of Secont and from
  //! Nce + Nci.
  Standard_EXPORT math_Uzawa(const math_Matrix& Cont, const math_Vector& Secont, const math_Vector& StartingPoint, const Standard_Integer Nci, const Standard_Integer Nce, const Standard_Real EpsLix = 1.0e-06, const Standard_Real EpsLic = 1.0e-06, const Standard_Integer NbIterations = 500);
  
  //! Returns true if the computations are successful, otherwise returns false.
    Standard_Boolean IsDone() const;
  
  //! Returns the vector solution of the system above.
  //! An exception is raised if NotDone.
    const math_Vector& Value() const;
  
  //! Returns the initial error Cont*StartingPoint-Secont.
  //! An exception is raised if NotDone.
    const math_Vector& InitialError() const;
  
  //! returns the duale variables V of the systeme.
  Standard_EXPORT void Duale (math_Vector& V) const;
  
  //! Returns the difference between X solution and the
  //! StartingPoint.
  //! An exception is raised if NotDone.
    const math_Vector& Error() const;
  
  //! returns the number of iterations really done.
  //! An exception is raised if NotDone.
    Standard_Integer NbIterations() const;
  
  //! returns the inverse matrix of (C * Transposed(C)).
  //! This result is needed for the computation of the gradient
  //! when approximating a curve.
    const math_Matrix& InverseCont() const;
  
  //! Prints information on the current state of the object.
  Standard_EXPORT void Dump (Standard_OStream& o) const;




protected:

  
  //! Is used internally by the two constructors above.
  Standard_EXPORT void Perform (const math_Matrix& Cont, const math_Vector& Secont, const math_Vector& StartingPoint, const Standard_Integer Nci, const Standard_Integer Nce, const Standard_Real EpsLix = 1.0e-06, const Standard_Real EpsLic = 1.0e-06, const Standard_Integer NbIterations = 500);




private:



  math_Vector Resul;
  math_Vector Erruza;
  math_Vector Errinit;
  math_Vector Vardua;
  math_Matrix CTCinv;
  Standard_Integer NbIter;
  Standard_Boolean Done;


};


#include <math_Uzawa.lxx>





#endif // _math_Uzawa_HeaderFile
