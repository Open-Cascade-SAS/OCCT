// Created on: 2001-09-11
// Created by: Julia DOROVSKIKH
// Copyright (c) 2001-2012 OPEN CASCADE SAS
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


#include <XmlXCAFDrivers_DocumentStorageDriver.ixx>

#include <XmlDrivers.hxx>
#include <XmlMXCAFDoc.hxx>

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
