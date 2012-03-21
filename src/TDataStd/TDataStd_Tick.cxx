// Created on: 2007-05-29
// Created by: Vlad Romashko
// Copyright (c) 2007-2012 OPEN CASCADE SAS
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


#include <TDataStd_Tick.ixx>

//=======================================================================
//function : GetID
//purpose  : 
//=======================================================================
const Standard_GUID& TDataStd_Tick::GetID () 
{
  static Standard_GUID TDataStd_TickID("40DC60CD-30B9-41be-B002-4169EFB34EA5");
  return TDataStd_TickID; 
}

//=======================================================================
//function : Set
//purpose  : 
//=======================================================================
Handle(TDataStd_Tick) TDataStd_Tick::Set (const TDF_Label& L)
{
  Handle(TDataStd_Tick) A;
  if (!L.FindAttribute(TDataStd_Tick::GetID (), A)) 
  {
    A = new TDataStd_Tick();
    L.AddAttribute(A);
  }
  return A;
}

//=======================================================================
//function : TDataStd_Tick
//purpose  : 
//=======================================================================
TDataStd_Tick::TDataStd_Tick () 
{
}

//=======================================================================
//function : ID
//purpose  : 
//=======================================================================
const Standard_GUID& TDataStd_Tick::ID () const 
{ 
  return GetID(); 
}

//=======================================================================
//function : NewEmpty
//purpose  : 
//=======================================================================
Handle(TDF_Attribute) TDataStd_Tick::NewEmpty() const
{ 
  return new TDataStd_Tick(); 
}

//=======================================================================
//function : Restore
//purpose  : 
//=======================================================================
void TDataStd_Tick::Restore (const Handle(TDF_Attribute)& other) 
{
  // There are no fields in this attribute.
}

//=======================================================================
//function : Paste
//purpose  : 
//=======================================================================
void TDataStd_Tick::Paste(const Handle(TDF_Attribute)& Into,
			  const Handle(TDF_RelocationTable)& RT) const
{
  // There are no fields in this attribute.
}    

//=======================================================================
//function : Dump
//purpose  : 
//=======================================================================
Standard_OStream& TDataStd_Tick::Dump (Standard_OStream& anOS) const
{ 
  anOS << "Tick";
  return anOS;
}
