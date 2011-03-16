//static const char* sccsid = "@(#)Expr_Exponential.cxx	3.2 95/01/10"; // Do not delete this line. Used by sccs.
// Copyright: 	Matra-Datavision 1991
// File:	Expr_Exponential.cxx
// Created:	Mon May 27 17:30:32 1991
// Author:	Arnaud BOUZY
//		<adn>

#include <Expr_Exponential.ixx>
#include <Expr_NumericValue.hxx>
#include <Expr_LogOfe.hxx>
#include <Expr_Product.hxx>
#include <Expr_Operators.hxx>
#include <Expr.hxx>

Expr_Exponential::Expr_Exponential(const Handle(Expr_GeneralExpression)& exp)
{
  CreateOperand(exp);
}

Handle(Expr_GeneralExpression) Expr_Exponential::ShallowSimplified () const
{
  Handle(Expr_GeneralExpression) myexp = Operand();
  if (myexp->IsKind(STANDARD_TYPE(Expr_NumericValue))) {
    Handle(Expr_NumericValue) myNVexp = Handle(Expr_NumericValue)::DownCast(myexp);
    return new Expr_NumericValue(Exp(myNVexp->GetValue()));
  }
  if (myexp->IsKind(STANDARD_TYPE(Expr_LogOfe))) {
    return myexp->SubExpression(1);
  }
  Handle(Expr_Exponential) me = this;
  return me;
}

Handle(Expr_GeneralExpression) Expr_Exponential::Copy () const
{
  return new Expr_Exponential(Expr::CopyShare(Operand()));
}

Standard_Boolean Expr_Exponential::IsIdentical (const Handle(Expr_GeneralExpression)& Other) const
{
  if (Other->IsKind(STANDARD_TYPE(Expr_Exponential))) {
    Handle(Expr_GeneralExpression) myexp = Operand();
    return myexp->IsIdentical(Other->SubExpression(1));
  }
  return Standard_False;
}

Standard_Boolean Expr_Exponential::IsLinear () const
{
  return !ContainsUnknowns();
}

Handle(Expr_GeneralExpression) Expr_Exponential::Derivative (const Handle(Expr_NamedUnknown)& X) const
{
  if (!Contains(X)) {
    return new Expr_NumericValue(0.0);
  }
  Handle(Expr_GeneralExpression) myexp = Operand();
  Handle(Expr_GeneralExpression) myder = myexp->Derivative(X);
  Handle(Expr_Product) resu = Expr::CopyShare(this) * myder;
  return resu->ShallowSimplified();
}

Standard_Real Expr_Exponential::Evaluate(const Expr_Array1OfNamedUnknown& vars, const TColStd_Array1OfReal& vals) const
{
  return ::Exp(Operand()->Evaluate(vars,vals));
}

TCollection_AsciiString Expr_Exponential::String() const
{
  TCollection_AsciiString str("Exp(");
  str += Operand()->String();
  str += ")";
  return str;
}
