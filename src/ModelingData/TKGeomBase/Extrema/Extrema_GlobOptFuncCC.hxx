// Created on: 2014-01-20
// Created by: Alexaner Malyshev
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

#ifndef _Extrema_GlobOptFuncCC_HeaderFile
#define _Extrema_GlobOptFuncCC_HeaderFile

#include <Adaptor2d_Curve2d.hxx>
#include <Adaptor3d_Curve.hxx>
#include <math_Matrix.hxx>
#include <math_Vector.hxx>
#include <math_MultipleVarFunctionWithHessian.hxx>

//! This class implements function which calculate Eucluidean distance
//! between point on curve and point on other curve in case of C1 and C2 continuity is C0.
class Extrema_GlobOptFuncCCC0 : public math_MultipleVarFunction
{
public:
  Standard_EXPORT Extrema_GlobOptFuncCCC0(const Adaptor3d_Curve& C1, const Adaptor3d_Curve& C2);

  Standard_EXPORT Extrema_GlobOptFuncCCC0(const Adaptor2d_Curve2d& C1, const Adaptor2d_Curve2d& C2);

  Standard_EXPORT virtual Standard_Integer NbVariables() const;

  Standard_EXPORT virtual Standard_Boolean Value(const math_Vector& X, Standard_Real& F);

private:
  Extrema_GlobOptFuncCCC0& operator=(const Extrema_GlobOptFuncCCC0& theOther);

  const Adaptor3d_Curve *  myC1_3d, *myC2_3d;
  const Adaptor2d_Curve2d *myC1_2d, *myC2_2d;
  Standard_Integer         myType;
};

//! This class implements function which calculate Eucluidean distance
//! between point on curve and point on other curve in case of C1 and C2 continuity is C1.
class Extrema_GlobOptFuncCCC1 : public math_MultipleVarFunctionWithGradient
{
public:
  Standard_EXPORT Extrema_GlobOptFuncCCC1(const Adaptor3d_Curve& C1, const Adaptor3d_Curve& C2);

  Standard_EXPORT Extrema_GlobOptFuncCCC1(const Adaptor2d_Curve2d& C1, const Adaptor2d_Curve2d& C2);

  Standard_EXPORT virtual Standard_Integer NbVariables() const;

  Standard_EXPORT virtual Standard_Boolean Value(const math_Vector& X, Standard_Real& F);

  Standard_EXPORT virtual Standard_Boolean Gradient(const math_Vector& X, math_Vector& G);

  Standard_EXPORT virtual Standard_Boolean Values(const math_Vector& X,
                                                  Standard_Real&     F,
                                                  math_Vector&       G);

private:
  Extrema_GlobOptFuncCCC1& operator=(const Extrema_GlobOptFuncCCC1& theOther);

  const Adaptor3d_Curve *  myC1_3d, *myC2_3d;
  const Adaptor2d_Curve2d *myC1_2d, *myC2_2d;
  Standard_Integer         myType;
};

//! This class implements function which calculate Eucluidean distance
//! between point on curve and point on other curve in case of C1 and C2 continuity is C2.
class Extrema_GlobOptFuncCCC2 : public math_MultipleVarFunctionWithHessian
{
public:
  Standard_EXPORT Extrema_GlobOptFuncCCC2(const Adaptor3d_Curve& C1, const Adaptor3d_Curve& C2);

  Standard_EXPORT Extrema_GlobOptFuncCCC2(const Adaptor2d_Curve2d& C1, const Adaptor2d_Curve2d& C2);

  Standard_EXPORT virtual Standard_Integer NbVariables() const;

  Standard_EXPORT virtual Standard_Boolean Value(const math_Vector& X, Standard_Real& F);

  Standard_EXPORT virtual Standard_Boolean Gradient(const math_Vector& X, math_Vector& G);

  Standard_EXPORT virtual Standard_Boolean Values(const math_Vector& X,
                                                  Standard_Real&     F,
                                                  math_Vector&       G);

  Standard_EXPORT virtual Standard_Boolean Values(const math_Vector& X,
                                                  Standard_Real&     F,
                                                  math_Vector&       G,
                                                  math_Matrix&       H);

private:
  Extrema_GlobOptFuncCCC2& operator=(const Extrema_GlobOptFuncCCC2& theOther);

  const Adaptor3d_Curve *  myC1_3d, *myC2_3d;
  const Adaptor2d_Curve2d *myC1_2d, *myC2_2d;
  Standard_Integer         myType;
};

#endif
