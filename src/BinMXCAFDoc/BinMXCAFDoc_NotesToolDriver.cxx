// Created on: 2017-02-10
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
#include <BinMXCAFDoc_NotesToolDriver.hxx>
#include <XCAFDoc_NotesTool.hxx>

IMPLEMENT_STANDARD_RTTIEXT(BinMXCAFDoc_NotesToolDriver, BinMDF_ADriver)

//=======================================================================
//function :
//purpose  : 
//=======================================================================
BinMXCAFDoc_NotesToolDriver::BinMXCAFDoc_NotesToolDriver(const Handle(CDM_MessageDriver)& theMsgDriver)
  : BinMDF_ADriver(theMsgDriver, STANDARD_TYPE(XCAFDoc_NotesTool)->Name())
{
}

//=======================================================================
//function :
//purpose  : 
//=======================================================================
Handle(TDF_Attribute) BinMXCAFDoc_NotesToolDriver::NewEmpty() const 
{
  return new XCAFDoc_NotesTool();
}

//=======================================================================
//function :
//purpose  : 
//=======================================================================
Standard_Boolean BinMXCAFDoc_NotesToolDriver::Paste(const BinObjMgt_Persistent&  /*theSource*/,
                                                    const Handle(TDF_Attribute)& /*theTarget*/,
                                                    BinObjMgt_RRelocationTable&  /*theRelocTable*/) const
{
  return Standard_True;
}

//=======================================================================
//function :
//purpose  : 
//=======================================================================
void BinMXCAFDoc_NotesToolDriver::Paste(const Handle(TDF_Attribute)& /*theSource*/,
					                              BinObjMgt_Persistent&        /*theTarget*/,
					                              BinObjMgt_SRelocationTable&  /*theRelocTable*/) const
{
}
