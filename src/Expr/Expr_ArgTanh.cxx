//static const char* sccsid = "@(#)Expr_ArgTanh.cxx	3.2 95/01/10"; // Do not delete this line. Used by sccs.
// Copyright: 	Matra-Datavision 1991
// File:	Expr_ArgTanh.cxx
// Created:	Mon May 27 14:02:51 1991
// Author:	Arnaud BOUZY
//		<adn>

#include <Expr_ArgTanh.ixx>
#include <Expr_NumericValue.hxx>
#include <Expr_Tanh.hxx>
#include <Expr_Division.hxx>
#include <Expr_Square.hxx>
#include <Expr_Difference.hxx>
#include <Expr_Operators.hxx>
#include <Expr.hxx>

Expr_ArgTanh::Expr_ArgTanh (const Handle(Expr_GeneralExpression)& exp)
{
  CreateOperand(exp);
}

Handle(Expr_GeneralExpression) Expr_ArgTanh::ShallowSimplified () const
{
  Handle(Expr_GeneralExpression) op = Operand();
  if (op->IsKind(STANDARD_TYPE(Expr_NumericValue))) {
    Handle(Expr_NumericValue) valop = Handle(Expr_NumericValue)::DownCast(op);
    return new Expr_NumericValue(ATanh(valop->GetValue()));
  }
  if (op->IsKind(STANDARD_TYPE(Expr_Tanh))) {
    return op->SubExpression(1);
  }
  Handle(Expr_ArgTanh) me = this;
  return me;
}

Handle(Expr_GeneralExpression) Expr_ArgTanh::Copy () const 
{
  return  new Expr_ArgTanh(Expr::CopyShare(Operand()));
}

Standard_Boolean Expr_ArgTanh::IsIdentical (const Handle(Expr_GeneralExpression)& Other) const
{
  if (!Other->IsKind(STANDARD_TYPE(Expr_ArgTanh))) {
    return Standard_False;
  }
  Handle(Expr_GeneralExpression) op = Operand();
  return op->IsIdentical(Other->SubExpression(1));
}

Standard_Boolean Expr_ArgTanh::IsLinear () const
{
  if (ContainsUnknowns()) {
    return Standard_False;
  }
  return Standard_True;
}

Handle(Expr_GeneralExpression) Expr_ArgTanh::Derivative (const Handle(Expr_NamedUnknown)& X) const
{
  if (!Contains(X)) {
    return new Expr_NumericValue(0.0);
  }
  Handle(Expr_GeneralExpression) op = Operand();
  Handle(Expr_GeneralExpression) derop = op->Derivative(X);

  Handle(Expr_Square) sq = new Expr_Square(Expr::CopyShare(op));

  // 1 - X2

  Handle(Expr_Difference) thedif = 1.0 - sq->ShallowSimplified(); 

  // ArgTanh'(F(X)) = F'(X)/(1 - F(X)2) 
  Handle(Expr_Division) thediv = derop / thedif->ShallowSimplified(); 

  return thediv->ShallowSimplified();
}

Standard_Real Expr_ArgTanh::Evaluate(const Expr_Array1OfNamedUnknown& vars, const TColStd_Array1OfReal& vals) const
{
  Standard_Real val = Operand()->Evaluate(vars,vals);
  return ::Log((1.0+val)/(1.0-val))/2.0;
}

TCollection_AsciiString Expr_ArgTanh::String() const
{
  TCollection_AsciiString str("ATanh(");
  str += Operand()->String();
  str += ")";
  return str;
}
