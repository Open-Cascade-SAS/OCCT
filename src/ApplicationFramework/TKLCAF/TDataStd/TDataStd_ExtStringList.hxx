// Created on: 2007-05-29
// Created by: Vlad Romashko
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

#ifndef _TDataStd_ExtStringList_HeaderFile
#define _TDataStd_ExtStringList_HeaderFile

#include <Standard.hxx>

#include <TCollection_ExtendedString.hxx>
#include <NCollection_List.hxx>
#include <TDF_Attribute.hxx>
#include <Standard_Integer.hxx>
#include <Standard_OStream.hxx>
#include <Standard_GUID.hxx>

class TDF_Label;
class TCollection_ExtendedString;
class TDF_RelocationTable;

//! Contains a list of ExtendedString.
class TDataStd_ExtStringList : public TDF_Attribute
{

public:
  //! Static methods
  //! ==============
  //! Returns the ID of the list of strings attribute.
  Standard_EXPORT static const Standard_GUID& GetID();

  //! Finds or creates a list of string values attribute with explicit user defined <guid>.
  Standard_EXPORT static occ::handle<TDataStd_ExtStringList> Set(const TDF_Label& label);

  //! Finds or creates a list of string values attribute.
  Standard_EXPORT static occ::handle<TDataStd_ExtStringList> Set(const TDF_Label&     label,
                                                            const Standard_GUID& theGuid);

  Standard_EXPORT TDataStd_ExtStringList();

  Standard_EXPORT bool IsEmpty() const;

  Standard_EXPORT int Extent() const;

  Standard_EXPORT void Prepend(const TCollection_ExtendedString& value);

  Standard_EXPORT void Append(const TCollection_ExtendedString& value);

  //! Sets the explicit GUID (user defined) for the attribute.
  Standard_EXPORT void SetID(const Standard_GUID& theGuid) override;

  //! Sets default GUID for the attribute.
  Standard_EXPORT void SetID() override;

  //! Inserts the <value> before the first meet of <before_value>.
  Standard_EXPORT bool InsertBefore(const TCollection_ExtendedString& value,
                                                const TCollection_ExtendedString& before_value);

  //! Inserts the <value> before the <index> position.
  //! The indices start with 1 .. Extent().
  Standard_EXPORT bool InsertBefore(const int            index,
                                                const TCollection_ExtendedString& before_value);

  //! Inserts the <value> after the first meet of <after_value>.
  Standard_EXPORT bool InsertAfter(const TCollection_ExtendedString& value,
                                               const TCollection_ExtendedString& after_value);

  //! Inserts the <value> after the <index> position.
  //! The indices start with 1 .. Extent().
  Standard_EXPORT bool InsertAfter(const int            index,
                                               const TCollection_ExtendedString& after_value);

  //! Removes the first meet of the <value>.
  Standard_EXPORT bool Remove(const TCollection_ExtendedString& value);

  //! Removes a value at <index> position.
  Standard_EXPORT bool Remove(const int index);

  Standard_EXPORT void Clear();

  Standard_EXPORT const TCollection_ExtendedString& First() const;

  Standard_EXPORT const TCollection_ExtendedString& Last() const;

  Standard_EXPORT const NCollection_List<TCollection_ExtendedString>& List() const;

  Standard_EXPORT const Standard_GUID& ID() const override;

  Standard_EXPORT void Restore(const occ::handle<TDF_Attribute>& With) override;

  Standard_EXPORT occ::handle<TDF_Attribute> NewEmpty() const override;

  Standard_EXPORT void Paste(const occ::handle<TDF_Attribute>&       Into,
                             const occ::handle<TDF_RelocationTable>& RT) const override;

  Standard_EXPORT virtual Standard_OStream& Dump(Standard_OStream& anOS) const override;

  //! Dumps the content of me into the stream
  Standard_EXPORT virtual void DumpJson(Standard_OStream& theOStream,
                                        int  theDepth = -1) const override;

  DEFINE_STANDARD_RTTIEXT(TDataStd_ExtStringList, TDF_Attribute)

private:
  NCollection_List<TCollection_ExtendedString> myList;
  Standard_GUID                 myID;
};

#endif // _TDataStd_ExtStringList_HeaderFile
