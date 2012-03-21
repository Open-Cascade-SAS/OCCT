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




#include <TDocStd_Modified.ixx>
#include <Standard_DomainError.hxx>
#include <TDF_Data.hxx>



//=======================================================================
//function : IsEmpty
//purpose  : 
//=======================================================================

Standard_Boolean TDocStd_Modified::IsEmpty(const TDF_Label& access) 
{
  Handle(TDocStd_Modified) MDF;
  if (!access.Root().FindAttribute (TDocStd_Modified::GetID(), MDF)) {
    return Standard_True;
  }
  else return MDF->IsEmpty();
}

//=======================================================================
//function : Add
//purpose  : 
//=======================================================================

Standard_Boolean TDocStd_Modified::Add(const TDF_Label& alabel) 
{  
  Handle(TDocStd_Modified) MDF;
  if (!alabel.Root().FindAttribute (TDocStd_Modified::GetID(), MDF)) {
    MDF = new TDocStd_Modified();
    alabel.Root().AddAttribute(MDF);
  }
  return MDF->AddLabel (alabel);
}

//=======================================================================
//function : Remove
//purpose  : 
//=======================================================================

Standard_Boolean TDocStd_Modified::Remove(const TDF_Label& alabel) 
{  
  Handle(TDocStd_Modified) MDF;
  if (!alabel.Root().FindAttribute (TDocStd_Modified::GetID(), MDF)) {
    return Standard_True;
  }
  else return MDF->RemoveLabel (alabel);
}

//=======================================================================
//function : Contains
//purpose  : 
//=======================================================================

Standard_Boolean TDocStd_Modified::Contains (const TDF_Label& alabel) 
{  
  Handle(TDocStd_Modified) MDF;
  if (!alabel.Root().FindAttribute (TDocStd_Modified::GetID(), MDF)) {
    return Standard_False;
  }
  return (MDF->Get().Contains(alabel));
}

//=======================================================================
//function : Get
//purpose  : 
//=======================================================================

const TDF_LabelMap& TDocStd_Modified::Get (const TDF_Label& access) 
{  
  Handle(TDocStd_Modified) MDF;
  if (!access.Root().FindAttribute (TDocStd_Modified::GetID(), MDF)) {
    Standard_DomainError::Raise("TDocStd_Modified::Get : IsEmpty");
  }
  return MDF->Get();
}

//=======================================================================
//function : Clear
//purpose  : 
//=======================================================================

void TDocStd_Modified::Clear (const TDF_Label& access) 
{  
  Handle(TDocStd_Modified) MDF;
  if (!access.Root().FindAttribute (TDocStd_Modified::GetID(), MDF)) {
    return;
  }
  else MDF->Clear();
}

//=======================================================================
//function : GetID
//purpose  : 
//=======================================================================

const Standard_GUID& TDocStd_Modified::GetID() 
{ 
  static Standard_GUID TDocStd_ModifiedID ("2a96b622-ec8b-11d0-bee7-080009dc3333");
  return TDocStd_ModifiedID; 
}


//=======================================================================
//function : TDocStd_Modified
//purpose  : 
//=======================================================================

TDocStd_Modified::TDocStd_Modified () { }


//=======================================================================
//function : AddLabel
//purpose  : 
//=======================================================================

Standard_Boolean TDocStd_Modified::AddLabel (const TDF_Label& L) 
{
  Backup ();
  return myModified.Add(L);
}


//=======================================================================
//function : RemoveLabel
//purpose  : 
//=======================================================================

Standard_Boolean TDocStd_Modified::RemoveLabel (const TDF_Label& L) 
{
  Backup ();
  return myModified.Remove(L);
}

//=======================================================================
//function : IsEmpty
//purpose  : 
//=======================================================================

Standard_Boolean TDocStd_Modified::IsEmpty () const
{
  return myModified.IsEmpty();
}


//=======================================================================
//function : Clear
//purpose  : 
//=======================================================================

void TDocStd_Modified::Clear () 
{  
  Backup ();
  myModified.Clear();
}

//=======================================================================
//function : Get 
//purpose  : 
//=======================================================================

const TDF_LabelMap& TDocStd_Modified::Get () const
{  
  return myModified;
}


//=======================================================================
//function : ID
//purpose  : 
//=======================================================================

const Standard_GUID& TDocStd_Modified::ID () const { return GetID(); }


//=======================================================================
//function : NewEmpty
//purpose  : 
//=======================================================================

Handle(TDF_Attribute) TDocStd_Modified::NewEmpty () const
{
  return new TDocStd_Modified ();
}

//=======================================================================
//function : Restore
//purpose  : 
//=======================================================================

void TDocStd_Modified::Restore(const Handle(TDF_Attribute)& With) {
  Handle(TDocStd_Modified) MDF = Handle(TDocStd_Modified)::DownCast(With);
  myModified = MDF->myModified;
}

//=======================================================================
//function : Paste
//purpose  : 
//=======================================================================

void TDocStd_Modified::Paste (const Handle(TDF_Attribute)& Into,
			      const Handle(TDF_RelocationTable)& RT) const
{
}

//=======================================================================
//function : Dump
//purpose  : 
//=======================================================================

Standard_OStream& TDocStd_Modified::Dump (Standard_OStream& anOS) const
{  
  anOS << "Modified";
  return anOS;
}

