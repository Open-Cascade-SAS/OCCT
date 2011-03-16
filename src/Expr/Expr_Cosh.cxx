//static const char* sccsid = "@(#)Expr_Cosh.cxx	3.2 95/01/10"; // Do not delete this line. Used by sccs.
// Copyright: 	Matra-Datavision 1991
// File:	Expr_Cosh.cxx
// Created:	Mon May 27 15:43:41 1991
// Author:	Arnaud BOUZY
//		<adn>

#include <Expr_Cosh.ixx>
#include <Expr_NumericValue.hxx>
#include <Expr_ArgCosh.hxx>
#include <Expr_Sinh.hxx>
#include <Expr_Product.hxx>
#include <Expr_Operators.hxx>
#include <Expr.hxx>

Expr_Cosh::Expr_Cosh(const Handle(Expr_GeneralExpression)& exp)
{
  CreateOperand(exp);
}

Handle(Expr_GeneralExpression) Expr_Cosh::ShallowSimplified () const
{
  Handle(Expr_GeneralExpression) myexp = Operand();
  if (myexp->IsKind(STANDARD_TYPE(Expr_NumericValue))) {
    Handle(Expr_NumericValue) myNVexp = Handle(Expr_NumericValue)::DownCast(myexp);
    return new Expr_NumericValue(Cosh(myNVexp->GetValue()));
  }
  if (myexp->IsKind(STANDARD_TYPE(Expr_ArgCosh))) {
    return myexp->SubExpression(1);
  }
  Handle(Expr_Cosh) me = this;
  return me;
}

Handle(Expr_GeneralExpression) Expr_Cosh::Copy () const
{
  return new Expr_Cosh(Expr::CopyShare(Operand()));
}

Standard_Boolean Expr_Cosh::IsIdentical (const Handle(Expr_GeneralExpression)& Other) const
{
  if (Other->IsKind(STANDARD_TYPE(Expr_Cosh))) {
    Handle(Expr_GeneralExpression) myexp = Operand();
    return myexp->IsIdentical(Other->SubExpression(1));
  }
  return Standard_False;
}

Standard_Boolean Expr_Cosh::IsLinear () const
{
  return !ContainsUnknowns();
}

Handle(Expr_GeneralExpression) Expr_Cosh::Derivative (const Handle(Expr_NamedUnknown)& X) const
{
  if (!Contains(X)) {
    return  new Expr_NumericValue(0.0);
  }
  Handle(Expr_GeneralExpression) myexp = Operand();
  Handle(Expr_GeneralExpression) myder = myexp->Derivative(X);
  Handle(Expr_Sinh) firstder = new Expr_Sinh(Expr::CopyShare(myexp));

  Handle(Expr_Product) resu = firstder->ShallowSimplified() * myder;
  return resu->ShallowSimplified();
}

Standard_Real Expr_Cosh::Evaluate(const Expr_Array1OfNamedUnknown& vars, const TColStd_Array1OfReal& vals) const
{
  Standard_Real val = Operand()->Evaluate(vars,vals);
  return (::Exp(val)+::Exp(-val))/2.0;
}

TCollection_AsciiString Expr_Cosh::String() const
{
  TCollection_AsciiString str("Cosh(");
  str += Operand()->String();
  str += ")";
  return str;
}
