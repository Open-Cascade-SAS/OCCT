//static const char* sccsid = "@(#)ExprIntrp_Generator.cxx	3.2 95/01/10"; // Do not delete this line. Used by sccs.
// Copyright: 	Matra-Datavision 1992
// File:	ExprIntrp_Generator.cxx
// Created:	Mon Jul 20 17:06:29 1992
// Author:	Arnaud BOUZY
//		<adn>

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

