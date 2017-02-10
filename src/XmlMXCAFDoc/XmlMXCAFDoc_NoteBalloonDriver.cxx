// Created on: 2017-08-10
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
#include <XCAFDoc_NoteBalloon.hxx>
#include <XmlMXCAFDoc_NoteBalloonDriver.hxx>
#include <XmlObjMgt_Persistent.hxx>

IMPLEMENT_STANDARD_RTTIEXT(XmlMXCAFDoc_NoteBalloonDriver, XmlMXCAFDoc_NoteCommentDriver)

//=======================================================================
//function :
//purpose  : 
//=======================================================================
XmlMXCAFDoc_NoteBalloonDriver::XmlMXCAFDoc_NoteBalloonDriver(const Handle(CDM_MessageDriver)& theMsgDriver)
  : XmlMXCAFDoc_NoteCommentDriver(theMsgDriver, STANDARD_TYPE(XCAFDoc_NoteBalloon)->Name())
{
}

//=======================================================================
//function :
//purpose  : 
//=======================================================================
Handle(TDF_Attribute) XmlMXCAFDoc_NoteBalloonDriver::NewEmpty() const
{
  return new XCAFDoc_NoteBalloon();
}

//=======================================================================
//function :
//purpose  : 
//=======================================================================
XmlMXCAFDoc_NoteBalloonDriver::XmlMXCAFDoc_NoteBalloonDriver(const Handle(CDM_MessageDriver)& theMsgDriver,
                                                             Standard_CString                 theName)
  : XmlMXCAFDoc_NoteCommentDriver(theMsgDriver, theName)
{

}
