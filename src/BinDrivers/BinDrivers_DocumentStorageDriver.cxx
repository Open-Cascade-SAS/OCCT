// Created on: 2002-10-29
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

#include <BinDrivers_DocumentStorageDriver.hxx>

#include <BinDrivers.hxx>
#include <BinLDrivers_DocumentSection.hxx>
#include <BinMDF_ADriver.hxx>
#include <BinMDF_ADriverTable.hxx>
#include <BinMNaming_NamedShapeDriver.hxx>
#include <CDM_MessageDriver.hxx>
#include <Standard_ErrorHandler.hxx>
#include <Standard_NotImplemented.hxx>
#include <Standard_Type.hxx>
#include <TCollection_AsciiString.hxx>
#include <TNaming_NamedShape.hxx>

IMPLEMENT_STANDARD_RTTIEXT(BinDrivers_DocumentStorageDriver,BinLDrivers_DocumentStorageDriver)

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
//function : IsWithTriangles
//purpose  :
//=======================================================================
Standard_Boolean BinDrivers_DocumentStorageDriver::IsWithTriangles() const
{
  if (myDrivers.IsNull())
  {
    return Standard_False;
  }

  Handle(BinMDF_ADriver) aDriver;
  myDrivers->GetDriver (STANDARD_TYPE(TNaming_NamedShape), aDriver);
  Handle(BinMNaming_NamedShapeDriver) aShapesDriver = Handle(BinMNaming_NamedShapeDriver)::DownCast(aDriver);
  return !aShapesDriver.IsNull()
       && aShapesDriver->IsWithTriangles();
}

//=======================================================================
//function : SetWithTriangles
//purpose  :
//=======================================================================
void BinDrivers_DocumentStorageDriver::SetWithTriangles (const Handle(CDM_MessageDriver)& theMessageDriver,
                                                         const Standard_Boolean theWithTriangulation)
{
  if (myDrivers.IsNull())
  {
    myDrivers = AttributeDrivers (theMessageDriver);
  }
  if (myDrivers.IsNull())
  {
    return;
  }

  Handle(BinMDF_ADriver) aDriver;
  myDrivers->GetDriver (STANDARD_TYPE(TNaming_NamedShape), aDriver);
  Handle(BinMNaming_NamedShapeDriver) aShapesDriver = Handle(BinMNaming_NamedShapeDriver)::DownCast(aDriver);
  if (aShapesDriver.IsNull())
  {
    throw Standard_NotImplemented("Internal Error - TNaming_NamedShape is not found!");
  }

  aShapesDriver->SetWithTriangles (theWithTriangulation);
}

//=======================================================================
//function : WriteShapeSection
//purpose  : Implements WriteShapeSection
//=======================================================================
void BinDrivers_DocumentStorageDriver::WriteShapeSection
                               (BinLDrivers_DocumentSection&   theSection,
                                Standard_OStream&              theOS)
{
  const Standard_Size aShapesSectionOffset = (Standard_Size) theOS.tellp();
  
  Handle(BinMDF_ADriver) aDriver;
  if (myDrivers->GetDriver(STANDARD_TYPE(TNaming_NamedShape), aDriver))
  {
    try { 
      OCC_CATCH_SIGNALS
      Handle(BinMNaming_NamedShapeDriver) aNamedShapeDriver =
        Handle(BinMNaming_NamedShapeDriver)::DownCast (aDriver);
      aNamedShapeDriver->WriteShapeSection (theOS);
    }
    catch(Standard_Failure const& anException) {
      TCollection_ExtendedString anErrorStr ("Error: ");
      TCollection_ExtendedString aStr = 
	anErrorStr + "BinDrivers_DocumentStorageDriver, Shape Section :";
      WriteMessage (aStr  + anException.GetMessageString());
    }
  }
   
  // Write the section info in the TOC.
  theSection.Write (theOS, aShapesSectionOffset);
}
