// Created on: 2001-07-09
// Created by: Julia DOROVSKIKH
// Copyright (c) 2001-2014 OPEN CASCADE SAS
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


#include <CDM_MessageDriver.hxx>
#include <XmlMDF_ADriverTable.hxx>
#include <XmlMFunction.hxx>
#include <XmlMFunction_FunctionDriver.hxx>
#include <XmlMFunction_GraphNodeDriver.hxx>
#include <XmlMFunction_ScopeDriver.hxx>

//=======================================================================
//function : AddDrivers
//purpose  : 
//=======================================================================
void XmlMFunction::AddDrivers (const Handle(XmlMDF_ADriverTable)& aDriverTable,
                               const Handle(CDM_MessageDriver)&   aMessageDriver)
{
  aDriverTable->AddDriver(new XmlMFunction_FunctionDriver(aMessageDriver));
  aDriverTable->AddDriver(new XmlMFunction_ScopeDriver(aMessageDriver));
  aDriverTable->AddDriver(new XmlMFunction_GraphNodeDriver(aMessageDriver));
}
