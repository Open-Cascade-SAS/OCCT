// Created on: 2009-04-06
// Created by: Sergey ZARITCHNY
// Copyright (c) 2009-2012 OPEN CASCADE SAS
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



#include <TDataXtd_Placement.ixx>
//=======================================================================
//function : GetID
//purpose  : 
//=======================================================================

const Standard_GUID& TDataXtd_Placement::GetID () 
{
  static Standard_GUID TDataXtd_PlacementID ("2a96b60b-ec8b-11d0-bee7-080009dc3333");
  return TDataXtd_PlacementID; 
}


//=======================================================================
//function : Set
//purpose  : 
//=======================================================================

Handle(TDataXtd_Placement) TDataXtd_Placement::Set (const TDF_Label& L)
{
  Handle (TDataXtd_Placement) A;
  if (!L.FindAttribute (TDataXtd_Placement::GetID (), A)) {
    A = new TDataXtd_Placement ();
    L.AddAttribute(A);
  }
  return A;
}


//=======================================================================
//function : TDataXtd_Placement
//purpose  : 
//=======================================================================

TDataXtd_Placement::TDataXtd_Placement () { }



//=======================================================================
//function : ID
//purpose  : 
//=======================================================================

const Standard_GUID& TDataXtd_Placement::ID () const { return GetID(); }


//=======================================================================
//function : NewEmpty
//purpose  : 
//=======================================================================

Handle(TDF_Attribute) TDataXtd_Placement::NewEmpty() const
{ 
  return new TDataXtd_Placement(); 
}


//=======================================================================
//function : Restore
//purpose  : 
//=======================================================================

void TDataXtd_Placement::Restore (const Handle(TDF_Attribute)& other) 
{

}

//=======================================================================
//function : Paste
//purpose  : 
//=======================================================================

void TDataXtd_Placement::Paste(const Handle(TDF_Attribute)& Into,
			       const Handle(TDF_RelocationTable)& RT) const
{

}    



//=======================================================================
//function : Dump
//purpose  : 
//=======================================================================

Standard_OStream& TDataXtd_Placement::Dump (Standard_OStream& anOS) const
{ 
  anOS << "Placement";
  return anOS;
}
