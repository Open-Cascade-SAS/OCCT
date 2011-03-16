// File:      XmlMPrsStd.cxx
// Created:   Mon Jul  9 12:29:49 MSK DST 2001
// Author:    Julia DOROVSKIKH
// Copyright: Matra Datavision 2001

#include <XmlMPrsStd.ixx>

#include <XmlMPrsStd_PositionDriver.hxx>
#include <XmlMPrsStd_AISPresentationDriver.hxx>

///=======================================================================
//function : AddDrivers
//purpose  : 
//=======================================================================
void XmlMPrsStd::AddDrivers (const Handle(XmlMDF_ADriverTable)& aDriverTable,
                             const Handle(CDM_MessageDriver)&   aMessageDriver)
{
  aDriverTable->AddDriver(new XmlMPrsStd_PositionDriver(aMessageDriver)); 
  aDriverTable->AddDriver(new XmlMPrsStd_AISPresentationDriver(aMessageDriver));
}
