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

#include <BinMDF_TypeADriverMap.hxx>
#include <BinMDF_TypeIdMap.hxx>
#include <Standard_Transient.hxx>
#include <Standard_Integer.hxx>
#include <TColStd_IndexedMapOfTransient.hxx>
#include <TColStd_SequenceOfAsciiString.hxx>
class BinMDF_ADriver;


class BinMDF_ADriverTable;
DEFINE_STANDARD_HANDLE(BinMDF_ADriverTable, Standard_Transient)

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
  Standard_EXPORT void AddDriver (const Handle(BinMDF_ADriver)& theDriver);

  //! Adds a translation driver for the derived attribute. The base driver must be already added.
  //! @param theInstance is newly created attribute, detached from any label
  Standard_EXPORT void AddDerivedDriver (const Handle(TDF_Attribute)& theInstance);

  //! Adds a translation driver for the derived attribute. The base driver must be already added.
  //! @param theDerivedType is registered attribute type using IMPLEMENT_DERIVED_ATTRIBUTE macro
  Standard_EXPORT const Handle(Standard_Type)& AddDerivedDriver (Standard_CString theDerivedType);

  //! Assigns the IDs to the drivers of the given Types.
  //! It uses indices in the map as IDs.
  //! Useful in storage procedure.
  Standard_EXPORT void AssignIds (const TColStd_IndexedMapOfTransient& theTypes);
  
  //! Assigns the IDs to the drivers of the given Type Names;
  //! It uses indices in the sequence as IDs.
  //! Useful in retrieval procedure.
  Standard_EXPORT void AssignIds (const TColStd_SequenceOfAsciiString& theTypeNames);
  
  //! Gets a driver <theDriver> according to <theType>.
  //! Returns Type ID if the driver was assigned an ID; 0 otherwise.
  Standard_Integer GetDriver (const Handle(Standard_Type)& theType, Handle(BinMDF_ADriver)& theDriver);
  
  //! Returns a driver according to <theTypeId>.
  //! Returns null handle if a driver is not found
  Handle(BinMDF_ADriver) GetDriver (const Standard_Integer theTypeId);




  DEFINE_STANDARD_RTTIEXT(BinMDF_ADriverTable,Standard_Transient)

protected:




private:

  
  //! Assigns the ID to the driver of the Type
    void AssignId (const Handle(Standard_Type)& theType, const Standard_Integer theId);

  BinMDF_TypeADriverMap myMap;
  BinMDF_TypeIdMap myMapId;


};


#include <BinMDF_ADriverTable.lxx>





#endif // _BinMDF_ADriverTable_HeaderFile
