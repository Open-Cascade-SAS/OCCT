// Created on: 2001-07-09
// Created by: Julia DOROVSKIKH
// Copyright (c) 2001-2014 OPEN CASCADE SAS
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

#include <Message_Messenger.hxx>
#include <Standard_Type.hxx>
#include <TCollection_ExtendedString.hxx>
#include <TNaming_NamedShape.hxx>
#include <XmlDrivers.hxx>
#include <XmlDrivers_DocumentStorageDriver.hxx>
#include <XmlMDF_ADriverTable.hxx>
#include <XmlMNaming_NamedShapeDriver.hxx>
#include <XmlObjMgt_Element.hxx>

IMPLEMENT_STANDARD_RTTIEXT(XmlDrivers_DocumentStorageDriver, XmlLDrivers_DocumentStorageDriver)

//=================================================================================================

XmlDrivers_DocumentStorageDriver::XmlDrivers_DocumentStorageDriver(
  const TCollection_ExtendedString& theCopyright)
    : XmlLDrivers_DocumentStorageDriver(theCopyright)
{
}

//=================================================================================================

occ::handle<XmlMDF_ADriverTable> XmlDrivers_DocumentStorageDriver::AttributeDrivers(
  const occ::handle<Message_Messenger>& theMessageDriver)
{
  return XmlDrivers::AttributeDrivers(theMessageDriver);
}

//=======================================================================
// function : WriteShapeSection
// purpose  : Implements WriteShapeSection
//=======================================================================
bool XmlDrivers_DocumentStorageDriver::WriteShapeSection(
  XmlObjMgt_Element&           theElement,
  const TDocStd_FormatVersion  theStorageFormatVersion,
  const Message_ProgressRange& theRange)
{
  bool       isShape(false);
  occ::handle<XmlMDF_ADriver> aDriver;
  if (myDrivers->GetDriver(STANDARD_TYPE(TNaming_NamedShape), aDriver))
  {
    occ::handle<XmlMNaming_NamedShapeDriver> aNamedShapeDriver =
      occ::down_cast<XmlMNaming_NamedShapeDriver>(aDriver);
    aNamedShapeDriver->WriteShapeSection(theElement, theStorageFormatVersion, theRange);
    isShape = true;
  }
  return isShape;
}
