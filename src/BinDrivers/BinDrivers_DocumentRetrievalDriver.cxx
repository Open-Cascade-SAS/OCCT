// File:      BinDrivers_DocumentRetrievalDriver.cxx
// Created:   31.10.02 11:12:22
// Author:    Michael SAZONOV
// Copyright: Open CASCADE 2002

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

