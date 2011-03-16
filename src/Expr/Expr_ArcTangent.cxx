//static const char* sccsid = "@(#)Expr_ArcTangent.cxx	3.2 95/01/10"; // Do not delete this line. Used by sccs.
// Copyright: 	Matra-Datavision 1991
// File:	Expr_ArcTangent.cxx
// Created:	Mon May 27 11:44:23 1991
// Author:	Arnaud BOUZY
//		<adn>


#include <Expr_ArcTangent.ixx>
#include <Expr_NumericValue.hxx>
#include <Expr_Tangent.hxx>
#include <Expr_Division.hxx>
#include <Expr_Square.hxx>
#include <Expr_Sum.hxx>
#include <Expr_Operators.hxx>
#include <Expr.hxx>

Expr_ArcTangent::Expr_ArcTangent (const Handle(Expr_GeneralExpression)& exp)
{
  CreateOperand(exp);
}

Handle(Expr_GeneralExpression) Expr_ArcTangent::ShallowSimplified () const
{
  Handle(Expr_GeneralExpression) op = Operand();
  if (op->IsKind(STANDARD_TYPE(Expr_NumericValue))) {
    Handle(Expr_NumericValue) valop = Handle(Expr_NumericValue)::DownCast(op);
    return new Expr_NumericValue(ATan(valop->GetValue()));
  }
  if (op->IsKind(STANDARD_TYPE(Expr_Tangent))) {
    return op->SubExpression(1);
  }
  Handle(Expr_ArcTangent) me = this;
  return me;
}

Handle(Expr_GeneralExpression) Expr_ArcTangent::Copy () const 
{
  return  new Expr_ArcTangent(Expr::CopyShare(Operand()));
}

Standard_Boolean Expr_ArcTangent::IsIdentical (const Handle(Expr_GeneralExpression)& Other) const
{
  if (!Other->IsKind(STANDARD_TYPE(Expr_ArcTangent))) {
    return Standard_False;
  }
  Handle(Expr_GeneralExpression) op = Operand();
  return op->IsIdentical(Other->SubExpression(1));
}

Standard_Boolean Expr_ArcTangent::IsLinear () const
{
  if (ContainsUnknowns()) {
    return Standard_False;
  }
  return Standard_True;
}

Handle(Expr_GeneralExpression) Expr_ArcTangent::Derivative (const Handle(Expr_NamedUnknown)& X) const
{
  if (!Contains(X)) {
    return new Expr_NumericValue(0.0);
  }
  Handle(Expr_GeneralExpression) op = Operand();
  Handle(Expr_GeneralExpression) derop = op->Derivative(X);

  Handle(Expr_Square) sq = new Expr_Square(Expr::CopyShare(op));
  // 1 + X2
  Handle(Expr_Sum) thesum = 1.0 + sq->ShallowSimplified();

  // ArcTangent'(F(X)) = F'(X)/(1+F(X)2) 
  Handle(Expr_Division) thediv = derop / thesum->ShallowSimplified(); 

  return thediv->ShallowSimplified();
}

Standard_Real Expr_ArcTangent::Evaluate(const Expr_Array1OfNamedUnknown& vars, const TColStd_Array1OfReal& vals) const
{
  return ::ATan(Operand()->Evaluate(vars,vals));
}

TCollection_AsciiString Expr_ArcTangent::String() const
{
  TCollection_AsciiString str("ATan(");
  str += Operand()->String();
  str += ")";
  return str;
}
