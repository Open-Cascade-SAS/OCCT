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

#ifndef _math_FunctionRoots_HeaderFile
#define _math_FunctionRoots_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>

#include <NCollection_Sequence.hxx>
#include <Standard_Integer.hxx>
#include <Standard_OStream.hxx>
class math_FunctionWithDerivative;

//! This class implements an algorithm which finds all the real roots of
//! a function with derivative within a given range.
//! Knowledge of the derivative is required.
class math_FunctionRoots
{
public:
  DEFINE_STANDARD_ALLOC

  //! Calculates all the real roots of a function F-K within the range
  //! A..B. without conditions on A and B
  //! A solution X is found when
  //! abs(Xi - Xi-1) <= Epsx and abs(F(Xi)-K) <= EpsF.
  //! The function is considered as null between A and B if
  //! abs(F-K) <= EpsNull within this range.
  Standard_EXPORT math_FunctionRoots(math_FunctionWithDerivative& F,
                                     const double                 A,
                                     const double                 B,
                                     const int                    NbSample,
                                     const double                 EpsX    = 0.0,
                                     const double                 EpsF    = 0.0,
                                     const double                 EpsNull = 0.0,
                                     const double                 K       = 0.0);

  //! Returns true if the computations are successful, otherwise returns false.
  bool IsDone() const;

  //! returns true if the function is considered as null between A and B.
  //! Exceptions
  //! StdFail_NotDone if the algorithm fails (and IsDone returns false).
  bool IsAllNull() const;

  //! Returns the number of solutions found.
  //! Exceptions
  //! StdFail_NotDone if the algorithm fails (and IsDone returns false).
  int NbSolutions() const;

  //! Returns the Nth value of the root of function F.
  //! Exceptions
  //! StdFail_NotDone if the algorithm fails (and IsDone returns false).
  double Value(const int Nieme) const;

  //! returns the StateNumber of the Nieme root.
  //! Exception RangeError is raised if Nieme is < 1
  //! or Nieme > NbSolutions.
  int StateNumber(const int Nieme) const;

  //! Prints on the stream o information on the current state
  //! of the object.
  Standard_EXPORT void Dump(Standard_OStream& o) const;

private:
  bool                         Done;
  bool                         AllNull;
  NCollection_Sequence<double> Sol;
  NCollection_Sequence<int>    NbStateSol;
};

#include <math_FunctionRoots.lxx>

#endif // _math_FunctionRoots_HeaderFile
