// Created on: 2004-11-24
// Created by: Edward AGAPOV
// Copyright (c) 2004-2014 OPEN CASCADE SAS
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

// The original implementation Copyright: (C) RINA S.p.A

#ifndef XmlTObjDrivers_ModelDriver_HeaderFile
#define XmlTObjDrivers_ModelDriver_HeaderFile

#include <XmlMDF_ADriver.hxx>

class XmlTObjDrivers_ModelDriver : public XmlMDF_ADriver
{

public:
  Standard_EXPORT XmlTObjDrivers_ModelDriver(
    const occ::handle<Message_Messenger>& theMessageDriver);
  // constructor

  Standard_EXPORT occ::handle<TDF_Attribute> NewEmpty() const override;
  // Creates a new attribute

  Standard_EXPORT bool Paste(const XmlObjMgt_Persistent&       Source,
                             const occ::handle<TDF_Attribute>& Target,
                             XmlObjMgt_RRelocationTable&       RelocTable) const override;
  // Translate the contents of <aSource> and put it
  // into <aTarget>, using the relocation table
  // <aRelocTable> to keep the sharings.
  // Set CurrentModel of TObj_Persistence into Target TObj_TModel
  // if its GUID and GUID stored in Source are same

  Standard_EXPORT void Paste(const occ::handle<TDF_Attribute>& Source,
                             XmlObjMgt_Persistent&             Target,
                             XmlObjMgt_SRelocationTable&       RelocTable) const override;
  // Translate the contents of <aSource> and put it
  // into <aTarget>, using the relocation table
  // <aRelocTable> to keep the sharings.
  // a Model is stored as its GUID

public:
  // CASCADE RTTI
  DEFINE_STANDARD_RTTIEXT(XmlTObjDrivers_ModelDriver, XmlMDF_ADriver)
};

// Define handle class
#endif

#ifdef _MSC_VER
#pragma once
#endif
