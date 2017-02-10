// Created on: 2017-02-14
// Created by: Sergey NIKONOV
// Copyright (c) 2008-2017 OPEN CASCADE SAS
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

#include <CDM_MessageDriver.hxx>
#include <Standard_Type.hxx>
#include <TDF_Attribute.hxx>
#include <XCAFDoc_NotesTool.hxx>
#include <XmlMXCAFDoc_NotesToolDriver.hxx>
#include <XmlObjMgt_Persistent.hxx>

IMPLEMENT_STANDARD_RTTIEXT(XmlMXCAFDoc_NotesToolDriver, XmlMDF_ADriver)

//=======================================================================
//function :
//purpose  : 
//=======================================================================
XmlMXCAFDoc_NotesToolDriver::XmlMXCAFDoc_NotesToolDriver(const Handle(CDM_MessageDriver)& theMsgDriver)
  : XmlMDF_ADriver(theMsgDriver, STANDARD_TYPE(XCAFDoc_NotesTool)->Name())
{
}

//=======================================================================
//function :
//purpose  : 
//=======================================================================
Handle(TDF_Attribute) XmlMXCAFDoc_NotesToolDriver::NewEmpty() const
{
  return new XCAFDoc_NotesTool();
}

//=======================================================================
//function :
//purpose  : 
//=======================================================================
Standard_Boolean XmlMXCAFDoc_NotesToolDriver::Paste(const XmlObjMgt_Persistent&  /*theSource*/,
                                                    const Handle(TDF_Attribute)& /*theTarget*/,
                                                    XmlObjMgt_RRelocationTable&  /*theRelocTable*/) const
{
  return Standard_True;
}

//=======================================================================
//function :
//purpose  : 
//=======================================================================
void XmlMXCAFDoc_NotesToolDriver::Paste(const Handle(TDF_Attribute)& /*theSource*/,
                                        XmlObjMgt_Persistent&        /*theTarget*/,
                                        XmlObjMgt_SRelocationTable&  /*theRelocTable*/) const
{
}
