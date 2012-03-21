// Created on: 2004-11-24
// Created by: Michael SAZONOV
// Copyright (c) 2004-2012 OPEN CASCADE SAS
//
// The content of this file is subject to the Open CASCADE Technology Public
// License Version 6.5 (the "License"). You may not use the content of this file
// except in compliance with the License. Please obtain a copy of the License
// at http://www.opencascade.org and read it completely before using this file.
//
// The Initial Developer of the Original Code is Open CASCADE S.A.S., having its
// main offices at: 1, place des Freres Montgolfier, 78280 Guyancourt, France.
//
// The Original Code and all software distributed under the License is
// distributed on an "AS IS" basis, without warranty of any kind, and the
// Initial Developer hereby disclaims all such warranties, including without
// limitation, any warranties of merchantability, fitness for a particular
// purpose or non-infringement. Please see the License for the specific terms
// and conditions governing the rights and limitations under the License.

// The original implementation Copyright: (C) RINA S.p.A

#include <XmlTObjDrivers_DocumentStorageDriver.hxx>
#include <XmlLDrivers.hxx>
#include <XmlTObjDrivers.hxx>

IMPLEMENT_STANDARD_HANDLE (XmlTObjDrivers_DocumentStorageDriver,
                           XmlLDrivers_DocumentStorageDriver)
IMPLEMENT_STANDARD_RTTIEXT(XmlTObjDrivers_DocumentStorageDriver,
                           XmlLDrivers_DocumentStorageDriver)

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
