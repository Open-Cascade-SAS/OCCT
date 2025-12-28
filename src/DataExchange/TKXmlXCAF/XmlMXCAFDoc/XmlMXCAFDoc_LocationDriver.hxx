// Created on: 2001-09-11
// Created by: Julia DOROVSKIKH
// Copyright (c) 2001-2014 OPEN CASCADE SAS
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

#ifndef _XmlMXCAFDoc_LocationDriver_HeaderFile
#define _XmlMXCAFDoc_LocationDriver_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <TopTools_LocationSetPtr.hxx>
#include <XmlMDF_ADriver.hxx>
#include <XmlObjMgt_RRelocationTable.hxx>
#include <XmlObjMgt_SRelocationTable.hxx>
#include <XmlObjMgt_Element.hxx>
class Message_Messenger;
class TDF_Attribute;
class XmlObjMgt_Persistent;
class TopLoc_Location;

//! Attribute Driver.
class XmlMXCAFDoc_LocationDriver : public XmlMDF_ADriver
{

public:
  Standard_EXPORT XmlMXCAFDoc_LocationDriver(
    const occ::handle<Message_Messenger>& theMessageDriver);

  Standard_EXPORT occ::handle<TDF_Attribute> NewEmpty() const override;

  Standard_EXPORT bool Paste(const XmlObjMgt_Persistent&       Source,
                             const occ::handle<TDF_Attribute>& Target,
                             XmlObjMgt_RRelocationTable&       RelocTable) const override;

  Standard_EXPORT void Paste(const occ::handle<TDF_Attribute>& Source,
                             XmlObjMgt_Persistent&             Target,
                             XmlObjMgt_SRelocationTable&       RelocTable) const override;

  //! Translate a non storable Location to a storable Location.
  Standard_EXPORT void Translate(const TopLoc_Location&      theLoc,
                                 XmlObjMgt_Element&          theParent,
                                 XmlObjMgt_SRelocationTable& theMap) const;

  //! Translate a storable Location to a non storable Location.
  Standard_EXPORT bool Translate(const XmlObjMgt_Element&    theParent,
                                 TopLoc_Location&            theLoc,
                                 XmlObjMgt_RRelocationTable& theMap) const;

  void SetSharedLocations(const TopTools_LocationSetPtr& theLocations);

  DEFINE_STANDARD_RTTIEXT(XmlMXCAFDoc_LocationDriver, XmlMDF_ADriver)

private:
  TopTools_LocationSetPtr myLocations;
};

#include <XmlMXCAFDoc_LocationDriver.lxx>

#endif // _XmlMXCAFDoc_LocationDriver_HeaderFile
