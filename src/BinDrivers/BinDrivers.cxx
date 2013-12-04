// Created on: 2002-10-29
// Created by: Michael SAZONOV
// Copyright (c) 2002-2014 OPEN CASCADE SAS
//
// This file is part of Open CASCADE Technology software library.
//
// This library is free software; you can redistribute it and / or modify it
// under the terms of the GNU Lesser General Public version 2.1 as published
// by the Free Software Foundation, with special exception defined in the file
// OCCT_LGPL_EXCEPTION.txt. Consult the file LICENSE_LGPL_21.txt included in OCCT
// distribution for complete text of the license and disclaimer of any warranty.
//
// Alternatively, this file may be used under the terms of Open CASCADE
// commercial license or contractual agreement.

#include <BinDrivers.ixx>
#include <BinLDrivers.hxx>
#include <BinMDF.hxx>
#include <BinMDataStd.hxx>
#include <BinMDataXtd.hxx>
#include <BinMNaming.hxx>
#include <BinMDocStd.hxx>
#include <BinMFunction.hxx>
#include <BinMPrsStd.hxx>
#include <Standard_Failure.hxx>
#include <BinDrivers_DocumentStorageDriver.hxx>
#include <BinDrivers_DocumentRetrievalDriver.hxx>
#include <Plugin_Macro.hxx>

static Standard_GUID BinStorageDriver  ("03a56835-8269-11d5-aab2-0050044b1af1");
static Standard_GUID BinRetrievalDriver("03a56836-8269-11d5-aab2-0050044b1af1");

//=======================================================================
//function : Factory
//purpose  : PLUGIN FACTORY
//=======================================================================
Handle(Standard_Transient) BinDrivers::Factory(const Standard_GUID& theGUID)
{
  if (theGUID == BinStorageDriver)
  {
    cout << "BinDrivers : Storage Plugin" << endl;
    static Handle(BinDrivers_DocumentStorageDriver) model_sd =
      new BinDrivers_DocumentStorageDriver;
    return model_sd;
  }

  if (theGUID == BinRetrievalDriver)
  {
    cout << "BinDrivers : Retrieval Plugin" << endl;
    static Handle(BinDrivers_DocumentRetrievalDriver) model_rd =
      new BinDrivers_DocumentRetrievalDriver;
    return model_rd;
  }

  Standard_Failure::Raise ("BinDrivers : unknown GUID");
  return NULL;
}

//=======================================================================
//function : AttributeDrivers
//purpose  :
//=======================================================================

Handle(BinMDF_ADriverTable) BinDrivers::AttributeDrivers 
                         (const Handle(CDM_MessageDriver)& aMsgDrv)
{
  Handle(BinMDF_ADriverTable) aTable = new BinMDF_ADriverTable;

  BinMDF        ::AddDrivers (aTable, aMsgDrv);
  BinMDataStd   ::AddDrivers (aTable, aMsgDrv);
  BinMDataXtd   ::AddDrivers (aTable, aMsgDrv);  
  BinMNaming    ::AddDrivers (aTable, aMsgDrv);
  BinMDocStd    ::AddDrivers (aTable, aMsgDrv);
  BinMFunction  ::AddDrivers (aTable, aMsgDrv);
  BinMPrsStd    ::AddDrivers (aTable, aMsgDrv);
  return aTable;
}

//=======================================================================
//function : StorageVersion
//purpose  : 
//=======================================================================

TCollection_AsciiString BinDrivers::StorageVersion()
{
  return BinLDrivers::StorageVersion();
}

#ifdef _MSC_VER
#pragma warning(disable:4190) /* disable warning on C++ type returned by C function; should be OK for C++ usage */
#endif
PLUGIN(BinDrivers)
