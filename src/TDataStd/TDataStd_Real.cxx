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



#include <TDataStd_Real.ixx>
#include <TDF_Reference.hxx>
#include <TDataStd.hxx>


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
//function : Set
//purpose  : 
//=======================================================================

Handle(TDataStd_Real) TDataStd_Real::Set (const TDF_Label&    L,
                                          const Standard_Real V) 
{
  Handle(TDataStd_Real) A;
  if (!L.FindAttribute(TDataStd_Real::GetID(), A)) {
    A = new TDataStd_Real ();
    L.AddAttribute(A);
  }
  A->Set (V); 
  return A;
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
    const TDF_Label& label = reference->Get();
    return label.IsAttribute (TDataStd_Real::GetID());
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

const Standard_GUID& TDataStd_Real::ID() const { return GetID(); }


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
  myDimension = R->GetDimension();
}



//=======================================================================
//function : Paste
//purpose  : 
//=======================================================================

void TDataStd_Real::Paste (const Handle(TDF_Attribute)& Into,
			   const Handle(TDF_RelocationTable)& RT) const
{ 
  Handle(TDataStd_Real) R = Handle(TDataStd_Real)::DownCast (Into);
  R->Set(myValue);
  R->SetDimension(myDimension);
}

//=======================================================================
//function : Dump
//purpose  : 
//=======================================================================

Standard_OStream& TDataStd_Real::Dump (Standard_OStream& anOS) const
{  
  anOS << "Real "; 
  TDataStd::Print(GetDimension(),anOS);
  return anOS;
}
 
