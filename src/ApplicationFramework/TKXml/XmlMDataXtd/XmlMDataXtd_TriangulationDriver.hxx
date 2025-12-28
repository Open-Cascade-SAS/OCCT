// Created on: 2016-11-10
// Created by: Anton KOZULIN
// Copyright (c) 2016 OPEN CASCADE SAS
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

#ifndef _XmlMDataXtd_TriangulationDriver_HeaderFile
#define _XmlMDataXtd_TriangulationDriver_HeaderFile

#include <Standard.hxx>

#include <XmlMDF_ADriver.hxx>
#include <XmlObjMgt_RRelocationTable.hxx>
#include <XmlObjMgt_SRelocationTable.hxx>
class Message_Messenger;
class TDF_Attribute;
class XmlObjMgt_Persistent;

//! TDataStd_Mesh attribute XML Driver.
class XmlMDataXtd_TriangulationDriver : public XmlMDF_ADriver
{

public:
  Standard_EXPORT XmlMDataXtd_TriangulationDriver(
    const occ::handle<Message_Messenger>& theMessageDriver);

  Standard_EXPORT occ::handle<TDF_Attribute> NewEmpty() const override;

  Standard_EXPORT bool Paste(const XmlObjMgt_Persistent&       Source,
                             const occ::handle<TDF_Attribute>& Target,
                             XmlObjMgt_RRelocationTable&       RelocTable) const override;

  Standard_EXPORT void Paste(const occ::handle<TDF_Attribute>& Source,
                             XmlObjMgt_Persistent&             Target,
                             XmlObjMgt_SRelocationTable&       RelocTable) const override;

  DEFINE_STANDARD_RTTIEXT(XmlMDataXtd_TriangulationDriver, XmlMDF_ADriver)

private:
  void GetReal(Standard_IStream& IS, double& theValue) const;
};

#endif // _XmlMDataXtd_TriangulationDriver_HeaderFile
