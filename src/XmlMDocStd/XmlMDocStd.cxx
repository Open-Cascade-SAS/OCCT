// File:      XmlMDocStd.cxx
// Created:   05.09.01 09:40:49
// Author:    Julia DOROVSKIKH
// Copyright: Open Cascade 2001
// History:

#include <XmlMDocStd.ixx>

#include <XmlMDocStd_XLinkDriver.hxx>

//=======================================================================
//function : AddDrivers
//purpose  : 
//=======================================================================
void XmlMDocStd::AddDrivers (const Handle(XmlMDF_ADriverTable)& aDriverTable,
                             const Handle(CDM_MessageDriver)&   aMessageDriver)
{
  aDriverTable->AddDriver (new XmlMDocStd_XLinkDriver(aMessageDriver)); 
}
