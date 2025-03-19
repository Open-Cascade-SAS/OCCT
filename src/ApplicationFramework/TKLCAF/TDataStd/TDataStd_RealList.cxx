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
#include <TColStd_ListIteratorOfListOfReal.hxx>
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
static Handle(TDataStd_RealList) SetAttr(const TDF_Label& label, const Standard_GUID& theGuid)
{
  Handle(TDataStd_RealList) A;
  if (!label.FindAttribute(theGuid, A))
  {
    A = new TDataStd_RealList;
    A->SetID(theGuid);
    label.AddAttribute(A);
  }
  return A;
}

//=======================================================================
// function : TDataStd_RealList
// purpose  : Empty Constructor
//=======================================================================
TDataStd_RealList::TDataStd_RealList()
    : myID(GetID())
{
}

//=================================================================================================

Handle(TDataStd_RealList) TDataStd_RealList::Set(const TDF_Label& label)
{
  return SetAttr(label, GetID());
}

//=======================================================================
// function : Set
// purpose  : Set user defined attribute with specific ID
//=======================================================================
Handle(TDataStd_RealList) TDataStd_RealList::Set(const TDF_Label&     label,
                                                 const Standard_GUID& theGuid)
{
  return SetAttr(label, theGuid);
}

//=================================================================================================

Standard_Boolean TDataStd_RealList::IsEmpty() const
{
  return myList.IsEmpty();
}

//=================================================================================================

Standard_Integer TDataStd_RealList::Extent() const
{
  return myList.Extent();
}

//=================================================================================================

void TDataStd_RealList::Prepend(const Standard_Real value)
{
  Backup();
  myList.Prepend(value);
}

//=================================================================================================

void TDataStd_RealList::Append(const Standard_Real value)
{
  Backup();
  myList.Append(value);
}

//=================================================================================================

Standard_Boolean TDataStd_RealList::InsertBefore(const Standard_Real value,
                                                 const Standard_Real before_value)
{
  TColStd_ListIteratorOfListOfReal itr(myList);
  for (; itr.More(); itr.Next())
  {
    if (itr.Value() == before_value)
    {
      Backup();
      myList.InsertBefore(value, itr);
      return Standard_True;
    }
  }
  return Standard_False;
}

// Inserts the <value> before the <index> position.
// The indices start with 1 .. Extent().
Standard_Boolean TDataStd_RealList::InsertBeforeByIndex(const Standard_Integer index,
                                                        const Standard_Real    before_value)
{
  Standard_Integer                 i(1);
  Standard_Boolean                 found(Standard_False);
  TColStd_ListIteratorOfListOfReal itr(myList);
  for (; itr.More(); itr.Next(), ++i)
  {
    if (i == index)
    {
      Backup();
      myList.InsertBefore(before_value, itr);
      found = Standard_True;
      break;
    }
  }
  return found;
}

//=================================================================================================

Standard_Boolean TDataStd_RealList::InsertAfter(const Standard_Real value,
                                                const Standard_Real after_value)
{
  TColStd_ListIteratorOfListOfReal itr(myList);
  for (; itr.More(); itr.Next())
  {
    if (itr.Value() == after_value)
    {
      Backup();
      myList.InsertAfter(value, itr);
      return Standard_True;
    }
  }
  return Standard_False;
}

// Inserts the <value> after the <index> position.
// The indices start with 1 .. Extent().
Standard_Boolean TDataStd_RealList::InsertAfterByIndex(const Standard_Integer index,
                                                       const Standard_Real    after_value)
{
  Standard_Integer                 i(1);
  Standard_Boolean                 found(Standard_False);
  TColStd_ListIteratorOfListOfReal itr(myList);
  for (; itr.More(); itr.Next(), ++i)
  {
    if (i == index)
    {
      Backup();
      myList.InsertAfter(after_value, itr);
      found = Standard_True;
      break;
    }
  }
  return found;
}

//=================================================================================================

Standard_Boolean TDataStd_RealList::Remove(const Standard_Real value)
{
  TColStd_ListIteratorOfListOfReal itr(myList);
  for (; itr.More(); itr.Next())
  {
    if (itr.Value() == value)
    {
      Backup();
      myList.Remove(itr);
      return Standard_True;
    }
  }
  return Standard_False;
}

//=======================================================================
// function : Remove
// purpose  : Removes a value at the <index> position.
//=======================================================================
Standard_Boolean TDataStd_RealList::RemoveByIndex(const Standard_Integer index)
{
  Standard_Integer                 i(1);
  Standard_Boolean                 found(Standard_False);
  TColStd_ListIteratorOfListOfReal itr(myList);
  for (; itr.More(); itr.Next(), ++i)
  {
    if (i == index)
    {
      Backup();
      myList.Remove(itr);
      found = Standard_True;
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

Standard_Real TDataStd_RealList::First() const
{
  return myList.First();
}

//=================================================================================================

Standard_Real TDataStd_RealList::Last() const
{
  return myList.Last();
}

//=================================================================================================

const TColStd_ListOfReal& TDataStd_RealList::List() const
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

//=======================================================================
// function : SetID
// purpose  : sets default ID
//=======================================================================
void TDataStd_RealList::SetID()
{
  Backup();
  myID = GetID();
}

//=================================================================================================

Handle(TDF_Attribute) TDataStd_RealList::NewEmpty() const
{
  return new TDataStd_RealList();
}

//=================================================================================================

void TDataStd_RealList::Restore(const Handle(TDF_Attribute)& With)
{
  myList.Clear();
  Handle(TDataStd_RealList)        aList = Handle(TDataStd_RealList)::DownCast(With);
  TColStd_ListIteratorOfListOfReal itr(aList->List());
  for (; itr.More(); itr.Next())
  {
    myList.Append(itr.Value());
  }
  myID = aList->ID();
}

//=================================================================================================

void TDataStd_RealList::Paste(const Handle(TDF_Attribute)& Into,
                              const Handle(TDF_RelocationTable)&) const
{
  Handle(TDataStd_RealList) aList = Handle(TDataStd_RealList)::DownCast(Into);
  aList->Clear();
  TColStd_ListIteratorOfListOfReal itr(myList);
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
  Standard_Character sguid[Standard_GUID_SIZE_ALLOC];
  myID.ToCString(sguid);
  anOS << sguid;
  anOS << std::endl;
  return anOS;
}

//=================================================================================================

void TDataStd_RealList::DumpJson(Standard_OStream& theOStream, Standard_Integer theDepth) const
{
  OCCT_DUMP_TRANSIENT_CLASS_BEGIN(theOStream)

  OCCT_DUMP_BASE_CLASS(theOStream, theDepth, TDF_Attribute)

  for (TColStd_ListOfReal::Iterator aListIt(myList); aListIt.More(); aListIt.Next())
  {
    const Standard_Real& aValue = aListIt.Value();
    OCCT_DUMP_FIELD_VALUE_NUMERICAL(theOStream, aValue)
  }
}
