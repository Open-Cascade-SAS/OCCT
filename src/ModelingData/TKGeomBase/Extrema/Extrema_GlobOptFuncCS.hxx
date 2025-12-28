// Created on: 2014-06-23
// Created by: Alexander Malyshev
// Copyright (c) 2014-2014 OPEN CASCADE SAS
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
// commercial license or contractual agreement

#ifndef _Extrema_GlobOptFuncCS_HeaderFile
#define _Extrema_GlobOptFuncCS_HeaderFile

#include <Adaptor3d_Surface.hxx>
#include <math_Matrix.hxx>
#include <math_Vector.hxx>
#include <math_MultipleVarFunctionWithHessian.hxx>

//! This class implements function which calculate square Eucluidean distance
//! between point on curve and point on surface in case of continuity is C2.
class Extrema_GlobOptFuncCS : public math_MultipleVarFunctionWithHessian
{
public:
  //! Curve and surface should exist during all the lifetime of Extrema_GlobOptFuncCS.
  Standard_EXPORT Extrema_GlobOptFuncCS(const Adaptor3d_Curve* C, const Adaptor3d_Surface* S);

  Standard_EXPORT virtual int NbVariables() const;

  Standard_EXPORT virtual bool Value(const math_Vector& theX, double& theF);

  Standard_EXPORT virtual bool Gradient(const math_Vector& theX, math_Vector& theG);

  Standard_EXPORT virtual bool Values(const math_Vector& theX, double& theF, math_Vector& theG);

  Standard_EXPORT virtual bool Values(const math_Vector& theX,
                                      double&            theF,
                                      math_Vector&       theG,
                                      math_Matrix&       theH);

private:
  bool checkInputData(const math_Vector& X, double& cu, double& su, double& sv);

  void value(double cu, double su, double sv, double& F);

  void gradient(double cu, double su, double sv, math_Vector& G);

  void hessian(double cu, double su, double sv, math_Matrix& H);

  Extrema_GlobOptFuncCS& operator=(const Extrema_GlobOptFuncCS& theOther);

  const Adaptor3d_Curve*   myC;
  const Adaptor3d_Surface* myS;
};

#endif
