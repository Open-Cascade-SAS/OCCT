// Created on: 2016-10-24
// Created by: Irina KRYLOVA
// Copyright (c) 2016 OPEN CASCADE SAS
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


#include <BinMXCAFDoc_ViewDriver.hxx>
#include <BinObjMgt_Persistent.hxx>
#include <CDM_MessageDriver.hxx>
#include <Standard_Type.hxx>
#include <TDF_Attribute.hxx>
#include <XCAFDoc_View.hxx>

IMPLEMENT_STANDARD_RTTIEXT(BinMXCAFDoc_ViewDriver, BinMDF_ADriver)

//=======================================================================
//function : Constructor
//purpose  : 
//=======================================================================
BinMXCAFDoc_ViewDriver::BinMXCAFDoc_ViewDriver (const Handle(CDM_MessageDriver)& theMsgDriver)
: BinMDF_ADriver(theMsgDriver, STANDARD_TYPE(XCAFDoc_View)->Name())
{
}

//=======================================================================
//function : NewEmpty
//purpose  : 
//=======================================================================
Handle(TDF_Attribute) BinMXCAFDoc_ViewDriver::NewEmpty() const
{
  return new XCAFDoc_View();
}

//=======================================================================
//function : Paste
//purpose  : 
//=======================================================================
Standard_Boolean BinMXCAFDoc_ViewDriver::Paste (const BinObjMgt_Persistent&  /*theSource*/,
                                                const Handle(TDF_Attribute)& /*theTarget*/,
                                                BinObjMgt_RRelocationTable&  /*theRelocTable*/) const
{
  return Standard_True;
}

//=======================================================================
//function : Paste
//purpose  : 
//=======================================================================
void BinMXCAFDoc_ViewDriver::Paste (const Handle(TDF_Attribute)&  /*theSource*/,
                                    BinObjMgt_Persistent&         /*theTarget*/,
                                    BinObjMgt_SRelocationTable&   /*theRelocTable*/) const
{
}
