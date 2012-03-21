// Created on: 2002-10-31
// Created by: Michael SAZONOV
// Copyright (c) 2002-2012 OPEN CASCADE SAS
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


#include <BinDrivers_DocumentRetrievalDriver.ixx>
#include <BinDrivers.hxx>
#include <BinLDrivers_DocumentSection.hxx>
#include <BinMNaming.hxx>
#include <BinMNaming_NamedShapeDriver.hxx>
#include <TNaming_NamedShape.hxx>
#include <BinMDataStd.hxx>
#include <TCollection_ExtendedString.hxx>
#include <Standard_Failure.hxx>
#include <Standard_IStream.hxx>
#include <Standard_ErrorHandler.hxx>
//=======================================================================
//function : BinDrivers_DocumentRetrievalDriver
//purpose  : Constructor
//=======================================================================

BinDrivers_DocumentRetrievalDriver::BinDrivers_DocumentRetrievalDriver ()
{
}

//=======================================================================
//function : AttributeDrivers
//purpose  :
//=======================================================================

Handle(BinMDF_ADriverTable) BinDrivers_DocumentRetrievalDriver::AttributeDrivers
       (const Handle(CDM_MessageDriver)& theMessageDriver)
{
  return BinDrivers::AttributeDrivers (theMessageDriver);
}

//=======================================================================
//function : ReadShapeSection
//purpose  : 
//=======================================================================

void BinDrivers_DocumentRetrievalDriver::ReadShapeSection
                              (BinLDrivers_DocumentSection& /*theSection*/,
                               Standard_IStream&            theIS,
			       const Standard_Boolean       /*isMess*/)

{
  // Read Shapes
  Handle(BinMNaming_NamedShapeDriver) aNamedShapeDriver;
  if (myDrivers->GetDriver(STANDARD_TYPE(TNaming_NamedShape),aNamedShapeDriver))
    {
      try {
	OCC_CATCH_SIGNALS
	  aNamedShapeDriver->ReadShapeSection (theIS);
      }
      catch(Standard_Failure) {
	Handle(Standard_Failure) aFailure = Standard_Failure::Caught();
	const TCollection_ExtendedString aMethStr
	  ("BinDrivers_DocumentRetrievalDriver: ");
	WriteMessage (aMethStr + "error of Shape Section " +
		      aFailure->GetMessageString());
      }
    }
}

//=======================================================================
//function : CheckShapeSection
//purpose  : 
//=======================================================================
void BinDrivers_DocumentRetrievalDriver::CheckShapeSection(
			      const Storage_Position& /*ShapeSectionPos*/, 
				 	         Standard_IStream& /*IS*/)
{}

//=======================================================================
//function : PropagateDocumentVersion
//purpose  : 
//=======================================================================
void BinDrivers_DocumentRetrievalDriver::PropagateDocumentVersion(
				    const Standard_Integer theDocVersion )
{
  BinMDataStd::SetDocumentVersion(theDocVersion);
  BinMNaming::SetDocumentVersion(theDocVersion);
}

