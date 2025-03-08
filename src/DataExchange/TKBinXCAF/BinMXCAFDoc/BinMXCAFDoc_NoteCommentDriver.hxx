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

class BinMXCAFDoc_NoteCommentDriver;
DEFINE_STANDARD_HANDLE(BinMXCAFDoc_NoteCommentDriver, BinMXCAFDoc_NoteDriver)

class BinMXCAFDoc_NoteCommentDriver : public BinMXCAFDoc_NoteDriver
{
public:
  Standard_EXPORT BinMXCAFDoc_NoteCommentDriver(const Handle(Message_Messenger)& theMsgDriver);

  Standard_EXPORT Handle(TDF_Attribute) NewEmpty() const Standard_OVERRIDE;

  Standard_EXPORT Standard_Boolean
    Paste(const BinObjMgt_Persistent&  theSource,
          const Handle(TDF_Attribute)& theTarget,
          BinObjMgt_RRelocationTable&  theRelocTable) const Standard_OVERRIDE;

  Standard_EXPORT void Paste(const Handle(TDF_Attribute)& theSource,
                             BinObjMgt_Persistent&        theTarget,
                             BinObjMgt_SRelocationTable&  theRelocTable) const Standard_OVERRIDE;

  DEFINE_STANDARD_RTTIEXT(BinMXCAFDoc_NoteCommentDriver, BinMXCAFDoc_NoteDriver)

protected:
  BinMXCAFDoc_NoteCommentDriver(const Handle(Message_Messenger)& theMsgDriver,
                                Standard_CString                 theName);
};

#endif // _BinMXCAFDoc_NoteCommentDriver_HeaderFile
