// Created on: 2002-10-29
// Created by: Michael SAZONOV
// Copyright (c) 2002-2014 OPEN CASCADE SAS
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

#include <BinDrivers_DocumentStorageDriver.ixx>
#include <Standard_ErrorHandler.hxx>
#include <TCollection_AsciiString.hxx>
#include <BinDrivers.hxx>
#include <BinMDF_ADriver.hxx>
#include <BinMNaming_NamedShapeDriver.hxx>
#include <TNaming_NamedShape.hxx>
//=======================================================================
//function : BinDrivers_DocumentStorageDriver
//purpose  : Constructor
//=======================================================================

BinDrivers_DocumentStorageDriver::BinDrivers_DocumentStorageDriver ()
{
}

//=======================================================================
//function : AttributeDrivers
//purpose  :
//=======================================================================

Handle(BinMDF_ADriverTable) BinDrivers_DocumentStorageDriver::AttributeDrivers
       (const Handle(CDM_MessageDriver)& theMessageDriver)
{
  return BinDrivers::AttributeDrivers (theMessageDriver);
}

//=======================================================================
//function : WriteShapeSection
//purpose  : Implements WriteShapeSection
//=======================================================================
void BinDrivers_DocumentStorageDriver::WriteShapeSection
                               (BinLDrivers_DocumentSection&   theSection,
                                Standard_OStream&              theOS)
{

#if defined(_DEBUG) || defined(DEB)
  TCollection_ExtendedString aMethStr ("BinDrivers_DocumentStorageDriver, ");
#else
  TCollection_ExtendedString aMethStr;
#endif
  const Standard_Size aShapesSectionOffset = (Standard_Size) theOS.tellp();
  
  Handle(BinMNaming_NamedShapeDriver) aNamedShapeDriver;
  if (myDrivers->GetDriver(STANDARD_TYPE(TNaming_NamedShape), aNamedShapeDriver)) {   
    try { 
      OCC_CATCH_SIGNALS  aNamedShapeDriver->WriteShapeSection (theOS);
    }
    catch(Standard_Failure) {
      TCollection_ExtendedString anErrorStr ("Error: ");
      Handle(Standard_Failure) aFailure = Standard_Failure::Caught();
      TCollection_ExtendedString aStr = 
	anErrorStr + aMethStr + "Shape Section :";
      WriteMessage (aStr  + aFailure->GetMessageString());
    }
  }
   
  // Write the section info in the TOC.
  theSection.Write (theOS, aShapesSectionOffset);
}
