//static const char* sccsid = "@(#)Expr_UnaryMinus.cxx	3.2 95/01/10"; // Do not delete this line. Used by sccs.
// Copyright: 	Matra-Datavision 1991
// File:	Expr_UnaryMinus.cxx
// Created:	Tue Apr 16 17:02:04 1991
// Author:	Arnaud BOUZY
//		<adn>

#include <Expr_UnaryMinus.ixx>
#include <Expr_NumericValue.hxx>
#include <Expr_Operators.hxx>
#include <Expr.hxx>
#include <Standard_OutOfRange.hxx>

Expr_UnaryMinus::Expr_UnaryMinus(const Handle(Expr_GeneralExpression)& exp)
{
  CreateOperand(exp);
}

Handle(Expr_GeneralExpression) Expr_UnaryMinus::ShallowSimplified () const
{
  Handle(Expr_GeneralExpression) myexp = Operand();
  if (myexp->IsKind(STANDARD_TYPE(Expr_NumericValue))) {
    Handle(Expr_NumericValue) myNVexp = Handle(Expr_NumericValue)::DownCast(myexp);
    return new Expr_NumericValue(-myNVexp->GetValue());
  }
  if (myexp->IsKind(STANDARD_TYPE(Expr_UnaryMinus))) {
    return myexp->SubExpression(1);
  }
  Handle(Expr_UnaryMinus) me = this;
  return me;
}

Handle(Expr_GeneralExpression) Expr_UnaryMinus::Copy () const
{
  return -(Expr::CopyShare(Operand()));
}

Standard_Boolean Expr_UnaryMinus::IsIdentical (const Handle(Expr_GeneralExpression)& Other) const
{
  if (Other->IsKind(STANDARD_TYPE(Expr_UnaryMinus))) {
    Handle(Expr_GeneralExpression) myexp = Operand();
    return myexp->IsIdentical(Other->SubExpression(1));
  }
  return Standard_False;
}

Standard_Boolean Expr_UnaryMinus::IsLinear () const
{
  Handle(Expr_GeneralExpression) myexp = Operand();
  return myexp->IsLinear();
}

Handle(Expr_GeneralExpression) Expr_UnaryMinus::Derivative (const Handle(Expr_NamedUnknown)& X) const
{
  Handle(Expr_GeneralExpression) myder = Operand();
  myder = myder->Derivative(X);
  Handle(Expr_UnaryMinus) resu = - myder;
  return resu->ShallowSimplified();
}

Handle(Expr_GeneralExpression) Expr_UnaryMinus::NDerivative (const Handle(Expr_NamedUnknown)& X, const Standard_Integer N) const
{
  if (N <= 0) {
    Standard_OutOfRange::Raise();
  }
  Handle(Expr_GeneralExpression) myder = Operand();
  myder = myder->NDerivative(X,N);
  Handle(Expr_UnaryMinus) resu = - myder;
  return resu->ShallowSimplified();
}

Standard_Real Expr_UnaryMinus::Evaluate(const Expr_Array1OfNamedUnknown& vars, const TColStd_Array1OfReal& vals) const
{
  return - Operand()->Evaluate(vars,vals);
}

TCollection_AsciiString Expr_UnaryMinus::String() const
{
  TCollection_AsciiString str;
  Handle(Expr_GeneralExpression) op = Operand();
  if (op->NbSubExpressions() > 1) {
    str = "-(";
    str += op->String();
    str += ")";
  }
  else {
    str = "-";
    str += op->String();
  }
  return str;
}
