// Created on: 1991-01-10
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

#ifndef _Expr_GeneralExpression_HeaderFile
#define _Expr_GeneralExpression_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <Standard_Transient.hxx>
#include <Standard_Integer.hxx>
#include <Standard_Real.hxx>
#include <Expr_Array1OfNamedUnknown.hxx>
#include <TColStd_Array1OfReal.hxx>
class Expr_NamedUnknown;
class TCollection_AsciiString;

class Expr_GeneralExpression;
DEFINE_STANDARD_HANDLE(Expr_GeneralExpression, Standard_Transient)

//! Defines the general purposes of any expression.
class Expr_GeneralExpression : public Standard_Transient
{

public:
  //! Returns the number of sub-expressions contained
  //! in <me> ( >= 0)
  Standard_EXPORT virtual Standard_Integer NbSubExpressions() const = 0;

  //! Returns the <I>-th sub-expression of <me>
  //! raises OutOfRange if <I> > NbSubExpressions(me)
  Standard_EXPORT virtual const Handle(Expr_GeneralExpression)& SubExpression(
    const Standard_Integer I) const = 0;

  //! Returns a GeneralExpression after replacement of
  //! NamedUnknowns by an associated expression and after
  //! values computation.
  Standard_EXPORT virtual Handle(Expr_GeneralExpression) Simplified() const = 0;

  //! Returns a GeneralExpression after a simplification
  //! of the arguments of <me>.
  Standard_EXPORT virtual Handle(Expr_GeneralExpression) ShallowSimplified() const = 0;

  //! Returns a copy of <me> having the same unknowns and
  //! functions.
  Standard_EXPORT virtual Handle(Expr_GeneralExpression) Copy() const = 0;

  //! Tests if <me> contains NamedUnknowns.
  Standard_EXPORT virtual Standard_Boolean ContainsUnknowns() const = 0;

  //! Tests if <exp> is contained in <me>.
  Standard_EXPORT virtual Standard_Boolean Contains(
    const Handle(Expr_GeneralExpression)& exp) const = 0;

  //! Tests if <me> is linear on every NamedUnknown it
  //! contains.
  Standard_EXPORT virtual Standard_Boolean IsLinear() const = 0;

  //! Tests if <me> can be shared by one or more expressions
  //! or must be copied. This method returns False as a
  //! default value. To be redefined ( especially for
  //! NamedUnknown).
  Standard_EXPORT virtual Standard_Boolean IsShareable() const;

  //! Tests if <me> and <Other> define the same expression.
  //! Warning: This method does not include any simplification before
  //! testing. It could also be very slow; to be used
  //! carefully.
  Standard_EXPORT virtual Standard_Boolean IsIdentical(
    const Handle(Expr_GeneralExpression)& Other) const = 0;

  //! Returns the derivative on <X> unknown of <me>
  Standard_EXPORT virtual Handle(Expr_GeneralExpression) Derivative(
    const Handle(Expr_NamedUnknown)& X) const = 0;

  //! Returns the <N>-th derivative on <X> unknown of <me>.
  //! Raise OutOfRange if N <= 0
  Standard_EXPORT virtual Handle(Expr_GeneralExpression) NDerivative(
    const Handle(Expr_NamedUnknown)& X,
    const Standard_Integer           N) const;

  //! Replaces all occurrences of <var> with copies of <with>
  //! in <me>. Copies of <with> are made with the Copy() method.
  //! Raises InvalidOperand if <with> contains <me>.
  Standard_EXPORT virtual void Replace(const Handle(Expr_NamedUnknown)&      var,
                                       const Handle(Expr_GeneralExpression)& with) = 0;

  //! Returns the value of <me> (as a Real) by
  //! replacement of <vars> by <vals>.
  //! Raises NotEvaluable if <me> contains NamedUnknown not
  //! in <vars> or NumericError if result cannot be computed.
  Standard_EXPORT virtual Standard_Real Evaluate(const Expr_Array1OfNamedUnknown& vars,
                                                 const TColStd_Array1OfReal&      vals) const = 0;

  //! Returns the value of <me> (as a Real) by
  //! replacement of <vars> by <vals>.
  //! Raises NotEvaluable if <me> contains NamedUnknown not
  //! in <vars> or NumericError if result cannot be computed.
  Standard_EXPORT Standard_Real EvaluateNumeric() const;

  //! returns a string representing <me> in a readable way.
  Standard_EXPORT virtual TCollection_AsciiString String() const = 0;

  DEFINE_STANDARD_RTTIEXT(Expr_GeneralExpression, Standard_Transient)

protected:
private:
};

#endif // _Expr_GeneralExpression_HeaderFile
