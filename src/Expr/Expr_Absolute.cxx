//static const char* sccsid = "@(#)Expr_Absolute.cxx	3.2 95/01/10"; // Do not delete this line. Used by sccs.
// Copyright: 	Matra-Datavision 1991
// File:	Expr_Absolute.cxx
// Created:	Mon May 27 10:13:50 1991
// Author:	Arnaud BOUZY
//		<adn>


#include <Expr_Absolute.ixx>
#include <Expr_NumericValue.hxx>
#include <Expr_UnaryMinus.hxx>
#include <Expr_Sign.hxx>
#include <Expr_Product.hxx>
#include <Expr_Operators.hxx>
#include <Expr.hxx>

Expr_Absolute::Expr_Absolute (const Handle(Expr_GeneralExpression)& exp)
{
  CreateOperand(exp);
}

Handle(Expr_GeneralExpression) Expr_Absolute::ShallowSimplified () const
{
  Handle(Expr_GeneralExpression) op = Operand();
  if (op->IsKind(STANDARD_TYPE(Expr_NumericValue))) {
    Handle(Expr_NumericValue) valop = Handle(Expr_NumericValue)::DownCast(op);
    return new Expr_NumericValue(Abs(valop->GetValue()));
  }
  if (op->IsKind(STANDARD_TYPE(Expr_UnaryMinus))) {
    return new Expr_Absolute(op->SubExpression(1));
  }
  Handle(Expr_Absolute) me = this;
  return me;
}

Handle(Expr_GeneralExpression) Expr_Absolute::Copy () const 
{
  return  new Expr_Absolute(Expr::CopyShare(Operand()));
}

Standard_Boolean Expr_Absolute::IsIdentical (const Handle(Expr_GeneralExpression)& Other) const
{
  if (!Other->IsKind(STANDARD_TYPE(Expr_Absolute))) {
    return Standard_False;
  }
  Handle(Expr_GeneralExpression) op = Operand();
  return op->IsIdentical(Other->SubExpression(1));
}

Standard_Boolean Expr_Absolute::IsLinear () const
{
  return !ContainsUnknowns();
}

Handle(Expr_GeneralExpression) Expr_Absolute::Derivative (const Handle(Expr_NamedUnknown)& X) const
{
  Handle(Expr_GeneralExpression) op = Operand();
  Handle(Expr_GeneralExpression) derop = op->Derivative(X);
  Handle(Expr_Sign) myder = new Expr_Sign(Expr::CopyShare(op));
  Handle(Expr_Product) resul = myder->ShallowSimplified() * derop;
  return resul->ShallowSimplified();
}


Standard_Real Expr_Absolute::Evaluate(const Expr_Array1OfNamedUnknown& vars, const TColStd_Array1OfReal& vals) const
{
  return ::Abs(Operand()->Evaluate(vars,vals));
}

TCollection_AsciiString Expr_Absolute::String() const
{
  TCollection_AsciiString str("Abs(");
  str += Operand()->String();
  str += ")";
  return str;
}
