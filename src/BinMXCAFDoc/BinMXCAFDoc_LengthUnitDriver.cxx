// Copyright (c) 2021 OPEN CASCADE SAS
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

#include <BinMXCAFDoc_LengthUnitDriver.hxx>
#include <BinObjMgt_Persistent.hxx>
#include <Message_Messenger.hxx>
#include <Standard_Type.hxx>
#include <TDF_Attribute.hxx>
#include <XCAFDoc_LengthUnit.hxx>

IMPLEMENT_STANDARD_RTTIEXT(BinMXCAFDoc_LengthUnitDriver, BinMDF_ADriver)

//=======================================================================
//function : BinMXCAFDoc_LengthUnitDriver
//purpose  : Constructor
//=======================================================================
BinMXCAFDoc_LengthUnitDriver::BinMXCAFDoc_LengthUnitDriver(const Handle(Message_Messenger)& theMsgDriver)
     : BinMDF_ADriver(theMsgDriver, STANDARD_TYPE(XCAFDoc_LengthUnit)->Name()) {
}

//=======================================================================
//function : NewEmpty
//purpose  :
//=======================================================================
Handle(TDF_Attribute) BinMXCAFDoc_LengthUnitDriver::NewEmpty() const {
  return new XCAFDoc_LengthUnit();
}

//=======================================================================
//function : Paste
//purpose  :
//=======================================================================
Standard_Boolean BinMXCAFDoc_LengthUnitDriver::Paste(const BinObjMgt_Persistent& theSource,
                                                     const Handle(TDF_Attribute)& theTarget,
                                                     BinObjMgt_RRelocationTable& theRelocTable) const 
{
  (void)theRelocTable;
  Handle(XCAFDoc_LengthUnit) anAtt = Handle(XCAFDoc_LengthUnit)::DownCast(theTarget);
  TCollection_AsciiString aName;
  Standard_Real aScaleFactor = 1.;
  Standard_Boolean isOk = theSource >> aName >> aScaleFactor;
  if(isOk) {
    anAtt->Set(aName, aScaleFactor);
  }
  return isOk;
}

//=======================================================================
//function : Paste
//purpose  :
//=======================================================================
void BinMXCAFDoc_LengthUnitDriver::Paste(const Handle(TDF_Attribute)& theSource,
                                         BinObjMgt_Persistent& theTarget,
                                         BinObjMgt_SRelocationTable& theRelocTable) const
{
  (void)theRelocTable;
  Handle(XCAFDoc_LengthUnit) anAtt = Handle(XCAFDoc_LengthUnit)::DownCast(theSource);
  theTarget << anAtt->GetUnitName() << anAtt->GetUnitValue();
}
