// Created on: 2001-08-27
// Created by: Alexander GRIGORIEV
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

#include <XmlMDataStd.hxx>

#include <Message_Messenger.hxx>
#include <XmlMDataStd_AsciiStringDriver.hxx>
#include <XmlMDataStd_BooleanArrayDriver.hxx>
#include <XmlMDataStd_BooleanListDriver.hxx>
#include <XmlMDataStd_ByteArrayDriver.hxx>
#include <XmlMDataStd_ExpressionDriver.hxx>
#include <XmlMDataStd_ExtStringArrayDriver.hxx>
#include <XmlMDataStd_ExtStringListDriver.hxx>
#include <XmlMDataStd_IntegerArrayDriver.hxx>
#include <XmlMDataStd_IntegerDriver.hxx>
#include <XmlMDataStd_IntegerListDriver.hxx>
#include <XmlMDataStd_IntPackedMapDriver.hxx>
#include <XmlMDataStd_NamedDataDriver.hxx>
#include <XmlMDataStd_GenericExtStringDriver.hxx>
#include <XmlMDataStd_RealArrayDriver.hxx>
#include <XmlMDataStd_RealDriver.hxx>
#include <XmlMDataStd_RealListDriver.hxx>
#include <XmlMDataStd_ReferenceArrayDriver.hxx>
#include <XmlMDataStd_ReferenceListDriver.hxx>
#include <XmlMDataStd_GenericEmptyDriver.hxx>
#include <XmlMDataStd_TreeNodeDriver.hxx>
#include <XmlMDataStd_UAttributeDriver.hxx>
#include <XmlMDataStd_VariableDriver.hxx>
#include <XmlMDF_ADriverTable.hxx>

//=======================================================================
//function : AddDrivers
//purpose  : 
//=======================================================================
void XmlMDataStd::AddDrivers (const Handle(XmlMDF_ADriverTable)& aDriverTable,
                              const Handle(Message_Messenger)&   anMsgDrv)
{
  aDriverTable-> AddDriver (new XmlMDataStd_IntegerArrayDriver     (anMsgDrv));
  aDriverTable-> AddDriver (new XmlMDataStd_RealArrayDriver        (anMsgDrv));
  aDriverTable-> AddDriver (new XmlMDataStd_UAttributeDriver       (anMsgDrv));
  aDriverTable-> AddDriver (new XmlMDataStd_GenericExtStringDriver (anMsgDrv));
  aDriverTable-> AddDriver (new XmlMDataStd_IntegerDriver          (anMsgDrv));
  aDriverTable-> AddDriver (new XmlMDataStd_RealDriver             (anMsgDrv));
  aDriverTable-> AddDriver (new XmlMDataStd_VariableDriver         (anMsgDrv));
  aDriverTable-> AddDriver (new XmlMDataStd_ExpressionDriver       (anMsgDrv));
  aDriverTable-> AddDriver (new XmlMDataStd_TreeNodeDriver         (anMsgDrv));
  aDriverTable-> AddDriver (new XmlMDataStd_ExtStringArrayDriver   (anMsgDrv));
  aDriverTable-> AddDriver (new XmlMDataStd_GenericEmptyDriver     (anMsgDrv));
  aDriverTable-> AddDriver (new XmlMDataStd_IntegerListDriver      (anMsgDrv));
  aDriverTable-> AddDriver (new XmlMDataStd_RealListDriver         (anMsgDrv));
  aDriverTable-> AddDriver (new XmlMDataStd_ExtStringListDriver    (anMsgDrv));
  aDriverTable-> AddDriver (new XmlMDataStd_BooleanListDriver      (anMsgDrv));
  aDriverTable-> AddDriver (new XmlMDataStd_ReferenceListDriver    (anMsgDrv));
  aDriverTable-> AddDriver (new XmlMDataStd_BooleanArrayDriver     (anMsgDrv));
  aDriverTable-> AddDriver (new XmlMDataStd_ReferenceArrayDriver   (anMsgDrv));
  aDriverTable-> AddDriver (new XmlMDataStd_ByteArrayDriver        (anMsgDrv));
  aDriverTable-> AddDriver (new XmlMDataStd_NamedDataDriver        (anMsgDrv));
  aDriverTable-> AddDriver (new XmlMDataStd_AsciiStringDriver      (anMsgDrv));
  aDriverTable-> AddDriver (new XmlMDataStd_IntPackedMapDriver     (anMsgDrv));
}
