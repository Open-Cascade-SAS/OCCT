// Created on: 2002-10-31
// Created by: Michael SAZONOV
// Copyright (c) 2002-2014 OPEN CASCADE SAS
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


#include <BinDrivers.hxx>
#include <BinDrivers_DocumentRetrievalDriver.hxx>
#include <BinLDrivers_DocumentSection.hxx>
#include <BinMDataStd.hxx>
#include <BinMDF_ADriverTable.hxx>
#include <BinMNaming.hxx>
#include <BinMNaming_NamedShapeDriver.hxx>
#include <CDM_MessageDriver.hxx>
#include <Standard_ErrorHandler.hxx>
#include <Standard_Failure.hxx>
#include <Standard_IStream.hxx>
#include <Standard_Type.hxx>
#include <TCollection_ExtendedString.hxx>
#include <TNaming_NamedShape.hxx>

IMPLEMENT_STANDARD_RTTIEXT(BinDrivers_DocumentRetrievalDriver,BinLDrivers_DocumentRetrievalDriver)

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
  Handle(BinMDF_ADriver) aDriver;
  if (myDrivers->GetDriver(STANDARD_TYPE(TNaming_NamedShape),aDriver))
  {
    try {
      OCC_CATCH_SIGNALS
      Handle(BinMNaming_NamedShapeDriver) aNamedShapeDriver =
        Handle(BinMNaming_NamedShapeDriver)::DownCast (aDriver);
      aNamedShapeDriver->ReadShapeSection (theIS);
    }
    catch(Standard_Failure const& anException) {
      const TCollection_ExtendedString aMethStr
        ("BinDrivers_DocumentRetrievalDriver: ");
      WriteMessage (aMethStr + "error of Shape Section " +
        anException.GetMessageString());
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
//function : Clear
//purpose  : 
//=======================================================================
void BinDrivers_DocumentRetrievalDriver::Clear()
{
  // Clear NamedShape driver
  Handle(BinMDF_ADriver) aDriver;
  if (myDrivers->GetDriver(STANDARD_TYPE(TNaming_NamedShape), aDriver))
  {
    Handle(BinMNaming_NamedShapeDriver) aNamedShapeDriver =
      Handle(BinMNaming_NamedShapeDriver)::DownCast(aDriver);
    aNamedShapeDriver->Clear();
  }
  BinLDrivers_DocumentRetrievalDriver::Clear();
}

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

