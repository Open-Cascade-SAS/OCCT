// Created on: 2000-09-07
// Created by: TURIN  Anatoliy
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


#include <StdDrivers_DocumentRetrievalDriver.ixx>
#include <MDF_ARDriverHSequence.hxx>

#include <MDF.hxx>
#include <MDataStd.hxx> 
#include <MDataXtd.hxx>
#include <MNaming.hxx>  
#include <MDocStd.hxx> 
#include <MFunction.hxx>
#include <MPrsStd.hxx>
#include <CDM_MessageDriver.hxx>
//=======================================================================
//function : PAppStd_DocumentRetrievalDriver
//purpose  : 
//=======================================================================

StdDrivers_DocumentRetrievalDriver::StdDrivers_DocumentRetrievalDriver() {}

//=======================================================================
//function : AttributeDrivers
//purpose  : 
//=======================================================================

Handle(MDF_ARDriverTable) StdDrivers_DocumentRetrievalDriver::AttributeDrivers(const Handle(CDM_MessageDriver)& theMsgDriver)
{ 
  Handle(MDF_ARDriverTable) aRetrievalTable  = new MDF_ARDriverTable(); 
  //
  Handle(MDF_ARDriverHSequence) HARSeq = new MDF_ARDriverHSequence();  
  MDF::AddRetrievalDrivers(HARSeq, theMsgDriver);  
  MDataStd::AddRetrievalDrivers(HARSeq, theMsgDriver);
  MDataXtd::AddRetrievalDrivers(HARSeq, theMsgDriver);
  MDocStd::AddRetrievalDrivers(HARSeq, theMsgDriver);
  MFunction::AddRetrievalDrivers(HARSeq, theMsgDriver);
  MNaming::AddRetrievalDrivers(HARSeq, theMsgDriver); 
  MPrsStd::AddRetrievalDrivers(HARSeq, theMsgDriver); 
 
  //
  aRetrievalTable->SetDrivers(HARSeq);
  return aRetrievalTable;
}

