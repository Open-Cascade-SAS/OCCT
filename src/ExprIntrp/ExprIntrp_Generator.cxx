// Created on: 1992-07-20
// Created by: Arnaud BOUZY
// Copyright (c) 1992-1999 Matra Datavision
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


#include <ExprIntrp_Generator.ixx>
//#include <ExprIntrp_yaccanal.hxx>
#include <ExprIntrp_Analysis.hxx>
#include <TCollection_AsciiString.hxx>

Standard_EXPORT ExprIntrp_Analysis ExprIntrp_Recept;


ExprIntrp_Generator::ExprIntrp_Generator()
{
}

void ExprIntrp_Generator::Use(const Handle(Expr_NamedFunction)& func)
{
  myFunctions.Append(func);
}

void ExprIntrp_Generator::Use(const Handle(Expr_NamedExpression)& named)
{
  myNamed.Append(named);
}

const ExprIntrp_SequenceOfNamedFunction& ExprIntrp_Generator::GetFunctions() const
{
  return myFunctions;
}

const ExprIntrp_SequenceOfNamedExpression & ExprIntrp_Generator::GetNamed() const
{
  return myNamed;
}

Handle(Expr_NamedFunction) ExprIntrp_Generator::GetFunction (const TCollection_AsciiString& name) const
{
  for (Standard_Integer i=1; i<= myFunctions.Length(); i++) {
    if (name == myFunctions(i)->GetName()) {
      return myFunctions(i);
    }
  }
  Handle(Expr_NamedFunction) curfunc;
  return curfunc;
}

Handle(Expr_NamedExpression) ExprIntrp_Generator::GetNamed (const TCollection_AsciiString& name) const
{
  for (Standard_Integer i=1;i<=myNamed.Length();i++){
    if (name == myNamed(i)->GetName()) {
      return myNamed(i);
    }
  }
  Handle(Expr_NamedExpression) curexp;
  return curexp;
}

