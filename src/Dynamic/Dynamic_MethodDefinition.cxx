// Created on: 1994-08-25
// Created by: Gilles DEBARBOUILLE
// Copyright (c) 1994-1999 Matra Datavision
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



#include <Dynamic_MethodDefinition.ixx>
#include <TCollection_AsciiString.hxx>
#include <Dynamic_VariableNode.hxx>
#include <Dynamic_Variable.hxx>
#include <Dynamic_VariableGroup.hxx>
#include <Dynamic_Parameter.hxx>


//=======================================================================
//function : Dynamic_MethodDefinition
//purpose  : 
//=======================================================================

Dynamic_MethodDefinition::Dynamic_MethodDefinition(const Standard_CString aname)
{
  thename = new TCollection_HAsciiString(aname);
}

//=======================================================================
//function : Type
//purpose  : 
//=======================================================================

TCollection_AsciiString Dynamic_MethodDefinition::Type() const
{
  return thename->String();
}

//=======================================================================
//function : AddVariable
//purpose  : 
//=======================================================================

void Dynamic_MethodDefinition::AddVariable(const Handle(Dynamic_Parameter)& aparameter,
					   const Dynamic_ModeEnum amode,
					   const Standard_Boolean agroup)
{
  Handle(Dynamic_VariableNode) variablenode;
  Handle(Dynamic_Variable) variable;

  variablenode = FirstVariableNode();
  while(!variablenode.IsNull())
    {
      variable = variablenode->Object();
      if(variable->Parameter()->Name() == aparameter->Name())
	{
	  variable->Mode(amode);
	  variable->Parameter(aparameter);
	  return;
	}
      variablenode = variablenode->Next();
    }
  if(agroup) variable = new Dynamic_VariableGroup();
  else      variable = new Dynamic_Variable();
  variable->Mode(amode);
  variable->Parameter(aparameter);
  Dynamic_Method::Variable(variable);
}

//=======================================================================
//function : Dump
//purpose  : 
//=======================================================================

void Dynamic_MethodDefinition::Dump(Standard_OStream& astream) const
{
  TCollection_AsciiString string = thename->String();
  astream<<"Method Definition : "<<string<<endl;
  Dynamic_Method::Dump(astream);
}
