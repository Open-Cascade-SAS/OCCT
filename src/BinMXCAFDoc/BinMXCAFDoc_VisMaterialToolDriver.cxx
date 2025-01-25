// Copyright (c) 2019 OPEN CASCADE SAS
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

#include <BinMXCAFDoc_VisMaterialToolDriver.hxx>

#include <XCAFDoc_VisMaterialTool.hxx>

IMPLEMENT_STANDARD_RTTIEXT(BinMXCAFDoc_VisMaterialToolDriver, BinMDF_ADriver)

//=================================================================================================

BinMXCAFDoc_VisMaterialToolDriver::BinMXCAFDoc_VisMaterialToolDriver(
  const Handle(Message_Messenger)& theMsgDriver)
    : BinMDF_ADriver(theMsgDriver, STANDARD_TYPE(XCAFDoc_VisMaterialTool)->Name())
{
  //
}

//=================================================================================================

Handle(TDF_Attribute) BinMXCAFDoc_VisMaterialToolDriver::NewEmpty() const
{
  return new XCAFDoc_VisMaterialTool();
}

//=================================================================================================

Standard_Boolean BinMXCAFDoc_VisMaterialToolDriver::Paste(const BinObjMgt_Persistent&,
                                                          const Handle(TDF_Attribute)&,
                                                          BinObjMgt_RRelocationTable&) const
{
  return Standard_True;
}

//=================================================================================================

void BinMXCAFDoc_VisMaterialToolDriver::Paste(const Handle(TDF_Attribute)&,
                                              BinObjMgt_Persistent&,
                                              BinObjMgt_SRelocationTable&) const
{
  //
}
