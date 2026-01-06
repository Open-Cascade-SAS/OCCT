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
#include <BinMDF_ADriverTable.hxx>
#include <BinMNaming_NamedShapeDriver.hxx>
#include <Message_Messenger.hxx>
#include <Standard_ErrorHandler.hxx>
#include <Standard_NotImplemented.hxx>
#include <Standard_Type.hxx>
#include <TNaming_NamedShape.hxx>

IMPLEMENT_STANDARD_RTTIEXT(BinDrivers_DocumentStorageDriver, BinLDrivers_DocumentStorageDriver)

//=================================================================================================

BinDrivers_DocumentStorageDriver::BinDrivers_DocumentStorageDriver() = default;

//=================================================================================================

occ::handle<BinMDF_ADriverTable> BinDrivers_DocumentStorageDriver::AttributeDrivers(
  const occ::handle<Message_Messenger>& theMessageDriver)
{
  return BinDrivers::AttributeDrivers(theMessageDriver);
}

//=================================================================================================

bool BinDrivers_DocumentStorageDriver::IsWithTriangles() const
{
  if (myDrivers.IsNull())
  {
    return false;
  }

  occ::handle<BinMDF_ADriver> aDriver;
  myDrivers->GetDriver(STANDARD_TYPE(TNaming_NamedShape), aDriver);
  occ::handle<BinMNaming_NamedShapeDriver> aShapesDriver =
    occ::down_cast<BinMNaming_NamedShapeDriver>(aDriver);
  return !aShapesDriver.IsNull() && aShapesDriver->IsWithTriangles();
}

//=================================================================================================

void BinDrivers_DocumentStorageDriver::SetWithTriangles(
  const occ::handle<Message_Messenger>& theMessageDriver,
  const bool                            theWithTriangulation)
{
  if (myDrivers.IsNull())
  {
    myDrivers = AttributeDrivers(theMessageDriver);
  }
  if (myDrivers.IsNull())
  {
    return;
  }

  occ::handle<BinMDF_ADriver> aDriver;
  myDrivers->GetDriver(STANDARD_TYPE(TNaming_NamedShape), aDriver);
  occ::handle<BinMNaming_NamedShapeDriver> aShapesDriver =
    occ::down_cast<BinMNaming_NamedShapeDriver>(aDriver);
  if (aShapesDriver.IsNull())
  {
    throw Standard_NotImplemented("Internal Error - TNaming_NamedShape is not found!");
  }

  aShapesDriver->SetWithTriangles(theWithTriangulation);
}

void BinDrivers_DocumentStorageDriver::EnableQuickPartWriting(
  const occ::handle<Message_Messenger>& theMessageDriver,
  const bool                            theValue)
{
  if (myDrivers.IsNull())
  {
    myDrivers = AttributeDrivers(theMessageDriver);
  }
  if (myDrivers.IsNull())
  {
    return;
  }

  occ::handle<BinMDF_ADriver> aDriver;
  myDrivers->GetDriver(STANDARD_TYPE(TNaming_NamedShape), aDriver);
  occ::handle<BinMNaming_NamedShapeDriver> aShapesDriver =
    occ::down_cast<BinMNaming_NamedShapeDriver>(aDriver);
  if (aShapesDriver.IsNull())
  {
    throw Standard_NotImplemented("Internal Error - TNaming_NamedShape is not found!");
  }

  aShapesDriver->EnableQuickPart(theValue);
}

//=================================================================================================

void BinDrivers_DocumentStorageDriver::Clear()
{
  // Clear NamedShape driver
  occ::handle<BinMDF_ADriver> aDriver;
  if (myDrivers->GetDriver(STANDARD_TYPE(TNaming_NamedShape), aDriver))
  {
    occ::handle<BinMNaming_NamedShapeDriver> aNamedShapeDriver =
      occ::down_cast<BinMNaming_NamedShapeDriver>(aDriver);
    aNamedShapeDriver->Clear();
  }
  BinLDrivers_DocumentStorageDriver::Clear();
}

//=================================================================================================

bool BinDrivers_DocumentStorageDriver::IsWithNormals() const
{
  if (myDrivers.IsNull())
  {
    return false;
  }

  occ::handle<BinMDF_ADriver> aDriver;
  myDrivers->GetDriver(STANDARD_TYPE(TNaming_NamedShape), aDriver);
  occ::handle<BinMNaming_NamedShapeDriver> aShapesDriver =
    occ::down_cast<BinMNaming_NamedShapeDriver>(aDriver);
  return !aShapesDriver.IsNull() && aShapesDriver->IsWithNormals();
}

//=================================================================================================

void BinDrivers_DocumentStorageDriver::SetWithNormals(
  const occ::handle<Message_Messenger>& theMessageDriver,
  const bool                            theWithNormals)
{
  if (myDrivers.IsNull())
  {
    myDrivers = AttributeDrivers(theMessageDriver);
  }
  if (myDrivers.IsNull())
  {
    return;
  }

  occ::handle<BinMDF_ADriver> aDriver;
  myDrivers->GetDriver(STANDARD_TYPE(TNaming_NamedShape), aDriver);
  occ::handle<BinMNaming_NamedShapeDriver> aShapesDriver =
    occ::down_cast<BinMNaming_NamedShapeDriver>(aDriver);
  if (aShapesDriver.IsNull())
  {
    throw Standard_NotImplemented("Internal Error - TNaming_NamedShape is not found!");
  }

  aShapesDriver->SetWithNormals(theWithNormals);
}

//=======================================================================
// function : WriteShapeSection
// purpose  : Implements WriteShapeSection
//=======================================================================
void BinDrivers_DocumentStorageDriver::WriteShapeSection(BinLDrivers_DocumentSection& theSection,
                                                         Standard_OStream&            theOS,
                                                         const TDocStd_FormatVersion  theDocVer,
                                                         const Message_ProgressRange& theRange)
{
  const size_t aShapesSectionOffset = (size_t)theOS.tellp();

  occ::handle<BinMDF_ADriver> aDriver;
  if (myDrivers->GetDriver(STANDARD_TYPE(TNaming_NamedShape), aDriver))
  {
    try
    {
      OCC_CATCH_SIGNALS
      occ::handle<BinMNaming_NamedShapeDriver> aNamedShapeDriver =
        occ::down_cast<BinMNaming_NamedShapeDriver>(aDriver);
      aNamedShapeDriver->WriteShapeSection(theOS, theDocVer, theRange);
    }
    catch (Standard_Failure const& anException)
    {
      TCollection_ExtendedString anErrorStr("BinDrivers_DocumentStorageDriver, Shape Section :");
      myMsgDriver->Send(anErrorStr + anException.what(), Message_Fail);
    }
  }

  // Write the section info in the TOC.
  theSection.Write(theOS, aShapesSectionOffset, theDocVer);
}
