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


#include <Standard_GUID.hxx>
#include <Standard_Type.hxx>
#include <TColStd_ListIteratorOfListOfInteger.hxx>
#include <TDataStd_IntegerList.hxx>
#include <TDF_Attribute.hxx>
#include <TDF_Label.hxx>
#include <TDF_RelocationTable.hxx>

IMPLEMENT_STANDARD_RTTIEXT(TDataStd_IntegerList,TDF_Attribute)

//=======================================================================
//function : GetID
//purpose  : 
//=======================================================================
const Standard_GUID& TDataStd_IntegerList::GetID() 
{ 
  static Standard_GUID TDataStd_IntegerListID ("E406AA18-FF3F-483b-9A78-1A5EA5D1AA52");
  return TDataStd_IntegerListID; 
}

//=======================================================================
//function : SetAttr
//purpose  : Implements Set functionality
//=======================================================================
static Handle(TDataStd_IntegerList) SetAttr(const TDF_Label&       label,
                                            const Standard_GUID&   theGuid) 
{
  Handle(TDataStd_IntegerList) A;
  if (!label.FindAttribute (theGuid, A)) 
  {
    A = new TDataStd_IntegerList;
    A->SetID(theGuid);
    label.AddAttribute(A);
  }
  return A;
}

//=======================================================================
//function : TDataStd_IntegerList
//purpose  : Empty Constructor
//=======================================================================
TDataStd_IntegerList::TDataStd_IntegerList() 
{

}

//=======================================================================
//function : Set
//purpose  : 
//=======================================================================
Handle(TDataStd_IntegerList) TDataStd_IntegerList::Set(const TDF_Label& label) 
{
  return SetAttr(label, GetID());
}

//=======================================================================
//function : Set
//purpose  : Set user defined attribute with specific ID
//=======================================================================
Handle(TDataStd_IntegerList) TDataStd_IntegerList::Set(const TDF_Label& label,
                                                       const Standard_GUID& theGuid) 
{
  return SetAttr(label, theGuid);
}

//=======================================================================
//function : IsEmpty
//purpose  : 
//=======================================================================
Standard_Boolean TDataStd_IntegerList::IsEmpty() const
{
  return myList.IsEmpty();
}

//=======================================================================
//function : Extent
//purpose  : 
//=======================================================================
Standard_Integer TDataStd_IntegerList::Extent() const
{
  return myList.Extent();
}

//=======================================================================
//function : Prepend
//purpose  : 
//=======================================================================
void TDataStd_IntegerList::Prepend(const Standard_Integer value)
{
  Backup();
  myList.Prepend(value);
}

//=======================================================================
//function : Append
//purpose  : 
//=======================================================================
void TDataStd_IntegerList::Append(const Standard_Integer value)
{
  Backup();
  myList.Append(value);
}

//=======================================================================
//function : InsertBefore
//purpose  : 
//=======================================================================
Standard_Boolean TDataStd_IntegerList::InsertBefore(const Standard_Integer value,
                                                    const Standard_Integer before_value)
{
  TColStd_ListIteratorOfListOfInteger itr(myList);
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
Standard_Boolean TDataStd_IntegerList::InsertBeforeByIndex (const Standard_Integer index,
                                                            const Standard_Integer before_value)
{
  Standard_Integer i(1);
  Standard_Boolean found(Standard_False);
  TColStd_ListIteratorOfListOfInteger itr(myList);
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

//=======================================================================
//function : InsertAfter
//purpose  : 
//=======================================================================
Standard_Boolean TDataStd_IntegerList::InsertAfter(const Standard_Integer value,
                                                   const Standard_Integer after_value)
{
  TColStd_ListIteratorOfListOfInteger itr(myList);
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
Standard_Boolean TDataStd_IntegerList::InsertAfterByIndex (const Standard_Integer index,
                                                           const Standard_Integer after_value)
{
  Standard_Integer i(1);
  Standard_Boolean found(Standard_False);
  TColStd_ListIteratorOfListOfInteger itr(myList);
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

//=======================================================================
//function : Remove
//purpose  : 
//=======================================================================
Standard_Boolean TDataStd_IntegerList::Remove(const Standard_Integer value)
{
  TColStd_ListIteratorOfListOfInteger itr(myList);
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
//function : Remove
//purpose  : Removes the <value> at the <index> position.
//=======================================================================
Standard_Boolean TDataStd_IntegerList::RemoveByIndex (const Standard_Integer index)
{
  Standard_Integer i(1);
  Standard_Boolean found(Standard_False);
  TColStd_ListIteratorOfListOfInteger itr(myList);
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

//=======================================================================
//function : Clear
//purpose  : 
//=======================================================================
void TDataStd_IntegerList::Clear()
{
  Backup();
  myList.Clear();
}

//=======================================================================
//function : First
//purpose  : 
//=======================================================================
Standard_Integer TDataStd_IntegerList::First() const
{
  return myList.First();
}

//=======================================================================
//function : Last
//purpose  : 
//=======================================================================
Standard_Integer TDataStd_IntegerList::Last() const
{
  return myList.Last();
}

//=======================================================================
//function : List
//purpose  : 
//=======================================================================
const TColStd_ListOfInteger& TDataStd_IntegerList::List() const
{
  return myList;
}

//=======================================================================
//function : ID
//purpose  : 
//=======================================================================
const Standard_GUID& TDataStd_IntegerList::ID () const 
{ 
  return myID; 
}

//=======================================================================
//function : SetID
//purpose  :
//=======================================================================

void TDataStd_IntegerList::SetID( const Standard_GUID&  theGuid)
{  
  if(myID == theGuid) return;
  Backup();
  myID = theGuid;
}

//=======================================================================
//function : SetID
//purpose  : sets default ID
//=======================================================================

void TDataStd_IntegerList::SetID()
{  
  Backup();
  myID = GetID();
}

//=======================================================================
//function : NewEmpty
//purpose  : 
//=======================================================================
Handle(TDF_Attribute) TDataStd_IntegerList::NewEmpty () const
{  
  return new TDataStd_IntegerList(); 
}

//=======================================================================
//function : Restore
//purpose  : 
//=======================================================================
void TDataStd_IntegerList::Restore(const Handle(TDF_Attribute)& With) 
{
  myList.Clear();
  Handle(TDataStd_IntegerList) aList = Handle(TDataStd_IntegerList)::DownCast(With);
  TColStd_ListIteratorOfListOfInteger itr(aList->List());
  for (; itr.More(); itr.Next())
  {
    myList.Append(itr.Value());
  }
  myID = aList->ID();
}

//=======================================================================
//function : Paste
//purpose  : 
//=======================================================================
void TDataStd_IntegerList::Paste (const Handle(TDF_Attribute)& Into,
                                  const Handle(TDF_RelocationTable)& ) const
{
  Handle(TDataStd_IntegerList) aList = Handle(TDataStd_IntegerList)::DownCast(Into);
  aList->Clear();
  TColStd_ListIteratorOfListOfInteger itr(myList);
  for (; itr.More(); itr.Next())
  {
    aList->Append(itr.Value());
  }
  aList->SetID(myID);
}

//=======================================================================
//function : Dump
//purpose  : 
//=======================================================================
Standard_OStream& TDataStd_IntegerList::Dump (Standard_OStream& anOS) const
{  
  anOS << "\nIntegerList: ";
  Standard_Character sguid[Standard_GUID_SIZE_ALLOC];
  myID.ToCString(sguid);
  anOS << sguid;
  anOS << endl;
  return anOS;
}
