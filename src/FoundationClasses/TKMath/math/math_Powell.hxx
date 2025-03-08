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

#ifndef _math_Powell_HeaderFile
#define _math_Powell_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>

#include <math_Vector.hxx>
#include <Standard_Integer.hxx>
#include <math_Status.hxx>
#include <math_Matrix.hxx>
#include <Standard_OStream.hxx>
class math_MultipleVarFunction;

//! This class implements the Powell method to find the minimum of
//! function of multiple variables (the gradient does not have to be known).
class math_Powell
{
public:
  DEFINE_STANDARD_ALLOC

  //! Constructor. Initialize new entity.
  Standard_EXPORT math_Powell(const math_MultipleVarFunction& theFunction,
                              const Standard_Real             theTolerance,
                              const Standard_Integer          theNbIterations = 200,
                              const Standard_Real             theZEPS         = 1.0e-12);

  //! Destructor
  Standard_EXPORT virtual ~math_Powell();

  //! Computes Powell minimization on the function F given
  //! theStartingPoint, and an initial matrix theStartingDirection
  //! whose columns contain the initial set of directions.
  //! The solution F = Fi is found when:
  //! 2.0 * abs(Fi - Fi-1) =< Tolerance * (abs(Fi) + abs(Fi-1) + ZEPS).
  Standard_EXPORT void Perform(math_MultipleVarFunction& theFunction,
                               const math_Vector&        theStartingPoint,
                               const math_Matrix&        theStartingDirections);

  //! Solution F = Fi is found when:
  //! 2.0 * abs(Fi - Fi-1) <= Tolerance * (abs(Fi) + abs(Fi-1)) + ZEPS.
  //! The maximum number of iterations allowed is given by NbIterations.
  virtual Standard_Boolean IsSolutionReached(math_MultipleVarFunction& theFunction);

  //! Returns true if the computations are successful, otherwise returns false.
  Standard_Boolean IsDone() const;

  //! returns the location vector of the minimum.
  //! Exception NotDone is raised if the minimum was not found.
  const math_Vector& Location() const;

  //! outputs the location vector of the minimum in Loc.
  //! Exception NotDone is raised if the minimum was not found.
  //! Exception DimensionError is raised if the range of Loc is not
  //! equal to the range of the StartingPoint.
  void Location(math_Vector& Loc) const;

  //! Returns the value of the minimum.
  //! Exception NotDone is raised if the minimum was not found.
  Standard_Real Minimum() const;

  //! Returns the number of iterations really done during the
  //! computation of the minimum.
  //! Exception NotDone is raised if the minimum was not found.
  Standard_Integer NbIterations() const;

  //! Prints information on the current state of the object.
  //! Is used to redefine the operator <<.
  Standard_EXPORT void Dump(Standard_OStream& o) const;

protected:
  math_Vector   TheLocation;
  Standard_Real TheMinimum;
  Standard_Real TheLocationError;
  Standard_Real PreviousMinimum;
  Standard_Real XTol;
  Standard_Real EPSZ;

private:
  Standard_Boolean Done;
  Standard_Integer Iter;
  math_Status      TheStatus;
  math_Matrix      TheDirections;
  Standard_Integer State;
  Standard_Integer Itermax;
};

#include <math_Powell.lxx>

#endif // _math_Powell_HeaderFile
