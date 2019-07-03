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

#include <XmlMXCAFDoc_VisMaterialToolDriver.hxx>

#include <Message_Messenger.hxx>
#include <XCAFDoc_VisMaterialTool.hxx>
#include <XmlObjMgt_Persistent.hxx>

IMPLEMENT_STANDARD_RTTIEXT(XmlMXCAFDoc_VisMaterialToolDriver, XmlMDF_ADriver)

//=======================================================================
//function : XmlMXCAFDoc_VisMaterialToolDriver
//purpose  :
//=======================================================================
XmlMXCAFDoc_VisMaterialToolDriver::XmlMXCAFDoc_VisMaterialToolDriver (const Handle(Message_Messenger)& theMsgDriver)
: XmlMDF_ADriver (theMsgDriver, "xcaf", "VisMaterialTool")
{
  //
}

//=======================================================================
//function : NewEmpty
//purpose  :
//=======================================================================
Handle(TDF_Attribute) XmlMXCAFDoc_VisMaterialToolDriver::NewEmpty() const
{
  return new XCAFDoc_VisMaterialTool();
}

//=======================================================================
//function : Paste
//purpose  :
//=======================================================================
Standard_Boolean XmlMXCAFDoc_VisMaterialToolDriver::Paste (const XmlObjMgt_Persistent& ,
                                                           const Handle(TDF_Attribute)& ,
                                                           XmlObjMgt_RRelocationTable& ) const
{
  return Standard_True;
}

//=======================================================================
//function : Paste
//purpose  :
//=======================================================================
void XmlMXCAFDoc_VisMaterialToolDriver::Paste (const Handle(TDF_Attribute)& ,
                                               XmlObjMgt_Persistent& ,
                                               XmlObjMgt_SRelocationTable& ) const
{
  //
}
