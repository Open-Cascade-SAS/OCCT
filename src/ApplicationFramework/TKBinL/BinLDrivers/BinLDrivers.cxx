// Created on: 2002-10-29
// Created by: Michael SAZONOV
// Copyright (c) 2002-2014 OPEN CASCADE SAS
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

#include <BinLDrivers.hxx>
#include <BinLDrivers_DocumentRetrievalDriver.hxx>
#include <BinLDrivers_DocumentStorageDriver.hxx>
#include <BinMDataStd.hxx>
#include <BinMDF.hxx>
#include <BinMDF_ADriverTable.hxx>
#include <BinMDocStd.hxx>
#include <BinMFunction.hxx>
#include <Plugin_Macro.hxx>
#include <Standard_Failure.hxx>
#include <Standard_GUID.hxx>
#include <Standard_Transient.hxx>
#include <TDocStd_Application.hxx>

// #include <BinMNaming.hxx>
static Standard_GUID BinLStorageDriver("13a56835-8269-11d5-aab2-0050044b1af1");
static Standard_GUID BinLRetrievalDriver("13a56836-8269-11d5-aab2-0050044b1af1");

//=======================================================================
// function : Factory
// purpose  : PLUGIN FACTORY
//=======================================================================
const Handle(Standard_Transient)& BinLDrivers::Factory(const Standard_GUID& theGUID)
{
  if (theGUID == BinLStorageDriver)
  {
#ifdef OCCT_DEBUG
    std::cout << "BinLDrivers : Storage Plugin" << std::endl;
#endif
    static Handle(Standard_Transient) model_sd = new BinLDrivers_DocumentStorageDriver;
    return model_sd;
  }

  if (theGUID == BinLRetrievalDriver)
  {
#ifdef OCCT_DEBUG
    std::cout << "BinLDrivers : Retrieval Plugin" << std::endl;
#endif
    static Handle(Standard_Transient) model_rd = new BinLDrivers_DocumentRetrievalDriver;
    return model_rd;
  }

  throw Standard_Failure("BinLDrivers : unknown GUID");
}

//=================================================================================================

void BinLDrivers::DefineFormat(const Handle(TDocStd_Application)& theApp)
{
  theApp->DefineFormat("BinLOcaf",
                       "Binary Lite OCAF Document",
                       "cbfl",
                       new BinLDrivers_DocumentRetrievalDriver,
                       new BinLDrivers_DocumentStorageDriver);
}

//=================================================================================================

Handle(BinMDF_ADriverTable) BinLDrivers::AttributeDrivers(const Handle(Message_Messenger)& aMsgDrv)
{
  Handle(BinMDF_ADriverTable) aTable = new BinMDF_ADriverTable;

  BinMDF ::AddDrivers(aTable, aMsgDrv);
  BinMDataStd ::AddDrivers(aTable, aMsgDrv);
  BinMFunction ::AddDrivers(aTable, aMsgDrv);
  BinMDocStd ::AddDrivers(aTable, aMsgDrv);

  return aTable;
}

PLUGIN(BinLDrivers)
