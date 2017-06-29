// Created on: 2000-09-07
// Created by: TURIN Anatoliy
// Copyright (c) 2000-2015 OPEN CASCADE SAS
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

#include <StdDrivers.hxx>
#include <StdDrivers_DocumentRetrievalDriver.hxx>

#include <StdLPersistent.hxx>
#include <StdPersistent.hxx>
#include <ShapePersistent.hxx>

#include <Standard_Failure.hxx>
#include <Standard_GUID.hxx>
#include <Plugin_Macro.hxx>

#include <PCDM_StorageDriver.hxx>
#include <TDocStd_Application.hxx>

static Standard_GUID StdRetrievalDriver ("ad696001-5b34-11d1-b5ba-00a0c9064368");

//=======================================================================
//function : Factory
//purpose  : Depending from the ID, returns a list of storage
//           or retrieval attribute drivers. Used for plugin
//=======================================================================
Handle(Standard_Transient) StdDrivers::Factory (const Standard_GUID& aGUID)
{
  if (aGUID == StdRetrievalDriver)
  {
#ifdef OCCT_DEBUG
    cout << "StdDrivers : Retrieval Plugin" << endl;
#endif

    static Handle(StdDrivers_DocumentRetrievalDriver) model_rd = new StdDrivers_DocumentRetrievalDriver;
    return model_rd;
  }
 
  throw Standard_Failure("StdDrivers : unknown GUID");
}

//=======================================================================
//function : DefineFormat
//purpose  : 
//=======================================================================
void StdDrivers::DefineFormat (const Handle(TDocStd_Application)& theApp)
{
  theApp->DefineFormat ("MDTV-Standard", "Standard OCAF Document", "std",
                        new StdDrivers_DocumentRetrievalDriver, 0);
}

//=======================================================================
//function : BindTypes
//purpose  : Register types
//=======================================================================
void StdDrivers::BindTypes (StdObjMgt_MapOfInstantiators& theMap)
{
  StdLPersistent ::BindTypes (theMap);
  StdPersistent  ::BindTypes (theMap);
  ShapePersistent::BindTypes (theMap);
}

// Declare entry point PLUGINFACTORY
PLUGIN (StdDrivers)
