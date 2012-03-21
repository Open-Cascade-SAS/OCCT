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

#include <XmlLDrivers.hxx>
#include <XmlTObjDrivers.hxx>
#include <XmlMDF_ADriverTable.hxx>
#include <XmlTObjDrivers_DocumentStorageDriver.hxx>
#include <XmlTObjDrivers_DocumentRetrievalDriver.hxx>
#include <XmlTObjDrivers_ModelDriver.hxx>
#include <XmlTObjDrivers_ObjectDriver.hxx>
#include <XmlTObjDrivers_ReferenceDriver.hxx>
#include <XmlTObjDrivers_XYZDriver.hxx>
#include <XmlTObjDrivers_IntSparseArrayDriver.hxx>

#include <Plugin_Macro.hxx>

// avoid warnings on 'extern "C"' functions returning C++ classes
#ifdef WNT
#pragma warning(4:4190)
#endif

static Standard_GUID XmlStorageDriver  ("f78ff4a0-a779-11d5-aab4-0050044b1af1");
static Standard_GUID XmlRetrievalDriver("f78ff4a1-a779-11d5-aab4-0050044b1af1");

Handle(Standard_Transient) XmlTObjDrivers::Factory(const Standard_GUID& aGUID)
{
  if(aGUID == XmlStorageDriver)
  {
    cout << "XmlTObjDrivers : Storage Plugin" << endl;
    static Handle(XmlTObjDrivers_DocumentStorageDriver) model_sd
      = new XmlTObjDrivers_DocumentStorageDriver
        ("Copyright: Open CASCADE 2004"); // default copyright
    return model_sd;
  }

  if(aGUID == XmlRetrievalDriver)
  {  
    cout << "XmlTObjDrivers : Retrieval Plugin" << endl;
    static Handle (XmlTObjDrivers_DocumentRetrievalDriver) model_rd
      = new XmlTObjDrivers_DocumentRetrievalDriver;
    return model_rd;
  }

  return XmlLDrivers::Factory (aGUID);
}

//=======================================================================
//function : AddDrivers
//purpose  : 
//=======================================================================
void XmlTObjDrivers::AddDrivers (const Handle(XmlMDF_ADriverTable)& aDriverTable,
                                 const Handle(CDM_MessageDriver)&   anMsgDrv)
{
  aDriverTable -> AddDriver (new XmlTObjDrivers_ModelDriver         (anMsgDrv));
  aDriverTable -> AddDriver (new XmlTObjDrivers_ObjectDriver        (anMsgDrv));
  aDriverTable -> AddDriver (new XmlTObjDrivers_ReferenceDriver     (anMsgDrv));
  aDriverTable -> AddDriver (new XmlTObjDrivers_XYZDriver           (anMsgDrv));
  aDriverTable -> AddDriver (new XmlTObjDrivers_IntSparseArrayDriver(anMsgDrv));
}

PLUGIN(XmlTObjDrivers)
