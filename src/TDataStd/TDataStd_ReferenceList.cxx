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
#include <TDataStd_ReferenceList.hxx>
#include <TDF_Attribute.hxx>
#include <TDF_DataSet.hxx>
#include <TDF_Label.hxx>
#include <TDF_ListIteratorOfLabelList.hxx>
#include <TDF_RelocationTable.hxx>

//=======================================================================
//function : GetID
//purpose  : 
//=======================================================================
const Standard_GUID& TDataStd_ReferenceList::GetID() 
{ 
  static Standard_GUID TDataStd_ReferenceListID ("FCC1A658-59FF-4218-931B-0320A2B469A7");
  return TDataStd_ReferenceListID; 
}

//=======================================================================
//function : TDataStd_ReferenceList
//purpose  : Empty Constructor
//=======================================================================
TDataStd_ReferenceList::TDataStd_ReferenceList() 
{

}

//=======================================================================
//function : Set
//purpose  : 
//=======================================================================
Handle(TDataStd_ReferenceList) TDataStd_ReferenceList::Set(const TDF_Label& label) 
{
  Handle(TDataStd_ReferenceList) A;
  if (!label.FindAttribute (TDataStd_ReferenceList::GetID(), A)) 
  {
    A = new TDataStd_ReferenceList;
    label.AddAttribute(A);
  }
  return A;
}

//=======================================================================
//function : IsEmpty
//purpose  : 
//=======================================================================
Standard_Boolean TDataStd_ReferenceList::IsEmpty() const
{
  return myList.IsEmpty();
}

//=======================================================================
//function : Extent
//purpose  : 
//=======================================================================
Standard_Integer TDataStd_ReferenceList::Extent() const
{
  return myList.Extent();
}

//=======================================================================
//function : Prepend
//purpose  : 
//=======================================================================
void TDataStd_ReferenceList::Prepend(const TDF_Label& value)
{
  Backup();
  myList.Prepend(value);
}

//=======================================================================
//function : Append
//purpose  : 
//=======================================================================
void TDataStd_ReferenceList::Append(const TDF_Label& value)
{
  Backup();
  myList.Append(value);
}

//=======================================================================
//function : InsertBefore
//purpose  : 
//=======================================================================
Standard_Boolean TDataStd_ReferenceList::InsertBefore(const TDF_Label& value,
						      const TDF_Label& before_value)
{
  TDF_ListIteratorOfLabelList itr(myList);
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
Standard_Boolean TDataStd_ReferenceList::InsertAfter(const TDF_Label& value,
						     const TDF_Label& after_value)
{
  TDF_ListIteratorOfLabelList itr(myList);
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
Standard_Boolean TDataStd_ReferenceList::Remove(const TDF_Label& value)
{
  TDF_ListIteratorOfLabelList itr(myList);
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
void TDataStd_ReferenceList::Clear()
{
  Backup();
  myList.Clear();
}

//=======================================================================
//function : First
//purpose  : 
//=======================================================================
const TDF_Label& TDataStd_ReferenceList::First() const
{
  return myList.First();
}

//=======================================================================
//function : Last
//purpose  : 
//=======================================================================
const TDF_Label& TDataStd_ReferenceList::Last() const
{
  return myList.Last();
}

//=======================================================================
//function : List
//purpose  : 
//=======================================================================
const TDF_LabelList& TDataStd_ReferenceList::List() const
{
  return myList;
}

//=======================================================================
//function : ID
//purpose  : 
//=======================================================================
const Standard_GUID& TDataStd_ReferenceList::ID () const 
{ 
  return GetID(); 
}

//=======================================================================
//function : NewEmpty
//purpose  : 
//=======================================================================
Handle(TDF_Attribute) TDataStd_ReferenceList::NewEmpty () const
{  
  return new TDataStd_ReferenceList(); 
}

//=======================================================================
//function : Restore
//purpose  : 
//=======================================================================
void TDataStd_ReferenceList::Restore(const Handle(TDF_Attribute)& With) 
{
  myList.Clear();
  Handle(TDataStd_ReferenceList) aList = Handle(TDataStd_ReferenceList)::DownCast(With);
  TDF_ListIteratorOfLabelList itr(aList->List());
  for (; itr.More(); itr.Next())
  {
    myList.Append(itr.Value());
  }
}

//=======================================================================
//function : Paste
//purpose  : 
//=======================================================================
void TDataStd_ReferenceList::Paste (const Handle(TDF_Attribute)& Into,
				    const Handle(TDF_RelocationTable)& RT) const
{
  Handle(TDataStd_ReferenceList) aList = Handle(TDataStd_ReferenceList)::DownCast(Into);
  aList->Clear();
  TDF_ListIteratorOfLabelList itr(myList);
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
}

//=======================================================================
//function : References
//purpose  : Adds the referenced attributes or labels.
//=======================================================================
void TDataStd_ReferenceList::References(const Handle(TDF_DataSet)& aDataSet) const
{
  if (!Label().IsImported()) 
  {
    TDF_ListIteratorOfLabelList itr(myList);
    for (; itr.More(); itr.Next())
    {
      aDataSet->AddLabel(itr.Value());
    }
  }
}

//=======================================================================
//function : Dump
//purpose  : 
//=======================================================================
Standard_OStream& TDataStd_ReferenceList::Dump (Standard_OStream& anOS) const
{  
  anOS << "ReferenceList";
  return anOS;
}
