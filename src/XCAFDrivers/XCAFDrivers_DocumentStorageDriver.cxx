// Created on: 2000-05-24
// Created by: Edward AGAPOV
// Copyright (c) 2000-2012 OPEN CASCADE SAS
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
