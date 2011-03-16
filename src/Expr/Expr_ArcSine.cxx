//static const char* sccsid = "@(#)Expr_ArcSine.cxx	3.2 95/01/10"; // Do not delete this line. Used by sccs.
// Copyright: 	Matra-Datavision 1991
// File:	Expr_ArcSine.cxx
// Created:	Mon May 27 11:31:25 1991
// Author:	Arnaud BOUZY
//		<adn>

#include <Expr_ArcSine.ixx>
#include <Expr_NumericValue.hxx>
#include <Expr_Sine.hxx>
#include <Expr_Division.hxx>
#include <Expr_Square.hxx>
#include <Expr_Difference.hxx>
#include <Expr_SquareRoot.hxx>
#include <Expr_Operators.hxx>
#include <Expr.hxx>

Expr_ArcSine::Expr_ArcSine (const Handle(Expr_GeneralExpression)& exp)
{
  CreateOperand(exp);
}

Handle(Expr_GeneralExpression) Expr_ArcSine::ShallowSimplified () const
{
  Handle(Expr_GeneralExpression) op = Operand();
  if (op->IsKind(STANDARD_TYPE(Expr_NumericValue))) {
    Handle(Expr_NumericValue) valop = Handle(Expr_NumericValue)::DownCast(op);
    return new Expr_NumericValue(ASin(valop->GetValue()));
  }
  if (op->IsKind(STANDARD_TYPE(Expr_Sine))) {
    return op->SubExpression(1);
  }
  Handle(Expr_ArcSine) me = this;
  return me;
}

Handle(Expr_GeneralExpression) Expr_ArcSine::Copy () const 
{
  return  new Expr_ArcSine(Expr::CopyShare(Operand()));
}

Standard_Boolean Expr_ArcSine::IsIdentical (const Handle(Expr_GeneralExpression)& Other) const
{
  if (!Other->IsKind(STANDARD_TYPE(Expr_ArcSine))) {
    return Standard_False;
  }
  Handle(Expr_GeneralExpression) op = Operand();
  return op->IsIdentical(Other->SubExpression(1));
}

Standard_Boolean Expr_ArcSine::IsLinear () const
{
  if (ContainsUnknowns()) {
    return Standard_False;
  }
  return Standard_True;
}

Handle(Expr_GeneralExpression) Expr_ArcSine::Derivative (const Handle(Expr_NamedUnknown)& X) const
{
  if (!Contains(X)) {
    return new Expr_NumericValue(0.0);
  }
  Handle(Expr_GeneralExpression) op = Operand();
  Handle(Expr_GeneralExpression) derop = op->Derivative(X);

  Handle(Expr_Square) sq = new Expr_Square(Expr::CopyShare(op));
  // 1 - X2
  Handle(Expr_Difference) thedif = 1.0 - sq->ShallowSimplified(); 

  // sqrt(1-X2)
  Handle(Expr_SquareRoot) theroot = new Expr_SquareRoot(thedif->ShallowSimplified());

  // ArcSine'(F(X)) = F'(X)/sqrt(1-F(X)2) 
  Handle(Expr_Division) thediv = derop / theroot->ShallowSimplified(); 

  return thediv->ShallowSimplified();
}

Standard_Real Expr_ArcSine::Evaluate(const Expr_Array1OfNamedUnknown& vars, const TColStd_Array1OfReal& vals) const
{
  return ::ASin(Operand()->Evaluate(vars,vals));
}

TCollection_AsciiString Expr_ArcSine::String() const
{
  TCollection_AsciiString str("ASin(");
  str += Operand()->String();
  str += ")";
  return str;
}
