// Created on: 1999-07-12
// Created by: Denis PASCAL
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



#include <TDocStd_Owner.ixx>

//=======================================================================
//function : GetID
//purpose  : 
//=======================================================================

const Standard_GUID& TDocStd_Owner::GetID() 
{ 
  static Standard_GUID TDocStd_OwnerID ("2a96b617-ec8b-11d0-bee7-080009dc3333");
  return TDocStd_OwnerID; 
}


//=======================================================================
//function : SetDocument
//purpose  : 
//=======================================================================

void TDocStd_Owner::SetDocument (const Handle(TDF_Data)& indata,
				 const Handle(TDocStd_Document)& D) 
{
  Handle(TDocStd_Owner) A;
  if (!indata->Root().FindAttribute (TDocStd_Owner::GetID(), A)) {
    A = new TDocStd_Owner (); 
    A->SetDocument(D);
    indata->Root().AddAttribute(A);
  }
  else {  
    Standard_DomainError::Raise("TDocStd_Owner::SetDocument : already called");
  }
}

//=======================================================================
//function : GetDocument
//purpose  : 
//=======================================================================

Handle(TDocStd_Document) TDocStd_Owner::GetDocument (const Handle(TDF_Data)& ofdata)
{
  Handle(TDocStd_Owner) A;
  if (!ofdata->Root().FindAttribute (TDocStd_Owner::GetID(), A)) {
    Standard_DomainError::Raise("TDocStd_Owner::GetDocument : document not found");
  }
  return A->GetDocument();
}

//=======================================================================
//function : TDocStd_Owner
//purpose  : 
//=======================================================================

TDocStd_Owner::TDocStd_Owner () { }


//=======================================================================
//function : SetDocument
//purpose  : 
//=======================================================================

void TDocStd_Owner::SetDocument(const Handle( TDocStd_Document)& D) 
{
  myDocument = D;
}


//=======================================================================
//function : Get
//purpose  : 
//=======================================================================

Handle(TDocStd_Document) TDocStd_Owner::GetDocument () const 
{ return myDocument; 
}


//=======================================================================
//function : ID
//purpose  : 
//=======================================================================

const Standard_GUID& TDocStd_Owner::ID () const { return GetID(); }


//=======================================================================
//function : NewEmpty
//purpose  : 
//=======================================================================

Handle(TDF_Attribute) TDocStd_Owner::NewEmpty () const
{
  Handle(TDF_Attribute) dummy;
  return dummy;
}

//=======================================================================
//function : Restore
//purpose  : 
//=======================================================================

void TDocStd_Owner::Restore(const Handle(TDF_Attribute)& With) 
{
}

//=======================================================================
//function : Paste
//purpose  : 
//=======================================================================

void TDocStd_Owner::Paste (const Handle(TDF_Attribute)& Into,
			      const Handle(TDF_RelocationTable)& RT) const
{
}

//=======================================================================
//function : Dump
//purpose  : 
//=======================================================================

Standard_OStream& TDocStd_Owner::Dump (Standard_OStream& anOS) const
{  
  anOS << "Owner";
  return anOS;
}

