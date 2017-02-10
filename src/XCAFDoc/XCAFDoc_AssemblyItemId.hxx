// Created on: 2017-02-16
// Created by: Sergey NIKONOV
// Copyright (c) 2000-2017 OPEN CASCADE SAS
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

#ifndef _XCAFDoc_AssemblyItemId_HeaderFile
#define _XCAFDoc_AssemblyItemId_HeaderFile

#include <Standard_GUID.hxx>
#include <TColStd_ListOfAsciiString.hxx>

//! Unique item identifier in the hierarchical product structure.
//! A full path to an assembly component in the “part-of” graph starting from 
//! the root node. 
class XCAFDoc_AssemblyItemId
{

public:

  //! Constructs an empty item ID.
  Standard_EXPORT XCAFDoc_AssemblyItemId();

  //! Constructs an item ID from a list of strings, where every 
  //! string is a label entry.
  //! \param [in] thePath - list of label entries.
  Standard_EXPORT XCAFDoc_AssemblyItemId(const TColStd_ListOfAsciiString& thePath);

  //! Constructs an item ID from a formatted path, where label entries
  //! are separated by '/' symbol.
  //! \param [in] theString - formatted full path.
  Standard_EXPORT XCAFDoc_AssemblyItemId(const TCollection_AsciiString& theString);

  //! Initializes the item ID from a list of strings, where every 
  //! string is a label entry.
  //! \param [in] thePath - list of label entries.
  Standard_EXPORT void Init(const TColStd_ListOfAsciiString& thePath);

  //! Initializes the item ID from a formatted path, where label entries
  //! are separated by '/' symbol.
  //! \param [in] theString - formatted full path.
  Standard_EXPORT void Init(const TCollection_AsciiString& theString);

  //! Returns true if the full path is empty, otherwise - false.
  Standard_EXPORT Standard_Boolean IsNull() const;

  //! Clears the full path.
  Standard_EXPORT void Nullify();

  //! Checks if this item is a child of the given item.
  //! \param [in] theOther - potentially ancestor item.
  //! \return true if the item is a child of theOther item, otherwise - false.
  Standard_EXPORT Standard_Boolean IsChild(const XCAFDoc_AssemblyItemId& theOther) const;

  //! Checks if this item is a direct child of the given item.
  //! \param [in] theOther - potentially parent item.
  //! \return true if the item is a direct child of theOther item, otherwise - false.
  Standard_EXPORT Standard_Boolean IsDirectChild(const XCAFDoc_AssemblyItemId& theOther) const;

  //! Checks for item IDs equality.
  //! \param [in] theOther - the item ID to check equality with.
  //! \return true if this ID is equal to theOther, otherwise - false.
  Standard_EXPORT Standard_Boolean IsEqual(const XCAFDoc_AssemblyItemId& theOther) const;

  //! Returns the full path as a list of label entries.
  Standard_EXPORT const TColStd_ListOfAsciiString& GetPath() const;

  //! Returns the full pass as a formatted string.
  Standard_EXPORT TCollection_AsciiString ToString() const;

  struct Hasher
  {
    static int HashCode(const XCAFDoc_AssemblyItemId& theItem, 
                        const int                     upper)
    {
      return ::HashCode(theItem.ToString(), upper);
    }

    static int IsEqual(const XCAFDoc_AssemblyItemId& theItem1,
                       const XCAFDoc_AssemblyItemId& theItem2)
    {
      return theItem1.IsEqual(theItem2);
    }
  };

private:

  TColStd_ListOfAsciiString myPath; ///< List of label entries

};

#endif // _XCAFDoc_AssemblyItemId_HeaderFile
