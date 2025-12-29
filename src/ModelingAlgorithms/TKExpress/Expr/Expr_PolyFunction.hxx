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
#include <Expr_GeneralExpression.hxx>
#include <NCollection_Array1.hxx>
class Expr_GeneralFunction;
class Expr_GeneralExpression;
class Expr_NamedUnknown;
class TCollection_AsciiString;

//! Defines the use of an n-ary function in an expression
//! with given arguments.
class Expr_PolyFunction : public Expr_PolyExpression
{

public:
  //! Creates <me> as <func>(<exps_1>,<exps_2>,...,<exps_n>)
  Standard_EXPORT Expr_PolyFunction(
    const occ::handle<Expr_GeneralFunction>&                       func,
    const NCollection_Array1<occ::handle<Expr_GeneralExpression>>& exps);

  //! Returns the function defining <me>.
  Standard_EXPORT occ::handle<Expr_GeneralFunction> Function() const;

  //! Returns a GeneralExpression after a simplification
  //! of the arguments of <me>.
  Standard_EXPORT occ::handle<Expr_GeneralExpression> ShallowSimplified() const override;

  //! Returns a copy of <me> having the same unknowns and functions.
  Standard_EXPORT occ::handle<Expr_GeneralExpression> Copy() const override;

  //! Tests if <me> and <Other> define the same expression.
  //! This method does not include any simplification before
  //! testing.
  Standard_EXPORT bool IsIdentical(const occ::handle<Expr_GeneralExpression>& Other) const override;

  Standard_EXPORT bool IsLinear() const override;

  //! Returns the derivative on <X> unknown of <me>
  Standard_EXPORT occ::handle<Expr_GeneralExpression> Derivative(
    const occ::handle<Expr_NamedUnknown>& X) const override;

  //! Returns the value of <me> (as a Real) by
  //! replacement of <vars> by <vals>.
  //! Raises NotEvaluable if <me> contains NamedUnknown not
  //! in <vars> or NumericError if result cannot be computed.
  Standard_EXPORT double Evaluate(const NCollection_Array1<occ::handle<Expr_NamedUnknown>>& vars,
                                  const NCollection_Array1<double>& vals) const override;

  //! returns a string representing <me> in a readable way.
  Standard_EXPORT TCollection_AsciiString String() const override;

  DEFINE_STANDARD_RTTIEXT(Expr_PolyFunction, Expr_PolyExpression)

private:
  occ::handle<Expr_GeneralFunction> myFunction;
};

#endif // _Expr_PolyFunction_HeaderFile
