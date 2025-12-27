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

#include <TDataStd_ReferenceList.hxx>

#include <Standard_Dump.hxx>
#include <Standard_GUID.hxx>
#include <Standard_Type.hxx>
#include <TDF_Attribute.hxx>
#include <TDF_DataSet.hxx>
#include <TDF_Label.hxx>
#include <TDF_RelocationTable.hxx>
#include <TDF_Tool.hxx>

IMPLEMENT_STANDARD_RTTIEXT(TDataStd_ReferenceList, TDF_Attribute)

//=================================================================================================

const Standard_GUID& TDataStd_ReferenceList::GetID()
{
  static Standard_GUID TDataStd_ReferenceListID("FCC1A658-59FF-4218-931B-0320A2B469A7");
  return TDataStd_ReferenceListID;
}

//=======================================================================
// function : SetAttr
// purpose  : Implements Set functionality
//=======================================================================
static occ::handle<TDataStd_ReferenceList> SetAttr(const TDF_Label& label, const Standard_GUID& theGuid)
{
  occ::handle<TDataStd_ReferenceList> A;
  if (!label.FindAttribute(theGuid, A))
  {
    A = new TDataStd_ReferenceList;
    A->SetID(theGuid);
    label.AddAttribute(A);
  }
  return A;
}

//=================================================================================================

TDataStd_ReferenceList::TDataStd_ReferenceList()
    : myID(GetID())
{
}

//=================================================================================================

occ::handle<TDataStd_ReferenceList> TDataStd_ReferenceList::Set(const TDF_Label& label)
{
  return SetAttr(label, GetID());
}

//=======================================================================
// function : Set
// purpose  : Set user defined attribute with specific ID
//=======================================================================
occ::handle<TDataStd_ReferenceList> TDataStd_ReferenceList::Set(const TDF_Label&     label,
                                                           const Standard_GUID& theGuid)
{
  return SetAttr(label, theGuid);
}

//=================================================================================================

bool TDataStd_ReferenceList::IsEmpty() const
{
  return myList.IsEmpty();
}

//=================================================================================================

int TDataStd_ReferenceList::Extent() const
{
  return myList.Extent();
}

//=================================================================================================

void TDataStd_ReferenceList::Prepend(const TDF_Label& value)
{
  Backup();
  myList.Prepend(value);
}

//=================================================================================================

void TDataStd_ReferenceList::Append(const TDF_Label& value)
{
  Backup();
  myList.Append(value);
}

//=================================================================================================

bool TDataStd_ReferenceList::InsertBefore(const TDF_Label& value,
                                                      const TDF_Label& before_value)
{
  NCollection_List<TDF_Label>::Iterator itr(myList);
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

// Inserts the label before the <index> position.
// The indices start with 1 .. Extent().
bool TDataStd_ReferenceList::InsertBefore(const int index,
                                                      const TDF_Label&       before_value)
{
  int            i(1);
  bool            found(false);
  NCollection_List<TDF_Label>::Iterator itr(myList);
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

bool TDataStd_ReferenceList::InsertAfter(const TDF_Label& value,
                                                     const TDF_Label& after_value)
{
  NCollection_List<TDF_Label>::Iterator itr(myList);
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

// Inserts the label after the <index> position.
// The indices start with 1 .. Extent().
bool TDataStd_ReferenceList::InsertAfter(const int index,
                                                     const TDF_Label&       after_value)
{
  int            i(1);
  bool            found(false);
  NCollection_List<TDF_Label>::Iterator itr(myList);
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

bool TDataStd_ReferenceList::Remove(const TDF_Label& value)
{
  NCollection_List<TDF_Label>::Iterator itr(myList);
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
// purpose  : Removes a label at the <index> position.
//=======================================================================
bool TDataStd_ReferenceList::Remove(const int index)
{
  int            i(1);
  bool            found(false);
  NCollection_List<TDF_Label>::Iterator itr(myList);
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

void TDataStd_ReferenceList::Clear()
{
  Backup();
  myList.Clear();
}

//=================================================================================================

const TDF_Label& TDataStd_ReferenceList::First() const
{
  return myList.First();
}

//=================================================================================================

const TDF_Label& TDataStd_ReferenceList::Last() const
{
  return myList.Last();
}

//=================================================================================================

const NCollection_List<TDF_Label>& TDataStd_ReferenceList::List() const
{
  return myList;
}

//=================================================================================================

const Standard_GUID& TDataStd_ReferenceList::ID() const
{
  return myID;
}

//=================================================================================================

void TDataStd_ReferenceList::SetID(const Standard_GUID& theGuid)
{
  if (myID == theGuid)
    return;
  Backup();
  myID = theGuid;
}

//=================================================================================================

void TDataStd_ReferenceList::SetID()
{
  Backup();
  myID = GetID();
}

//=================================================================================================

occ::handle<TDF_Attribute> TDataStd_ReferenceList::NewEmpty() const
{
  return new TDataStd_ReferenceList();
}

//=================================================================================================

void TDataStd_ReferenceList::Restore(const occ::handle<TDF_Attribute>& With)
{
  myList.Clear();
  occ::handle<TDataStd_ReferenceList> aList = occ::down_cast<TDataStd_ReferenceList>(With);
  NCollection_List<TDF_Label>::Iterator    itr(aList->List());
  for (; itr.More(); itr.Next())
  {
    myList.Append(itr.Value());
  }
  myID = aList->ID();
}

//=================================================================================================

void TDataStd_ReferenceList::Paste(const occ::handle<TDF_Attribute>&       Into,
                                   const occ::handle<TDF_RelocationTable>& RT) const
{
  occ::handle<TDataStd_ReferenceList> aList = occ::down_cast<TDataStd_ReferenceList>(Into);
  aList->Clear();
  NCollection_List<TDF_Label>::Iterator itr(myList);
  for (; itr.More(); itr.Next())
  {
    TDF_Label L = itr.Value(), rL;
    if (!L.IsNull())
    {
      if (!RT->HasRelocation(L, rL))
        rL = L;
      aList->Append(rL);
    }
  }
  aList->SetID(myID);
}

//=======================================================================
// function : References
// purpose  : Adds the referenced attributes or labels.
//=======================================================================
void TDataStd_ReferenceList::References(const occ::handle<TDF_DataSet>& aDataSet) const
{
  if (!Label().IsImported())
  {
    NCollection_List<TDF_Label>::Iterator itr(myList);
    for (; itr.More(); itr.Next())
    {
      aDataSet->AddLabel(itr.Value());
    }
  }
}

//=================================================================================================

Standard_OStream& TDataStd_ReferenceList::Dump(Standard_OStream& anOS) const
{
  anOS << "\nReferenceList: ";
  char sguid[Standard_GUID_SIZE_ALLOC];
  myID.ToCString(sguid);
  anOS << sguid;
  anOS << std::endl;
  return anOS;
}

//=================================================================================================

void TDataStd_ReferenceList::DumpJson(Standard_OStream& theOStream, int theDepth) const
{
  OCCT_DUMP_TRANSIENT_CLASS_BEGIN(theOStream)

  OCCT_DUMP_BASE_CLASS(theOStream, theDepth, TDF_Attribute)

  TCollection_AsciiString aLabel;
  for (NCollection_List<TDF_Label>::Iterator aListIt(myList); aListIt.More(); aListIt.Next())
  {
    aLabel.Clear();
    TDF_Tool::Entry(aListIt.Value(), aLabel);
    OCCT_DUMP_FIELD_VALUE_STRING(theOStream, aLabel)
  }
}
