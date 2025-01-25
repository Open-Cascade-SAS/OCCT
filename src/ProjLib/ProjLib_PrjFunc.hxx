// Created on: 1997-11-06
// Created by: Roman BORISOV
// Copyright (c) 1997-1999 Matra Datavision
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

#ifndef _ProjLib_PrjFunc_HeaderFile
#define _ProjLib_PrjFunc_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>

#include <math_FunctionSetWithDerivatives.hxx>
#include <math_Vector.hxx>

class math_Matrix;
class gp_Pnt2d;

class ProjLib_PrjFunc : public math_FunctionSetWithDerivatives
{
public:
  DEFINE_STANDARD_ALLOC

  Standard_EXPORT ProjLib_PrjFunc(const Adaptor3d_Curve*   C,
                                  const Standard_Real      FixVal,
                                  const Adaptor3d_Surface* S,
                                  const Standard_Integer   Fix);

  //! returns the number of variables of the function.
  Standard_EXPORT Standard_Integer NbVariables() const;

  //! returns the number of equations of the function.
  Standard_EXPORT Standard_Integer NbEquations() const;

  //! computes the values <F> of the Functions for the
  //! variable <X>.
  //! Returns True if the computation was done successfully,
  //! False otherwise.
  Standard_EXPORT Standard_Boolean Value(const math_Vector& X, math_Vector& F);

  //! returns the values <D> of the derivatives for the
  //! variable <X>.
  //! Returns True if the computation was done successfully,
  //! False otherwise.
  Standard_EXPORT Standard_Boolean Derivatives(const math_Vector& X, math_Matrix& D);

  //! returns the values <F> of the functions and the derivatives
  //! <D> for the variable <X>.
  //! Returns True if the computation was done successfully,
  //! False otherwise.
  Standard_EXPORT Standard_Boolean Values(const math_Vector& X, math_Vector& F, math_Matrix& D);

  //! returns  point  on  surface
  Standard_EXPORT gp_Pnt2d Solution() const;

private:
  const Adaptor3d_Curve*   myCurve;
  const Adaptor3d_Surface* mySurface;
  Standard_Real            myt;
  Standard_Real            myU;
  Standard_Real            myV;
  Standard_Integer         myFix;
  Standard_Real            myNorm;
};

#endif // _ProjLib_PrjFunc_HeaderFile
