// Created on: 2004-11-24
// Created by: Michael SAZONOV
// Copyright (c) 2004-2014 OPEN CASCADE SAS
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

// The original implementation Copyright: (C) RINA S.p.A

#include <XmlTObjDrivers_DocumentStorageDriver.hxx>
#include <XmlLDrivers.hxx>
#include <XmlTObjDrivers.hxx>

IMPLEMENT_STANDARD_RTTIEXT(XmlTObjDrivers_DocumentStorageDriver,XmlLDrivers_DocumentStorageDriver)

//=======================================================================
//function : XmlTObjDrivers_DocumentStorageDriver
//purpose  : 
//=======================================================================

XmlTObjDrivers_DocumentStorageDriver::XmlTObjDrivers_DocumentStorageDriver
                                (const TCollection_ExtendedString& theCopyright)
     : XmlLDrivers_DocumentStorageDriver (theCopyright)
{
}

//=======================================================================
//function : AttributeDrivers
//purpose  : 
//=======================================================================

Handle(XmlMDF_ADriverTable)
        XmlTObjDrivers_DocumentStorageDriver::AttributeDrivers
                        (const Handle(CDM_MessageDriver)& theMsgDrv)
{
  // Standard drivers
  Handle(XmlMDF_ADriverTable) aTable = XmlLDrivers::AttributeDrivers (theMsgDrv);

  // Native drivers
  XmlTObjDrivers::AddDrivers(aTable, theMsgDrv);

  return aTable;
}
