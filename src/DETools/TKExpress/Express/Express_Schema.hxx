// Copyright (c) 1999-2020 OPEN CASCADE SAS
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

#ifndef _Express_Schema_HeaderFile
#define _Express_Schema_HeaderFile

#include <Express_DataMapOfAsciiStringItem.hxx>
#include <Express_HSequenceOfItem.hxx>
#include <Standard_Type.hxx>

class TCollection_HAsciiString;
class Express_HSequenceOfItem;
class Express_Item;
class TCollection_AsciiString;
class Express_Type;

//! Represents a schema as a list of items and provides general
//! tools for generating HXX/CXX files (including dictionary of
//! item names)
class Express_Schema : public Standard_Transient
{

public:
  //! Creates a schema with given name and given set of items
  //! and calls Prepare()
  Standard_EXPORT Express_Schema(const Standard_CString                 theName,
                                 const Handle(Express_HSequenceOfItem)& theItems);

  //! Creates a schema with given name and given set of items
  //! and calls Prepare()
  Standard_EXPORT Express_Schema(const Handle(TCollection_HAsciiString)& theName,
                                 const Handle(Express_HSequenceOfItem)&  theItems);

  //! Returns schema name
  Standard_EXPORT const Handle(TCollection_HAsciiString)& Name() const;

  //! Returns sequence of items
  Standard_EXPORT const Handle(Express_HSequenceOfItem)& Items() const;

  //! Returns number of items
  Standard_EXPORT Standard_Integer NbItems() const;

  //! Returns item by index
  Standard_EXPORT Handle(Express_Item) Item(const Standard_Integer theNum) const;

  //! Returns item by name
  Standard_EXPORT Handle(Express_Item) Item(
    const Standard_CString theName,
    const Standard_Boolean theSilent = Standard_False) const;

  //! Returns item by name
  Standard_EXPORT Handle(Express_Item) Item(const TCollection_AsciiString& theName) const;

  //! Returns item by name
  Standard_EXPORT Handle(Express_Item) Item(const Handle(TCollection_HAsciiString)& theName) const;

  DEFINE_STANDARD_RTTIEXT(Express_Schema, Standard_Transient)

protected:
private:
  //! Prepares data for further work. Converts all item names
  //! from EXPRESS style (aaa_bb) to CASCADE style (AaaBb).
  //! Then, makes a dictionary of item names and sets handles
  //! to all items referred initially by name
  Standard_EXPORT void Prepare();

  //! Prepares type for work by setting its handle to item in the
  //! schema according to dictionary (for types which refer items
  //! by name)
  Standard_EXPORT void PrepareType(const Handle(Express_Type)& theType) const;

private:
  Handle(TCollection_HAsciiString) myName;
  Handle(Express_HSequenceOfItem)  myItems;
  Express_DataMapOfAsciiStringItem myDict;
};

#endif // _Express_Schema_HeaderFile
