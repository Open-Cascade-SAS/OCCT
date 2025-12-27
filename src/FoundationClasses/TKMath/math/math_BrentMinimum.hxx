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

#include <Standard_Real.hxx>
#include <Standard_OStream.hxx>
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
  Standard_EXPORT math_BrentMinimum(const double    TolX,
                                    const int NbIterations = 100,
                                    const double    ZEPS         = 1.0e-12);

  //! This constructor should be used in a sub-class to initialize
  //! correctly all the fields of this class.
  //! It has to be used if F(Bx) is known.
  Standard_EXPORT math_BrentMinimum(const double    TolX,
                                    const double    Fbx,
                                    const int NbIterations = 100,
                                    const double    ZEPS         = 1.0e-12);

  //! Destructor
  Standard_EXPORT virtual ~math_BrentMinimum();

  //! Brent minimization is performed on function F from a given
  //! bracketing triplet of abscissas Ax, Bx, Cx (such that Bx is
  //! between Ax and Cx, F(Bx) is less than both F(Bx) and F(Cx))
  //! The solution is found when: abs(Xi - Xi-1) <= TolX * abs(Xi) + ZEPS;
  Standard_EXPORT void Perform(math_Function&      F,
                               const double Ax,
                               const double Bx,
                               const double Cx);

  //! This method is called at the end of each iteration to check if the
  //! solution is found.
  //! It can be redefined in a sub-class to implement a specific test to
  //! stop the iterations.
  virtual bool IsSolutionReached(math_Function& theFunction);

  //! Returns true if the computations are successful, otherwise returns false.
  bool IsDone() const;

  //! returns the location value of the minimum.
  //! Exception NotDone is raised if the minimum was not found.
  double Location() const;

  //! returns the value of the minimum.
  //! Exception NotDone is raised if the minimum was not found.
  double Minimum() const;

  //! returns the number of iterations really done during the
  //! computation of the minimum.
  //! Exception NotDone is raised if the minimum was not found.
  int NbIterations() const;

  //! Prints on the stream o information on the current state
  //! of the object.
  //! Is used to redefine the operator <<.
  Standard_EXPORT void Dump(Standard_OStream& o) const;

protected:
  double a;
  double b;
  double x;
  double fx;
  double fv;
  double fw;
  double XTol;
  double EPSZ;

private:
  bool Done;
  int iter;
  int Itermax;
  bool myF;
};

#include <math_BrentMinimum.lxx>

#endif // _math_BrentMinimum_HeaderFile
