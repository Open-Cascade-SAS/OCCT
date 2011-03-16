//static const char* sccsid = "@(#)Expr_ArcCosine.cxx	3.2 95/01/10"; // Do not delete this line. Used by sccs.
// Copyright: 	Matra-Datavision 1991
// File:	Expr_ArcCosine.cxx
// Created:	Mon May 27 10:39:35 1991
// Author:	Arnaud BOUZY
//		<adn>


#include <Expr_ArcCosine.ixx>
#include <Expr_NumericValue.hxx>
#include <Expr_Cosine.hxx>
#include <Expr_Division.hxx>
#include <Expr_Square.hxx>
#include <Expr_Difference.hxx>
#include <Expr_SquareRoot.hxx>
#include <Expr_UnaryMinus.hxx>
#include <Expr_Product.hxx>
#include <Expr_Operators.hxx>
#include <Expr.hxx>

Expr_ArcCosine::Expr_ArcCosine (const Handle(Expr_GeneralExpression)& exp)
{
  CreateOperand(exp);
}

Handle(Expr_GeneralExpression) Expr_ArcCosine::ShallowSimplified () const
{
  Handle(Expr_GeneralExpression) op = Operand();
  if (op->IsKind(STANDARD_TYPE(Expr_NumericValue))) {
    Handle(Expr_NumericValue) valop = Handle(Expr_NumericValue)::DownCast(op);
    return new Expr_NumericValue(ACos(valop->GetValue()));
  }
  if (op->IsKind(STANDARD_TYPE(Expr_Cosine))) {
    return op->SubExpression(1);
  }
  Handle(Expr_ArcCosine) me = this;
  return me;
}

Handle(Expr_GeneralExpression) Expr_ArcCosine::Copy () const 
{
  return  new Expr_ArcCosine(Expr::CopyShare(Operand()));
}

Standard_Boolean Expr_ArcCosine::IsIdentical (const Handle(Expr_GeneralExpression)& Other) const
{
  if (!Other->IsKind(STANDARD_TYPE(Expr_ArcCosine))) {
    return Standard_False;
  }
  Handle(Expr_GeneralExpression) op = Operand();
  return op->IsIdentical(Other->SubExpression(1));
}

Standard_Boolean Expr_ArcCosine::IsLinear () const
{
  if (ContainsUnknowns()) {
    return Standard_False;
  }
  return Standard_True;
}

Handle(Expr_GeneralExpression) Expr_ArcCosine::Derivative (const Handle(Expr_NamedUnknown)& X) const
{
  if (!Contains(X)) {
    return new Expr_NumericValue(0.0);
  }
  Handle(Expr_GeneralExpression) op = Operand();
  Handle(Expr_GeneralExpression) derop = op->Derivative(X);

  Handle(Expr_Square) sq = new Expr_Square(Expr::CopyShare(op));
  // 1 - X2
  Handle(Expr_Difference) thedif = 1.0 - sq->ShallowSimplified(); 

  Handle(Expr_SquareRoot) theroot = new Expr_SquareRoot(thedif->ShallowSimplified());
  // 1/ sqrt(1-X2)
  Handle(Expr_UnaryMinus) theder = - (1.0 / theroot->ShallowSimplified());

  // ArcCosine'(F(X)) = -1/sqrt(1-F(X)2) * F'(X)

  Handle(Expr_Product) result = theder->ShallowSimplified() * derop;

  return result->ShallowSimplified();
}

Standard_Real Expr_ArcCosine::Evaluate(const Expr_Array1OfNamedUnknown& vars, const TColStd_Array1OfReal& vals) const
{
  return ::ACos(Operand()->Evaluate(vars,vals));
}

TCollection_AsciiString Expr_ArcCosine::String() const
{
  TCollection_AsciiString str("ACos(");
  str += Operand()->String();
  str += ")";
  return str;
}
