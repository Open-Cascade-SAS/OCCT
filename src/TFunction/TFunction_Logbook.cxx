// Created on: 1999-07-20
// Created by: Vladislav ROMASHKO
// Copyright (c) 1999-1999 Matra Datavision
// Copyright (c) 1999-2014 OPEN CASCADE SAS
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


#include <Standard_OStream.hxx>
#include <TCollection_AsciiString.hxx>
#include <TDF_ChildIterator.hxx>
#include <TDF_Label.hxx>
#include <TDF_LabelMap.hxx>
#include <TDF_MapIteratorOfLabelMap.hxx>
#include <TDF_RelocationTable.hxx>
#include <TDF_Tool.hxx>
#include <TFunction_Logbook.hxx>
#include <Standard_GUID.hxx>

//=======================================================================
//function : GetID
//purpose  : Static method to get an ID
//=======================================================================
const Standard_GUID& TFunction_Logbook::GetID() 
{  
  static Standard_GUID TFunction_LogbookID("CF519724-5CA4-4B90-835F-8919BE1DDE4B");
  return TFunction_LogbookID; 
}

//=======================================================================
//function : Set
//purpose  : Finds or creates a Scope attribute
//=======================================================================

Handle(TFunction_Logbook) TFunction_Logbook::Set(const TDF_Label& Access)
{
  Handle(TFunction_Logbook) S;
  if (!Access.Root().FindAttribute(TFunction_Logbook::GetID(), S)) 
  {
    S = new TFunction_Logbook();
    Access.Root().AddAttribute(S);
  }
  return S;
}

//=======================================================================
//function : ID
//purpose  : Returns GUID of the function
//=======================================================================

const Standard_GUID& TFunction_Logbook::ID() const
{ 
  return GetID(); 
}

//=======================================================================
//function : TFunction_Logbook
//purpose  : A Logbook creation
//=======================================================================
TFunction_Logbook::TFunction_Logbook():isDone(Standard_False)
{}

//=======================================================================
//function : Clear
//purpose  : Clears the valid and modified labels
//=======================================================================

void TFunction_Logbook::Clear()
{
  if (!IsEmpty())
  {
    Backup();
    myTouched.Clear();
    myImpacted.Clear();
    myValid.Clear();
  }
}

//=======================================================================
//function : IsEmpty
//purpose  : Returns Standard_True if the nothing is reccorded in the logbook
//=======================================================================

Standard_Boolean TFunction_Logbook::IsEmpty () const
{
  return (myTouched.IsEmpty() && myImpacted.IsEmpty() && myValid.IsEmpty());
}

//=======================================================================
//function : IsModified
//purpose  : Returns Standard_True if the label is modified
//=======================================================================

Standard_Boolean TFunction_Logbook::IsModified(const TDF_Label& L,
                                               const Standard_Boolean WithChildren) const
{
  if (myTouched.Contains(L))
    return Standard_True;
  if (myImpacted.Contains(L))
    return Standard_True;
  if (WithChildren)
  {
    TDF_ChildIterator itr(L);
    for (; itr.More(); itr.Next())
    {
      if (IsModified(itr.Value(), Standard_True))
      {
        return Standard_True;
      }
    }
  }
  return Standard_False;
}

//=======================================================================
//function : SetValid
//purpose  : 
//=======================================================================

void TFunction_Logbook::SetValid(const TDF_Label& L,
                                 const Standard_Boolean WithChildren)
{
  Backup();
  myValid.Add(L);
  if (WithChildren)
  {
    TDF_ChildIterator itr(L, Standard_True);
    for (; itr.More(); itr.Next())
    {
      myValid.Add(itr.Value());
    }
  }
}

void TFunction_Logbook::SetValid(const TDF_LabelMap& Ls)
{
  Backup();
  TDF_MapIteratorOfLabelMap itrm(Ls);
  for (; itrm.More(); itrm.Next())
  {
    const TDF_Label& L = itrm.Key();
    myValid.Add(L);
  }
}

//=======================================================================
//function : SetImpacted
//purpose  : 
//=======================================================================

void TFunction_Logbook::SetImpacted(const TDF_Label& L,
                                    const Standard_Boolean WithChildren)
{
  Backup();
  myImpacted.Add(L);
  if (WithChildren)
  {
    TDF_ChildIterator itr(L, Standard_True);
    for (; itr.More(); itr.Next())
    {
      myImpacted.Add(itr.Value());
    }
  }  
}

//=======================================================================
//function : GetValid
//purpose  : Returns valid labels.
//=======================================================================

void TFunction_Logbook::GetValid(TDF_LabelMap& Ls) const
{
  // Copy valid labels.
  TDF_MapIteratorOfLabelMap itrm(myValid);
  for (; itrm.More(); itrm.Next())
  {
    const TDF_Label& L = itrm.Key();
    Ls.Add(L);
  }
}

//=======================================================================
//function : Restore
//purpose  : Undos (and redos) the attribute.
//=======================================================================

void TFunction_Logbook::Restore(const Handle(TDF_Attribute)& other) 
{
  Handle(TFunction_Logbook) logbook = Handle(TFunction_Logbook)::DownCast(other);

  // Status.
  isDone = logbook->isDone;

  // Valid labels
  TDF_MapIteratorOfLabelMap itrm;
  for (itrm.Initialize(logbook->myValid); itrm.More(); itrm.Next())
  {
    myValid.Add(itrm.Key());
  }
  // Touched labels
  for (itrm.Initialize(logbook->myTouched); itrm.More(); itrm.Next())
  {
    myTouched.Add(itrm.Key());
  }
  // Impacted labels
  for (itrm.Initialize(logbook->myImpacted); itrm.More(); itrm.Next())
  {
    myImpacted.Add(itrm.Key());
  }
}

//=======================================================================
//function : Paste
//purpose  : Method for Copy mechanism
//=======================================================================

void TFunction_Logbook::Paste(const Handle(TDF_Attribute)& into,
                              const Handle(TDF_RelocationTable)& RT) const
{
  Handle(TFunction_Logbook) logbook = Handle(TFunction_Logbook)::DownCast(into);
  
  // Status.
  logbook->isDone = isDone;

  // Touched.
  logbook->myTouched.Clear();
  TDF_MapIteratorOfLabelMap itr(myTouched);
  for (; itr.More(); itr.Next())
  {
    const TDF_Label& L = itr.Value();
    if (!L.IsNull())
    {
      TDF_Label relocL;
      if (RT->HasRelocation(L, relocL))
        logbook->myTouched.Add(relocL);
      else
        logbook->myTouched.Add(L);
    }
  }

  // Impacted.
  logbook->myImpacted.Clear();
  itr.Initialize(myImpacted);
  for (; itr.More(); itr.Next())
  {
    const TDF_Label& L = itr.Value();
    if (!L.IsNull())
    {
      TDF_Label relocL;
      if (RT->HasRelocation(L, relocL))
        logbook->myImpacted.Add(relocL);
      else
        logbook->myImpacted.Add(L);
    }
  }

  // Valid.
  logbook->myValid.Clear();
  itr.Initialize(myValid);
  for (; itr.More(); itr.Next())
  {
    const TDF_Label& L = itr.Value();
    if (!L.IsNull())
    {
      TDF_Label relocL;
      if (RT->HasRelocation(L, relocL))
        logbook->myValid.Add(relocL);
      else
        logbook->myValid.Add(L);
    }
  }
}

//=======================================================================
//function : NewEmpty
//purpose  : Returns new empty graph node attribute
//=======================================================================

Handle(TDF_Attribute) TFunction_Logbook::NewEmpty() const
{
  return new TFunction_Logbook();
}

//=======================================================================
//function : Dump
//purpose  : Dump of modifications
//=======================================================================

Standard_OStream& TFunction_Logbook::Dump(Standard_OStream& stream) const
{
  TDF_MapIteratorOfLabelMap itr;
  TCollection_AsciiString as;
  
  stream<<"Done = "<<isDone<<endl;
  stream<<"Touched labels: "<<endl;
  for (itr.Initialize(myTouched); itr.More(); itr.Next())
  {
    TDF_Tool::Entry(itr.Key(), as);
    stream<<as<<endl;
  }
  stream<<"Impacted labels: "<<endl;
  for (itr.Initialize(myImpacted); itr.More(); itr.Next())
  {
    TDF_Tool::Entry(itr.Key(), as);
    stream<<as<<endl;
  }  
  stream<<"Valid labels: "<<endl;
  for (itr.Initialize(myValid); itr.More(); itr.Next())
  {
    TDF_Tool::Entry(itr.Key(), as);
    stream<<as<<endl;
  }  

  return stream;
}
