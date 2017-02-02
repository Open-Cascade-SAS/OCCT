// Created on: 2005-04-18
// Created by: Eugeny NAPALKOV
// Copyright (c) 2005-2014 OPEN CASCADE SAS
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


#include <BinDrivers.hxx>
#include <BinMDF_ADriverTable.hxx>
#include <BinMXCAFDoc.hxx>
#include <BinXCAFDrivers.hxx>
#include <BinXCAFDrivers_DocumentRetrievalDriver.hxx>
#include <BinXCAFDrivers_DocumentStorageDriver.hxx>
#include <CDM_MessageDriver.hxx>
#include <Plugin_Macro.hxx>
#include <Standard_Failure.hxx>
#include <Standard_GUID.hxx>
#include <TDocStd_Application.hxx>

static Standard_GUID BinXCAFStorageDriver  ("a78ff496-a779-11d5-aab4-0050044b1af1");
static Standard_GUID BinXCAFRetrievalDriver("a78ff497-a779-11d5-aab4-0050044b1af1");

//=======================================================================
//function :
//purpose  : 
//=======================================================================
const Handle(Standard_Transient)& BinXCAFDrivers::Factory(const Standard_GUID& theGUID) {

  if (theGUID == BinXCAFStorageDriver)
  {
#ifdef OCCT_DEBUG
    cout << "BinXCAFDrivers : Storage Plugin" << endl;
#endif
    static Handle(Standard_Transient) model_sd =
      new BinXCAFDrivers_DocumentStorageDriver;
    return model_sd;
  }

  if (theGUID == BinXCAFRetrievalDriver)
  {
#ifdef OCCT_DEBUG
    cout << "BinXCAFDrivers : Retrieval Plugin" << endl;
#endif
    static Handle(Standard_Transient) model_rd =
      new BinXCAFDrivers_DocumentRetrievalDriver;
    return model_rd;
  }


  throw Standard_Failure("XCAFBinDrivers : unknown GUID");
}

//=======================================================================
//function : DefineFormat
//purpose  : 
//=======================================================================
void BinXCAFDrivers::DefineFormat (const Handle(TDocStd_Application)& theApp)
{
  theApp->DefineFormat ("BinXCAF", "Binary XCAF Document", "xbf",
                        new BinXCAFDrivers_DocumentRetrievalDriver, 
                        new BinXCAFDrivers_DocumentStorageDriver);
}

//=======================================================================
//function :
//purpose  : 
//=======================================================================
Handle(BinMDF_ADriverTable) BinXCAFDrivers::AttributeDrivers(const Handle(CDM_MessageDriver)& aMsgDrv) {
  // Standard Drivers
  Handle(BinMDF_ADriverTable) aTable = BinDrivers::AttributeDrivers(aMsgDrv);

  // XCAF Drivers
  BinMXCAFDoc::AddDrivers(aTable, aMsgDrv);

  return aTable;
}

PLUGIN(BinXCAFDrivers)
