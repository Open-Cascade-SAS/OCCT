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

#ifndef _TDataStd_BooleanArray_HeaderFile
#define _TDataStd_BooleanArray_HeaderFile

#include <Standard.hxx>

#include <NCollection_Array1.hxx>
#include <NCollection_HArray1.hxx>
#include <Standard_Integer.hxx>
#include <TDF_Attribute.hxx>
#include <Standard_OStream.hxx>
#include <Standard_GUID.hxx>
class TDF_Label;
class TDF_RelocationTable;

//! An array of boolean values.
class TDataStd_BooleanArray : public TDF_Attribute
{

public:
  //! Static methods
  //! ==============
  //! Returns an ID for array.
  Standard_EXPORT static const Standard_GUID& GetID();

  //! Finds or creates an attribute with internal boolean array.
  Standard_EXPORT static occ::handle<TDataStd_BooleanArray> Set(const TDF_Label& label,
                                                                const int        lower,
                                                                const int        upper);

  //! Finds or creates an attribute with the array using explicit user defined <guid>.
  Standard_EXPORT static occ::handle<TDataStd_BooleanArray> Set(const TDF_Label&     label,
                                                                const Standard_GUID& theGuid,
                                                                const int            lower,
                                                                const int            upper);

  //! Initialize the inner array with bounds from <lower> to <upper>
  Standard_EXPORT void Init(const int lower, const int upper);

  //! Sets the <Index>th element of the array to <Value>
  //! OutOfRange exception is raised if <Index> doesn't respect Lower and Upper bounds of the
  //! internal array.
  Standard_EXPORT void SetValue(const int index, const bool value);

  //! Sets the explicit GUID (user defined) for the attribute.
  Standard_EXPORT void SetID(const Standard_GUID& theGuid) override;

  //! Sets default GUID for the attribute.
  Standard_EXPORT void SetID() override;

  //! Return the value of the <Index>th element of the array.
  Standard_EXPORT bool Value(const int Index) const;

  bool operator()(const int Index) const { return Value(Index); }

  //! Returns the lower boundary of the array.
  Standard_EXPORT int Lower() const;

  //! Returns the upper boundary of the array.
  Standard_EXPORT int Upper() const;

  //! Returns the number of elements in the array.
  Standard_EXPORT int Length() const;

  Standard_EXPORT const occ::handle<NCollection_HArray1<uint8_t>>& InternalArray() const;

  Standard_EXPORT void SetInternalArray(const occ::handle<NCollection_HArray1<uint8_t>>& values);

  Standard_EXPORT TDataStd_BooleanArray();

  Standard_EXPORT const Standard_GUID& ID() const override;

  Standard_EXPORT void Restore(const occ::handle<TDF_Attribute>& with) override;

  Standard_EXPORT occ::handle<TDF_Attribute> NewEmpty() const override;

  Standard_EXPORT void Paste(const occ::handle<TDF_Attribute>&       into,
                             const occ::handle<TDF_RelocationTable>& RT) const override;

  Standard_EXPORT Standard_OStream& Dump(Standard_OStream& OS) const override;

  //! Dumps the content of me into the stream
  Standard_EXPORT void DumpJson(Standard_OStream& theOStream, int theDepth = -1) const override;

  DEFINE_STANDARD_RTTIEXT(TDataStd_BooleanArray, TDF_Attribute)

private:
  occ::handle<NCollection_HArray1<uint8_t>> myValues;
  int                                       myLower;
  int                                       myUpper;
  Standard_GUID                             myID;
};

#endif // _TDataStd_BooleanArray_HeaderFile
