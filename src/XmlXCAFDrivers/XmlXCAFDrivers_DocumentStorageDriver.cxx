// Created on: 2001-09-11
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
#include <Standard_Type.hxx>
#include <TCollection_ExtendedString.hxx>
#include <XmlDrivers.hxx>
#include <XmlMDF_ADriverTable.hxx>
#include <XmlMXCAFDoc.hxx>
#include <XmlXCAFDrivers_DocumentStorageDriver.hxx>

IMPLEMENT_STANDARD_RTTIEXT(XmlXCAFDrivers_DocumentStorageDriver,XmlDrivers_DocumentStorageDriver)

//=======================================================================
//function : XmlXCAFDrivers_DocumentStorageDriver
//purpose  : 
//=======================================================================
XmlXCAFDrivers_DocumentStorageDriver::XmlXCAFDrivers_DocumentStorageDriver
                                (const TCollection_ExtendedString& theCopyright)
     : XmlDrivers_DocumentStorageDriver (theCopyright)
{
  AddNamespace ("xcaf","http://www.opencascade.org/OCAF/XML/XCAF");
}

//=======================================================================
//function : AttributeDrivers
//purpose  : 
//=======================================================================
Handle(XmlMDF_ADriverTable)
        XmlXCAFDrivers_DocumentStorageDriver::AttributeDrivers
                        (const Handle(CDM_MessageDriver)& theMsgDrv) 
{
  // Standard drivers
  Handle(XmlMDF_ADriverTable) aTable = XmlDrivers::AttributeDrivers (theMsgDrv);

  // Native drivers
  XmlMXCAFDoc::AddDrivers(aTable, theMsgDrv);

  return aTable;
}
