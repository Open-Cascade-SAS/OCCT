// Created on: 1997-03-06
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


#include <Standard_GUID.hxx>
#include <Standard_Type.hxx>
#include <TDataStd_Integer.hxx>
#include <TDF_Attribute.hxx>
#include <TDF_Label.hxx>
#include <TDF_Reference.hxx>
#include <TDF_RelocationTable.hxx>

IMPLEMENT_STANDARD_RTTIEXT(TDataStd_Integer,TDF_Attribute)

//=======================================================================
//function : GetID
//purpose  : 
//=======================================================================
const Standard_GUID& TDataStd_Integer::GetID() 
{ 
  static Standard_GUID TDataStd_IntegerID ("2a96b606-ec8b-11d0-bee7-080009dc3333");
  return TDataStd_IntegerID; 
}

//=======================================================================
//function : SetAttr
//purpose  : Implements Set functionality
//=======================================================================
static Handle(TDataStd_Integer) SetAttr(const TDF_Label&     label,
                                        const Standard_Integer  V,
                                        const Standard_GUID& theGuid)
{
  Handle(TDataStd_Integer) A;
  if (!label.FindAttribute(theGuid, A)) {
    A = new TDataStd_Integer ();
    A->SetID(theGuid);
    label.AddAttribute(A);
  }
  A->Set (V); 
  return A;
}

//=======================================================================
//function : Set
//purpose  : 
//=======================================================================

Handle(TDataStd_Integer) TDataStd_Integer::Set (const TDF_Label&        L,
                                                const Standard_Integer  V) 

{
  return SetAttr(L, V, GetID());
}

//=======================================================================
//function : Set
//purpose  : Set user defined attribute
//=======================================================================

Handle(TDataStd_Integer) TDataStd_Integer::Set (const TDF_Label&    L,
                                                const Standard_GUID& theGuid,
                                                const Standard_Integer V) 
{
  return SetAttr(L, V, theGuid);
}
//=======================================================================
//function : TDataStd_Integer
//purpose  : Empty Constructor
//=======================================================================

TDataStd_Integer::TDataStd_Integer ()
     : myValue (-1)
 { }


//=======================================================================
//function : Set
//purpose  : 
//=======================================================================

void TDataStd_Integer::Set(const Standard_Integer v) 
{
  // OCC2932 correction
  if(myValue == v) return;

  Backup();
  myValue = v;
}


//=======================================================================
//function : Get
//purpose  : 
//=======================================================================

Standard_Integer TDataStd_Integer::Get () const { return myValue; }


//=======================================================================
//function : IsCaptured
//purpose  : 
//=======================================================================

Standard_Boolean TDataStd_Integer::IsCaptured() const
{
  Handle(TDF_Reference) R;
  return (Label().FindAttribute(TDF_Reference::GetID(),R));
}

//=======================================================================
//function : ID
//purpose  : 
//=======================================================================

const Standard_GUID& TDataStd_Integer::ID () const { return myID; }

//=======================================================================
//function : SetID
//purpose  :
//=======================================================================

void TDataStd_Integer::SetID( const Standard_GUID&  theGuid)
{  
  if(myID == theGuid) return;

  Backup();
  myID = theGuid;
}

//=======================================================================
//function : SetID
//purpose  : sets default ID
//=======================================================================
void TDataStd_Integer::SetID()
{
  Backup();
  myID = GetID();
}

//=======================================================================
//function : NewEmpty
//purpose  : 
//=======================================================================

Handle(TDF_Attribute) TDataStd_Integer::NewEmpty () const
{
  return new TDataStd_Integer();
}

//=======================================================================
//function : Restore
//purpose  : 
//=======================================================================

void TDataStd_Integer::Restore(const Handle(TDF_Attribute)& With) 
{
  Handle(TDataStd_Integer) anInt = Handle(TDataStd_Integer)::DownCast (With);
  myValue = anInt->Get();
  myID = anInt->ID();
}

//=======================================================================
//function : Paste
//purpose  : 
//=======================================================================

void TDataStd_Integer::Paste (const Handle(TDF_Attribute)& Into,
                              const Handle(TDF_RelocationTable)& /*RT*/) const
{
  Handle(TDataStd_Integer) anInt = Handle(TDataStd_Integer)::DownCast (Into);
  anInt->Set(myValue);
  anInt->SetID(myID);
}

//=======================================================================
//function : Dump
//purpose  : 
//=======================================================================

Standard_OStream& TDataStd_Integer::Dump (Standard_OStream& anOS) const
{  
  anOS << "Integer:: "<< this <<" : ";
  anOS << myValue; 
  Standard_Character sguid[Standard_GUID_SIZE_ALLOC];
  myID.ToCString(sguid);
  anOS << sguid;
//
  anOS <<"\nAttribute fields: ";
  TDF_Attribute::Dump(anOS);
  return anOS;
}

