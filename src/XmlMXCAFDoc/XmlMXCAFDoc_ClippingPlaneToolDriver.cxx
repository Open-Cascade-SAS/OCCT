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


#include <CDM_MessageDriver.hxx>
#include <Standard_Type.hxx>
#include <TDF_Attribute.hxx>
#include <XCAFDoc_ClippingPlaneTool.hxx>
#include <XmlMXCAFDoc_ClippingPlaneToolDriver.hxx>
#include <XmlObjMgt_Persistent.hxx>

IMPLEMENT_STANDARD_RTTIEXT(XmlMXCAFDoc_ClippingPlaneToolDriver,XmlMDF_ADriver)

//=======================================================================
//function :
//purpose  : 
//=======================================================================
XmlMXCAFDoc_ClippingPlaneToolDriver::XmlMXCAFDoc_ClippingPlaneToolDriver
  (const Handle(CDM_MessageDriver)& theMsgDriver)
: XmlMDF_ADriver (theMsgDriver, "xcaf", "ClippingPlaneTool")
{
}

//=======================================================================
//function :
//purpose  : 
//=======================================================================
Handle(TDF_Attribute) XmlMXCAFDoc_ClippingPlaneToolDriver::NewEmpty() const
{
  return new XCAFDoc_ClippingPlaneTool();
}

//=======================================================================
//function :
//purpose  : 
//=======================================================================
Standard_Boolean XmlMXCAFDoc_ClippingPlaneToolDriver::Paste(const XmlObjMgt_Persistent& ,
                                                     const Handle(TDF_Attribute)& ,
                                                     XmlObjMgt_RRelocationTable& ) const
{
  return Standard_True;
}

//=======================================================================
//function :
//purpose  : 
//=======================================================================
void XmlMXCAFDoc_ClippingPlaneToolDriver::Paste(const Handle(TDF_Attribute)& ,
                                         XmlObjMgt_Persistent& ,
                                         XmlObjMgt_SRelocationTable& ) const
{
}
