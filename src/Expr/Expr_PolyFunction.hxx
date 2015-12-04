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

#ifndef _Expr_PolyFunction_HeaderFile
#define _Expr_PolyFunction_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <Expr_PolyExpression.hxx>
#include <Expr_Array1OfGeneralExpression.hxx>
#include <Standard_Boolean.hxx>
#include <Standard_Real.hxx>
#include <Expr_Array1OfNamedUnknown.hxx>
#include <TColStd_Array1OfReal.hxx>
class Expr_GeneralFunction;
class Standard_NumericError;
class Standard_OutOfRange;
class Expr_NotEvaluable;
class Expr_GeneralExpression;
class Expr_NamedUnknown;
class TCollection_AsciiString;


class Expr_PolyFunction;
DEFINE_STANDARD_HANDLE(Expr_PolyFunction, Expr_PolyExpression)

//! Defines the use of an n-ary function in an expression
//! with given arguments.
class Expr_PolyFunction : public Expr_PolyExpression
{

public:

  
  //! Creates <me> as <func>(<exps_1>,<exps_2>,...,<exps_n>)
  Standard_EXPORT Expr_PolyFunction(const Handle(Expr_GeneralFunction)& func, const Expr_Array1OfGeneralExpression& exps);
  
  //! Returns the function defining <me>.
  Standard_EXPORT Handle(Expr_GeneralFunction) Function() const;
  
  //! Returns a GeneralExpression after a simplification
  //! of the arguments of <me>.
  Standard_EXPORT Handle(Expr_GeneralExpression) ShallowSimplified() const Standard_OVERRIDE;
  
  //! Returns a copy of <me> having the same unknowns and functions.
  Standard_EXPORT Handle(Expr_GeneralExpression) Copy() const Standard_OVERRIDE;
  
  //! Tests if <me> and <Other> define the same expression.
  //! This method does not include any simplification before
  //! testing.
  Standard_EXPORT Standard_Boolean IsIdentical (const Handle(Expr_GeneralExpression)& Other) const Standard_OVERRIDE;
  
  Standard_EXPORT Standard_Boolean IsLinear() const Standard_OVERRIDE;
  
  //! Returns the derivative on <X> unknown of <me>
  Standard_EXPORT Handle(Expr_GeneralExpression) Derivative (const Handle(Expr_NamedUnknown)& X) const Standard_OVERRIDE;
  
  //! Returns the value of <me> (as a Real) by
  //! replacement of <vars> by <vals>.
  //! Raises NotEvaluable if <me> contains NamedUnknown not
  //! in <vars> or NumericError if result cannot be computed.
  Standard_EXPORT Standard_Real Evaluate (const Expr_Array1OfNamedUnknown& vars, const TColStd_Array1OfReal& vals) const Standard_OVERRIDE;
  
  //! returns a string representing <me> in a readable way.
  Standard_EXPORT TCollection_AsciiString String() const Standard_OVERRIDE;




  DEFINE_STANDARD_RTTIEXT(Expr_PolyFunction,Expr_PolyExpression)

protected:




private:


  Handle(Expr_GeneralFunction) myFunction;


};







#endif // _Expr_PolyFunction_HeaderFile
