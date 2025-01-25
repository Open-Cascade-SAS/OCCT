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
#include <TNaming_NamedShape.hxx>
#include <XmlDrivers.hxx>
#include <XmlDrivers_DocumentRetrievalDriver.hxx>
#include <XmlMDF_ADriverTable.hxx>
#include <XmlMNaming_NamedShapeDriver.hxx>
#include <XmlObjMgt_Element.hxx>

IMPLEMENT_STANDARD_RTTIEXT(XmlDrivers_DocumentRetrievalDriver, XmlLDrivers_DocumentRetrievalDriver)

//=================================================================================================

XmlDrivers_DocumentRetrievalDriver::XmlDrivers_DocumentRetrievalDriver() {}

//=================================================================================================

Handle(XmlMDF_ADriverTable) XmlDrivers_DocumentRetrievalDriver::AttributeDrivers(
  const Handle(Message_Messenger)& theMessageDriver)
{
  return XmlDrivers::AttributeDrivers(theMessageDriver);
}

//=======================================================================
// function : ReadShapeSection
// purpose  : Implementation of ReadShapeSection
//=======================================================================
Handle(XmlMDF_ADriver) XmlDrivers_DocumentRetrievalDriver::ReadShapeSection(
  const XmlObjMgt_Element&         theElement,
  const Handle(Message_Messenger)& theMsgDriver,
  const Message_ProgressRange&     theRange)
{
  if (myDrivers.IsNull())
    myDrivers = AttributeDrivers(theMsgDriver);
  Handle(XmlMDF_ADriver) aDriver;
  if (myDrivers->GetDriver(STANDARD_TYPE(TNaming_NamedShape), aDriver))
  {
    Handle(XmlMNaming_NamedShapeDriver) aNamedShapeDriver =
      Handle(XmlMNaming_NamedShapeDriver)::DownCast(aDriver);
    aNamedShapeDriver->ReadShapeSection(theElement, theRange);
  }
  return aDriver;
}

//=======================================================================
// function : ShapeSetCleaning
// purpose  : definition of ShapeSetCleaning
//=======================================================================
void XmlDrivers_DocumentRetrievalDriver::ShapeSetCleaning(const Handle(XmlMDF_ADriver)& theDriver)
{
  Handle(XmlMNaming_NamedShapeDriver) aNamedShapeDriver =
    Handle(XmlMNaming_NamedShapeDriver)::DownCast(theDriver);
  if (aNamedShapeDriver.IsNull() == Standard_False)
    aNamedShapeDriver->Clear();
}
