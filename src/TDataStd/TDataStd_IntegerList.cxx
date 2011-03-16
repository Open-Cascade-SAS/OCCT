// File:        TDataStd_IntegerList.cxx
// Created:     May 29 11:40:00 2007
// Author:      Vlad Romashko
//  	    	<vladislav.romashko@opencascade.com>
// Copyright:   Open CASCADE

#include <TDataStd_IntegerList.ixx>
#include <TColStd_ListIteratorOfListOfInteger.hxx>

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
  Handle(TDataStd_IntegerList) A;
  if (!label.FindAttribute (TDataStd_IntegerList::GetID(), A)) 
  {
    A = new TDataStd_IntegerList;
    label.AddAttribute(A);
  }
  return A;
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
  return GetID(); 
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
}

//=======================================================================
//function : Dump
//purpose  : 
//=======================================================================
Standard_OStream& TDataStd_IntegerList::Dump (Standard_OStream& anOS) const
{  
  anOS << "IntegerList";
  return anOS;
}
