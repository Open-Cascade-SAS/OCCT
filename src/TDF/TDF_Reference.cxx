// Created on: 2000-03-01
// Created by: Denis PASCAL
// Copyright (c) 2000-2014 OPEN CASCADE SAS
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
#include <TDF_DataSet.hxx>
#include <TDF_Label.hxx>
#include <TDF_Reference.hxx>
#include <TDF_RelocationTable.hxx>

IMPLEMENT_STANDARD_RTTIEXT(TDF_Reference,TDF_Attribute)

//=======================================================================
//function : GetID
//purpose  : 
//=======================================================================
const Standard_GUID& TDF_Reference::GetID () 
{
  static Standard_GUID TDF_ReferenceID("2a96b610-ec8b-11d0-bee7-080009dc3333");
  return TDF_ReferenceID;
}

//=======================================================================
//function : Set
//purpose  : 
//=======================================================================

Handle(TDF_Reference) TDF_Reference::Set (const TDF_Label& L,
					  const TDF_Label& Origin) 
{
  Handle(TDF_Reference) A;
  if (!L.FindAttribute (TDF_Reference::GetID (),A)) {
    A = new TDF_Reference ();
    L.AddAttribute (A);
  }
  A->Set (Origin); 
  return A;
}

//=======================================================================
//function : TDF_Reference
//purpose  : Empty Constructor
//=======================================================================

TDF_Reference::TDF_Reference () { }


//=======================================================================
//function : Set
//purpose  : 
//=======================================================================

void TDF_Reference::Set(const TDF_Label& Origin) 
{
  // OCC2932 correction
  if(myOrigin == Origin) return;

  Backup();
  myOrigin = Origin;
}

//=======================================================================
//function : Get
//purpose  : 
//=======================================================================

TDF_Label TDF_Reference::Get() const
{
  return myOrigin;
}


//=======================================================================
//function : ID
//purpose  : 
//=======================================================================

const Standard_GUID& TDF_Reference::ID() const { return GetID(); }


//=======================================================================
//function : NewEmpty
//purpose  : 
//=======================================================================

Handle(TDF_Attribute) TDF_Reference::NewEmpty () const
{  
  return new TDF_Reference(); 
}

//=======================================================================
//function : Restore
//purpose  : 
//=======================================================================

void TDF_Reference::Restore(const Handle(TDF_Attribute)& With) 
{
  myOrigin = Handle(TDF_Reference)::DownCast (With)->Get ();
}

//=======================================================================
//function : Paste
//purpose  : 
//=======================================================================

void TDF_Reference::Paste (const Handle(TDF_Attribute)& Into,
				const Handle(TDF_RelocationTable)& RT) const
{
  TDF_Label tLab;
  if (!myOrigin.IsNull()) {
    if (!RT->HasRelocation(myOrigin,tLab)) tLab = myOrigin;
  }
  Handle(TDF_Reference)::DownCast(Into)->Set(tLab);
}
//=======================================================================
//function : References
//purpose  : Adds the referenced attributes or labels.
//=======================================================================

void TDF_Reference::References(const Handle(TDF_DataSet)& aDataSet) const
{
  if (!Label().IsImported()) aDataSet->AddLabel( myOrigin); //pour real et entier mais surtout pas les parts ...
}

//=======================================================================
//function : Dump
//purpose  : 
//=======================================================================

Standard_OStream& TDF_Reference::Dump (Standard_OStream& anOS) const
{  
  anOS << "Reference";
  return anOS;
}


