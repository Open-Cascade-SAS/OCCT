// Created on: 1994-09-02
// Created by: Yves FRICAUD
// Copyright (c) 1994-1999 Matra Datavision
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

#ifndef _Geom2dLProp_FuncCurExt_HeaderFile
#define _Geom2dLProp_FuncCurExt_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <math_FunctionWithDerivative.hxx>
class Geom2d_Curve;

//! Function used to find the extremas of curvature in 2d.
class Geom2dLProp_FuncCurExt : public math_FunctionWithDerivative
{
public:
  DEFINE_STANDARD_ALLOC

  Standard_EXPORT Geom2dLProp_FuncCurExt(const occ::handle<Geom2d_Curve>& C, const double Tol);

  //! Returns the value for the variable <X>.
  Standard_EXPORT bool Value(const double X, double& F) override;

  //! Returns the derivative for the variable <X>.
  Standard_EXPORT bool Derivative(const double X, double& D) override;

  //! Returns the value of the function and the derivative
  //! for the variable <X>.
  Standard_EXPORT bool Values(const double X, double& F, double& D) override;

  //! True if Param corresponds to a minus
  //! of the radius of curvature.
  Standard_EXPORT bool IsMinKC(const double Param) const;

private:
  occ::handle<Geom2d_Curve> theCurve;
  double                    epsX;
};

#endif // _Geom2dLProp_FuncCurExt_HeaderFile
