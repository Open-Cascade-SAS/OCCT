// Created on: 2005-12-21
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

#ifndef _BRepGProp_TFunction_HeaderFile
#define _BRepGProp_TFunction_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <BRepGProp_Face.hxx>
#include <BRepGProp_UFunction.hxx>
#include <GProp_ValueType.hxx>
#include <Standard_Boolean.hxx>
#include <math_Function.hxx>
class gp_Pnt;

//! This class represents the integrand function for the outer
//! integral computation. The returned value represents the
//! integral of UFunction. It depends on the value type and the
//! flag IsByPoint.
class BRepGProp_TFunction : public math_Function
{
public:
  DEFINE_STANDARD_ALLOC

  //! Constructor. Initializes the function with the face, the
  //! location point, the flag IsByPoint, the coefficients
  //! theCoeff that have different meaning depending on the value
  //! of IsByPoint. The last two parameters are theUMin - the
  //! lower bound of the inner integral. This value is fixed for
  //! any integral. And the value of tolerance of inner integral
  //! computation.
  //! If IsByPoint is equal to Standard_True, the number of the
  //! coefficients is equal to 3 and they represent X, Y and Z
  //! coordinates (theCoeff[0], theCoeff[1] and theCoeff[2]
  //! correspondingly) of the shift if the inertia is computed
  //! with respect to the point different then the location.
  //! If IsByPoint is equal to Standard_False, the number of the
  //! coefficients is 4 and they represent the combination of
  //! plane parameters and shift values.
  Standard_EXPORT BRepGProp_TFunction(const BRepGProp_Face&  theSurface,
                                      const gp_Pnt&          theVertex,
                                      const Standard_Boolean IsByPoint,
                                      const Standard_Real*   theCoeffs,
                                      const Standard_Real    theUMin,
                                      const Standard_Real    theTolerance);

  Standard_EXPORT void Init();

  //! Setting the expected number of Kronrod points for the outer
  //! integral computation. This number is required for
  //! computation of a value of tolerance for inner integral
  //! computation. After GetStateNumber method call, this number
  //! is recomputed by the same law as in
  //! math_KronrodSingleIntegration, i.e. next number of points
  //! is equal to the current number plus a square root of the
  //! current number. If the law in math_KronrodSingleIntegration
  //! is changed, the modification algo should be modified
  //! accordingly.
  void SetNbKronrodPoints(const Standard_Integer theNbPoints);

  //! Setting the type of the value to be returned. This
  //! parameter is directly passed to the UFunction.
  void SetValueType(const GProp_ValueType aType);

  //! Setting the tolerance  for  inner integration
  void SetTolerance(const Standard_Real aTol);

  //! Returns the relative reached error of all values computation since
  //! the last call of GetStateNumber method.
  Standard_Real ErrorReached() const;

  //! Returns the absolut reached error of all values computation since
  //! the last call of GetStateNumber method.
  Standard_Real AbsolutError() const;

  //! Returns a value of the function. The value represents an
  //! integral of UFunction. It is computed with the predefined
  //! tolerance using the adaptive Gauss-Kronrod method.
  Standard_EXPORT virtual Standard_Boolean Value(const Standard_Real X,
                                                 Standard_Real&      F) Standard_OVERRIDE;

  //! Redefined  method. Remembers the error reached during
  //! computation of integral values since the object creation
  //! or the last call of GetStateNumber. It is invoked in each
  //! algorithm from the package math. Particularly in the
  //! algorithm math_KronrodSingleIntegration that is used to
  //! compute the integral of TFunction.
  Standard_EXPORT virtual Standard_Integer GetStateNumber() Standard_OVERRIDE;

protected:
private:
  BRepGProp_Face      mySurface;
  BRepGProp_UFunction myUFunction;
  Standard_Real       myUMin;
  Standard_Real       myTolerance;
  Standard_Real       myTolReached;
  Standard_Real       myErrReached;
  Standard_Real       myAbsError;
  GProp_ValueType     myValueType;
  Standard_Boolean    myIsByPoint;
  Standard_Integer    myNbPntOuter;
};

#include <BRepGProp_TFunction.lxx>

#endif // _BRepGProp_TFunction_HeaderFile
