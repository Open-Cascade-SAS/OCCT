// Created on: 2007-03-30
// Created by: Michael SAZONOV
// Copyright (c) 2007-2014 OPEN CASCADE SAS
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

#ifndef BinTObjDrivers_IntSparseArrayDriver_HeaderFile
#define BinTObjDrivers_IntSparseArrayDriver_HeaderFile

#include <BinMDF_ADriver.hxx>

class BinTObjDrivers_IntSparseArrayDriver : public BinMDF_ADriver
{

public:
  Standard_EXPORT BinTObjDrivers_IntSparseArrayDriver(
    const occ::handle<Message_Messenger>& theMessageDriver);
  // constructor

  Standard_EXPORT occ::handle<TDF_Attribute> NewEmpty() const override;
  // Creates a new attribute

  Standard_EXPORT bool Paste(const BinObjMgt_Persistent&       theSource,
                             const occ::handle<TDF_Attribute>& theTarget,
                             BinObjMgt_RRelocationTable&       theRelocTable) const override;
  // Translate the contents of <theSource> and put it
  // into <theTarget>

  Standard_EXPORT void Paste(
    const occ::handle<TDF_Attribute>&                        theSource,
    BinObjMgt_Persistent&                                    theTarget,
    NCollection_IndexedMap<occ::handle<Standard_Transient>>& theRelocTable) const override;
  // Translate the contents of <aSource> and put it
  // into <aTarget>

public:
  // CASCADE RTTI
  DEFINE_STANDARD_RTTIEXT(BinTObjDrivers_IntSparseArrayDriver, BinMDF_ADriver)
};

// Define handle class
#endif

#ifdef _MSC_VER
#pragma once
#endif
