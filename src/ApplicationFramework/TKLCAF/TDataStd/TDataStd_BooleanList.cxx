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

#include <TDataStd_BooleanList.hxx>

#include <Standard_Dump.hxx>
#include <Standard_GUID.hxx>
#include <Standard_Type.hxx>
#include <NCollection_List.hxx>
#include <TDF_Attribute.hxx>
#include <TDF_Label.hxx>
#include <TDF_RelocationTable.hxx>

IMPLEMENT_STANDARD_RTTIEXT(TDataStd_BooleanList, TDF_Attribute)

//=================================================================================================

const Standard_GUID& TDataStd_BooleanList::GetID()
{
  static Standard_GUID TDataStd_BooleanListID("23A9D60E-A033-44d8-96EE-015587A41BBC");
  return TDataStd_BooleanListID;
}

//=======================================================================
// function : SetAttr
// purpose  : Implements Set functionality
//=======================================================================
static occ::handle<TDataStd_BooleanList> SetAttr(const TDF_Label&     label,
                                                 const Standard_GUID& theGuid)
{
  occ::handle<TDataStd_BooleanList> A;
  if (!label.FindAttribute(theGuid, A))
  {
    A = new TDataStd_BooleanList;
    A->SetID(theGuid);
    label.AddAttribute(A);
  }
  return A;
}

//=================================================================================================

TDataStd_BooleanList::TDataStd_BooleanList()
    : myID(GetID())
{
}

//=================================================================================================

occ::handle<TDataStd_BooleanList> TDataStd_BooleanList::Set(const TDF_Label& label)
{
  return SetAttr(label, GetID());
}

//=======================================================================
// function : Set
// purpose  : Set user defined attribute with specific ID
//=======================================================================
occ::handle<TDataStd_BooleanList> TDataStd_BooleanList::Set(const TDF_Label&     label,
                                                            const Standard_GUID& theGuid)
{
  return SetAttr(label, theGuid);
}

//=================================================================================================

bool TDataStd_BooleanList::IsEmpty() const
{
  return myList.IsEmpty();
}

//=================================================================================================

int TDataStd_BooleanList::Extent() const
{
  return myList.Extent();
}

//=================================================================================================

void TDataStd_BooleanList::Prepend(const bool value)
{
  Backup();
  myList.Prepend(value ? 1 : 0);
}

//=================================================================================================

void TDataStd_BooleanList::Append(const bool value)
{
  Backup();
  myList.Append(value ? 1 : 0);
}

//=================================================================================================

void TDataStd_BooleanList::Clear()
{
  Backup();
  myList.Clear();
}

//=================================================================================================

bool TDataStd_BooleanList::First() const
{
  return myList.First() == 1;
}

//=================================================================================================

bool TDataStd_BooleanList::Last() const
{
  return myList.Last() == 1;
}

//=================================================================================================

const NCollection_List<uint8_t>& TDataStd_BooleanList::List() const
{
  return myList;
}

//=======================================================================
// function : InsertBefore
// purpose  : Inserts the <value> before the <index> position.
//=======================================================================
bool TDataStd_BooleanList::InsertBefore(const int index, const bool before_value)
{
  int                                 i(1);
  bool                                found(false);
  NCollection_List<uint8_t>::Iterator itr(myList);
  for (; itr.More(); itr.Next(), ++i)
  {
    if (i == index)
    {
      Backup();
      myList.InsertBefore(before_value ? 1 : 0, itr);
      found = true;
      break;
    }
  }
  return found;
}

//=======================================================================
// function : InsertAfter
// purpose  : Inserts the <value> after the <index> position.
//=======================================================================
bool TDataStd_BooleanList::InsertAfter(const int index, const bool after_value)
{
  int                                 i(1);
  bool                                found(false);
  NCollection_List<uint8_t>::Iterator itr(myList);
  for (; itr.More(); itr.Next(), ++i)
  {
    if (i == index)
    {
      Backup();
      myList.InsertAfter(after_value ? 1 : 0, itr);
      found = true;
      break;
    }
  }
  return found;
}

//=======================================================================
// function : Remove
// purpose  : Removes the <value> at the <index> position.
//=======================================================================
bool TDataStd_BooleanList::Remove(const int index)
{
  int                                 i(1);
  bool                                found(false);
  NCollection_List<uint8_t>::Iterator itr(myList);
  for (; itr.More(); itr.Next(), ++i)
  {
    if (i == index)
    {
      Backup();
      myList.Remove(itr);
      found = true;
      break;
    }
  }
  return found;
}

//=================================================================================================

const Standard_GUID& TDataStd_BooleanList::ID() const
{
  return myID;
}

//=================================================================================================

void TDataStd_BooleanList::SetID(const Standard_GUID& theGuid)
{
  if (myID == theGuid)
    return;
  Backup();
  myID = theGuid;
}

//=================================================================================================

void TDataStd_BooleanList::SetID()
{
  Backup();
  myID = GetID();
}

//=================================================================================================

occ::handle<TDF_Attribute> TDataStd_BooleanList::NewEmpty() const
{
  return new TDataStd_BooleanList();
}

//=================================================================================================

void TDataStd_BooleanList::Restore(const occ::handle<TDF_Attribute>& With)
{
  myList.Clear();
  occ::handle<TDataStd_BooleanList>   aList = occ::down_cast<TDataStd_BooleanList>(With);
  NCollection_List<uint8_t>::Iterator itr(aList->List());
  for (; itr.More(); itr.Next())
  {
    myList.Append(itr.Value() ? 1 : 0);
  }
  myID = aList->ID();
}

//=================================================================================================

void TDataStd_BooleanList::Paste(const occ::handle<TDF_Attribute>& Into,
                                 const occ::handle<TDF_RelocationTable>&) const
{
  occ::handle<TDataStd_BooleanList> aList = occ::down_cast<TDataStd_BooleanList>(Into);
  aList->Clear();
  NCollection_List<uint8_t>::Iterator itr(myList);
  for (; itr.More(); itr.Next())
  {
    aList->Append(itr.Value() != 0);
  }
  aList->SetID(myID);
}

//=================================================================================================

Standard_OStream& TDataStd_BooleanList::Dump(Standard_OStream& anOS) const
{
  anOS << "\nBooleanList: ";
  char sguid[Standard_GUID_SIZE_ALLOC];
  myID.ToCString(sguid);
  anOS << sguid;
  anOS << std::endl;
  return anOS;
}

//=================================================================================================

void TDataStd_BooleanList::DumpJson(Standard_OStream& theOStream, int theDepth) const
{
  OCCT_DUMP_TRANSIENT_CLASS_BEGIN(theOStream)

  OCCT_DUMP_BASE_CLASS(theOStream, theDepth, TDF_Attribute)

  for (NCollection_List<uint8_t>::Iterator aListIt(myList); aListIt.More(); aListIt.Next())
  {
    const uint8_t& aValue = aListIt.Value();
    OCCT_DUMP_FIELD_VALUE_NUMERICAL(theOStream, aValue)
  }
}
