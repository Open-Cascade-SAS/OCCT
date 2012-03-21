// Created on: 2000-03-01
// Created by: Denis PASCAL
// Copyright (c) 2000-2012 OPEN CASCADE SAS
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



#include <TDF_Reference.ixx>



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


