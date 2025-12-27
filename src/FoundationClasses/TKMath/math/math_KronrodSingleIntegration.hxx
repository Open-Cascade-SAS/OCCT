// Created on: 2005-12-08
// Created by: Sergey KHROMOV
// Copyright (c) 2005-2014 OPEN CASCADE SAS
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

#ifndef _math_KronrodSingleIntegration_HeaderFile
#define _math_KronrodSingleIntegration_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>

#include <Standard_Integer.hxx>
#include <math_Vector.hxx>
class math_Function;

//! This class implements the Gauss-Kronrod method of
//! integral computation.
class math_KronrodSingleIntegration
{
public:
  DEFINE_STANDARD_ALLOC

  //! An empty constructor.
  Standard_EXPORT math_KronrodSingleIntegration();

  //! Constructor. Takes the function, the lower and upper bound
  //! values, the initial number of Kronrod points
  Standard_EXPORT math_KronrodSingleIntegration(math_Function&         theFunction,
                                                const double    theLower,
                                                const double    theUpper,
                                                const int theNbPnts);

  //! Constructor. Takes the function, the lower and upper bound
  //! values, the initial number of Kronrod points, the
  //! tolerance value and the maximal number of iterations as
  //! parameters.
  Standard_EXPORT math_KronrodSingleIntegration(math_Function&         theFunction,
                                                const double    theLower,
                                                const double    theUpper,
                                                const int theNbPnts,
                                                const double    theTolerance,
                                                const int theMaxNbIter);

  //! Computation of the integral. Takes the function,
  //! the lower and upper bound values, the initial number
  //! of Kronrod points, the relative tolerance value and the
  //! maximal number of iterations as parameters.
  //! theNbPnts should be odd and greater then or equal to 3.
  Standard_EXPORT void Perform(math_Function&         theFunction,
                               const double    theLower,
                               const double    theUpper,
                               const int theNbPnts);

  //! Computation of the integral. Takes the function,
  //! the lower and upper bound values, the initial number
  //! of Kronrod points, the relative tolerance value and the
  //! maximal number of iterations as parameters.
  //! theNbPnts should be odd and greater then or equal to 3.
  //! Note that theTolerance is relative, i.e. the criterion of
  //! solution reaching is:
  //! std::abs(Kronrod - Gauss)/std::abs(Kronrod) < theTolerance.
  //! theTolerance should be positive.
  Standard_EXPORT void Perform(math_Function&         theFunction,
                               const double    theLower,
                               const double    theUpper,
                               const int theNbPnts,
                               const double    theTolerance,
                               const int theMaxNbIter);

  //! Returns true if computation is performed
  //! successfully.
  bool IsDone() const;

  //! Returns the value of the integral.
  double Value() const;

  //! Returns the value of the relative error reached.
  double ErrorReached() const;

  //! Returns the value of the relative error reached.
  double AbsolutError() const;

  //! Returns the number of Kronrod points
  //! for which the result is computed.
  int OrderReached() const;

  //! Returns the number of iterations
  //! that were made to compute result.
  int NbIterReached() const;

  Standard_EXPORT static bool GKRule(math_Function&      theFunction,
                                                 const double theLower,
                                                 const double theUpper,
                                                 const math_Vector&  theGaussP,
                                                 const math_Vector&  theGaussW,
                                                 const math_Vector&  theKronrodP,
                                                 const math_Vector&  theKronrodW,
                                                 double&      theValue,
                                                 double&      theError);

private:
  bool myIsDone;
  double    myValue;
  double    myErrorReached;
  double    myAbsolutError;
  int myNbPntsReached;
  int myNbIterReached;
};

#include <math_KronrodSingleIntegration.lxx>

#endif // _math_KronrodSingleIntegration_HeaderFile
