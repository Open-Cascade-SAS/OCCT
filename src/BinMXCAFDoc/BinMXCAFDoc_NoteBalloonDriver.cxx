// Created on: 2017-08-10
// Created by: Eugeny NIKONOV
// Copyright (c) 2005-2017 OPEN CASCADE SAS
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

#include <BinObjMgt_Persistent.hxx>
#include <CDM_MessageDriver.hxx>
#include <Standard_Type.hxx>
#include <TDF_Attribute.hxx>
#include <BinMXCAFDoc_NoteBalloonDriver.hxx>
#include <XCAFDoc_NoteBalloon.hxx>

IMPLEMENT_STANDARD_RTTIEXT(BinMXCAFDoc_NoteBalloonDriver, BinMXCAFDoc_NoteCommentDriver)

//=======================================================================
//function :
//purpose  : 
//=======================================================================
BinMXCAFDoc_NoteBalloonDriver::BinMXCAFDoc_NoteBalloonDriver(const Handle(CDM_MessageDriver)& theMsgDriver)
  : BinMXCAFDoc_NoteCommentDriver(theMsgDriver, STANDARD_TYPE(XCAFDoc_NoteBalloon)->Name())
{
}

//=======================================================================
//function :
//purpose  : 
//=======================================================================
Handle(TDF_Attribute) BinMXCAFDoc_NoteBalloonDriver::NewEmpty() const
{
  return new XCAFDoc_NoteBalloon();
}

//=======================================================================
//function :
//purpose  : 
//=======================================================================
BinMXCAFDoc_NoteBalloonDriver::BinMXCAFDoc_NoteBalloonDriver(const Handle(CDM_MessageDriver)& theMsgDriver,
                                                             Standard_CString                 theName)
  : BinMXCAFDoc_NoteCommentDriver(theMsgDriver, theName)
{

}
