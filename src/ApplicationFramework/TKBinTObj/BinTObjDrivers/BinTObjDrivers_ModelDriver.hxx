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

#ifndef BinTObjDrivers_ModelDriver_HeaderFile
#define BinTObjDrivers_ModelDriver_HeaderFile

#include <BinMDF_ADriver.hxx>

class BinTObjDrivers_ModelDriver : public BinMDF_ADriver
{

public:
  Standard_EXPORT BinTObjDrivers_ModelDriver(const Handle(Message_Messenger)& theMessageDriver);
  // constructor

  Standard_EXPORT Handle(TDF_Attribute) NewEmpty() const Standard_OVERRIDE;
  // Creates a new attribute

  Standard_EXPORT Standard_Boolean
    Paste(const BinObjMgt_Persistent&  Source,
          const Handle(TDF_Attribute)& Target,
          BinObjMgt_RRelocationTable&  RelocTable) const Standard_OVERRIDE;
  // Translate the contents of <aSource> and put it
  // into <aTarget>, using the relocation table
  // <aRelocTable> to keep the sharings.
  // Set CurrentModel of TObj_Persistence into Target TObj_TModel
  // if its GUID and GUID stored in Source are same

  Standard_EXPORT void Paste(const Handle(TDF_Attribute)& Source,
                             BinObjMgt_Persistent&        Target,
                             BinObjMgt_SRelocationTable&  RelocTable) const Standard_OVERRIDE;
  // Translate the contents of <aSource> and put it
  // into <aTarget>, using the relocation table
  // <aRelocTable> to keep the sharings.
  // a Model is stored as its GUID

public:
  // CASCADE RTTI
  DEFINE_STANDARD_RTTIEXT(BinTObjDrivers_ModelDriver, BinMDF_ADriver)
};

// Define handle class
DEFINE_STANDARD_HANDLE(BinTObjDrivers_ModelDriver, BinMDF_ADriver)

#endif

#ifdef _MSC_VER
#pragma once
#endif
