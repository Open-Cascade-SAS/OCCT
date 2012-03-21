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

#include <BinLDrivers.hxx>
#include <BinMDF_ADriverTable.hxx>
#include <BinTObjDrivers.hxx>
#include <BinTObjDrivers_DocumentStorageDriver.hxx>
#include <BinTObjDrivers_DocumentRetrievalDriver.hxx>
#include <BinTObjDrivers_IntSparseArrayDriver.hxx>
#include <BinTObjDrivers_ModelDriver.hxx>
#include <BinTObjDrivers_ObjectDriver.hxx>
#include <BinTObjDrivers_ReferenceDriver.hxx>
#include <BinTObjDrivers_XYZDriver.hxx>
#include <Plugin_Macro.hxx>

// avoid warnings on 'extern "C"' functions returning C++ classes
#ifdef WNT
#pragma warning(4:4190)
#endif

static Standard_GUID BinStorageDriver  ("f78ff4a2-a779-11d5-aab4-0050044b1af1");
static Standard_GUID BinRetrievalDriver("f78ff4a3-a779-11d5-aab4-0050044b1af1");

Handle(Standard_Transient) BinTObjDrivers::Factory(const Standard_GUID& aGUID)
{
  if(aGUID == BinStorageDriver)
  {
    cout << "BinTObjDrivers : Storage Plugin" << endl;
    static Handle(BinTObjDrivers_DocumentStorageDriver) model_sd
      = new BinTObjDrivers_DocumentStorageDriver;
    return model_sd;
  }

  if(aGUID == BinRetrievalDriver)
  {  
    cout << "BinTObjDrivers : Retrieval Plugin" << endl;
    static Handle (BinTObjDrivers_DocumentRetrievalDriver) model_rd
      = new BinTObjDrivers_DocumentRetrievalDriver;
    return model_rd;
  }

  return BinLDrivers::Factory (aGUID);
}

//=======================================================================
//function : AddDrivers
//purpose  : 
//=======================================================================
void BinTObjDrivers::AddDrivers (const Handle(BinMDF_ADriverTable)& aDriverTable,
                                 const Handle(CDM_MessageDriver)&   aMsgDrv)
{
  aDriverTable -> AddDriver (new BinTObjDrivers_ModelDriver      (aMsgDrv));
  aDriverTable -> AddDriver (new BinTObjDrivers_ObjectDriver     (aMsgDrv));
  aDriverTable -> AddDriver (new BinTObjDrivers_ReferenceDriver  (aMsgDrv));
  aDriverTable -> AddDriver (new BinTObjDrivers_XYZDriver        (aMsgDrv));
  aDriverTable -> AddDriver (new BinTObjDrivers_IntSparseArrayDriver (aMsgDrv));
}

PLUGIN(BinTObjDrivers)
