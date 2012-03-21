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
