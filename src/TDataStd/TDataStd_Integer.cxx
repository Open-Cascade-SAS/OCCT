// Created on: 1997-03-06
// Created by: Denis PASCAL
// Copyright (c) 1997-1999 Matra Datavision
// Copyright (c) 1999-2012 OPEN CASCADE SAS
//
// The content of this file is subject to the Open CASCADE Technology Public
// License Version 6.5 (the "License"). You may not use the content of this file
// except in compliance with the License. Please obtain a copy of the License
// at http://www.opencascade.org and read it completely before using this file.
//
// The Initial Developer of the Original Code is Open CASCADE S.A.S., having its
// main offices at: 1, place des Freres Montgolfier, 78280 Guyancourt, France.
//
// The Original Code and all software distributed under the License is
// distributed on an "AS IS" basis, without warranty of any kind, and the
// Initial Developer hereby disclaims all such warranties, including without
// limitation, any warranties of merchantability, fitness for a particular
// purpose or non-infringement. Please see the License for the specific terms
// and conditions governing the rights and limitations under the License.



#include <TDataStd_Integer.ixx>
#include <TDF_Reference.hxx>

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
                              const Handle(TDF_RelocationTable)& RT) const
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
  anOS << TDF_Attribute::Dump(anOS);
  return anOS;
}

