// Created by: DAUTRY Philippe
// Copyright (c) 1997-1999 Matra Datavision
// Copyright (c) 1999-2014 OPEN CASCADE SAS
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

#ifndef _TDF_HeaderFile
#define _TDF_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <Standard_Boolean.hxx>
class Standard_GUID;
class TCollection_ExtendedString;

//! This package provides data framework for binding
//! features and data structures.
//!
//! The feature structure is a tree used to bind
//! semantic information about each feature together.
//!
//! The only one concrete   attribute defined in  this
//! package is the TagSource attribute.This attribute
//! is used for  random creation of child labels under
//! a given label. Tags are randomly delivered.
class TDF
{
public:
  DEFINE_STANDARD_ALLOC

  //! Returns ID "00000000-0000-0000-0000-000000000000",
  //! sometimes used as null ID.
  Standard_EXPORT static const Standard_GUID& LowestID();

  //! Returns ID "ffffffff-ffff-ffff-ffff-ffffffffffff".
  Standard_EXPORT static const Standard_GUID& UppestID();

  //! Sets link between GUID and ProgID in hidden DataMap
  Standard_EXPORT static void AddLinkGUIDToProgID(const Standard_GUID&              ID,
                                                  const TCollection_ExtendedString& ProgID);

  //! Returns True if there is GUID for given <ProgID> then GUID is returned in <ID>
  Standard_EXPORT static Standard_Boolean GUIDFromProgID(const TCollection_ExtendedString& ProgID,
                                                         Standard_GUID&                    ID);

  //! Returns True if there is ProgID for given <ID> then ProgID is returned in <ProgID>
  Standard_EXPORT static Standard_Boolean ProgIDFromGUID(const Standard_GUID&        ID,
                                                         TCollection_ExtendedString& ProgID);
};

#endif // _TDF_HeaderFile
