// Created on: 2007-07-31
// Created by: Sergey ZARITCHNY
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
#include <TCollection_AsciiString.hxx>
#include <TDataStd_AsciiString.hxx>
#include <TDF_Attribute.hxx>
#include <TDF_Label.hxx>
#include <TDF_RelocationTable.hxx>

IMPLEMENT_STANDARD_RTTIEXT(TDataStd_AsciiString,TDF_Attribute)

//=======================================================================
//function : TDataStd_AsciiString
//purpose  : 
//=======================================================================
TDataStd_AsciiString::TDataStd_AsciiString():
  myID (GetID())
{
  myString.Clear();
}

//=======================================================================
//function : GetID
//purpose  : 
//=======================================================================

const Standard_GUID& TDataStd_AsciiString::GetID()
{
  static Standard_GUID theGUID ("3bbefc60-e618-11d4-ba38-0060b0ee18ea");
  return theGUID;
}

//=======================================================================
//function : ID
//purpose  : 
//=======================================================================

const Standard_GUID& TDataStd_AsciiString::ID() const
{
  return myID;
}

//=======================================================================
//function : Set
//purpose  : 
//=======================================================================

Handle(TDataStd_AsciiString) TDataStd_AsciiString::Set (
                             const TDF_Label& theLabel,
                             const TCollection_AsciiString& theAsciiString)
{
  Handle(TDataStd_AsciiString) A;
  if (!theLabel.FindAttribute(TDataStd_AsciiString::GetID(), A))
  {
    A = new TDataStd_AsciiString;
    A->SetID(GetID());
    theLabel.AddAttribute(A);
  }
  A->Set(theAsciiString);
  return A;
}

//=======================================================================
//function : Set
//purpose  : Set user defined attribute
//=======================================================================

Handle(TDataStd_AsciiString) TDataStd_AsciiString::Set (const TDF_Label&    L, const Standard_GUID& theGuid,
                                          const TCollection_AsciiString& theAsciiString) 
{
  Handle(TDataStd_AsciiString) A;
  if (!L.FindAttribute(theGuid, A)) {
    A = new TDataStd_AsciiString ();
    A->SetID(theGuid);
    L.AddAttribute(A);
  }
  A->Set (theAsciiString); 
  return A;
}
//=======================================================================
//function : Set
//purpose  : 
//=======================================================================

void TDataStd_AsciiString::Set (const TCollection_AsciiString& theAsciiString)
{
  if(myString == theAsciiString) return;
  Backup();
  myString = theAsciiString;
}

//=======================================================================
//function : Get
//purpose  : 
//=======================================================================

const TCollection_AsciiString& TDataStd_AsciiString::Get () const
{
  return myString;
}

//=======================================================================
//function : SetID
//purpose  :
//=======================================================================

void TDataStd_AsciiString::SetID( const Standard_GUID&  theGuid)
{  
  if(myID == theGuid) return;

  Backup();
  myID = theGuid;
}

//=======================================================================
//function : NewEmpty
//purpose  : 
//=======================================================================

Handle(TDF_Attribute) TDataStd_AsciiString::NewEmpty () const
{
  Handle(TDataStd_AsciiString) Att = new TDataStd_AsciiString();
  Att->SetID(myID);
  return Att; 
}

//=======================================================================
//function : Restore
//purpose  : 
//=======================================================================

void TDataStd_AsciiString::Restore (const Handle(TDF_Attribute)& theWith)
{
  Handle(TDataStd_AsciiString) R = Handle(TDataStd_AsciiString)::DownCast(theWith);
  myString = R->Get();
  myID     = R->ID();
}

//=======================================================================
//function : Paste
//purpose  : 
//=======================================================================

void TDataStd_AsciiString::Paste (const Handle(TDF_Attribute)& theInto,
                              const Handle(TDF_RelocationTable)& /* RT */) const
{ 
  Handle(TDataStd_AsciiString) R = Handle(TDataStd_AsciiString)::DownCast (theInto);
  R->Set(myString);
  R->SetID(myID);
}

//=======================================================================
//function : IsEmpty
//purpose  : 
//=======================================================================
Standard_Boolean TDataStd_AsciiString::IsEmpty () const
{
  return myString.IsEmpty();
}
//=======================================================================
//function : Dump
//purpose  : 
//=======================================================================

Standard_OStream& TDataStd_AsciiString::Dump(Standard_OStream& theOS) const
{
  Standard_OStream& anOS = TDF_Attribute::Dump( theOS );
  anOS << myString;
  anOS << " Name=|"<<myString<<"|";
  Standard_Character sguid[Standard_GUID_SIZE_ALLOC];
  myID.ToCString(sguid);
  anOS << sguid << "|" <<endl;
  return anOS;
}
