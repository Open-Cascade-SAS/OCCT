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
#include <TDataStd_BooleanList.hxx>
#include <TDataStd_ListIteratorOfListOfByte.hxx>
#include <TDF_Attribute.hxx>
#include <TDF_Label.hxx>
#include <TDF_RelocationTable.hxx>

IMPLEMENT_STANDARD_RTTIEXT(TDataStd_BooleanList,TDF_Attribute)

//=======================================================================
//function : GetID
//purpose  : 
//=======================================================================
const Standard_GUID& TDataStd_BooleanList::GetID() 
{ 
  static Standard_GUID TDataStd_BooleanListID ("23A9D60E-A033-44d8-96EE-015587A41BBC");
  return TDataStd_BooleanListID; 
}

//=======================================================================
//function : SetAttr
//purpose  : Implements Set functionality
//=======================================================================
static Handle(TDataStd_BooleanList) SetAttr(const TDF_Label&       label,
                                            const Standard_GUID&   theGuid) 
{
  Handle(TDataStd_BooleanList) A;
  if (!label.FindAttribute (theGuid, A)) 
  {
    A = new TDataStd_BooleanList;
    A->SetID(theGuid);
    label.AddAttribute(A);
  }
  return A;
}

//=======================================================================
//function : TDataStd_BooleanList
//purpose  : Empty Constructor
//=======================================================================
TDataStd_BooleanList::TDataStd_BooleanList() 
{

}

//=======================================================================
//function : Set
//purpose  : 
//=======================================================================
Handle(TDataStd_BooleanList) TDataStd_BooleanList::Set(const TDF_Label& label) 
{
  return SetAttr(label, GetID());
}

//=======================================================================
//function : Set
//purpose  : Set user defined attribute with specific ID
//=======================================================================
Handle(TDataStd_BooleanList) TDataStd_BooleanList::Set(const TDF_Label& label,
                                                       const Standard_GUID&   theGuid) 
{
  return SetAttr(label, theGuid); 
}
//=======================================================================
//function : IsEmpty
//purpose  : 
//=======================================================================
Standard_Boolean TDataStd_BooleanList::IsEmpty() const
{
  return myList.IsEmpty();
}

//=======================================================================
//function : Extent
//purpose  : 
//=======================================================================
Standard_Integer TDataStd_BooleanList::Extent() const
{
  return myList.Extent();
}

//=======================================================================
//function : Prepend
//purpose  : 
//=======================================================================
void TDataStd_BooleanList::Prepend(const Standard_Boolean value)
{
  Backup();
  myList.Prepend( value ? 1 : 0 );
}

//=======================================================================
//function : Append
//purpose  : 
//=======================================================================
void TDataStd_BooleanList::Append(const Standard_Boolean value)
{
  Backup();
  myList.Append( value ? 1 : 0 );
}

//=======================================================================
//function : Clear
//purpose  : 
//=======================================================================
void TDataStd_BooleanList::Clear()
{
  Backup();
  myList.Clear();
}

//=======================================================================
//function : First
//purpose  : 
//=======================================================================
Standard_Boolean TDataStd_BooleanList::First() const
{
  return myList.First() == 1;
}

//=======================================================================
//function : Last
//purpose  : 
//=======================================================================
Standard_Boolean TDataStd_BooleanList::Last() const
{
  return myList.Last() == 1;
}

//=======================================================================
//function : List
//purpose  : 
//=======================================================================
const TDataStd_ListOfByte& TDataStd_BooleanList::List() const
{
  return myList;
}

//=======================================================================
//function : InsertBefore
//purpose  : Inserts the <value> before the <index> position.
//=======================================================================
Standard_Boolean TDataStd_BooleanList::InsertBefore(const Standard_Integer index,
                                                    const Standard_Boolean before_value)
{
  Standard_Integer i(1);
  Standard_Boolean found(Standard_False);
  TDataStd_ListIteratorOfListOfByte itr(myList);
  for (; itr.More(); itr.Next(), ++i)
  {
    if (i == index)
    {
      Backup();
      myList.InsertBefore(before_value ? 1 : 0, itr);
      found = Standard_True;
      break;
    }
  }
  return found;
}

//=======================================================================
//function : InsertAfter
//purpose  : Inserts the <value> after the <index> position.
//=======================================================================
Standard_Boolean TDataStd_BooleanList::InsertAfter(const Standard_Integer index,
                                                   const Standard_Boolean after_value)
{
  Standard_Integer i(1);
  Standard_Boolean found(Standard_False);
  TDataStd_ListIteratorOfListOfByte itr(myList);
  for (; itr.More(); itr.Next(), ++i)
  {
    if (i == index)
    {
      Backup();
      myList.InsertAfter(after_value ? 1 : 0, itr);
      found = Standard_True;
      break;
    }
  }
  return found;
}

//=======================================================================
//function : Remove
//purpose  : Removes the <value> at the <index> position.
//=======================================================================
Standard_Boolean TDataStd_BooleanList::Remove(const Standard_Integer index)
{
  Standard_Integer i(1);
  Standard_Boolean found(Standard_False);
  TDataStd_ListIteratorOfListOfByte itr(myList);
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
//function : ID
//purpose  : 
//=======================================================================
const Standard_GUID& TDataStd_BooleanList::ID () const 
{ 
  return myID; 
}

//=======================================================================
//function : SetID
//purpose  :
//=======================================================================

void TDataStd_BooleanList::SetID( const Standard_GUID&  theGuid)
{  
  if(myID == theGuid) return;
  Backup();
  myID = theGuid;
}

//=======================================================================
//function : SetID
//purpose  : sets default ID
//=======================================================================

void TDataStd_BooleanList::SetID()
{  
  Backup();
  myID = GetID();
}

//=======================================================================
//function : NewEmpty
//purpose  : 
//=======================================================================
Handle(TDF_Attribute) TDataStd_BooleanList::NewEmpty () const
{  
  return new TDataStd_BooleanList(); 
}

//=======================================================================
//function : Restore
//purpose  : 
//=======================================================================
void TDataStd_BooleanList::Restore(const Handle(TDF_Attribute)& With) 
{
  myList.Clear();
  Handle(TDataStd_BooleanList) aList = Handle(TDataStd_BooleanList)::DownCast(With);
  TDataStd_ListIteratorOfListOfByte itr(aList->List());
  for (; itr.More(); itr.Next())
  {
    myList.Append (itr.Value() ? 1 : 0);
  }
  myID = aList->ID();
}

//=======================================================================
//function : Paste
//purpose  : 
//=======================================================================
void TDataStd_BooleanList::Paste (const Handle(TDF_Attribute)& Into,
				  const Handle(TDF_RelocationTable)& ) const
{
  Handle(TDataStd_BooleanList) aList = Handle(TDataStd_BooleanList)::DownCast(Into);
  aList->Clear();
  TDataStd_ListIteratorOfListOfByte itr(myList);
  for (; itr.More(); itr.Next())
  {
    aList->Append (itr.Value() != 0);
  }
  aList->SetID(myID);
}

//=======================================================================
//function : Dump
//purpose  : 
//=======================================================================
Standard_OStream& TDataStd_BooleanList::Dump (Standard_OStream& anOS) const
{  
  anOS << "\nBooleanList: ";
  Standard_Character sguid[Standard_GUID_SIZE_ALLOC];
  myID.ToCString(sguid);
  anOS << sguid;
  anOS << endl;
  return anOS;
}
