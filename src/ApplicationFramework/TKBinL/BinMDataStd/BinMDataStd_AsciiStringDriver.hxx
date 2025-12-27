// Created on: 2007-08-01
// Created by: Sergey ZARITCHNY
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

#ifndef _BinMDataStd_AsciiStringDriver_HeaderFile
#define _BinMDataStd_AsciiStringDriver_HeaderFile

#include <Standard.hxx>

#include <BinMDF_ADriver.hxx>
#include <BinObjMgt_RRelocationTable.hxx>
#include <Standard_Transient.hxx>
#include <NCollection_IndexedMap.hxx>
class Message_Messenger;
class TDF_Attribute;
class BinObjMgt_Persistent;

//! TDataStd_AsciiString attribute Driver.
class BinMDataStd_AsciiStringDriver : public BinMDF_ADriver
{

public:
  Standard_EXPORT BinMDataStd_AsciiStringDriver(const occ::handle<Message_Messenger>& theMessageDriver);

  Standard_EXPORT occ::handle<TDF_Attribute> NewEmpty() const override;

  //! persistent -> transient (retrieve)
  Standard_EXPORT bool
    Paste(const BinObjMgt_Persistent&  Source,
          const occ::handle<TDF_Attribute>& Target,
          BinObjMgt_RRelocationTable&  RelocTable) const override;

  //! transient -> persistent (store)
  Standard_EXPORT void Paste(const occ::handle<TDF_Attribute>& Source,
                             BinObjMgt_Persistent&        Target,
                             NCollection_IndexedMap<occ::handle<Standard_Transient>>&  RelocTable) const override;

  DEFINE_STANDARD_RTTIEXT(BinMDataStd_AsciiStringDriver, BinMDF_ADriver)

};

#endif // _BinMDataStd_AsciiStringDriver_HeaderFile
