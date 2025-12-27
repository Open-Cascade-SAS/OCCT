// Created on: 2001-07-09
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

#ifndef _XmlMDF_HeaderFile
#define _XmlMDF_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <XmlObjMgt_Element.hxx>
#include <Standard_Integer.hxx>
#include <TCollection_AsciiString.hxx>
#include <XmlMDF_ADriver.hxx>
#include <NCollection_DataMap.hxx>

#include <Message_ProgressRange.hxx>

class TDF_Data;
class XmlMDF_ADriverTable;
class TDF_Label;
class Message_Messenger;

//! This package provides classes and methods to
//! translate a transient DF into a persistent one and
//! vice versa.
//!
//! Driver
//!
//! A driver is a tool used to translate a transient
//! attribute into a persistent one and vice versa.
//!
//! Driver Table
//!
//! A driver table is an object building links between
//! object types and object drivers. In the
//! translation process, a driver table is asked to
//! give a translation driver for each current object
//! to be translated.
class XmlMDF
{
public:
  DEFINE_STANDARD_ALLOC

  //! Translates a transient <aSource> into a persistent
  //! <aTarget>.
  Standard_EXPORT static void FromTo(
    const occ::handle<TDF_Data>&            aSource,
    XmlObjMgt_Element&                 aTarget,
    XmlObjMgt_SRelocationTable&        aReloc,
    const occ::handle<XmlMDF_ADriverTable>& aDrivers,
    const Message_ProgressRange&       theRange = Message_ProgressRange());

  //! Translates a persistent <aSource> into a transient
  //! <aTarget>.
  //! Returns True if completed successfully (False on error)
  Standard_EXPORT static bool FromTo(
    const XmlObjMgt_Element&           aSource,
    occ::handle<TDF_Data>&                  aTarget,
    XmlObjMgt_RRelocationTable&        aReloc,
    const occ::handle<XmlMDF_ADriverTable>& aDrivers,
    const Message_ProgressRange&       theRange = Message_ProgressRange());

  //! Adds the attribute storage drivers to <aDriverSeq>.
  Standard_EXPORT static void AddDrivers(const occ::handle<XmlMDF_ADriverTable>& aDriverTable,
                                         const occ::handle<Message_Messenger>&   theMessageDriver);

private:
  Standard_EXPORT static int WriteSubTree(
    const TDF_Label&                   theLabel,
    XmlObjMgt_Element&                 theElement,
    XmlObjMgt_SRelocationTable&        aReloc,
    const occ::handle<XmlMDF_ADriverTable>& aDrivers,
    const Message_ProgressRange&       theRange = Message_ProgressRange());

  Standard_EXPORT static int ReadSubTree(
    const XmlObjMgt_Element&     theElement,
    const TDF_Label&             theLabel,
    XmlObjMgt_RRelocationTable&  aReloc,
    const NCollection_DataMap<TCollection_AsciiString, occ::handle<XmlMDF_ADriver>>&    aDrivers,
    const Message_ProgressRange& theRange = Message_ProgressRange());

  Standard_EXPORT static void CreateDrvMap(const occ::handle<XmlMDF_ADriverTable>& aDriverTable,
                                           NCollection_DataMap<TCollection_AsciiString, occ::handle<XmlMDF_ADriver>>&                anAsciiDriverMap);

  friend class XmlMDF_ADriver;
  friend class XmlMDF_TagSourceDriver;
  friend class XmlMDF_ReferenceDriver;
  friend class XmlMDF_ADriverTable;
};

#endif // _XmlMDF_HeaderFile
