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

#ifndef _Expr_PolyExpression_HeaderFile
#define _Expr_PolyExpression_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <Expr_SequenceOfGeneralExpression.hxx>
#include <Expr_GeneralExpression.hxx>
#include <Standard_Integer.hxx>
class Expr_NamedUnknown;

class Expr_PolyExpression;
DEFINE_STANDARD_HANDLE(Expr_PolyExpression, Expr_GeneralExpression)

class Expr_PolyExpression : public Expr_GeneralExpression
{

public:
  //! returns the number of operands contained in <me>
  Standard_EXPORT Standard_Integer NbOperands() const;

  //! Returns the <index>-th operand used in <me>.
  //! An exception is raised if index is out of range
  const Handle(Expr_GeneralExpression)& Operand(const Standard_Integer index) const;

  //! Sets the <index>-th operand used in <me>.
  //! An exception is raised if <index> is out of range
  //! Raises InvalidOperand if <exp> contains <me>.
  Standard_EXPORT void SetOperand(const Handle(Expr_GeneralExpression)& exp,
                                  const Standard_Integer                index);

  //! returns the number of sub-expressions contained
  //! in <me> ( >= 2)
  Standard_EXPORT Standard_Integer NbSubExpressions() const Standard_OVERRIDE;

  //! Returns the sub-expression denoted by <I> in <me>
  //! Raises OutOfRange if <I> > NbSubExpressions(me)
  Standard_EXPORT const Handle(Expr_GeneralExpression)& SubExpression(
    const Standard_Integer I) const Standard_OVERRIDE;

  //! Does <me> contains NamedUnknown ?
  Standard_EXPORT Standard_Boolean ContainsUnknowns() const Standard_OVERRIDE;

  //! Tests if <exp> is contained in <me>.
  Standard_EXPORT Standard_Boolean
    Contains(const Handle(Expr_GeneralExpression)& exp) const Standard_OVERRIDE;

  //! Replaces all occurrences of <var> with <with> in <me>
  //! Raises InvalidOperand if <with> contains <me>.
  Standard_EXPORT void Replace(const Handle(Expr_NamedUnknown)&      var,
                               const Handle(Expr_GeneralExpression)& with) Standard_OVERRIDE;

  //! Returns a GeneralExpression after replacement of
  //! NamedUnknowns by an associated expression and after
  //! values computation.
  Standard_EXPORT Handle(Expr_GeneralExpression) Simplified() const Standard_OVERRIDE;

  DEFINE_STANDARD_RTTIEXT(Expr_PolyExpression, Expr_GeneralExpression)

protected:
  //! initialize an empty list of operands.
  Standard_EXPORT Expr_PolyExpression();

  //! Adds an operand to the list of <me>.
  Standard_EXPORT void AddOperand(const Handle(Expr_GeneralExpression)& exp);

  //! Remove the operand denoted by <index> from the list of
  //! <me>.
  //! Raises exception if <index> is out of range or if
  //! removing operand intend to leave only one or no
  //! operand.
  Standard_EXPORT void RemoveOperand(const Standard_Integer index);

private:
  Expr_SequenceOfGeneralExpression myOperands;
};

#include <Expr_PolyExpression.lxx>

#endif // _Expr_PolyExpression_HeaderFile
