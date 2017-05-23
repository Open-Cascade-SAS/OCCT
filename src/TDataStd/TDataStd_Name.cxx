// Created on: 1997-07-31
// Created by: Denis PASCAL
// Copyright (c) 1997-1999 Matra Datavision
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


#include <Standard_DomainError.hxx>
#include <Standard_GUID.hxx>
#include <Standard_Type.hxx>
#include <TCollection_AsciiString.hxx>
#include <TCollection_ExtendedString.hxx>
#include <TDataStd_Name.hxx>
#include <TDF_Attribute.hxx>
#include <TDF_ChildIterator.hxx>
#include <TDF_Label.hxx>
#include <TDF_ListIteratorOfAttributeList.hxx>
#include <TDF_RelocationTable.hxx>
#include <TDF_Tool.hxx>

IMPLEMENT_STANDARD_RTTIEXT(TDataStd_Name,TDF_Attribute)

//=======================================================================
//function : GetID
//purpose  : 
//=======================================================================
const Standard_GUID& TDataStd_Name::GetID () 
{
  static Standard_GUID TDataStd_NameID("2a96b608-ec8b-11d0-bee7-080009dc3333");
  return TDataStd_NameID;
}

//=======================================================================
//function : SetAttr
//purpose  : Implements Set functionality
//=======================================================================
static Handle(TDataStd_Name) SetAttr(const TDF_Label&       label,
                                     const TCollection_ExtendedString& theString,
                                     const Standard_GUID& theGuid) 
{
  Handle(TDataStd_Name) N;
  if (!label.FindAttribute(theGuid, N)) {
    N = new TDataStd_Name ();
    N->SetID(theGuid);
    label.AddAttribute(N);
  }
  N->Set (theString); 
  return N;
}

//=======================================================================
//function : Set
//purpose  : 
//=======================================================================
Handle(TDataStd_Name) TDataStd_Name::Set
                                (const TDF_Label&                  label,
                                 const TCollection_ExtendedString& theString) 
{
  return SetAttr(label, theString, GetID());
}

//=======================================================================
//function : Set
//purpose  : Set user defined attribute
//=======================================================================

Handle(TDataStd_Name) TDataStd_Name::Set (const TDF_Label&    label, 
                                          const Standard_GUID& theGuid,
                                          const TCollection_ExtendedString& theString) 
{
  return SetAttr(label, theString, theGuid);
}
//=======================================================================
//function : TDataStd_Name
//purpose  : Empty Constructor
//=======================================================================

TDataStd_Name::TDataStd_Name ()
{}

//=======================================================================
//function : Set
//purpose  : 
//=======================================================================
void TDataStd_Name::Set (const TCollection_ExtendedString& S) 
{
  if(myString == S) return;
 
  Backup();
  myString = S;
}



//=======================================================================
//function : Get
//purpose  : 
//=======================================================================
const TCollection_ExtendedString& TDataStd_Name::Get () const
{
  return myString;
}

//=======================================================================
//function : SetID
//purpose  :
//=======================================================================

void TDataStd_Name::SetID( const Standard_GUID&  theGuid)
{  
  if(myID == theGuid) return;

  Backup();
  myID = theGuid;
}

//=======================================================================
//function : SetID
//purpose  : sets default ID
//=======================================================================
void TDataStd_Name::SetID()
{
  Backup();
  myID = GetID();
}

// TDF_Attribute methods
//=======================================================================
//function : ID
//purpose  : 
//=======================================================================

const Standard_GUID& TDataStd_Name::ID () const { return myID; }

//=======================================================================
//function : NewEmpty
//purpose  : 
//=======================================================================

Handle(TDF_Attribute) TDataStd_Name::NewEmpty () const
{
  return new TDataStd_Name();
}

//=======================================================================
//function : Restore
//purpose  : 
//=======================================================================

void TDataStd_Name::Restore(const Handle(TDF_Attribute)& with) 
{
   Handle(TDataStd_Name) anAtt = Handle(TDataStd_Name)::DownCast (with);
   myString = anAtt->Get();
   myID = anAtt->ID();
}


//=======================================================================
//function : Paste
//purpose  : 
//=======================================================================

void TDataStd_Name::Paste (const Handle(TDF_Attribute)& into,
                           const Handle(TDF_RelocationTable)&/* RT*/) const
{
  Handle(TDataStd_Name) anAtt = Handle(TDataStd_Name)::DownCast (into);
  anAtt->Set (myString);
  anAtt->SetID(myID);
}

//=======================================================================
//function : Dump
//purpose  : 
//=======================================================================

Standard_OStream& TDataStd_Name::Dump (Standard_OStream& anOS) const
{
  TDF_Attribute::Dump(anOS);
  anOS << " Name=|"<<myString<<"|";
  Standard_Character sguid[Standard_GUID_SIZE_ALLOC];
  myID.ToCString(sguid);
  anOS << sguid << endl;
  return anOS;
}
