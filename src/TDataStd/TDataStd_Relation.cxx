// Created on: 1997-12-10
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
#include <TCollection_AsciiString.hxx>
#include <TCollection_ExtendedString.hxx>
#include <TDataStd_Relation.hxx>
#include <TDataStd_Variable.hxx>
#include <TDF_Attribute.hxx>
#include <TDF_Label.hxx>
#include <TDF_ListIteratorOfAttributeList.hxx>
#include <TDF_RelocationTable.hxx>

IMPLEMENT_STANDARD_RTTIEXT(TDataStd_Relation,TDF_Attribute)

//=======================================================================
//function : GetID
//purpose  : 
//=======================================================================
const Standard_GUID& TDataStd_Relation::GetID() 
{  
  static Standard_GUID TDataStd_RelationID("ce24146b-8e57-11d1-8953-080009dc4425");
  return TDataStd_RelationID;
}

//=======================================================================
//function : Set
//purpose  : 
//=======================================================================

Handle(TDataStd_Relation) TDataStd_Relation::Set(const TDF_Label& L) 
{  
  Handle(TDataStd_Relation) A;
  if (!L.FindAttribute (TDataStd_Relation::GetID(), A)) {
    A = new TDataStd_Relation ();
    L.AddAttribute(A);
  }
  return A;
}

//=======================================================================
//function : TDataStd_Relation
//purpose  : 
//=======================================================================

TDataStd_Relation::TDataStd_Relation()
{
}


//=======================================================================
//function : Name
//purpose  : 
//=======================================================================
TCollection_ExtendedString TDataStd_Relation::Name () const 
{  
  return myRelation;  // ->String();
}

//=======================================================================
//function : SetRelation
//purpose  : 
//=======================================================================

void TDataStd_Relation::SetRelation(const TCollection_ExtendedString& R)
{
  // OCC2932 correction
  if(myRelation == R) return;

  Backup();
  myRelation = R;
}

//=======================================================================
//function : GetRelation
//purpose  : 
//=======================================================================

const TCollection_ExtendedString& TDataStd_Relation::GetRelation() const
{
  return myRelation;
}

//=======================================================================
//function : GetVariables
//purpose  : 
//=======================================================================

TDF_AttributeList& TDataStd_Relation::GetVariables()
{
  return myVariables;
}

//=======================================================================
//function : ID
//purpose  : 
//=======================================================================

const Standard_GUID& TDataStd_Relation::ID() const
{
  return GetID();
}

//=======================================================================
//function : Restore
//purpose  : 
//=======================================================================

void TDataStd_Relation::Restore(const Handle(TDF_Attribute)& With) 
{  
  Handle(TDataStd_Relation) REL = Handle(TDataStd_Relation)::DownCast (With);
  myRelation = REL->GetRelation();
  Handle(TDataStd_Variable) V;
  myVariables.Clear();
  for (TDF_ListIteratorOfAttributeList it (REL->GetVariables()); it.More(); it.Next()) {
    V = Handle(TDataStd_Variable)::DownCast(it.Value());
    myVariables.Append(V);
  }
}

//=======================================================================
//function : NewEmpty
//purpose  : 
//=======================================================================

Handle(TDF_Attribute) TDataStd_Relation::NewEmpty() const
{
  return new TDataStd_Relation();
}

//=======================================================================
//function : Paste
//purpose  : 
//=======================================================================

void TDataStd_Relation::Paste(const Handle(TDF_Attribute)& Into,
				     const Handle(TDF_RelocationTable)& RT) const
{  
  Handle(TDataStd_Relation) REL = Handle(TDataStd_Relation)::DownCast (Into); 
  REL->SetRelation(myRelation);  
  Handle(TDataStd_Variable) V1;
  for (TDF_ListIteratorOfAttributeList it (myVariables); it.More(); it.Next()) {
    V1 = Handle(TDataStd_Variable)::DownCast(it.Value());
    Handle(TDF_Attribute) V2;
    RT->HasRelocation (V1,V2);
    REL->GetVariables().Append(V2);
  }
}

//=======================================================================
//function : Dump
//purpose  : 
//=======================================================================

Standard_OStream& TDataStd_Relation::Dump(Standard_OStream& anOS) const
{ 
  anOS << "Relation";
  return anOS;
}

