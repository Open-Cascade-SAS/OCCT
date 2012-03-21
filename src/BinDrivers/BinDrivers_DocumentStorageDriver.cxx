// Created on: 2002-10-29
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
