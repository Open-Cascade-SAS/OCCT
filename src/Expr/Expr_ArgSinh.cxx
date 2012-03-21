// Created on: 1991-05-27
// Created by: Arnaud BOUZY
// Copyright (c) 1991-1999 Matra Datavision
// Copyright (c) 1999-2012 OPEN CASCADE SAS
//
// The content of this file is subject to the Open CASCADE Technology Public
// License Version 6.5 (the "License"). You may not use the content of this file
// except in compliance with the License. Please obtain a copy of the License
// at http://www.opencascade.org and read it completely before using this file.
//
// The Initial Developer of the Original Code is Open CASCADE S.A.S., having its
// main offices at: 1, place des Freres Montgolfier, 78280 Guyancourt, France.
//
// The Original Code and all software distributed under the License is
// distributed on an "AS IS" basis, without warranty of any kind, and the
// Initial Developer hereby disclaims all such warranties, including without
// limitation, any warranties of merchantability, fitness for a particular
// purpose or non-infringement. Please see the License for the specific terms
// and conditions governing the rights and limitations under the License.


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
