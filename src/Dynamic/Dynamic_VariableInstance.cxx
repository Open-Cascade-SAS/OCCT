// File:	Dynamic_VariableInstance.cxx
// Created:	Mon Sep  5 15:06:46 1994
// Author:	Gilles DEBARBOUILLE
//		<gde@watson>


#include <Dynamic_VariableInstance.ixx>

//=======================================================================
//function : Dynamic_VariableInstance
//purpose  : 
//=======================================================================

Dynamic_VariableInstance::Dynamic_VariableInstance() : Dynamic_AbstractVariableInstance()
{}

//=======================================================================
//function : Variable
//purpose  : 
//=======================================================================

void Dynamic_VariableInstance::Variable(const Handle(Dynamic_Variable)& avariable)
{
  thevariable = avariable;
}

//=======================================================================
//function : Variable
//purpose  : 
//=======================================================================

Handle(Dynamic_Variable) Dynamic_VariableInstance::Variable() const
{
  return thevariable;
}
