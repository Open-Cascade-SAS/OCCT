//static const char* sccsid = "@(#)Expr_ArgSinh.cxx	3.2 95/01/10"; // Do not delete this line. Used by sccs.
// Copyright: 	Matra-Datavision 1991
// File:	Expr_ArgSinh.cxx
// Created:	Mon May 27 12:10:02 1991
// Author:	Arnaud BOUZY
//		<adn>

#include <Expr_ArgSinh.ixx>
#include <Expr_NumericValue.hxx>
#include <Expr_Sinh.hxx>
#include <Expr_Division.hxx>
#include <Expr_Square.hxx>
#include <Expr_Sum.hxx>
#include <Expr_SquareRoot.hxx>
#include <Expr_Operators.hxx>
#include <Expr.hxx>

Expr_ArgSinh::Expr_ArgSinh (const Handle(Expr_GeneralExpression)& exp)
{
  CreateOperand(exp);
}

Handle(Expr_GeneralExpression) Expr_ArgSinh::ShallowSimplified () const
{
  Handle(Expr_GeneralExpression) op = Operand();
  if (op->IsKind(STANDARD_TYPE(Expr_NumericValue))) {
    Handle(Expr_NumericValue) valop = Handle(Expr_NumericValue)::DownCast(op);
    return new Expr_NumericValue(ASinh(valop->GetValue()));
  }
  if (op->IsKind(STANDARD_TYPE(Expr_Sinh))) {
    return op->SubExpression(1);
  }
  Handle(Expr_ArgSinh) me = this;
  return me;
}

Handle(Expr_GeneralExpression) Expr_ArgSinh::Copy () const 
{
  return  new Expr_ArgSinh(Expr::CopyShare(Operand()));
}

Standard_Boolean Expr_ArgSinh::IsIdentical (const Handle(Expr_GeneralExpression)& Other) const
{
  if (!Other->IsKind(STANDARD_TYPE(Expr_ArgSinh))) {
    return Standard_False;
  }
  Handle(Expr_GeneralExpression) op = Operand();
  return op->IsIdentical(Other->SubExpression(1));
}

Standard_Boolean Expr_ArgSinh::IsLinear () const
{
  if (ContainsUnknowns()) {
    return Standard_False;
  }
  return Standard_True;
}

Handle(Expr_GeneralExpression) Expr_ArgSinh::Derivative (const Handle(Expr_NamedUnknown)& X) const
{
  if (!Contains(X)) {
    return new Expr_NumericValue(0.0);
  }
  Handle(Expr_GeneralExpression) op = Operand();
  Handle(Expr_GeneralExpression) derop = op->Derivative(X);

  Handle(Expr_Square) sq = new Expr_Square(Expr::CopyShare(op));
  // X2 + 1
  Handle(Expr_Sum) thesum = sq->ShallowSimplified() + 1.0; 

  // sqrt(X2 + 1)
  Handle(Expr_SquareRoot) theroot = new Expr_SquareRoot(thesum->ShallowSimplified());

  // ArgSinh'(F(X)) = F'(X)/sqrt(F(X)2+1) 
  Handle(Expr_Division) thediv = derop / theroot->ShallowSimplified(); 

  return thediv->ShallowSimplified();
}

Standard_Real Expr_ArgSinh::Evaluate(const Expr_Array1OfNamedUnknown& vars, const TColStd_Array1OfReal& vals) const
{
  Standard_Real val = Operand()->Evaluate(vars,vals);
  return ::Log(val + ::Sqrt(::Square(val)+1.0));
}

TCollection_AsciiString Expr_ArgSinh::String() const
{
  TCollection_AsciiString str("ASinh(");
  str += Operand()->String();
  str += ")";
  return str;
}
