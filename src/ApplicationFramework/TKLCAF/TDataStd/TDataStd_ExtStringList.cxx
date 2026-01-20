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

#include <TDataStd_ExtStringList.hxx>

#include <Standard_Dump.hxx>
#include <Standard_GUID.hxx>
#include <Standard_Type.hxx>
#include <TCollection_ExtendedString.hxx>
#include <TCollection_ExtendedString.hxx>
#include <NCollection_List.hxx>
#include <TDF_Attribute.hxx>
#include <TDF_Label.hxx>
#include <TDF_RelocationTable.hxx>

IMPLEMENT_STANDARD_RTTIEXT(TDataStd_ExtStringList, TDF_Attribute)

//=================================================================================================

const Standard_GUID& TDataStd_ExtStringList::GetID()
{
  static Standard_GUID TDataStd_ExtStringListID("D13FBE0A-E084-4912-A99D-7713C59C0AC4");
  return TDataStd_ExtStringListID;
}

//=======================================================================
// function : SetAttr
// purpose  : Implements Set functionality
//=======================================================================
static occ::handle<TDataStd_ExtStringList> SetAttr(const TDF_Label& label, const Standard_GUID& theGuid)
{
  occ::handle<TDataStd_ExtStringList> A;
  if (!label.FindAttribute(theGuid, A))
  {
    A = new TDataStd_ExtStringList;
    A->SetID(theGuid);
    label.AddAttribute(A);
  }
  return A;
}

//=================================================================================================

TDataStd_ExtStringList::TDataStd_ExtStringList()
    : myID(GetID())
{
}

//=================================================================================================

occ::handle<TDataStd_ExtStringList> TDataStd_ExtStringList::Set(const TDF_Label& label)
{
  return SetAttr(label, GetID());
}

//=======================================================================
// function : Set
// purpose  : Set user defined attribute with specific ID
//=======================================================================
occ::handle<TDataStd_ExtStringList> TDataStd_ExtStringList::Set(const TDF_Label&     label,
                                                           const Standard_GUID& theGuid)
{
  return SetAttr(label, theGuid);
}

//=================================================================================================

bool TDataStd_ExtStringList::IsEmpty() const
{
  return myList.IsEmpty();
}

//=================================================================================================

int TDataStd_ExtStringList::Extent() const
{
  return myList.Extent();
}

//=================================================================================================

void TDataStd_ExtStringList::Prepend(const TCollection_ExtendedString& value)
{
  Backup();
  myList.Prepend(value);
}

//=================================================================================================

void TDataStd_ExtStringList::Append(const TCollection_ExtendedString& value)
{
  Backup();
  myList.Append(value);
}

//=================================================================================================

bool TDataStd_ExtStringList::InsertBefore(
  const TCollection_ExtendedString& value,
  const TCollection_ExtendedString& before_value)
{
  NCollection_List<TCollection_ExtendedString>::Iterator itr(myList);
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

//=======================================================================
// function : InsertBefore
// purpose  : Inserts the <value> before the <index> position.
//=======================================================================
bool TDataStd_ExtStringList::InsertBefore(
  const int            index,
  const TCollection_ExtendedString& before_value)
{
  int                            i(1);
  bool                            found(false);
  NCollection_List<TCollection_ExtendedString>::Iterator itr(myList);
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

bool TDataStd_ExtStringList::InsertAfter(const TCollection_ExtendedString& value,
                                                     const TCollection_ExtendedString& after_value)
{
  NCollection_List<TCollection_ExtendedString>::Iterator itr(myList);
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

//=======================================================================
// function : InsertAfter
// purpose  : Inserts the <value> after the <index> position.
//=======================================================================
bool TDataStd_ExtStringList::InsertAfter(const int            index,
                                                     const TCollection_ExtendedString& after_value)
{
  int                            i(1);
  bool                            found(false);
  NCollection_List<TCollection_ExtendedString>::Iterator itr(myList);
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

bool TDataStd_ExtStringList::Remove(const TCollection_ExtendedString& value)
{
  NCollection_List<TCollection_ExtendedString>::Iterator itr(myList);
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
// purpose  : Removes a value at <index> position.
//=======================================================================
bool TDataStd_ExtStringList::Remove(const int index)
{
  int                            i(1);
  bool                            found(false);
  NCollection_List<TCollection_ExtendedString>::Iterator itr(myList);
  for (; itr.More(); itr.Next(), ++i)
  {
    if (index == i)
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

void TDataStd_ExtStringList::Clear()
{
  Backup();
  myList.Clear();
}

//=================================================================================================

const TCollection_ExtendedString& TDataStd_ExtStringList::First() const
{
  return myList.First();
}

//=================================================================================================

const TCollection_ExtendedString& TDataStd_ExtStringList::Last() const
{
  return myList.Last();
}

//=================================================================================================

const NCollection_List<TCollection_ExtendedString>& TDataStd_ExtStringList::List() const
{
  return myList;
}

//=================================================================================================

const Standard_GUID& TDataStd_ExtStringList::ID() const
{
  return myID;
}

//=================================================================================================

void TDataStd_ExtStringList::SetID(const Standard_GUID& theGuid)
{
  if (myID == theGuid)
    return;
  Backup();
  myID = theGuid;
}

//=================================================================================================

void TDataStd_ExtStringList::SetID()
{
  Backup();
  myID = GetID();
}

//=================================================================================================

occ::handle<TDF_Attribute> TDataStd_ExtStringList::NewEmpty() const
{
  return new TDataStd_ExtStringList();
}

//=================================================================================================

void TDataStd_ExtStringList::Restore(const occ::handle<TDF_Attribute>& With)
{
  myList.Clear();
  occ::handle<TDataStd_ExtStringList> aList = occ::down_cast<TDataStd_ExtStringList>(With);
  NCollection_List<TCollection_ExtendedString>::Iterator itr(aList->List());
  for (; itr.More(); itr.Next())
  {
    myList.Append(itr.Value());
  }
  myID = aList->ID();
}

//=================================================================================================

void TDataStd_ExtStringList::Paste(const occ::handle<TDF_Attribute>& Into,
                                   const occ::handle<TDF_RelocationTable>&) const
{
  occ::handle<TDataStd_ExtStringList> aList = occ::down_cast<TDataStd_ExtStringList>(Into);
  aList->Clear();
  NCollection_List<TCollection_ExtendedString>::Iterator itr(myList);
  for (; itr.More(); itr.Next())
  {
    aList->Append(itr.Value());
  }
  aList->SetID(myID);
}

//=================================================================================================

Standard_OStream& TDataStd_ExtStringList::Dump(Standard_OStream& anOS) const
{
  anOS << "\nExtStringList: ";
  char sguid[Standard_GUID_SIZE_ALLOC];
  myID.ToCString(sguid);
  anOS << sguid;
  anOS << std::endl;
  return anOS;
}

//=================================================================================================

void TDataStd_ExtStringList::DumpJson(Standard_OStream& theOStream, int theDepth) const
{
  OCCT_DUMP_TRANSIENT_CLASS_BEGIN(theOStream)

  OCCT_DUMP_BASE_CLASS(theOStream, theDepth, TDF_Attribute)

  for (NCollection_List<TCollection_ExtendedString>::Iterator aListIt(myList); aListIt.More(); aListIt.Next())
  {
    const TCollection_ExtendedString& aValue = aListIt.Value();
    OCCT_DUMP_FIELD_VALUE_STRING(theOStream, aValue);
  }
}
