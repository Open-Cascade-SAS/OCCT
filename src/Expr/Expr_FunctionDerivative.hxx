// Created on: 1991-01-14
// Created by: Arnaud BOUZY
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

#ifndef _Expr_FunctionDerivative_HeaderFile
#define _Expr_FunctionDerivative_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <Standard_Integer.hxx>
#include <Expr_GeneralFunction.hxx>
#include <Standard_Real.hxx>
#include <Expr_Array1OfNamedUnknown.hxx>
#include <TColStd_Array1OfReal.hxx>
#include <Standard_Boolean.hxx>
class Expr_GeneralExpression;
class Expr_NamedUnknown;
class TCollection_AsciiString;


class Expr_FunctionDerivative;
DEFINE_STANDARD_HANDLE(Expr_FunctionDerivative, Expr_GeneralFunction)


class Expr_FunctionDerivative : public Expr_GeneralFunction
{

public:

  
  //! Creates a FunctionDerivative of degree <deg> relative
  //! to the <withX> variable.
  //! Raises OutOfRange if <deg> lower or equal to zero.
  Standard_EXPORT Expr_FunctionDerivative(const Handle(Expr_GeneralFunction)& func, const Handle(Expr_NamedUnknown)& withX, const Standard_Integer deg);
  
  //! Returns the number of variables of <me>.
  Standard_EXPORT Standard_Integer NbOfVariables() const Standard_OVERRIDE;
  
  //! Returns the variable denoted by <index> in <me>.
  //! Raises OutOfRange if <index> greater than
  //! NbOfVariables of <me>.
  Standard_EXPORT Handle(Expr_NamedUnknown) Variable (const Standard_Integer index) const Standard_OVERRIDE;
  
  //! Computes the value of <me> with the given variables.
  //! Raises DimensionMismatch if Length(vars) is different from
  //! Length(values).
  Standard_EXPORT Standard_Real Evaluate (const Expr_Array1OfNamedUnknown& vars, const TColStd_Array1OfReal& values) const Standard_OVERRIDE;
  
  //! Returns a copy of <me> with the same form.
  Standard_EXPORT Handle(Expr_GeneralFunction) Copy() const Standard_OVERRIDE;
  
  //! Returns Derivative of <me> for variable <var>.
  Standard_EXPORT Handle(Expr_GeneralFunction) Derivative (const Handle(Expr_NamedUnknown)& var) const Standard_OVERRIDE;
  
  //! Returns Derivative of <me> for variable <var> with
  //! degree <deg>.
  Standard_EXPORT Handle(Expr_GeneralFunction) Derivative (const Handle(Expr_NamedUnknown)& var, const Standard_Integer deg) const Standard_OVERRIDE;
  
  //! Tests if <me> and <func> are similar functions (same
  //! name and same used expression).
  Standard_EXPORT Standard_Boolean IsIdentical (const Handle(Expr_GeneralFunction)& func) const Standard_OVERRIDE;
  
  //! Tests if <me> is linear on variable on range <index>
  Standard_EXPORT Standard_Boolean IsLinearOnVariable (const Standard_Integer index) const Standard_OVERRIDE;
  
  //! Returns the function of which <me> is the derivative.
  Standard_EXPORT Handle(Expr_GeneralFunction) Function() const;
  
  //! Returns the degree of derivation of <me>.
  Standard_EXPORT Standard_Integer Degree() const;
  
  //! Returns the derivation variable of <me>.
  Standard_EXPORT Handle(Expr_NamedUnknown) DerivVariable() const;
  
  Standard_EXPORT TCollection_AsciiString GetStringName() const Standard_OVERRIDE;
  
  Standard_EXPORT Handle(Expr_GeneralExpression) Expression() const;
  
  Standard_EXPORT void UpdateExpression();


friend class Expr_NamedFunction;


  DEFINE_STANDARD_RTTIEXT(Expr_FunctionDerivative,Expr_GeneralFunction)

protected:




private:


  Handle(Expr_GeneralFunction) myFunction;
  Handle(Expr_GeneralExpression) myExp;
  Handle(Expr_NamedUnknown) myDerivate;
  Standard_Integer myDegree;


};







#endif // _Expr_FunctionDerivative_HeaderFile
