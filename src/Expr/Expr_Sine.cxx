//static const char* sccsid = "@(#)Expr_Sine.cxx	3.2 95/01/10"; // Do not delete this line. Used by sccs.
// Copyright: 	Matra-Datavision 1991
// File:	Expr_Sine.cxx
// Created:	Tue May 28 13:00:09 1991
// Author:	Arnaud BOUZY
//		<adn>

#include <Expr_Sine.ixx>
#include <Expr_NumericValue.hxx>
#include <Expr_ArcSine.hxx>
#include <Expr_Cosine.hxx>
#include <Expr_Product.hxx>
#include <Expr_Operators.hxx>
#include <Expr.hxx>

Expr_Sine::Expr_Sine(const Handle(Expr_GeneralExpression)& exp)
{
  CreateOperand(exp);
}

Handle(Expr_GeneralExpression) Expr_Sine::ShallowSimplified () const
{
  Handle(Expr_GeneralExpression) myexp = Operand();
  if (myexp->IsKind(STANDARD_TYPE(Expr_NumericValue))) {
    Handle(Expr_NumericValue) myNVexp = Handle(Expr_NumericValue)::DownCast(myexp);
    return new Expr_NumericValue(Sin(myNVexp->GetValue()));
  }
  if (myexp->IsKind(STANDARD_TYPE(Expr_ArcSine))) {
    return myexp->SubExpression(1);
  }
  Handle(Expr_Sine) me = this;
  return me;
}

Handle(Expr_GeneralExpression) Expr_Sine::Copy () const
{
  return new Expr_Sine(Expr::CopyShare(Operand()));
}

Standard_Boolean Expr_Sine::IsIdentical (const Handle(Expr_GeneralExpression)& Other) const
{
  if (Other->IsKind(STANDARD_TYPE(Expr_Sine))) {
    Handle(Expr_GeneralExpression) myexp = Operand();
    return myexp->IsIdentical(Other->SubExpression(1));
  }
  return Standard_False;
}

Standard_Boolean Expr_Sine::IsLinear () const
{
  return !ContainsUnknowns();
}

Handle(Expr_GeneralExpression) Expr_Sine::Derivative (const Handle(Expr_NamedUnknown)& X) const
{
  if (!Contains(X)) {
    return new Expr_NumericValue(0.0);
  }
  Handle(Expr_GeneralExpression) myexp = Operand();
  Handle(Expr_GeneralExpression) myder = myexp->Derivative(X);
  Handle(Expr_Cosine) firstder = new Expr_Cosine(Expr::CopyShare(myexp));
  Handle(Expr_Product) resu = firstder->ShallowSimplified() * myder;
  return resu->ShallowSimplified();
}

Standard_Real Expr_Sine::Evaluate(const Expr_Array1OfNamedUnknown& vars, const TColStd_Array1OfReal& vals) const
{
  return ::Sin(Operand()->Evaluate(vars,vals));
}

TCollection_AsciiString Expr_Sine::String() const
{
  TCollection_AsciiString str("Sin(");
  str += Operand()->String();
  str += ")";
  return str;
}
