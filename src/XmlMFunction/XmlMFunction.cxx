// File:      XmlMFunction.cxx
// Created:   Mon Jul  9 12:29:49 MSK DST 2001
// Author:    Julia DOROVSKIKH
// Copyright: Matra Datavision 2001

#include <XmlMFunction.ixx>

#include <XmlMFunction_FunctionDriver.hxx>
#include <XmlMFunction_ScopeDriver.hxx>
#include <XmlMFunction_GraphNodeDriver.hxx>

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
