// Created on: 2016-11-30
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


#include <BinMXCAFDoc_ClippingPlaneToolDriver.hxx>
#include <BinObjMgt_Persistent.hxx>
#include <CDM_MessageDriver.hxx>
#include <Standard_Type.hxx>
#include <TDF_Attribute.hxx>
#include <XCAFDoc_ClippingPlaneTool.hxx>

IMPLEMENT_STANDARD_RTTIEXT(BinMXCAFDoc_ClippingPlaneToolDriver, BinMDF_ADriver)

//=======================================================================
//function : 
//purpose  : 
//=======================================================================
BinMXCAFDoc_ClippingPlaneToolDriver::BinMXCAFDoc_ClippingPlaneToolDriver
  (const Handle(CDM_MessageDriver)& theMsgDriver)
  : BinMDF_ADriver(theMsgDriver, STANDARD_TYPE(XCAFDoc_ClippingPlaneTool)->Name())
{
}

//=======================================================================
//function : 
//purpose  : 
//=======================================================================
Handle(TDF_Attribute) BinMXCAFDoc_ClippingPlaneToolDriver::NewEmpty() const
{
  return new XCAFDoc_ClippingPlaneTool();
}

//=======================================================================
//function : 
//purpose  : 
//=======================================================================
Standard_Boolean BinMXCAFDoc_ClippingPlaneToolDriver::Paste
  (const BinObjMgt_Persistent& /*theSource*/,
  const Handle(TDF_Attribute)& /*theTarget*/,
  BinObjMgt_RRelocationTable& /*theRelocTable*/) const
{
  return Standard_True;
}

//=======================================================================
//function : 
//purpose  : 
//=======================================================================
void BinMXCAFDoc_ClippingPlaneToolDriver::Paste
  (const Handle(TDF_Attribute)& /*theSource*/,
   BinObjMgt_Persistent& /*theTarget*/,
   BinObjMgt_SRelocationTable& /*theRelocTable*/) const {
}
