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
#include <TDataStd.hxx>
#include <TDataStd_Real.hxx>
#include <TDF_Attribute.hxx>
#include <TDF_Label.hxx>
#include <TDF_Reference.hxx>
#include <TDF_RelocationTable.hxx>

IMPLEMENT_STANDARD_RTTIEXT(TDataStd_Real,TDF_Attribute)

//=======================================================================
//function : GetID
//purpose  : 
//=======================================================================
const Standard_GUID& TDataStd_Real::GetID() 
{
  static Standard_GUID TDataStd_RealID("2a96b60f-ec8b-11d0-bee7-080009dc3333");
  return TDataStd_RealID;
}

//=======================================================================
//function : SetAttr
//purpose  : Implements Set functionality
//=======================================================================
static Handle(TDataStd_Real) SetAttr(const TDF_Label&     label,
                                     const Standard_Real  V,
                                     const Standard_GUID& theGuid)
{
  Handle(TDataStd_Real) A;
  if (!label.FindAttribute(theGuid, A)) {
    A = new TDataStd_Real ();
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

Handle(TDataStd_Real) TDataStd_Real::Set (const TDF_Label&    L,
                                          const Standard_Real V) 
{
  return SetAttr(L, V, GetID());
}

//=======================================================================
//function : Set
//purpose  : User defined attribute
//=======================================================================

Handle(TDataStd_Real) TDataStd_Real::Set (const TDF_Label&    L,
                                          const Standard_GUID& theGuid,
                                          const Standard_Real V) 
{
  return SetAttr(L, V, theGuid);
}

//=======================================================================
//function : TDataStd_Real
//purpose  : Empty constructor
//=======================================================================

TDataStd_Real::TDataStd_Real ()
     : myValue     (RealFirst()),
       myDimension (TDataStd_SCALAR)
{}



//=======================================================================
//function : IsCaptured
//purpose  : 
//=======================================================================

Standard_Boolean TDataStd_Real::IsCaptured() const
{
  Handle(TDF_Reference) reference;
  // pour test

  if (Label().FindAttribute(TDF_Reference::GetID(),reference)) {
    const TDF_Label& aLabel = reference->Get();	
    return aLabel.IsAttribute (myID);

  }
  return Standard_False;
}


//=======================================================================
//function : Set
//purpose  : 
//=======================================================================

void TDataStd_Real::Set(const Standard_Real v) 
{
  // OCC2932 correction
  if( myValue == v) return;

  Backup();
  myValue = v;
}

//=======================================================================
//function : Get
//purpose  : 
//=======================================================================

Standard_Real TDataStd_Real::Get() const { return myValue; }


//=======================================================================
//function : SetDimension 
//purpose  : 
//=======================================================================

void TDataStd_Real::SetDimension (const TDataStd_RealEnum DIM) 
{
  // OCC2932 correction  
  if(myDimension == DIM) return;

  Backup();
  myDimension = DIM;
}


//=======================================================================
//function : GetDimension
//purpose  : 
//=======================================================================

TDataStd_RealEnum TDataStd_Real::GetDimension () const
{
  return myDimension;
}


//=======================================================================
//function : ID
//purpose  : 
//=======================================================================

const Standard_GUID& TDataStd_Real::ID() const { return myID; }

//=======================================================================
//function : SetID
//purpose  :
//=======================================================================

void TDataStd_Real::SetID( const Standard_GUID&  theGuid)
{  
  if(myID == theGuid) return;

  Backup();
  myID = theGuid;
}

//=======================================================================
//function : SetID
//purpose  : sets default ID
//=======================================================================
void TDataStd_Real::SetID()
{
  Backup();
  myID = GetID();
}
//=======================================================================
//function : NewEmpty
//purpose  : 
//=======================================================================

Handle(TDF_Attribute) TDataStd_Real::NewEmpty () const
{
  return new TDataStd_Real();
}

//=======================================================================
//function : Restore
//purpose  : 
//=======================================================================

void TDataStd_Real::Restore(const Handle(TDF_Attribute)& With) 
{
  Handle(TDataStd_Real) R = Handle(TDataStd_Real)::DownCast (With);
  myValue = R->Get();
  Standard_DISABLE_DEPRECATION_WARNINGS
  myDimension = R->GetDimension();
  Standard_ENABLE_DEPRECATION_WARNINGS
  myID = R->ID();
}



//=======================================================================
//function : Paste
//purpose  : 
//=======================================================================

void TDataStd_Real::Paste (const Handle(TDF_Attribute)& Into,
			   const Handle(TDF_RelocationTable)& /*RT*/) const
{ 
  Handle(TDataStd_Real) R = Handle(TDataStd_Real)::DownCast (Into);
  R->Set(myValue);
  Standard_DISABLE_DEPRECATION_WARNINGS
  R->SetDimension(myDimension);
  Standard_ENABLE_DEPRECATION_WARNINGS
  R->SetID(myID);
}

//=======================================================================
//function : Dump
//purpose  : 
//=======================================================================

Standard_OStream& TDataStd_Real::Dump (Standard_OStream& anOS) const
{  
  anOS << "Real "; 
  Standard_DISABLE_DEPRECATION_WARNINGS
  TDataStd::Print(GetDimension(), anOS);
  Standard_ENABLE_DEPRECATION_WARNINGS
  anOS << myValue; 
  Standard_Character sguid[Standard_GUID_SIZE_ALLOC];
  myID.ToCString(sguid);
  anOS << sguid;
  return anOS;
}
 
