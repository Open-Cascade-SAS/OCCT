//static const char* sccsid = "@(#)Expr_Square.cxx	3.2 95/01/10"; // Do not delete this line. Used by sccs.
// Copyright: 	Matra-Datavision 1991
// File:	Expr_Square.cxx
// Created:	Fri Apr 19 10:05:26 1991
// Author:	Arnaud BOUZY
//		<adn>

#include <Expr_Square.ixx>
#include <Expr_NumericValue.hxx>
#include <Expr_SquareRoot.hxx>
#include <Expr_SequenceOfGeneralExpression.hxx>
#include <Expr_Product.hxx>
#include <Expr_Exponentiate.hxx>
#include <Expr_Operators.hxx>
#include <Expr.hxx>

Expr_Square::Expr_Square (const Handle(Expr_GeneralExpression)& exp)
{
  CreateOperand(exp);
}

Handle(Expr_GeneralExpression) Expr_Square::ShallowSimplified () const
{
  Handle(Expr_GeneralExpression) myexp = Operand();
  if (myexp->IsKind(STANDARD_TYPE(Expr_NumericValue))) {
    Handle(Expr_NumericValue) myNVexp = Handle(Expr_NumericValue)::DownCast(myexp);
    return new Expr_NumericValue(Square(myNVexp->GetValue()));
  }
  if (myexp->IsKind(STANDARD_TYPE(Expr_SquareRoot))) {
    return myexp->SubExpression(1);
  }
  if (myexp->IsKind(STANDARD_TYPE(Expr_Square))) {
    Handle(Expr_GeneralExpression) op = myexp->SubExpression(1);
    Handle(Expr_NumericValue) val4 = new Expr_NumericValue(4.0);
    return new Expr_Exponentiate(op,val4);
  }
  if (myexp->IsKind(STANDARD_TYPE(Expr_Exponentiate))) {
    Handle(Expr_GeneralExpression) op = myexp->SubExpression(1);
    Handle(Expr_GeneralExpression) puis = myexp->SubExpression(2);
    Handle(Expr_Product) newpuis = 2.0 * puis;
    Handle(Expr_Exponentiate) res = new Expr_Exponentiate(op,newpuis->ShallowSimplified());
    return res->ShallowSimplified();
  }
  Handle(Expr_Square) me = this;
  return me;
}

Handle(Expr_GeneralExpression) Expr_Square::Copy () const
{
  return new Expr_Square(Expr::CopyShare(Operand()));
}

Standard_Boolean Expr_Square::IsIdentical (const Handle(Expr_GeneralExpression)& Other) const
{
  if (Other->IsKind(STANDARD_TYPE(Expr_Square))) {
    return Operand()->IsIdentical(Other->SubExpression(1));
  }
  return Standard_False;
}

Standard_Boolean Expr_Square::IsLinear () const
{
  return !ContainsUnknowns();
}

Handle(Expr_GeneralExpression) Expr_Square::Derivative (const Handle(Expr_NamedUnknown)& X) const
{
  if (!Contains(X)) {
    return  new Expr_NumericValue(0.0);
  }
  Handle(Expr_GeneralExpression) myder = Operand();
  myder = myder->Derivative(X);
  Handle(Expr_NumericValue) coef = new Expr_NumericValue(2.0);
  Expr_SequenceOfGeneralExpression ops;
  ops.Append(coef);
  ops.Append(myder);
  Handle(Expr_GeneralExpression) usedop = Expr::CopyShare(Operand());
  ops.Append(usedop);
  Handle(Expr_Product) resu = new Expr_Product(ops);
  return resu->ShallowSimplified();
}

Standard_Real Expr_Square::Evaluate(const Expr_Array1OfNamedUnknown& vars, const TColStd_Array1OfReal& vals) const
{
  Standard_Real val = Operand()->Evaluate(vars,vals);
  return val*val;
}

TCollection_AsciiString Expr_Square::String() const
{
  TCollection_AsciiString str;
  Handle(Expr_GeneralExpression) op = Operand();
  if (op->NbSubExpressions() > 1) {
    str = "(";
    str += op->String();
    str += ")^2";
  }
  else {
    str = op->String();
    str += "^2";
  }
  return str;
}
