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


#include <StdDrivers_DocumentStorageDriver.ixx>
#include <MDF.hxx> 
#include <MDataStd.hxx> 
#include <MDataXtd.hxx>
#include <MNaming.hxx>  
#include <MDocStd.hxx> 
#include <MPrsStd.hxx>
#include <MFunction.hxx>

#include <TNaming_Tool.hxx>
#include <TNaming_NamedShape.hxx>
#include <PDocStd_Document.hxx>
#include <TDocStd_Document.hxx>

#include <TDF_Label.hxx>
#include <MDF_ASDriverHSequence.hxx>
#include <MDF_SRelocationTable.hxx>
#include <CDM_MessageDriver.hxx>



//=======================================================================
//function : PAppStd_DocumentStorageDriver
//purpose  : 
//=======================================================================

StdDrivers_DocumentStorageDriver::StdDrivers_DocumentStorageDriver () {}


//=======================================================================
//function : AttributeDrivers
//purpose  : faire le Plugin
//=======================================================================

Handle(MDF_ASDriverTable) StdDrivers_DocumentStorageDriver::AttributeDrivers(const Handle(CDM_MessageDriver)& theMsgDriver) 
{
  Handle(MDF_ASDriverTable) aStorageTable  = new MDF_ASDriverTable(); 
  //
  Handle(MDF_ASDriverHSequence) HASSeq = new MDF_ASDriverHSequence();  
  MDF::AddStorageDrivers(HASSeq, theMsgDriver);   
  MDataStd::AddStorageDrivers(HASSeq, theMsgDriver);
  MDataXtd::AddStorageDrivers(HASSeq, theMsgDriver);
  MDocStd::AddStorageDrivers(HASSeq, theMsgDriver); 
  MFunction::AddStorageDrivers(HASSeq, theMsgDriver);
  MNaming::AddStorageDrivers(HASSeq, theMsgDriver);
  MPrsStd::AddStorageDrivers(HASSeq, theMsgDriver);
  //
  aStorageTable->SetDrivers(HASSeq);
  return aStorageTable;
}

//=======================================================================
//function : Make
//purpose  : 
//=======================================================================

void StdDrivers_DocumentStorageDriver::Make (const Handle(CDM_Document)& aDocument,
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
