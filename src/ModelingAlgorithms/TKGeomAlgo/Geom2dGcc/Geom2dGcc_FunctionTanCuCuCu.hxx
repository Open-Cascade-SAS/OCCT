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

#ifndef _Geom2dGcc_FunctionTanCuCuCu_HeaderFile
#define _Geom2dGcc_FunctionTanCuCuCu_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>

#include <Geom2dAdaptor_Curve.hxx>
#include <gp_Circ2d.hxx>
#include <gp_Lin2d.hxx>
#include <Geom2dGcc_Type1.hxx>
#include <math_FunctionSetWithDerivatives.hxx>
#include <math_Vector.hxx>
class gp_Pnt2d;
class gp_Vec2d;
class math_Matrix;

//! This abstract class describes a set on N Functions of
//! M independent variables.
class Geom2dGcc_FunctionTanCuCuCu : public math_FunctionSetWithDerivatives
{
public:
  DEFINE_STANDARD_ALLOC

  Standard_EXPORT Geom2dGcc_FunctionTanCuCuCu(const Geom2dAdaptor_Curve& C1,
                                              const Geom2dAdaptor_Curve& C2,
                                              const Geom2dAdaptor_Curve& C3);

  Standard_EXPORT Geom2dGcc_FunctionTanCuCuCu(const gp_Circ2d&           C1,
                                              const Geom2dAdaptor_Curve& C2,
                                              const Geom2dAdaptor_Curve& C3);

  Standard_EXPORT Geom2dGcc_FunctionTanCuCuCu(const gp_Circ2d&           C1,
                                              const gp_Circ2d&           C2,
                                              const Geom2dAdaptor_Curve& C3);

  Standard_EXPORT Geom2dGcc_FunctionTanCuCuCu(const gp_Circ2d&           C1,
                                              const gp_Lin2d&            L2,
                                              const Geom2dAdaptor_Curve& C3);

  Standard_EXPORT Geom2dGcc_FunctionTanCuCuCu(const gp_Lin2d&            L1,
                                              const gp_Lin2d&            L2,
                                              const Geom2dAdaptor_Curve& C3);

  Standard_EXPORT Geom2dGcc_FunctionTanCuCuCu(const gp_Lin2d&            L1,
                                              const Geom2dAdaptor_Curve& C2,
                                              const Geom2dAdaptor_Curve& C3);

  Standard_EXPORT Geom2dGcc_FunctionTanCuCuCu(const gp_Circ2d&           C1,
                                              const Geom2dAdaptor_Curve& C2,
                                              const gp_Pnt2d&            P3);

  Standard_EXPORT Geom2dGcc_FunctionTanCuCuCu(const gp_Lin2d&            L1,
                                              const Geom2dAdaptor_Curve& C2,
                                              const gp_Pnt2d&            P3);

  Standard_EXPORT Geom2dGcc_FunctionTanCuCuCu(const Geom2dAdaptor_Curve& C1,
                                              const gp_Pnt2d&            P2,
                                              const gp_Pnt2d&            P3);

  Standard_EXPORT void InitDerivative(const math_Vector& X,
                                      gp_Pnt2d&          Point1,
                                      gp_Pnt2d&          Point2,
                                      gp_Pnt2d&          Point3,
                                      gp_Vec2d&          Tan1,
                                      gp_Vec2d&          Tan2,
                                      gp_Vec2d&          Tan3,
                                      gp_Vec2d&          D21,
                                      gp_Vec2d&          D22,
                                      gp_Vec2d&          D23);

  //! Returns the number of variables of the function.
  Standard_EXPORT Standard_Integer NbVariables() const;

  //! Returns the number of equations of the function.
  Standard_EXPORT Standard_Integer NbEquations() const;

  //! Computes the values of the Functions for the variable <X>.
  Standard_EXPORT Standard_Boolean Value(const math_Vector& X, math_Vector& F);

  //! Returns the values of the derivatives for the variable <X>.
  Standard_EXPORT Standard_Boolean Derivatives(const math_Vector& X, math_Matrix& D);

  //! Returns the values of the functions and the derivatives
  //! for the variable <X>.
  Standard_EXPORT Standard_Boolean Values(const math_Vector& X, math_Vector& F, math_Matrix& D);

protected:
private:
  Geom2dAdaptor_Curve Curv1;
  Geom2dAdaptor_Curve Curv2;
  Geom2dAdaptor_Curve Curv3;
  gp_Circ2d           Circ1;
  gp_Circ2d           Circ2;
  gp_Lin2d            Lin1;
  gp_Lin2d            Lin2;
  Geom2dGcc_Type1     TheType;
};

#endif // _Geom2dGcc_FunctionTanCuCuCu_HeaderFile
