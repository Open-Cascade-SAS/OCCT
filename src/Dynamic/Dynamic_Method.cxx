// Created on: 1993-01-22
// Created by: Gilles DEBARBOUILLE
// Copyright (c) 1993-1999 Matra Datavision
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



#include <Dynamic_Method.ixx>
#include <Dynamic_Variable.hxx>
#include <Dynamic_Parameter.hxx>


//=======================================================================
//function : Dynamic_Method
//purpose  : 
//=======================================================================

Dynamic_Method::Dynamic_Method()
{}


//=======================================================================
//function : FirstVariableNode
//purpose  : 
//=======================================================================

Handle(Dynamic_VariableNode) Dynamic_Method::FirstVariableNode() const
{
  return thefirstvariablenode;
}

//=======================================================================
//function : Variable
//purpose  : 
//=======================================================================

Standard_Boolean Dynamic_Method::Variable(const Standard_CString avariable) const
{
  Handle(Dynamic_Variable) variable;
  Handle(Dynamic_VariableNode) definition = thefirstvariablenode;

  while(!definition.IsNull())
    {
      variable = definition->Object();
      if(variable->Parameter()->Name() == avariable) return Standard_True;
      definition = definition->Next();
    }
  return Standard_False;
}

//=======================================================================
//function : Variable
//purpose  : 
//=======================================================================

void Dynamic_Method::Variable(const Handle(Dynamic_Variable)& avariable)
{
  Handle(Dynamic_VariableNode) variablenode = new Dynamic_VariableNode(avariable);
  if(!thefirstvariablenode.IsNull()) variablenode->Next(thefirstvariablenode);
  thefirstvariablenode = variablenode;
}

//=======================================================================
//function : Value
//purpose  : 
//=======================================================================

Standard_Boolean Dynamic_Method::Value(const Standard_CString aname,
				       Handle(Dynamic_Parameter)& aparameter,
				       Dynamic_ModeEnum& amode) const
{
  Handle(Dynamic_Variable) variable;

  if(Value(aname,variable))
    {
      amode = variable->Mode();
      aparameter = variable->Parameter();
      return Standard_True;
    }
  else
    {
      return Standard_False;
    }
}

//=======================================================================
//function : Value
//purpose  : 
//=======================================================================

Standard_Boolean Dynamic_Method::Value(const Standard_CString aname,
				       Handle(Dynamic_Variable)& avariable) const
{
  Handle(Dynamic_VariableNode) variablenode;
  Handle(Dynamic_Variable) variable;

  variablenode = thefirstvariablenode;

  while(!variablenode.IsNull())
    {
      variable = variablenode->Object();
      if(variable->Parameter()->Name() == aname)
	{
	  avariable = variable;
	  return Standard_True;
	}
      variablenode = variablenode->Next();
    }
  
  return Standard_False;
}

//=======================================================================
//function : Dump
//purpose  : 
//=======================================================================

void Dynamic_Method::Dump(Standard_OStream& astream) const
{
  Handle(Dynamic_Variable) variable;
  Handle(Dynamic_VariableNode) definition = thefirstvariablenode;

  while(!definition.IsNull())
    {
      variable = definition->Object();
      variable->Dump(astream);
      astream<<" ; "<<endl;
      definition = definition->Next();
    }
  astream<<endl;
}
