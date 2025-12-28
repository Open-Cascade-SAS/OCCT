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
                                  const double             FixVal,
                                  const Adaptor3d_Surface* S,
                                  const int                Fix);

  //! returns the number of variables of the function.
  Standard_EXPORT int NbVariables() const override;

  //! returns the number of equations of the function.
  Standard_EXPORT int NbEquations() const override;

  //! computes the values <F> of the Functions for the
  //! variable <X>.
  //! Returns True if the computation was done successfully,
  //! False otherwise.
  Standard_EXPORT bool Value(const math_Vector& X, math_Vector& F) override;

  //! returns the values <D> of the derivatives for the
  //! variable <X>.
  //! Returns True if the computation was done successfully,
  //! False otherwise.
  Standard_EXPORT bool Derivatives(const math_Vector& X, math_Matrix& D) override;

  //! returns the values <F> of the functions and the derivatives
  //! <D> for the variable <X>.
  //! Returns True if the computation was done successfully,
  //! False otherwise.
  Standard_EXPORT bool Values(const math_Vector& X, math_Vector& F, math_Matrix& D) override;

  //! returns point on surface
  Standard_EXPORT gp_Pnt2d Solution() const;

private:
  const Adaptor3d_Curve*   myCurve;
  const Adaptor3d_Surface* mySurface;
  double                   myt;
  double                   myU;
  double                   myV;
  int                      myFix;
  double                   myNorm;
};

#endif // _ProjLib_PrjFunc_HeaderFile
