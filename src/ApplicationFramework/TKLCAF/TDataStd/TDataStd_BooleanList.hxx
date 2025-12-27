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

#ifndef _TDataStd_BooleanList_HeaderFile
#define _TDataStd_BooleanList_HeaderFile

#include <Standard.hxx>

#include <NCollection_List.hxx>
#include <TDF_Attribute.hxx>
#include <Standard_Integer.hxx>
#include <Standard_OStream.hxx>
#include <Standard_GUID.hxx>

class TDF_Label;
class TDF_RelocationTable;

//! Contains a list of bolleans.
class TDataStd_BooleanList : public TDF_Attribute
{

public:
  //! Static methods
  //! ==============
  //! Returns the ID of the list of booleans attribute.
  Standard_EXPORT static const Standard_GUID& GetID();

  //! Finds or creates a list of boolean values attribute.
  Standard_EXPORT static occ::handle<TDataStd_BooleanList> Set(const TDF_Label& label);

  //! Finds or creates a list of boolean values attribute with explicit user defined <guid>.
  Standard_EXPORT static occ::handle<TDataStd_BooleanList> Set(const TDF_Label&     label,
                                                          const Standard_GUID& theGuid);

  Standard_EXPORT TDataStd_BooleanList();

  Standard_EXPORT bool IsEmpty() const;

  Standard_EXPORT int Extent() const;

  Standard_EXPORT void Prepend(const bool value);

  Standard_EXPORT void Append(const bool value);

  Standard_EXPORT void Clear();

  Standard_EXPORT bool First() const;

  Standard_EXPORT bool Last() const;

  //! 1 - means TRUE,
  //! 0 - means FALSE.
  Standard_EXPORT const NCollection_List<uint8_t>& List() const;

  //! Inserts the <value> before the <index> position.
  //! The indices start with 1 .. Extent().
  Standard_EXPORT bool InsertBefore(const int index,
                                                const bool before_value);

  //! Inserts the <value> after the <index> position.
  //! The indices start with 1 .. Extent().
  Standard_EXPORT bool InsertAfter(const int index,
                                               const bool after_value);

  //! Removes a value at <index> position.
  Standard_EXPORT bool Remove(const int index);

  //! Sets the explicit GUID (user defined) for the attribute.
  Standard_EXPORT void SetID(const Standard_GUID& theGuid) override;

  //! Sets default GUID for the attribute.
  Standard_EXPORT void SetID() override;

  Standard_EXPORT const Standard_GUID& ID() const override;

  Standard_EXPORT void Restore(const occ::handle<TDF_Attribute>& With) override;

  Standard_EXPORT occ::handle<TDF_Attribute> NewEmpty() const override;

  Standard_EXPORT void Paste(const occ::handle<TDF_Attribute>&       Into,
                             const occ::handle<TDF_RelocationTable>& RT) const override;

  Standard_EXPORT virtual Standard_OStream& Dump(Standard_OStream& anOS) const override;

  //! Dumps the content of me into the stream
  Standard_EXPORT virtual void DumpJson(Standard_OStream& theOStream,
                                        int  theDepth = -1) const override;

  DEFINE_STANDARD_RTTIEXT(TDataStd_BooleanList, TDF_Attribute)

private:
  NCollection_List<uint8_t> myList;
  Standard_GUID       myID;
};

#endif // _TDataStd_BooleanList_HeaderFile
