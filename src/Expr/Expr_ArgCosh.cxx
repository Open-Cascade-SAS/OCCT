//static const char* sccsid = "@(#)Expr_ArgCosh.cxx	3.2 95/01/10"; // Do not delete this line. Used by sccs.
// Copyright: 	Matra-Datavision 1991
// File:	Expr_ArgCosh.cxx
// Created:	Mon May 27 11:59:06 1991
// Author:	Arnaud BOUZY
//		<adn>

#include <Expr_ArgCosh.ixx>
#include <Expr_NumericValue.hxx>
#include <Expr_Cosh.hxx>
#include <Expr_Division.hxx>
#include <Expr_Square.hxx>
#include <Expr_Difference.hxx>
#include <Expr_SquareRoot.hxx>
#include <Expr_Operators.hxx>
#include <Expr.hxx>

Expr_ArgCosh::Expr_ArgCosh (const Handle(Expr_GeneralExpression)& exp)
{
  CreateOperand(exp);
}

Handle(Expr_GeneralExpression) Expr_ArgCosh::ShallowSimplified () const
{
  Handle(Expr_GeneralExpression) op = Operand();
  if (op->IsKind(STANDARD_TYPE(Expr_NumericValue))) {
    Handle(Expr_NumericValue) valop = Handle(Expr_NumericValue)::DownCast(op);
    return new Expr_NumericValue(ACosh(valop->GetValue()));
  }
  if (op->IsKind(STANDARD_TYPE(Expr_Cosh))) {
    return op->SubExpression(1);
  }
  Handle(Expr_ArgCosh) me = this;
  return me;
}

Handle(Expr_GeneralExpression) Expr_ArgCosh::Copy () const 
{
  return  new Expr_ArgCosh(Expr::CopyShare(Operand()));
}

Standard_Boolean Expr_ArgCosh::IsIdentical (const Handle(Expr_GeneralExpression)& Other) const
{
  if (!Other->IsKind(STANDARD_TYPE(Expr_ArgCosh))) {
    return Standard_False;
  }
  Handle(Expr_GeneralExpression) op = Operand();
  return op->IsIdentical(Other->SubExpression(1));
}

Standard_Boolean Expr_ArgCosh::IsLinear () const
{
  if (ContainsUnknowns()) {
    return Standard_False;
  }
  return Standard_True;
}

Handle(Expr_GeneralExpression) Expr_ArgCosh::Derivative (const Handle(Expr_NamedUnknown)& X) const
{
  if (!Contains(X)) {
    return new Expr_NumericValue(0.0);
  }
  Handle(Expr_GeneralExpression) op = Operand();
  Handle(Expr_GeneralExpression) derop = op->Derivative(X);

  Handle(Expr_Square) sq = new Expr_Square(Expr::CopyShare(op));
  // X2 - 1
  Handle(Expr_Difference) thedif = sq->ShallowSimplified() - 1.0; 

  // sqrt(X2 - 1)
  Handle(Expr_SquareRoot) theroot = new Expr_SquareRoot(thedif->ShallowSimplified());

  // ArgCosh'(F(X)) = F'(X)/sqrt(F(X)2-1) 
  Handle(Expr_Division) thediv = derop / theroot->ShallowSimplified(); 

  return thediv->ShallowSimplified();
}

Standard_Real Expr_ArgCosh::Evaluate(const Expr_Array1OfNamedUnknown& vars, const TColStd_Array1OfReal& vals) const
{
  Standard_Real val = Operand()->Evaluate(vars,vals);
  return ::Log(val + ::Sqrt(::Square(val)-1.0));
}

TCollection_AsciiString Expr_ArgCosh::String() const
{
  TCollection_AsciiString str("ACosh(");
  str += Operand()->String();
  str += ")";
  return str;
}
