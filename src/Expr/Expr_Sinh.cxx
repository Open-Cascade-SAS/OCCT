//static const char* sccsid = "@(#)Expr_Sinh.cxx	3.2 95/01/10"; // Do not delete this line. Used by sccs.
// Copyright: 	Matra-Datavision 1991
// File:	Expr_Sinh.cxx
// Created:	Tue May 28 13:05:53 1991
// Author:	Arnaud BOUZY
//		<adn>

#include <Expr_Sinh.ixx>
#include <Expr_NumericValue.hxx>
#include <Expr_ArgSinh.hxx>
#include <Expr_Cosh.hxx>
#include <Expr_Product.hxx>
#include <Expr_Operators.hxx>
#include <Expr.hxx>

Expr_Sinh::Expr_Sinh(const Handle(Expr_GeneralExpression)& exp)
{
  CreateOperand(exp);
}

Handle(Expr_GeneralExpression) Expr_Sinh::ShallowSimplified () const
{
  Handle(Expr_GeneralExpression) myexp = Operand();
  if (myexp->IsKind(STANDARD_TYPE(Expr_NumericValue))) {
    Handle(Expr_NumericValue) myNVexp = Handle(Expr_NumericValue)::DownCast(myexp);
    return new Expr_NumericValue(Sinh(myNVexp->GetValue()));
  }
  if (myexp->IsKind(STANDARD_TYPE(Expr_ArgSinh))) {
    return myexp->SubExpression(1);
  }
  Handle(Expr_Sinh) me = this;
  return me;
}

Handle(Expr_GeneralExpression) Expr_Sinh::Copy () const
{
  return new Expr_Sinh(Expr::CopyShare(Operand()));
}

Standard_Boolean Expr_Sinh::IsIdentical (const Handle(Expr_GeneralExpression)& Other) const
{
  if (Other->IsKind(STANDARD_TYPE(Expr_Sinh))) {
    Handle(Expr_GeneralExpression) myexp = Operand();
    return myexp->IsIdentical(Other->SubExpression(1));
  }
  return Standard_False;
}

Standard_Boolean Expr_Sinh::IsLinear () const
{
  return !ContainsUnknowns();
}

Handle(Expr_GeneralExpression) Expr_Sinh::Derivative (const Handle(Expr_NamedUnknown)& X) const
{
  if (!Contains(X)) {
    return  new Expr_NumericValue(0.0);
  }
  Handle(Expr_GeneralExpression) myexp = Operand();
  Handle(Expr_GeneralExpression) myder = myexp->Derivative(X);
  Handle(Expr_Cosh) firstder = new Expr_Cosh(Expr::CopyShare(myexp));
  Handle(Expr_Product) resu = firstder->ShallowSimplified() * myder;
  return resu->ShallowSimplified();
}

Standard_Real Expr_Sinh::Evaluate(const Expr_Array1OfNamedUnknown& vars, const TColStd_Array1OfReal& vals) const 
{
  Standard_Real val = Operand()->Evaluate(vars,vals);
  return (::Exp(val)-::Exp(-val))/2.0;
}

TCollection_AsciiString Expr_Sinh::String() const
{
  TCollection_AsciiString str("Sinh(");
  str += Operand()->String();
  str += ")";
  return str;
}
