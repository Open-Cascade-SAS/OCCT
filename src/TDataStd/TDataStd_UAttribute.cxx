// Created on: 1999-06-11
// Created by: Sergey RUIN
// Copyright (c) 1999-1999 Matra Datavision
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



#include <TDataStd_UAttribute.ixx>
#include <TDataStd.hxx>
#include <TDF_Label.hxx>

//=======================================================================
//function : Set
//purpose  : 
//=======================================================================

Handle(TDataStd_UAttribute) TDataStd_UAttribute::Set (const TDF_Label& label, const Standard_GUID& guid )
{
  Handle(TDataStd_UAttribute) A;  
  if (!label.FindAttribute(guid, A)) {
    A = new TDataStd_UAttribute ();
    A->SetID(guid);
    label.AddAttribute(A);                      
  }
  return A;
}


//=======================================================================
//function : TDataStd_UAttribute
//purpose  : 
//=======================================================================

TDataStd_UAttribute::TDataStd_UAttribute()
{
}

//=======================================================================
//function : ID
//purpose  : 
//=======================================================================

const Standard_GUID& TDataStd_UAttribute::ID() const
{ return myID; }


//=======================================================================
//function : SetID
//purpose  : 
//=======================================================================

void TDataStd_UAttribute::SetID( const Standard_GUID&  guid)
{
  // OCC2932 correction
  if(myID == guid) return;

  Backup();
  myID = guid;
}


//=======================================================================
//function : NewEmpty
//purpose  : 
//=======================================================================

Handle(TDF_Attribute) TDataStd_UAttribute::NewEmpty () const
{ 
  Handle(TDataStd_UAttribute) A = new TDataStd_UAttribute();
  A->SetID(myID);
  return A; 
}

//=======================================================================
//function : Restore
//purpose  : 
//=======================================================================

void TDataStd_UAttribute::Restore(const Handle(TDF_Attribute)& with) 
{  
  Handle(TDataStd_UAttribute) A = Handle(TDataStd_UAttribute)::DownCast(with);
  SetID( A->ID() );
}

//=======================================================================
//function : Paste
//purpose  : 
//=======================================================================

void TDataStd_UAttribute::Paste (const Handle(TDF_Attribute)& into,
			   const Handle(TDF_RelocationTable)& RT) const
{
  Handle(TDataStd_UAttribute) A = Handle(TDataStd_UAttribute)::DownCast(into);
  A->SetID( myID );
}

//=======================================================================
//function : References
//purpose  : 
//=======================================================================

void TDataStd_UAttribute::References (const Handle(TDF_DataSet)& DS) const
{  
}
 
//=======================================================================
//function : Dump
//purpose  : 
//=======================================================================

Standard_OStream& TDataStd_UAttribute::Dump (Standard_OStream& anOS) const
{  
  anOS << "UAttribute";
  TDF_Attribute::Dump(anOS);
  return anOS;
}


