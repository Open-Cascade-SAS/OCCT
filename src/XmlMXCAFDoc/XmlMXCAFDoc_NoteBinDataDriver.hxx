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

#ifndef _XmlMXCAFDoc_NoteBinDataDriver_HeaderFile
#define _XmlMXCAFDoc_NoteBinDataDriver_HeaderFile

#include <XmlMXCAFDoc_NoteDriver.hxx>

class XmlMXCAFDoc_NoteBinDataDriver;
DEFINE_STANDARD_HANDLE(XmlMXCAFDoc_NoteBinDataDriver, XmlMXCAFDoc_NoteDriver)

//! Attribute Driver.
class XmlMXCAFDoc_NoteBinDataDriver : public XmlMXCAFDoc_NoteDriver
{
public:
  Standard_EXPORT XmlMXCAFDoc_NoteBinDataDriver(const Handle(Message_Messenger)& theMessageDriver);

  Standard_EXPORT Handle(TDF_Attribute) NewEmpty() const Standard_OVERRIDE;

  Standard_EXPORT Standard_Boolean
    Paste(const XmlObjMgt_Persistent&  theSource,
          const Handle(TDF_Attribute)& theTarget,
          XmlObjMgt_RRelocationTable&  theRelocTable) const Standard_OVERRIDE;

  Standard_EXPORT void Paste(const Handle(TDF_Attribute)& theSource,
                             XmlObjMgt_Persistent&        theTarget,
                             XmlObjMgt_SRelocationTable&  theRelocTable) const Standard_OVERRIDE;

  DEFINE_STANDARD_RTTIEXT(XmlMXCAFDoc_NoteBinDataDriver, XmlMXCAFDoc_NoteDriver)
};

#endif // _XmlMXCAFDoc_NoteBinDataDriver_HeaderFile
