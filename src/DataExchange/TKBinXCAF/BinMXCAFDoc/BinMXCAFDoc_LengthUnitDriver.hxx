// Copyright (c) 2021 OPEN CASCADE SAS
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

#ifndef _BinMXCAFDoc_LengthUnitDriver_HeaderFile
#define _BinMXCAFDoc_LengthUnitDriver_HeaderFile

#include <Standard.hxx>

#include <BinMDF_ADriver.hxx>
#include <BinObjMgt_RRelocationTable.hxx>
#include <Standard_Transient.hxx>
#include <NCollection_IndexedMap.hxx>
class Message_Messenger;
class TDF_Attribute;
class BinObjMgt_Persistent;

//! Attribute Driver.
class BinMXCAFDoc_LengthUnitDriver : public BinMDF_ADriver
{

public:
  Standard_EXPORT BinMXCAFDoc_LengthUnitDriver(const occ::handle<Message_Messenger>& theMsgDriver);

  Standard_EXPORT virtual occ::handle<TDF_Attribute> NewEmpty() const override;

  Standard_EXPORT virtual bool Paste(const BinObjMgt_Persistent&       theSource,
                                     const occ::handle<TDF_Attribute>& theTarget,
                                     BinObjMgt_RRelocationTable& theRelocTable) const override;

  Standard_EXPORT virtual void Paste(
    const occ::handle<TDF_Attribute>&                        theSource,
    BinObjMgt_Persistent&                                    theTarget,
    NCollection_IndexedMap<occ::handle<Standard_Transient>>& theRelocTable) const override;

  DEFINE_STANDARD_RTTIEXT(BinMXCAFDoc_LengthUnitDriver, BinMDF_ADriver)
};

#endif // _BinMXCAFDoc_LengthUnitDriver_HeaderFile
