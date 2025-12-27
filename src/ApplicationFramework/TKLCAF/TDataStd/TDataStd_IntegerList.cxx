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

#include <TDataStd_IntegerList.hxx>

#include <Standard_Dump.hxx>
#include <Standard_GUID.hxx>
#include <Standard_Type.hxx>
#include <Standard_Integer.hxx>
#include <NCollection_List.hxx>
#include <TDF_Attribute.hxx>
#include <TDF_Label.hxx>
#include <TDF_RelocationTable.hxx>

IMPLEMENT_STANDARD_RTTIEXT(TDataStd_IntegerList, TDF_Attribute)

//=================================================================================================

const Standard_GUID& TDataStd_IntegerList::GetID()
{
  static Standard_GUID TDataStd_IntegerListID("E406AA18-FF3F-483b-9A78-1A5EA5D1AA52");
  return TDataStd_IntegerListID;
}

//=======================================================================
// function : SetAttr
// purpose  : Implements Set functionality
//=======================================================================
static occ::handle<TDataStd_IntegerList> SetAttr(const TDF_Label& label, const Standard_GUID& theGuid)
{
  occ::handle<TDataStd_IntegerList> A;
  if (!label.FindAttribute(theGuid, A))
  {
    A = new TDataStd_IntegerList;
    A->SetID(theGuid);
    label.AddAttribute(A);
  }
  return A;
}

//=================================================================================================

TDataStd_IntegerList::TDataStd_IntegerList()
    : myID(GetID())
{
}

//=================================================================================================

occ::handle<TDataStd_IntegerList> TDataStd_IntegerList::Set(const TDF_Label& label)
{
  return SetAttr(label, GetID());
}

//=======================================================================
// function : Set
// purpose  : Set user defined attribute with specific ID
//=======================================================================
occ::handle<TDataStd_IntegerList> TDataStd_IntegerList::Set(const TDF_Label&     label,
                                                       const Standard_GUID& theGuid)
{
  return SetAttr(label, theGuid);
}

//=================================================================================================

bool TDataStd_IntegerList::IsEmpty() const
{
  return myList.IsEmpty();
}

//=================================================================================================

int TDataStd_IntegerList::Extent() const
{
  return myList.Extent();
}

//=================================================================================================

void TDataStd_IntegerList::Prepend(const int value)
{
  Backup();
  myList.Prepend(value);
}

//=================================================================================================

void TDataStd_IntegerList::Append(const int value)
{
  Backup();
  myList.Append(value);
}

//=================================================================================================

bool TDataStd_IntegerList::InsertBefore(const int value,
                                                    const int before_value)
{
  NCollection_List<int>::Iterator itr(myList);
  for (; itr.More(); itr.Next())
  {
    if (itr.Value() == before_value)
    {
      Backup();
      myList.InsertBefore(value, itr);
      return true;
    }
  }
  return false;
}

// Inserts the <value> before the <index> position.
// The indices start with 1 .. Extent().
bool TDataStd_IntegerList::InsertBeforeByIndex(const int index,
                                                           const int before_value)
{
  int                    i(1);
  bool                    found(false);
  NCollection_List<int>::Iterator itr(myList);
  for (; itr.More(); itr.Next(), ++i)
  {
    if (i == index)
    {
      Backup();
      myList.InsertBefore(before_value, itr);
      found = true;
      break;
    }
  }
  return found;
}

//=================================================================================================

bool TDataStd_IntegerList::InsertAfter(const int value,
                                                   const int after_value)
{
  NCollection_List<int>::Iterator itr(myList);
  for (; itr.More(); itr.Next())
  {
    if (itr.Value() == after_value)
    {
      Backup();
      myList.InsertAfter(value, itr);
      return true;
    }
  }
  return false;
}

// Inserts the <value> after the <index> position.
// The indices start with 1 .. Extent().
bool TDataStd_IntegerList::InsertAfterByIndex(const int index,
                                                          const int after_value)
{
  int                    i(1);
  bool                    found(false);
  NCollection_List<int>::Iterator itr(myList);
  for (; itr.More(); itr.Next(), ++i)
  {
    if (i == index)
    {
      Backup();
      myList.InsertAfter(after_value, itr);
      found = true;
      break;
    }
  }
  return found;
}

//=================================================================================================

bool TDataStd_IntegerList::Remove(const int value)
{
  NCollection_List<int>::Iterator itr(myList);
  for (; itr.More(); itr.Next())
  {
    if (itr.Value() == value)
    {
      Backup();
      myList.Remove(itr);
      return true;
    }
  }
  return false;
}

//=======================================================================
// function : Remove
// purpose  : Removes the <value> at the <index> position.
//=======================================================================
bool TDataStd_IntegerList::RemoveByIndex(const int index)
{
  int                    i(1);
  bool                    found(false);
  NCollection_List<int>::Iterator itr(myList);
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

void TDataStd_IntegerList::Clear()
{
  Backup();
  myList.Clear();
}

//=================================================================================================

int TDataStd_IntegerList::First() const
{
  return myList.First();
}

//=================================================================================================

int TDataStd_IntegerList::Last() const
{
  return myList.Last();
}

//=================================================================================================

const NCollection_List<int>& TDataStd_IntegerList::List() const
{
  return myList;
}

//=================================================================================================

const Standard_GUID& TDataStd_IntegerList::ID() const
{
  return myID;
}

//=================================================================================================

void TDataStd_IntegerList::SetID(const Standard_GUID& theGuid)
{
  if (myID == theGuid)
    return;
  Backup();
  myID = theGuid;
}

//=================================================================================================

void TDataStd_IntegerList::SetID()
{
  Backup();
  myID = GetID();
}

//=================================================================================================

occ::handle<TDF_Attribute> TDataStd_IntegerList::NewEmpty() const
{
  return new TDataStd_IntegerList();
}

//=================================================================================================

void TDataStd_IntegerList::Restore(const occ::handle<TDF_Attribute>& With)
{
  myList.Clear();
  occ::handle<TDataStd_IntegerList>        aList = occ::down_cast<TDataStd_IntegerList>(With);
  NCollection_List<int>::Iterator itr(aList->List());
  for (; itr.More(); itr.Next())
  {
    myList.Append(itr.Value());
  }
  myID = aList->ID();
}

//=================================================================================================

void TDataStd_IntegerList::Paste(const occ::handle<TDF_Attribute>& Into,
                                 const occ::handle<TDF_RelocationTable>&) const
{
  occ::handle<TDataStd_IntegerList> aList = occ::down_cast<TDataStd_IntegerList>(Into);
  aList->Clear();
  NCollection_List<int>::Iterator itr(myList);
  for (; itr.More(); itr.Next())
  {
    aList->Append(itr.Value());
  }
  aList->SetID(myID);
}

//=================================================================================================

Standard_OStream& TDataStd_IntegerList::Dump(Standard_OStream& anOS) const
{
  anOS << "\nIntegerList: ";
  char sguid[Standard_GUID_SIZE_ALLOC];
  myID.ToCString(sguid);
  anOS << sguid;
  anOS << std::endl;
  return anOS;
}

//=================================================================================================

void TDataStd_IntegerList::DumpJson(Standard_OStream& theOStream, int theDepth) const
{
  OCCT_DUMP_TRANSIENT_CLASS_BEGIN(theOStream)

  OCCT_DUMP_BASE_CLASS(theOStream, theDepth, TDF_Attribute)

  for (NCollection_List<int>::Iterator aListIt(myList); aListIt.More(); aListIt.Next())
  {
    const int& aValue = aListIt.Value();
    OCCT_DUMP_FIELD_VALUE_NUMERICAL(theOStream, aValue)
  }
}
