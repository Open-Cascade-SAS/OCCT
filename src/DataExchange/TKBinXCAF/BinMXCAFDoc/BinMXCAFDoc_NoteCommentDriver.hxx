// Created on: 2017-02-13
// Created by: Sergey NIKONOV
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

#ifndef _BinMXCAFDoc_NoteCommentDriver_HeaderFile
#define _BinMXCAFDoc_NoteCommentDriver_HeaderFile

#include <BinMXCAFDoc_NoteDriver.hxx>

class BinMXCAFDoc_NoteCommentDriver : public BinMXCAFDoc_NoteDriver
{
public:
  Standard_EXPORT BinMXCAFDoc_NoteCommentDriver(const occ::handle<Message_Messenger>& theMsgDriver);

  Standard_EXPORT occ::handle<TDF_Attribute> NewEmpty() const override;

  Standard_EXPORT bool
    Paste(const BinObjMgt_Persistent&  theSource,
          const occ::handle<TDF_Attribute>& theTarget,
          BinObjMgt_RRelocationTable&  theRelocTable) const override;

  Standard_EXPORT void Paste(const occ::handle<TDF_Attribute>& theSource,
                             BinObjMgt_Persistent&        theTarget,
                             NCollection_IndexedMap<occ::handle<Standard_Transient>>&  theRelocTable) const override;

  DEFINE_STANDARD_RTTIEXT(BinMXCAFDoc_NoteCommentDriver, BinMXCAFDoc_NoteDriver)

protected:
  BinMXCAFDoc_NoteCommentDriver(const occ::handle<Message_Messenger>& theMsgDriver,
                                const char*                 theName);
};

#endif // _BinMXCAFDoc_NoteCommentDriver_HeaderFile
