// Created on: 1999-07-12
// Created by: Denis PASCAL
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


#include <Standard_GUID.hxx>
#include <Standard_Type.hxx>
#include <TDF_Attribute.hxx>
#include <TDF_Data.hxx>
#include <TDF_RelocationTable.hxx>
#include <TDocStd_Document.hxx>
#include <TDocStd_Owner.hxx>

IMPLEMENT_STANDARD_RTTIEXT(TDocStd_Owner,TDF_Attribute)

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
    throw Standard_DomainError("TDocStd_Owner::SetDocument : already called");
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
    throw Standard_DomainError("TDocStd_Owner::GetDocument : document not found");
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

void TDocStd_Owner::Restore(const Handle(TDF_Attribute)&) 
{
}

//=======================================================================
//function : Paste
//purpose  : 
//=======================================================================

void TDocStd_Owner::Paste (const Handle(TDF_Attribute)&,
			      const Handle(TDF_RelocationTable)&) const
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

