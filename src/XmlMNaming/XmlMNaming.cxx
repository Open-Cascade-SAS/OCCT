// File:      XmlMNaming.cxx
// Created:   31.07.01 20:34:58
// Author:    Alexander GRIGORIEV
// Copyright: Open Cascade 2001
// History:

#include <XmlMNaming.ixx>

#include <XmlMNaming_NamedShapeDriver.hxx>
#include <XmlMNaming_NamingDriver.hxx>

static Standard_Integer myDocumentVersion = -1;
//=======================================================================
//function : AddStorageDrivers
//purpose  : 
//=======================================================================
void XmlMNaming::AddDrivers (const Handle(XmlMDF_ADriverTable)& aDriverTable,
                             const Handle(CDM_MessageDriver)&   aMessageDriver)
{
  aDriverTable->AddDriver (new XmlMNaming_NamedShapeDriver(aMessageDriver));
  aDriverTable->AddDriver (new XmlMNaming_NamingDriver(aMessageDriver));
}

//=======================================================================
//function : SetDocumentVersion
//purpose  : Sets current document version
//=======================================================================
void XmlMNaming::SetDocumentVersion(const Standard_Integer theVersion)
{
  myDocumentVersion = theVersion;
}
//=======================================================================
//function : DocumentVersion
//purpose  : Retrieved document version
//=======================================================================
Standard_Integer XmlMNaming::DocumentVersion()
{
  return myDocumentVersion;
}
