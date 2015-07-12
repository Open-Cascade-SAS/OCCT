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
//function : Set
//purpose  : 
//=======================================================================

Handle(TDataStd_Integer) TDataStd_Integer::Set (const TDF_Label&        L,
                                                const Standard_Integer  V) 

{
  Handle(TDataStd_Integer) A;
  if (!L.FindAttribute (TDataStd_Integer::GetID(), A)) {
    A = new TDataStd_Integer ();
    L.AddAttribute(A);
  }
  A->Set (V); 
  return A;
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

const Standard_GUID& TDataStd_Integer::ID () const { return GetID(); }


//=======================================================================
//function : NewEmpty
//purpose  : 
//=======================================================================

Handle(TDF_Attribute) TDataStd_Integer::NewEmpty () const
{  
  return new TDataStd_Integer (); 
}

//=======================================================================
//function : Restore
//purpose  : 
//=======================================================================

void TDataStd_Integer::Restore(const Handle(TDF_Attribute)& With) 
{
  myValue = Handle(TDataStd_Integer)::DownCast (With)->Get();
}

//=======================================================================
//function : Paste
//purpose  : 
//=======================================================================

void TDataStd_Integer::Paste (const Handle(TDF_Attribute)& Into,
                              const Handle(TDF_RelocationTable)& /*RT*/) const
{
  Handle(TDataStd_Integer)::DownCast(Into)->Set(myValue);
}

//=======================================================================
//function : Dump
//purpose  : 
//=======================================================================

Standard_OStream& TDataStd_Integer::Dump (Standard_OStream& anOS) const
{  
  anOS << "Integer:: "<< this <<" : ";
  anOS << myValue; 
//
  anOS <<"\nAttribute fields: ";
  TDF_Attribute::Dump(anOS);
  return anOS;
}

