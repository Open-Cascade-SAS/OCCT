//static const char* sccsid = "@(#)Expr_LogOfe.cxx	3.2 95/01/10"; // Do not delete this line. Used by sccs.
// Copyright: 	Matra-Datavision 1991
// File:	Expr_LogOfe.cxx
// Created:	Tue May 28 12:33:48 1991
// Author:	Arnaud BOUZY
//		<adn>

#include <Expr_LogOfe.ixx>
#include <Expr_NumericValue.hxx>
#include <Expr_Division.hxx>
#include <Expr_Exponential.hxx>
#include <Expr_Operators.hxx>
#include <Expr.hxx>

Expr_LogOfe::Expr_LogOfe(const Handle(Expr_GeneralExpression)& exp)
{
  CreateOperand(exp);
}

Handle(Expr_GeneralExpression) Expr_LogOfe::ShallowSimplified () const
{
  Handle(Expr_GeneralExpression) myexp = Operand();
  if (myexp->IsKind(STANDARD_TYPE(Expr_NumericValue))) {
    Handle(Expr_NumericValue) myNVexp = Handle(Expr_NumericValue)::DownCast(myexp);
    return new Expr_NumericValue(Log(myNVexp->GetValue()));
  }
  if (myexp->IsKind(STANDARD_TYPE(Expr_Exponential))) {
    return myexp->SubExpression(1);
  }
  Handle(Expr_LogOfe) me = this;
  return me;
}

Handle(Expr_GeneralExpression) Expr_LogOfe::Copy () const
{
  return new Expr_LogOfe(Expr::CopyShare(Operand()));
}

Standard_Boolean Expr_LogOfe::IsIdentical (const Handle(Expr_GeneralExpression)& Other) const
{
  if (Other->IsKind(STANDARD_TYPE(Expr_LogOfe))) {
    Handle(Expr_GeneralExpression) myexp = Operand();
    return myexp->IsIdentical(Other->SubExpression(1));
  }
  return Standard_False;
}

Standard_Boolean Expr_LogOfe::IsLinear () const
{
  return !ContainsUnknowns();
}

Handle(Expr_GeneralExpression) Expr_LogOfe::Derivative (const Handle(Expr_NamedUnknown)& X) const
{
  if (!Contains(X)) {
    return new Expr_NumericValue(0.0);
  }
  Handle(Expr_GeneralExpression) myexp = Operand();
  Handle(Expr_GeneralExpression) myder = myexp->Derivative(X);
  Handle(Expr_Division) thediv = myder / Expr::CopyShare(myexp);
  return thediv->ShallowSimplified();
}

Standard_Real Expr_LogOfe::Evaluate(const Expr_Array1OfNamedUnknown& vars, const TColStd_Array1OfReal& vals) const
{
  return ::Log(Operand()->Evaluate(vars,vals));
}

TCollection_AsciiString Expr_LogOfe::String() const
{
  TCollection_AsciiString str("Ln(");
  str += Operand()->String();
  str += ")";
  return str;
}
