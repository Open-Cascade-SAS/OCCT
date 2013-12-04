// Created on: 2000-05-24
// Created by: Edward AGAPOV
// Copyright (c) 2000-2014 OPEN CASCADE SAS
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

#include <XCAFDrivers_DocumentStorageDriver.ixx>
#include <MDF.hxx>
#include <MDF_ASDriverHSequence.hxx>
#include <MDF_ASDriverTable.hxx>
#include <MXCAFDoc.hxx>
#include <StdDrivers_DocumentStorageDriver.hxx>

//=======================================================================
//function : XCAFDrivers_DocumentStorageDriver
//purpose  : 
//=======================================================================

XCAFDrivers_DocumentStorageDriver::XCAFDrivers_DocumentStorageDriver() {}

//=======================================================================
//function : AttributeDrivers
//purpose  : 
//=======================================================================

Handle(MDF_ASDriverTable) XCAFDrivers_DocumentStorageDriver::AttributeDrivers(const Handle(CDM_MessageDriver)& theMessageDriver) 
{
  // Standard drivers
  Handle(StdDrivers_DocumentStorageDriver) aDocumentStorageDriver =
    new StdDrivers_DocumentStorageDriver;
  Handle(MDF_ASDriverTable) aStorageTable =
    aDocumentStorageDriver->AttributeDrivers(theMessageDriver);
  
  // Native drivers
  Handle(MDF_ASDriverHSequence) anHASSeq = new MDF_ASDriverHSequence();  
  MXCAFDoc::AddStorageDrivers          (anHASSeq, theMessageDriver);
  //
  //aStorageTable will append Native drivers
  aStorageTable->SetDrivers             (anHASSeq);
  return aStorageTable;
}
