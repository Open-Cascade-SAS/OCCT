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

#ifndef _XmlMXCAFDoc_ClippingPlaneToolDriver_HeaderFile
#define _XmlMXCAFDoc_ClippingPlaneToolDriver_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <XmlMDF_ADriver.hxx>
#include <Standard_Boolean.hxx>
#include <XmlObjMgt_RRelocationTable.hxx>
#include <XmlObjMgt_SRelocationTable.hxx>
class CDM_MessageDriver;
class TDF_Attribute;
class XmlObjMgt_Persistent;


class XmlMXCAFDoc_ClippingPlaneToolDriver;
DEFINE_STANDARD_HANDLE(XmlMXCAFDoc_ClippingPlaneToolDriver, XmlMDF_ADriver)

//! Attribute Driver.
class XmlMXCAFDoc_ClippingPlaneToolDriver : public XmlMDF_ADriver
{
public:

  Standard_EXPORT XmlMXCAFDoc_ClippingPlaneToolDriver(const Handle(CDM_MessageDriver)& theMsgDriver);

  Standard_EXPORT virtual Handle(TDF_Attribute) NewEmpty() const Standard_OVERRIDE;

  Standard_EXPORT virtual Standard_Boolean Paste(const XmlObjMgt_Persistent& theSource, const Handle(TDF_Attribute)& theTarget, XmlObjMgt_RRelocationTable& theRelocTable) const Standard_OVERRIDE;

  Standard_EXPORT virtual void Paste(const Handle(TDF_Attribute)& theSource, XmlObjMgt_Persistent& theTarget, XmlObjMgt_SRelocationTable& theRelocTable) const Standard_OVERRIDE;

  DEFINE_STANDARD_RTTIEXT(XmlMXCAFDoc_ClippingPlaneToolDriver, XmlMDF_ADriver)

};
#endif // _XmlMXCAFDoc_ClippingPlaneToolDriver_HeaderFile
