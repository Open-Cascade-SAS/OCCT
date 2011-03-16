// File:	Dynamic_CompositVariableInstance.cxx
// Created:	Mon Sep  5 15:06:46 1994
// Author:	Gilles DEBARBOUILLE
//		<gde@watson>


#include <Dynamic_CompositVariableInstance.ixx>

//=======================================================================
//function : Dynamic_CompositVariableInstance
//purpose  : 
//=======================================================================

Dynamic_CompositVariableInstance::Dynamic_CompositVariableInstance()
{}

//=======================================================================
//function : Variable
//purpose  : 
//=======================================================================

void Dynamic_CompositVariableInstance::Variable(const Handle(Dynamic_Variable)& avariable)
{
  Handle(Dynamic_VariableNode) variablenode = new Dynamic_VariableNode(avariable);
  variablenode->Next(thefirstvariablenode);
  thefirstvariablenode = variablenode;
}

//=======================================================================
//function : FirstVariable
//purpose  : 
//=======================================================================

Handle(Dynamic_VariableNode) Dynamic_CompositVariableInstance::FirstVariableNode() const
{
  return thefirstvariablenode;
}
