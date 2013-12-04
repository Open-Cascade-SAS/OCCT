// Created on: 1994-08-25
// Created by: Gilles DEBARBOUILLE
// Copyright (c) 1994-1999 Matra Datavision
// Copyright (c) 1999-2014 OPEN CASCADE SAS
//
// This file is part of Open CASCADE Technology software library.
//
// This library is free software; you can redistribute it and / or modify it
// under the terms of the GNU Lesser General Public version 2.1 as published
// by the Free Software Foundation, with special exception defined in the file
// OCCT_LGPL_EXCEPTION.txt. Consult the file LICENSE_LGPL_21.txt included in OCCT
// distribution for complete text of the license and disclaimer of any warranty.
//
// Alternatively, this file may be used under the terms of Open CASCADE
// commercial license or contractual agreement.

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
