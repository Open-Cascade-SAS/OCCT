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

#ifndef _Expr_UnaryExpression_HeaderFile
#define _Expr_UnaryExpression_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <Expr_GeneralExpression.hxx>
#include <Standard_Integer.hxx>
class Expr_NamedUnknown;

class Expr_UnaryExpression : public Expr_GeneralExpression
{

public:
  //! Returns the operand used
  const occ::handle<Expr_GeneralExpression>& Operand() const;

  //! Sets the operand used
  //! Raises InvalidOperand if <exp> contains <me>.
  Standard_EXPORT void SetOperand(const occ::handle<Expr_GeneralExpression>& exp);

  //! Returns the number of sub-expressions contained
  //! in <me> ( >= 0)
  Standard_EXPORT int NbSubExpressions() const override;

  //! Returns the <I>-th sub-expression of <me>.
  //! Raises OutOfRange if <I> > NbSubExpressions(me)
  Standard_EXPORT const occ::handle<Expr_GeneralExpression>& SubExpression(
    const int I) const override;

  //! Does <me> contains NamedUnknown ?
  Standard_EXPORT bool ContainsUnknowns() const override;

  //! Tests if <exp> is contained in <me>.
  Standard_EXPORT bool
    Contains(const occ::handle<Expr_GeneralExpression>& exp) const override;

  //! Replaces all occurrences of <var> with <with> in <me>
  //! Raises InvalidOperand if <with> contains <me>.
  Standard_EXPORT void Replace(const occ::handle<Expr_NamedUnknown>&      var,
                               const occ::handle<Expr_GeneralExpression>& with) override;

  //! Returns a GeneralExpression after replacement of
  //! NamedUnknowns by an associated expression, and after
  //! values computation.
  Standard_EXPORT occ::handle<Expr_GeneralExpression> Simplified() const override;

  DEFINE_STANDARD_RTTIEXT(Expr_UnaryExpression, Expr_GeneralExpression)

protected:
  //! Sets the operand used during creation
  Standard_EXPORT void CreateOperand(const occ::handle<Expr_GeneralExpression>& exp);

private:
  occ::handle<Expr_GeneralExpression> myOperand;
};

#include <Expr_UnaryExpression.lxx>

#endif // _Expr_UnaryExpression_HeaderFile
