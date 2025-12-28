// Created on: 2007-08-22
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

#ifndef _XmlMDataStd_IntPackedMapDriver_HeaderFile
#define _XmlMDataStd_IntPackedMapDriver_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <XmlMDF_ADriver.hxx>
#include <XmlObjMgt_RRelocationTable.hxx>
#include <XmlObjMgt_SRelocationTable.hxx>
class Message_Messenger;
class TDF_Attribute;
class XmlObjMgt_Persistent;

//! TDataStd_IntPackedMap attribute Driver.
class XmlMDataStd_IntPackedMapDriver : public XmlMDF_ADriver
{

public:
  Standard_EXPORT XmlMDataStd_IntPackedMapDriver(
    const occ::handle<Message_Messenger>& theMessageDriver);

  Standard_EXPORT occ::handle<TDF_Attribute> NewEmpty() const override;

  //! persistent -> transient (retrieve)
  Standard_EXPORT bool Paste(const XmlObjMgt_Persistent&       Source,
                             const occ::handle<TDF_Attribute>& Target,
                             XmlObjMgt_RRelocationTable&       RelocTable) const override;

  //! transient -> persistent (store)
  Standard_EXPORT void Paste(const occ::handle<TDF_Attribute>& Source,
                             XmlObjMgt_Persistent&             Target,
                             XmlObjMgt_SRelocationTable&       RelocTable) const override;

  DEFINE_STANDARD_RTTIEXT(XmlMDataStd_IntPackedMapDriver, XmlMDF_ADriver)
};

#endif // _XmlMDataStd_IntPackedMapDriver_HeaderFile
