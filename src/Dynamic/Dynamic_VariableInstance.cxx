// Created on: 1994-09-05
// Created by: Gilles DEBARBOUILLE
// Copyright (c) 1994-1999 Matra Datavision
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
