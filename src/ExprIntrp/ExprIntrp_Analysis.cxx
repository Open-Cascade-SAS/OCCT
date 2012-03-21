// Created on: 1992-02-25
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


#define _ExprIntrp_Analysis_SourceFile

#include <ExprIntrp_Analysis.ixx>
#include <Expr_NamedUnknown.hxx>

ExprIntrp_Analysis::ExprIntrp_Analysis() {}


void ExprIntrp_Analysis::Push(const Handle(Expr_GeneralExpression)& exp) 
{
  myGEStack.Push(exp);
}

void ExprIntrp_Analysis::PushRelation(const Handle(Expr_GeneralRelation)& rel) 
{
  myGRStack.Push(rel);
}

void ExprIntrp_Analysis::PushFunction(const Handle(Expr_GeneralFunction)& func)
{
  myGFStack.Push(func);
}

void ExprIntrp_Analysis::PushName(const TCollection_AsciiString& name) 
{
  myNameStack.Push(name);
}

void ExprIntrp_Analysis::PushValue(const Standard_Integer val) 
{
  myValueStack.Push(val);
}

Handle(Expr_GeneralExpression) ExprIntrp_Analysis::Pop()
{
  Handle(Expr_GeneralExpression) res;
  if (!myGEStack.IsEmpty()) {
    res = myGEStack.Top();
    myGEStack.Pop();
  }
  return res;
}

Handle(Expr_GeneralRelation) ExprIntrp_Analysis::PopRelation()
{
  Handle(Expr_GeneralRelation) res;
  if (!myGRStack.IsEmpty()) {
    res = myGRStack.Top();
    myGRStack.Pop();
  }
  return res;
}

Handle(Expr_GeneralFunction) ExprIntrp_Analysis::PopFunction()
{
  Handle(Expr_GeneralFunction) res;
  if (!myGFStack.IsEmpty()) {
    res = myGFStack.Top();
    myGFStack.Pop();
  }
  return res;
}

TCollection_AsciiString ExprIntrp_Analysis::PopName()
{
  TCollection_AsciiString res;
  if (!myNameStack.IsEmpty()) {
    res = myNameStack.Top();
    myNameStack.Pop();
  }
  return res;
}

Standard_Integer ExprIntrp_Analysis::PopValue()
{
  Standard_Integer res =0;
  if (!myValueStack.IsEmpty()) {
    res = myValueStack.Top();
    myValueStack.Pop();
  }
  return res;
}

Standard_Boolean ExprIntrp_Analysis::IsExpStackEmpty() const
{
  return myGEStack.IsEmpty();
}

Standard_Boolean ExprIntrp_Analysis::IsRelStackEmpty() const
{
  return myGRStack.IsEmpty();
}

void ExprIntrp_Analysis::ResetAll()
{
  myGEStack.Clear();
  myGRStack.Clear();
  myGFStack.Clear();
  myNameStack.Clear();
  myValueStack.Clear();
  myFunctions.Clear();
  myNamed.Clear();
}

void ExprIntrp_Analysis::SetMaster(const Handle(ExprIntrp_Generator)& agen)
{
  ResetAll();
  myMaster = agen;
  myFunctions = myMaster->GetFunctions();
  myNamed = myMaster->GetNamed();
}

void ExprIntrp_Analysis::Use(const Handle(Expr_NamedFunction)& func)
{
  myFunctions.Append(func);
  myMaster->Use(func);
}

void ExprIntrp_Analysis::Use(const Handle(Expr_NamedExpression)& named)
{
  myNamed.Append(named);
  myMaster->Use(named);
}

Handle(Expr_NamedExpression) ExprIntrp_Analysis::GetNamed(const TCollection_AsciiString& name)
{
  for (Standard_Integer i=1; i<= myNamed.Length();i++) {
    if (name == myNamed(i)->GetName()) {
      return myNamed(i);
    }
  }
  Handle(Expr_NamedExpression) curnamed;
  return curnamed;
}

Handle(Expr_NamedFunction) ExprIntrp_Analysis::GetFunction(const TCollection_AsciiString& name)
{
  for (Standard_Integer i=1; i<= myFunctions.Length();i++) {
    if (name == myFunctions(i)->GetName()) {
      return myFunctions(i);
    }
  }
  Handle(Expr_NamedFunction) curfunc;
  return curfunc;
}
