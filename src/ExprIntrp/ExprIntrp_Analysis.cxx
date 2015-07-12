// Created on: 1992-02-25
// Created by: Arnaud BOUZY
// Copyright (c) 1992-1999 Matra Datavision
// Copyright (c) 1999-2014 OPEN CASCADE SAS
//
// This file is part of Open CASCADE Technology software library.
//
// This library is free software; you can redistribute it and/or modify it under
// the terms of the GNU Lesser General Public License version 2.1 as published
// by the Free Software Foundation, with special exception defined in the file
// OCCT_LGPL_EXCEPTION.txt. Consult the file LICENSE_LGPL_21.txt included in OCCT
// distribution for complete text of the license and disclaimer of any warranty.
//
// Alternatively, this file may be used under the terms of Open CASCADE
// commercial license or contractual agreement.

#define _ExprIntrp_Analysis_SourceFile


#include <Expr_GeneralExpression.hxx>
#include <Expr_GeneralFunction.hxx>
#include <Expr_GeneralRelation.hxx>
#include <Expr_NamedExpression.hxx>
#include <Expr_NamedFunction.hxx>
#include <Expr_NamedUnknown.hxx>
#include <ExprIntrp_Analysis.hxx>
#include <ExprIntrp_Generator.hxx>
#include <TCollection_AsciiString.hxx>

ExprIntrp_Analysis::ExprIntrp_Analysis() {}


void ExprIntrp_Analysis::Push(const Handle(Expr_GeneralExpression)& exp) 
{
  myGEStack.Prepend(exp);
}

void ExprIntrp_Analysis::PushRelation(const Handle(Expr_GeneralRelation)& rel) 
{
  myGRStack.Prepend(rel);
}

void ExprIntrp_Analysis::PushFunction(const Handle(Expr_GeneralFunction)& func)
{
  myGFStack.Prepend(func);
}

void ExprIntrp_Analysis::PushName(const TCollection_AsciiString& name) 
{
  myNameStack.Prepend(name);
}

void ExprIntrp_Analysis::PushValue(const Standard_Integer val) 
{
  myValueStack.Prepend(val);
}

Handle(Expr_GeneralExpression) ExprIntrp_Analysis::Pop()
{
  Handle(Expr_GeneralExpression) res;
  if (!myGEStack.IsEmpty()) {
    res = myGEStack.First();
    myGEStack.RemoveFirst();
  }
  return res;
}

Handle(Expr_GeneralRelation) ExprIntrp_Analysis::PopRelation()
{
  Handle(Expr_GeneralRelation) res;
  if (!myGRStack.IsEmpty()) {
    res = myGRStack.First();
    myGRStack.RemoveFirst();
  }
  return res;
}

Handle(Expr_GeneralFunction) ExprIntrp_Analysis::PopFunction()
{
  Handle(Expr_GeneralFunction) res;
  if (!myGFStack.IsEmpty()) {
    res = myGFStack.First();
    myGFStack.RemoveFirst();
  }
  return res;
}

TCollection_AsciiString ExprIntrp_Analysis::PopName()
{
  TCollection_AsciiString res;
  if (!myNameStack.IsEmpty()) {
    res = myNameStack.First();
    myNameStack.RemoveFirst();
  }
  return res;
}

Standard_Integer ExprIntrp_Analysis::PopValue()
{
  Standard_Integer res =0;
  if (!myValueStack.IsEmpty()) {
    res = myValueStack.First();
    myValueStack.RemoveFirst();
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
