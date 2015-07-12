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
#include <TCollection_ExtendedString.hxx>
#include <TDataStd_ExtStringList.hxx>
#include <TDataStd_ListIteratorOfListOfExtendedString.hxx>
#include <TDF_Attribute.hxx>
#include <TDF_Label.hxx>
#include <TDF_RelocationTable.hxx>

//=======================================================================
//function : GetID
//purpose  : 
//=======================================================================
const Standard_GUID& TDataStd_ExtStringList::GetID() 
{ 
  static Standard_GUID TDataStd_ExtStringListID ("D13FBE0A-E084-4912-A99D-7713C59C0AC4");
  return TDataStd_ExtStringListID; 
}

//=======================================================================
//function : TDataStd_ExtStringList
//purpose  : Empty Constructor
//=======================================================================
TDataStd_ExtStringList::TDataStd_ExtStringList() 
{

}

//=======================================================================
//function : Set
//purpose  : 
//=======================================================================
Handle(TDataStd_ExtStringList) TDataStd_ExtStringList::Set(const TDF_Label& label) 
{
  Handle(TDataStd_ExtStringList) A;
  if (!label.FindAttribute (TDataStd_ExtStringList::GetID(), A)) 
  {
    A = new TDataStd_ExtStringList;
    label.AddAttribute(A);
  }
  return A;
}

//=======================================================================
//function : IsEmpty
//purpose  : 
//=======================================================================
Standard_Boolean TDataStd_ExtStringList::IsEmpty() const
{
  return myList.IsEmpty();
}

//=======================================================================
//function : Extent
//purpose  : 
//=======================================================================
Standard_Integer TDataStd_ExtStringList::Extent() const
{
  return myList.Extent();
}

//=======================================================================
//function : Prepend
//purpose  : 
//=======================================================================
void TDataStd_ExtStringList::Prepend(const TCollection_ExtendedString& value)
{
  Backup();
  myList.Prepend(value);
}

//=======================================================================
//function : Append
//purpose  : 
//=======================================================================
void TDataStd_ExtStringList::Append(const TCollection_ExtendedString& value)
{
  Backup();
  myList.Append(value);
}

//=======================================================================
//function : InsertBefore
//purpose  : 
//=======================================================================
Standard_Boolean TDataStd_ExtStringList::InsertBefore(const TCollection_ExtendedString& value,
						      const TCollection_ExtendedString& before_value)
{
  TDataStd_ListIteratorOfListOfExtendedString itr(myList);
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

//=======================================================================
//function : InsertAfter
//purpose  : 
//=======================================================================
Standard_Boolean TDataStd_ExtStringList::InsertAfter(const TCollection_ExtendedString& value,
						     const TCollection_ExtendedString& after_value)
{
  TDataStd_ListIteratorOfListOfExtendedString itr(myList);
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

//=======================================================================
//function : Remove
//purpose  : 
//=======================================================================
Standard_Boolean TDataStd_ExtStringList::Remove(const TCollection_ExtendedString& value)
{
  TDataStd_ListIteratorOfListOfExtendedString itr(myList);
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
//function : Clear
//purpose  : 
//=======================================================================
void TDataStd_ExtStringList::Clear()
{
  Backup();
  myList.Clear();
}

//=======================================================================
//function : First
//purpose  : 
//=======================================================================
const TCollection_ExtendedString& TDataStd_ExtStringList::First() const
{
  return myList.First();
}

//=======================================================================
//function : Last
//purpose  : 
//=======================================================================
const TCollection_ExtendedString& TDataStd_ExtStringList::Last() const
{
  return myList.Last();
}

//=======================================================================
//function : List
//purpose  : 
//=======================================================================
const TDataStd_ListOfExtendedString& TDataStd_ExtStringList::List() const
{
  return myList;
}

//=======================================================================
//function : ID
//purpose  : 
//=======================================================================
const Standard_GUID& TDataStd_ExtStringList::ID () const 
{ 
  return GetID(); 
}

//=======================================================================
//function : NewEmpty
//purpose  : 
//=======================================================================
Handle(TDF_Attribute) TDataStd_ExtStringList::NewEmpty () const
{  
  return new TDataStd_ExtStringList(); 
}

//=======================================================================
//function : Restore
//purpose  : 
//=======================================================================
void TDataStd_ExtStringList::Restore(const Handle(TDF_Attribute)& With) 
{
  myList.Clear();
  Handle(TDataStd_ExtStringList) aList = Handle(TDataStd_ExtStringList)::DownCast(With);
  TDataStd_ListIteratorOfListOfExtendedString itr(aList->List());
  for (; itr.More(); itr.Next())
  {
    myList.Append(itr.Value());
  }
}

//=======================================================================
//function : Paste
//purpose  : 
//=======================================================================
void TDataStd_ExtStringList::Paste (const Handle(TDF_Attribute)& Into,
				    const Handle(TDF_RelocationTable)& ) const
{
  Handle(TDataStd_ExtStringList) aList = Handle(TDataStd_ExtStringList)::DownCast(Into);
  aList->Clear();
  TDataStd_ListIteratorOfListOfExtendedString itr(myList);
  for (; itr.More(); itr.Next())
  {
    aList->Append(itr.Value());
  }
}

//=======================================================================
//function : Dump
//purpose  : 
//=======================================================================
Standard_OStream& TDataStd_ExtStringList::Dump (Standard_OStream& anOS) const
{  
  anOS << "ExtStringList";
  return anOS;
}
