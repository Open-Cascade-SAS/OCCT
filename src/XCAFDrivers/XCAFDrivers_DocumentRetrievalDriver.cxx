// Created on: 2000-05-24
// Created by: Edward AGAPOV
// Copyright (c) 2000-2014 OPEN CASCADE SAS
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

#include <XCAFDrivers_DocumentRetrievalDriver.ixx>
#include <MDF.hxx>
#include <MDataStd.hxx> 
#include <MDF_ARDriverHSequence.hxx>
#include <MXCAFDoc.hxx>
#include <StdDrivers_DocumentRetrievalDriver.hxx>

//=======================================================================
//function : XCAFDrivers_DocumentRetrievalDriver
//purpose  : 
//=======================================================================

XCAFDrivers_DocumentRetrievalDriver::XCAFDrivers_DocumentRetrievalDriver() {}

//=======================================================================
//function : AttributeDrivers
//purpose  : 
//=======================================================================

Handle(MDF_ARDriverTable) XCAFDrivers_DocumentRetrievalDriver::AttributeDrivers(const Handle(CDM_MessageDriver)& theMessageDriver)
{ 
  // Standard drivers
  Handle(StdDrivers_DocumentRetrievalDriver) aDocumentRetrievalDriver = new StdDrivers_DocumentRetrievalDriver;
  Handle(MDF_ARDriverTable) aRetrievalTable = aDocumentRetrievalDriver->AttributeDrivers(theMessageDriver);
  
  // Native drivers
  Handle(MDF_ARDriverHSequence) anHARSeq = new MDF_ARDriverHSequence();  
  MXCAFDoc::AddRetrievalDrivers        (anHARSeq, theMessageDriver);
  //
  aRetrievalTable->SetDrivers           (anHARSeq); //aRetrievalTable will append Native drivers
  return aRetrievalTable;
}
