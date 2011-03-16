// File:      BinDrivers_DocumentStorageDriver.cxx
// Created:   29.10.02 11:53:01
// Author:    Michael SAZONOV
// Copyright: Open CASCADE 2002

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
