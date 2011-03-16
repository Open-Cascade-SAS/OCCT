//static const char* sccsid = "@(#)Expr_UnaryExpression.cxx	3.2 95/01/10"; // Do not delete this line. Used by sccs.
// Copyright: 	Matra-Datavision 1991
// File:	Expr_UnaryExpression.cxx
// Created:	Mon Apr 15 14:24:44 1991
// Author:	Arnaud BOUZY
//		<adn>

#include <Expr_UnaryExpression.ixx>
#include <Expr_NamedUnknown.hxx>
#include <Expr_InvalidOperand.hxx>
#include <Standard_OutOfRange.hxx>

void Expr_UnaryExpression::SetOperand (const Handle(Expr_GeneralExpression)& exp)
{
  Handle(Expr_UnaryExpression) me = this;
  if (exp == me) {
    Expr_InvalidOperand::Raise();
  }
  if (exp->Contains(me)) {
    Expr_InvalidOperand::Raise();
  }
  myOperand = exp;
}

void Expr_UnaryExpression::CreateOperand (const Handle(Expr_GeneralExpression)& exp)
{
  myOperand = exp;
}

Standard_Integer Expr_UnaryExpression::NbSubExpressions () const
{
  return 1;
}

const Handle(Expr_GeneralExpression)& Expr_UnaryExpression::SubExpression (const Standard_Integer I) const
{
  if (I != 1) {
    Standard_OutOfRange::Raise();
  }
  return myOperand;
}

Standard_Boolean Expr_UnaryExpression::ContainsUnknowns () const
{
  if (!myOperand->IsKind(STANDARD_TYPE(Expr_NamedUnknown))) {
    return myOperand->ContainsUnknowns();
  }
  return Standard_True;
}

Standard_Boolean Expr_UnaryExpression::Contains (const Handle(Expr_GeneralExpression)& exp) const
{
  if (myOperand != exp) {
    return myOperand->Contains(exp);
  }
  return Standard_True;
}

void Expr_UnaryExpression::Replace (const Handle(Expr_NamedUnknown)& var, const Handle(Expr_GeneralExpression)& with)
{
  if (myOperand == var) {
    SetOperand(with);
  }
  else {
    if (myOperand->Contains(var)) {
      myOperand->Replace(var,with);
    }
  }
}


Handle(Expr_GeneralExpression) Expr_UnaryExpression::Simplified() const
{
  Handle(Expr_UnaryExpression) cop = Handle(Expr_UnaryExpression)::DownCast(Copy());
  Handle(Expr_GeneralExpression) op = cop->Operand();
  cop->SetOperand(op->Simplified());
  return cop->ShallowSimplified();
}

