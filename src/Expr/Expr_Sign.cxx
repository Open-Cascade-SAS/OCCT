//static const char* sccsid = "@(#)Expr_Sign.cxx	3.2 95/01/10"; // Do not delete this line. Used by sccs.
// Copyright: 	Matra-Datavision 1991
// File:	Expr_Sign.cxx
// Created:	Tue May 28 12:54:14 1991
// Author:	Arnaud BOUZY
//		<adn>

#include <Expr_Sign.ixx>
#include <Expr_NumericValue.hxx>
#include <Expr.hxx>

Expr_Sign::Expr_Sign (const Handle(Expr_GeneralExpression)& exp)
{
  CreateOperand(exp);
}

Handle(Expr_GeneralExpression) Expr_Sign::ShallowSimplified () const
{
  Handle(Expr_GeneralExpression) op = Operand();
  if (op->IsKind(STANDARD_TYPE(Expr_NumericValue))) {
    Handle(Expr_NumericValue) valop = Handle(Expr_NumericValue)::DownCast(op);
    return new Expr_NumericValue(Expr::Sign(valop->GetValue()));
  }
  Handle(Expr_Sign) me = this;
  return me;
}

Handle(Expr_GeneralExpression) Expr_Sign::Copy () const 
{
  return  new Expr_Sign(Expr::CopyShare(Operand()));
}

Standard_Boolean Expr_Sign::IsIdentical (const Handle(Expr_GeneralExpression)& Other) const
{
  if (!Other->IsKind(STANDARD_TYPE(Expr_Sign))) {
    return Standard_False;
  }
  Handle(Expr_GeneralExpression) op = Operand();
  return op->IsIdentical(Other->SubExpression(1));
}

Standard_Boolean Expr_Sign::IsLinear () const
{
  return !ContainsUnknowns();
}

Handle(Expr_GeneralExpression) Expr_Sign::Derivative (const Handle(Expr_NamedUnknown)& ) const
{
  return new Expr_NumericValue(0.0);
}

Standard_Real Expr_Sign::Evaluate(const Expr_Array1OfNamedUnknown& vars, const TColStd_Array1OfReal& vals) const
{
  Standard_Real res = Operand()->Evaluate(vars,vals);
  return Expr::Sign(res);
}

TCollection_AsciiString Expr_Sign::String() const
{
  TCollection_AsciiString str("Sign(");
  str += Operand()->String();
  str += ")";
  return str;
}
