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

#ifndef _math_FRPR_HeaderFile
#define _math_FRPR_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>

#include <math_Vector.hxx>
#include <math_Status.hxx>
#include <Standard_OStream.hxx>
class math_MultipleVarFunctionWithGradient;

//! this class implements the Fletcher-Reeves-Polak_Ribiere minimization
//! algorithm of a function of multiple variables.
//! Knowledge of the function's gradient is required.
class math_FRPR
{
public:
  DEFINE_STANDARD_ALLOC

  //! Initializes the computation of the minimum of F.
  //! Warning: constructor does not perform computations.
  Standard_EXPORT math_FRPR(const math_MultipleVarFunctionWithGradient& theFunction,
                            const Standard_Real                         theTolerance,
                            const Standard_Integer                      theNbIterations = 200,
                            const Standard_Real                         theZEPS         = 1.0e-12);

  //! Destructor
  Standard_EXPORT virtual ~math_FRPR();

  //! The solution F = Fi is found when
  //! 2.0 * abs(Fi - Fi-1) <= Tolerance * (abs(Fi) + abs(Fi-1) + ZEPS).
  Standard_EXPORT void Perform(math_MultipleVarFunctionWithGradient& theFunction,
                               const math_Vector&                    theStartingPoint);

  //! The solution F = Fi is found when:
  //! 2.0 * abs(Fi - Fi-1) <= Tolerance * (abs(Fi) + abs(Fi-1)) + ZEPS.
  //! The maximum number of iterations allowed is given by NbIterations.
  virtual Standard_Boolean IsSolutionReached(math_MultipleVarFunctionWithGradient& theFunction);

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

  //! returns the value of the minimum.
  //! Exception NotDone is raised if the minimum was not found.
  Standard_Real Minimum() const;

  //! returns the gradient vector at the minimum.
  //! Exception NotDone is raised if the minimum was not found.
  const math_Vector& Gradient() const;

  //! outputs the gradient vector at the minimum in Grad.
  //! Exception NotDone is raised if the minimum was not found.
  //! Exception DimensionError is raised if the range of Grad is not
  //! equal to the range of the StartingPoint.
  void Gradient(math_Vector& Grad) const;

  //! returns the number of iterations really done during the
  //! computation of the minimum.
  //! Exception NotDone is raised if the minimum was not found.
  Standard_Integer NbIterations() const;

  //! Prints on the stream o information on the current state
  //! of the object.
  //! Is used to redefine the operator <<.
  Standard_EXPORT void Dump(Standard_OStream& o) const;

protected:
  math_Vector   TheLocation;
  math_Vector   TheGradient;
  Standard_Real TheMinimum;
  Standard_Real PreviousMinimum;
  Standard_Real XTol;
  Standard_Real EPSZ;

private:
  Standard_Boolean Done;
  Standard_Integer Iter;
  Standard_Integer State;
  math_Status      TheStatus;
  Standard_Integer Itermax;
};

#include <math_FRPR.lxx>

#endif // _math_FRPR_HeaderFile
