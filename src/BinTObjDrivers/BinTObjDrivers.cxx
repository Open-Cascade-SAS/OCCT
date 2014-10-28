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

static Standard_GUID BinStorageDriver  ("f78ff4a2-a779-11d5-aab4-0050044b1af1");
static Standard_GUID BinRetrievalDriver("f78ff4a3-a779-11d5-aab4-0050044b1af1");

Handle(Standard_Transient) BinTObjDrivers::Factory(const Standard_GUID& aGUID)
{
  if(aGUID == BinStorageDriver)
  {
#ifdef OCCT_DEBUG
    cout << "BinTObjDrivers : Storage Plugin" << endl;
#endif
    static Handle(BinTObjDrivers_DocumentStorageDriver) model_sd
      = new BinTObjDrivers_DocumentStorageDriver;
    return model_sd;
  }

  if(aGUID == BinRetrievalDriver)
  {  
#ifdef OCCT_DEBUG
    cout << "BinTObjDrivers : Retrieval Plugin" << endl;
#endif
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

#ifdef _MSC_VER
#pragma warning(disable:4190) /* disable warning on C++ type returned by C function; should be OK for C++ usage */
#endif
PLUGIN(BinTObjDrivers)
