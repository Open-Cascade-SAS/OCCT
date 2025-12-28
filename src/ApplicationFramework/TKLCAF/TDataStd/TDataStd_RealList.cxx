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

#include <TDataStd_RealList.hxx>

#include <Standard_Dump.hxx>
#include <Standard_GUID.hxx>
#include <Standard_Type.hxx>
#include <NCollection_List.hxx>
#include <TDF_Attribute.hxx>
#include <TDF_Label.hxx>
#include <TDF_RelocationTable.hxx>

IMPLEMENT_STANDARD_RTTIEXT(TDataStd_RealList, TDF_Attribute)

//=================================================================================================

const Standard_GUID& TDataStd_RealList::GetID()
{
  static Standard_GUID TDataStd_RealListID("349ACE18-7CD6-4525-9938-FBBF22AA54D3");
  return TDataStd_RealListID;
}

//=======================================================================
// function : SetAttr
// purpose  : Implements Set functionality
//=======================================================================
static occ::handle<TDataStd_RealList> SetAttr(const TDF_Label& label, const Standard_GUID& theGuid)
{
  occ::handle<TDataStd_RealList> A;
  if (!label.FindAttribute(theGuid, A))
  {
    A = new TDataStd_RealList;
    A->SetID(theGuid);
    label.AddAttribute(A);
  }
  return A;
}

//=================================================================================================

TDataStd_RealList::TDataStd_RealList()
    : myID(GetID())
{
}

//=================================================================================================

occ::handle<TDataStd_RealList> TDataStd_RealList::Set(const TDF_Label& label)
{
  return SetAttr(label, GetID());
}

//=======================================================================
// function : Set
// purpose  : Set user defined attribute with specific ID
//=======================================================================
occ::handle<TDataStd_RealList> TDataStd_RealList::Set(const TDF_Label&     label,
                                                 const Standard_GUID& theGuid)
{
  return SetAttr(label, theGuid);
}

//=================================================================================================

bool TDataStd_RealList::IsEmpty() const
{
  return myList.IsEmpty();
}

//=================================================================================================

int TDataStd_RealList::Extent() const
{
  return myList.Extent();
}

//=================================================================================================

void TDataStd_RealList::Prepend(const double value)
{
  Backup();
  myList.Prepend(value);
}

//=================================================================================================

void TDataStd_RealList::Append(const double value)
{
  Backup();
  myList.Append(value);
}

//=================================================================================================

bool TDataStd_RealList::InsertBefore(const double value,
                                                 const double before_value)
{
  NCollection_List<double>::Iterator itr(myList);
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
bool TDataStd_RealList::InsertBeforeByIndex(const int index,
                                                        const double    before_value)
{
  int                 i(1);
  bool                 found(false);
  NCollection_List<double>::Iterator itr(myList);
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

bool TDataStd_RealList::InsertAfter(const double value,
                                                const double after_value)
{
  NCollection_List<double>::Iterator itr(myList);
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
bool TDataStd_RealList::InsertAfterByIndex(const int index,
                                                       const double    after_value)
{
  int                 i(1);
  bool                 found(false);
  NCollection_List<double>::Iterator itr(myList);
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

bool TDataStd_RealList::Remove(const double value)
{
  NCollection_List<double>::Iterator itr(myList);
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
// purpose  : Removes a value at the <index> position.
//=======================================================================
bool TDataStd_RealList::RemoveByIndex(const int index)
{
  int                 i(1);
  bool                 found(false);
  NCollection_List<double>::Iterator itr(myList);
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

void TDataStd_RealList::Clear()
{
  Backup();
  myList.Clear();
}

//=================================================================================================

double TDataStd_RealList::First() const
{
  return myList.First();
}

//=================================================================================================

double TDataStd_RealList::Last() const
{
  return myList.Last();
}

//=================================================================================================

const NCollection_List<double>& TDataStd_RealList::List() const
{
  return myList;
}

//=================================================================================================

const Standard_GUID& TDataStd_RealList::ID() const
{
  return myID;
}

//=================================================================================================

void TDataStd_RealList::SetID(const Standard_GUID& theGuid)
{
  if (myID == theGuid)
    return;
  Backup();
  myID = theGuid;
}

//=================================================================================================

void TDataStd_RealList::SetID()
{
  Backup();
  myID = GetID();
}

//=================================================================================================

occ::handle<TDF_Attribute> TDataStd_RealList::NewEmpty() const
{
  return new TDataStd_RealList();
}

//=================================================================================================

void TDataStd_RealList::Restore(const occ::handle<TDF_Attribute>& With)
{
  myList.Clear();
  occ::handle<TDataStd_RealList>        aList = occ::down_cast<TDataStd_RealList>(With);
  NCollection_List<double>::Iterator itr(aList->List());
  for (; itr.More(); itr.Next())
  {
    myList.Append(itr.Value());
  }
  myID = aList->ID();
}

//=================================================================================================

void TDataStd_RealList::Paste(const occ::handle<TDF_Attribute>& Into,
                              const occ::handle<TDF_RelocationTable>&) const
{
  occ::handle<TDataStd_RealList> aList = occ::down_cast<TDataStd_RealList>(Into);
  aList->Clear();
  NCollection_List<double>::Iterator itr(myList);
  for (; itr.More(); itr.Next())
  {
    aList->Append(itr.Value());
  }
  aList->SetID(myID);
}

//=================================================================================================

Standard_OStream& TDataStd_RealList::Dump(Standard_OStream& anOS) const
{
  anOS << "\nRealList: ";
  char sguid[Standard_GUID_SIZE_ALLOC];
  myID.ToCString(sguid);
  anOS << sguid;
  anOS << std::endl;
  return anOS;
}

//=================================================================================================

void TDataStd_RealList::DumpJson(Standard_OStream& theOStream, int theDepth) const
{
  OCCT_DUMP_TRANSIENT_CLASS_BEGIN(theOStream)

  OCCT_DUMP_BASE_CLASS(theOStream, theDepth, TDF_Attribute)

  for (NCollection_List<double>::Iterator aListIt(myList); aListIt.More(); aListIt.Next())
  {
    const double& aValue = aListIt.Value();
    OCCT_DUMP_FIELD_VALUE_NUMERICAL(theOStream, aValue)
  }
}
