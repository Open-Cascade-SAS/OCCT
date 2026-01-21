// Created on: 1995-09-01
// Created by: Christian CAILLET
// Copyright (c) 1995-1999 Matra Datavision
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

#ifndef _Interface_BitMap_HeaderFile
#define _Interface_BitMap_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <Standard_Integer.hxx>
#include <NCollection_Array1.hxx>
#include <NCollection_HArray1.hxx>
#include <TCollection_AsciiString.hxx>
#include <NCollection_Sequence.hxx>
#include <NCollection_HSequence.hxx>

//! A bit map simply allows to associate a boolean flag to each
//! item of a list, such as a list of entities, etc... numbered
//! between 1 and a positive count nbitems
//!
//! The BitMap class allows to associate several binary flags,
//! each of one is identified by a number from 0 to a count
//! which can remain at zero or be positive : nbflags
//!
//! Flags lists over than numflag=0 are added after creation
//! Each of one can be named, hence the user can identify it
//! either by its flag number or by a name which gives a flag n0
//! (flag n0 0 has no name)
class Interface_BitMap
{
public:
  DEFINE_STANDARD_ALLOC

  //! Creates a empty BitMap
  Standard_EXPORT Interface_BitMap();

  //! Creates a BitMap for <nbitems> items
  //! One flag is defined, n0 0
  //! <resflags> prepares allocation for <resflags> more flags
  //! Flags values start at false
  Standard_EXPORT Interface_BitMap(const int nbitems, const int resflags = 0);

  //! Initialize empty bit by <nbitems> items
  //! One flag is defined, n0 0
  //! <resflags> prepares allocation for <resflags> more flags
  //! Flags values start at false
  Standard_EXPORT void Initialize(const int nbitems, const int resflags = 0);

  //! Creates a BitMap from another one
  //! if <copied> is True, copies data
  //! else, data are not copied, only the header object is
  Standard_EXPORT Interface_BitMap(const Interface_BitMap& other, const bool copied = false);

  //! Initialize a BitMap from another one
  Standard_EXPORT void Initialize(const Interface_BitMap& other, const bool copied = false);

  //! Reservates for a count of more flags
  Standard_EXPORT void Reservate(const int moreflags);

  //! Sets for a new count of items, which can be either less or
  //! greater than the former one
  //! For new items, their flags start at false
  Standard_EXPORT void SetLength(const int nbitems);

  //! Adds a flag, a name can be attached to it
  //! Returns its flag number
  //! Makes required reservation
  Standard_EXPORT int AddFlag(const char* const name = "");

  //! Adds several flags (<more>) with no name
  //! Returns the number of last added flag
  Standard_EXPORT int AddSomeFlags(const int more);

  //! Removes a flag given its number.
  //! Returns True if done, false if num is out of range
  Standard_EXPORT bool RemoveFlag(const int num);

  //! Sets a name for a flag, given its number
  //! name can be empty (to erase the name of a flag)
  //! Returns True if done, false if : num is out of range, or
  //! name non-empty already set to another flag
  Standard_EXPORT bool SetFlagName(const int num, const char* const name);

  //! Returns the count of flags (flag 0 not included)
  Standard_EXPORT int NbFlags() const;

  //! Returns the count of items (i.e. the length of the bitmap)
  Standard_EXPORT int Length() const;

  //! Returns the name recorded for a flag, or an empty string
  Standard_EXPORT const char* FlagName(const int num) const;

  //! Returns the number or a flag given its name, or zero
  Standard_EXPORT int FlagNumber(const char* const name) const;

  //! Returns the value (true/false) of a flag, from :
  //! - the number of the item
  //! - the flag number, by default 0
  Standard_EXPORT bool Value(const int item, const int flag = 0) const;

  //! Sets a new value for a flag
  Standard_EXPORT void SetValue(const int item, const bool val, const int flag = 0) const;

  //! Sets a flag to True
  Standard_EXPORT void SetTrue(const int item, const int flag = 0) const;

  //! Sets a flag to False
  Standard_EXPORT void SetFalse(const int item, const int flag = 0) const;

  //! Returns the former value for a flag and sets it to True
  //! (before : value returned; after : True)
  Standard_EXPORT bool CTrue(const int item, const int flag = 0) const;

  //! Returns the former value for a flag and sets it to False
  //! (before : value returned; after : False)
  Standard_EXPORT bool CFalse(const int item, const int flag = 0) const;

  //! Initialises all the values of Flag Number <flag> to a given
  //! value <val>
  Standard_EXPORT void Init(const bool val, const int flag = 0) const;

  //! Clear all field of bit map
  Standard_EXPORT void Clear();

private:
  int                                                         thenbitems;
  int                                                         thenbwords;
  int                                                         thenbflags;
  occ::handle<NCollection_HArray1<int>>                       theflags;
  occ::handle<NCollection_HSequence<TCollection_AsciiString>> thenames;
};

#endif // _Interface_BitMap_HeaderFile
