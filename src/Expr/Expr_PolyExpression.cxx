//static const char* sccsid = "@(#)Expr_PolyExpression.cxx	3.2 95/01/10"; // Do not delete this line. Used by sccs.
// Copyright: 	Matra-Datavision 1991
// File:	Expr_PolyExpression.cxx
// Created:	Tue Apr 16 09:55:23 1991
// Author:	Arnaud BOUZY
//		<adn>

#include <Expr_PolyExpression.ixx>
#include <Expr_NamedUnknown.hxx>
#include <Expr_InvalidOperand.hxx>
#include <Standard_OutOfRange.hxx>
#include <Standard_DimensionMismatch.hxx>

Expr_PolyExpression::Expr_PolyExpression()
{
}

Standard_Integer Expr_PolyExpression::NbOperands () const
{
  return myOperands.Length();
}

void Expr_PolyExpression::SetOperand (const Handle(Expr_GeneralExpression)& exp, const Standard_Integer index)
{
  Handle(Expr_PolyExpression) me = this;
  if (exp == me) {
    Expr_InvalidOperand::Raise();
  }
  if (exp->Contains(me)) {
    Expr_InvalidOperand::Raise();
  }
  myOperands(index) = exp;
}

void Expr_PolyExpression::AddOperand (const Handle(Expr_GeneralExpression)& exp)
{
  myOperands.Append(exp);
}

void Expr_PolyExpression::RemoveOperand (const Standard_Integer index)
{
  if (myOperands.Length() <= 2) {
    Standard_DimensionMismatch::Raise();
  }
  myOperands.Remove(index);
}

Standard_Integer Expr_PolyExpression::NbSubExpressions () const
{
  return NbOperands();
}

const Handle(Expr_GeneralExpression)& Expr_PolyExpression::SubExpression (const Standard_Integer I) const
{
  return Operand(I);
}

Standard_Boolean Expr_PolyExpression::ContainsUnknowns () const 
{
  Standard_Boolean found = Standard_False;
  Standard_Integer nbop = NbOperands();
  Standard_Integer i = 1;
  Handle(Expr_GeneralExpression) expop;

  while ((!found) && (i <= nbop)) {
    expop = Operand(i);
    found = expop->IsKind(STANDARD_TYPE(Expr_NamedUnknown));
    i++;
  }
  i = 1;
  while ((!found) && (i <= nbop)) {
    expop = Operand(i);
    found = expop->ContainsUnknowns();
    i++;
  }
  return found;
}

Standard_Boolean Expr_PolyExpression::Contains (const Handle(Expr_GeneralExpression)& exp) const
{
  Standard_Boolean found = Standard_False;
  Standard_Integer nbop = NbOperands();
  Standard_Integer i = 1;
  Handle(Expr_GeneralExpression) expop;

  while ((!found) && (i <= nbop)) {
    expop = Operand(i);
    found = (expop == exp);
    i++;
  }
  i = 1;
  while ((!found) && (i <= nbop)) {
    expop = Operand(i);
    found = expop->Contains(exp);
    i++;
  }
  return found;
}

void Expr_PolyExpression::Replace (const Handle(Expr_NamedUnknown)& var, const Handle(Expr_GeneralExpression)& with)
{
  Standard_Integer nbop = NbOperands();
  Standard_Integer i;
  Handle(Expr_GeneralExpression) expop;

  for(i=1;i <= nbop; i++) {
    expop = Operand(i);
    if (expop == var) {
      SetOperand(with,i);
    }
    else {
      if (expop->Contains(var)) {
	expop->Replace(var,with);
      }
    }
  }
}


Handle(Expr_GeneralExpression) Expr_PolyExpression::Simplified() const
{
  Handle(Expr_PolyExpression) cop = Handle(Expr_PolyExpression)::DownCast(Copy());
  Standard_Integer i;
  Standard_Integer max = cop->NbOperands();
  Handle(Expr_GeneralExpression) op;
  for (i=1; i<= max; i++) {
    op = cop->Operand(i);
    cop->SetOperand(op->Simplified(),i);
  }
  return cop->ShallowSimplified();
}

