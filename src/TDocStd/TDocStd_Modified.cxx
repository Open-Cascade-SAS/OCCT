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


#include <Standard_DomainError.hxx>
#include <Standard_GUID.hxx>
#include <Standard_Type.hxx>
#include <TDF_Attribute.hxx>
#include <TDF_Data.hxx>
#include <TDF_Label.hxx>
#include <TDF_RelocationTable.hxx>
#include <TDocStd_Modified.hxx>

IMPLEMENT_STANDARD_RTTIEXT(TDocStd_Modified,TDF_Attribute)

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
    throw Standard_DomainError("TDocStd_Modified::Get : IsEmpty");
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

void TDocStd_Modified::Paste (const Handle(TDF_Attribute)&,
			      const Handle(TDF_RelocationTable)&) const
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

