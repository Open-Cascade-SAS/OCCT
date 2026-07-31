// Created on: 2017-02-14
// Created by: Sergey NIKONOV
// Copyright (c) 2008-2017 OPEN CASCADE SAS
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

#ifndef _XmlMXCAFDoc_NoteCommentDriver_HeaderFile
#define _XmlMXCAFDoc_NoteCommentDriver_HeaderFile

#include <XmlMXCAFDoc_NoteDriver.hxx>

//! Attribute Driver.
class XmlMXCAFDoc_NoteCommentDriver : public XmlMXCAFDoc_NoteDriver
{
public:
  Standard_EXPORT XmlMXCAFDoc_NoteCommentDriver(
    const occ::handle<Message_Messenger>& theMessageDriver);

  Standard_EXPORT occ::handle<TDF_Attribute> NewEmpty() const override;

  Standard_EXPORT bool Paste(const XmlObjMgt_Persistent&       theSource,
                             const occ::handle<TDF_Attribute>& theTarget,
                             XmlObjMgt_RRelocationTable&       theRelocTable) const override;

  Standard_EXPORT void Paste(const occ::handle<TDF_Attribute>& theSource,
                             XmlObjMgt_Persistent&             theTarget,
                             XmlObjMgt_SRelocationTable&       theRelocTable) const override;

  DEFINE_STANDARD_RTTIEXT(XmlMXCAFDoc_NoteCommentDriver, XmlMXCAFDoc_NoteDriver)

protected:
  XmlMXCAFDoc_NoteCommentDriver(const occ::handle<Message_Messenger>& theMsgDriver,
                                const char*                           theName);
};

#endif // _XmlMXCAFDoc_NoteCommentDriver_HeaderFile
