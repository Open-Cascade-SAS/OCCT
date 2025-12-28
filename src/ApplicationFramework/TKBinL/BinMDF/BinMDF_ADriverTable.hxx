// Created on: 2002-10-29
// Created by: Michael SAZONOV
// Copyright (c) 2002-2014 OPEN CASCADE SAS
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

#ifndef _BinMDF_ADriverTable_HeaderFile
#define _BinMDF_ADriverTable_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <BinMDF_ADriver.hxx>
#include <NCollection_DataMap.hxx>
#include <Standard_Integer.hxx>
#include <NCollection_DoubleMap.hxx>
#include <Standard_Transient.hxx>
#include <NCollection_IndexedMap.hxx>
#include <TCollection_AsciiString.hxx>
#include <NCollection_Sequence.hxx>
class BinMDF_ADriver;

//! A driver table is an object building links between
//! object types and object drivers. In the
//! translation process, a driver table is asked to
//! give a translation driver for each current object
//! to be translated.
class BinMDF_ADriverTable : public Standard_Transient
{

public:
  //! Constructor
  Standard_EXPORT BinMDF_ADriverTable();

  //! Adds a translation driver <theDriver>.
  Standard_EXPORT void AddDriver(const occ::handle<BinMDF_ADriver>& theDriver);

  //! Adds a translation driver for the derived attribute. The base driver must be already added.
  //! @param theInstance is newly created attribute, detached from any label
  Standard_EXPORT void AddDerivedDriver(const occ::handle<TDF_Attribute>& theInstance);

  //! Adds a translation driver for the derived attribute. The base driver must be already added.
  //! @param theDerivedType is registered attribute type using IMPLEMENT_DERIVED_ATTRIBUTE macro
  Standard_EXPORT const occ::handle<Standard_Type>& AddDerivedDriver(const char* theDerivedType);

  //! Assigns the IDs to the drivers of the given Types.
  //! It uses indices in the map as IDs.
  //! Useful in storage procedure.
  Standard_EXPORT void AssignIds(
    const NCollection_IndexedMap<occ::handle<Standard_Transient>>& theTypes);

  //! Assigns the IDs to the drivers of the given Type Names;
  //! It uses indices in the sequence as IDs.
  //! Useful in retrieval procedure.
  Standard_EXPORT void AssignIds(const NCollection_Sequence<TCollection_AsciiString>& theTypeNames);

  //! Gets a driver <theDriver> according to <theType>.
  //! Returns Type ID if the driver was assigned an ID; 0 otherwise.
  int GetDriver(const occ::handle<Standard_Type>& theType, occ::handle<BinMDF_ADriver>& theDriver);

  //! Returns a driver according to <theTypeId>.
  //! Returns null handle if a driver is not found
  occ::handle<BinMDF_ADriver> GetDriver(const int theTypeId);

  DEFINE_STANDARD_RTTIEXT(BinMDF_ADriverTable, Standard_Transient)

private:
  //! Assigns the ID to the driver of the Type
  void AssignId(const occ::handle<Standard_Type>& theType, const int theId);

  NCollection_DataMap<occ::handle<Standard_Type>, occ::handle<BinMDF_ADriver>> myMap;
  NCollection_DoubleMap<occ::handle<Standard_Type>, int>                       myMapId;
};

#include <BinMDF_ADriverTable.lxx>

#endif // _BinMDF_ADriverTable_HeaderFile
