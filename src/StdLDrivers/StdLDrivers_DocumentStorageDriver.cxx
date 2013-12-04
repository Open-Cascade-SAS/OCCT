// Created on: 2000-09-07
// Created by: TURIN  Anatoliy
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

#include <StdLDrivers_DocumentStorageDriver.ixx>
#include <MDF.hxx> 
#include <MDataStd.hxx> 
#include <MDocStd.hxx> 
#include <PDocStd_Document.hxx>
#include <TDocStd_Document.hxx>
#include <MFunction.hxx>
#include <TDF_Label.hxx>
#include <MDF_ASDriverHSequence.hxx>
#include <MDF_SRelocationTable.hxx>

#include <CDM_MessageDriver.hxx>


//=======================================================================
//function : StdLDrivers_DocumentStorageDriver
//purpose  : 
//=======================================================================

StdLDrivers_DocumentStorageDriver::StdLDrivers_DocumentStorageDriver () {}


//=======================================================================
//function : AttributeDrivers
//purpose  : faire le Plugin
//=======================================================================

Handle(MDF_ASDriverTable) StdLDrivers_DocumentStorageDriver::AttributeDrivers(const Handle(CDM_MessageDriver)& theMsgDriver) 
{
  Handle(MDF_ASDriverTable) aStorageTable  = new MDF_ASDriverTable(); 
  //
  Handle(MDF_ASDriverHSequence) HASSeq = new MDF_ASDriverHSequence();  
  MDF::AddStorageDrivers(HASSeq, theMsgDriver);   
  MDataStd::AddStorageDrivers(HASSeq, theMsgDriver);
  MFunction::AddStorageDrivers(HASSeq, theMsgDriver);
  MDocStd::AddStorageDrivers(HASSeq, theMsgDriver); 
  aStorageTable->SetDrivers(HASSeq);
  return aStorageTable;
}

//=======================================================================
//function : Make
//purpose  : 
//=======================================================================

void StdLDrivers_DocumentStorageDriver::Make (const Handle(CDM_Document)& aDocument,
					   PCDM_SequenceOfDocument& Documents)
{  
  Handle(PDocStd_Document) PDOC;   
  Handle(MDF_SRelocationTable) Reloc = new MDF_SRelocationTable();
  Handle(TDocStd_Document) TDOC = Handle(TDocStd_Document)::DownCast(aDocument);
  if (!TDOC.IsNull()) {
    PDOC = new PDocStd_Document ();
    Paste (TDOC,PDOC,Reloc);
  }
  Documents.Append(PDOC);
}
