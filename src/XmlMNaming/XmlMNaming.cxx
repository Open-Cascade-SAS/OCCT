// Created on: 2001-07-31
// Created by: Alexander GRIGORIEV
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
#include <XmlMNaming.hxx>
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
