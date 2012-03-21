// Created on: 1997-07-29
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



#include <TDataStd_NoteBook.ixx>
#include <TDF_TagSource.hxx>
#include <TDataStd_Real.hxx>
#include <TDataStd_Integer.hxx>
#include <TDF_Label.hxx> 
#include <Standard_DomainError.hxx>

//=======================================================================
//function : Find
//purpose  : 
//=======================================================================

Standard_Boolean TDataStd_NoteBook::Find (const TDF_Label& current,
					  Handle(TDataStd_NoteBook)& N) 
{  
  TDF_Label L = current;
  Handle(TDataStd_NoteBook) NB;
  if (L.IsNull()) return Standard_False; 

  while (1) {
    if(L.FindAttribute(TDataStd_NoteBook::GetID(), NB)) break; 
    L = L.Father();
    if (L.IsNull()) break; 
  }

  if (!NB.IsNull()) { 
    N = NB;
    return Standard_True; 
  }
  return Standard_False; 
}


//=======================================================================
//function : New
//purpose  : 
//=======================================================================

Handle(TDataStd_NoteBook) TDataStd_NoteBook::New (const TDF_Label& label)
{  
  if (label.HasAttribute()) {
    Standard_DomainError::Raise("TDataStd_NoteBook::New : not an empty label");
  }
  Handle(TDataStd_NoteBook) NB = new TDataStd_NoteBook ();  
  label.AddAttribute(NB);
  TDF_TagSource::Set(label);    // distributeur de sous label
  return NB;
}

//=======================================================================
//function : GetID
//purpose  : 
//=======================================================================

const Standard_GUID& TDataStd_NoteBook::GetID() 
{
  static Standard_GUID TDataStd_NoteBookID("2a96b609-ec8b-11d0-bee7-080009dc3333");
  return TDataStd_NoteBookID;
}


//=======================================================================
//function : TDataStd_NoteBook
//purpose  : 
//=======================================================================

TDataStd_NoteBook::TDataStd_NoteBook()
{
}


//=======================================================================
//function : Append Real Variable
//purpose  : 
//=======================================================================

Handle(TDataStd_Real) TDataStd_NoteBook::Append(const Standard_Real value,
						const Standard_Boolean ) 
{
  TDF_Label newlabel =  TDF_TagSource::NewChild (Label());
  Handle(TDataStd_Real) variable = TDataStd_Real::Set ( newlabel, value); 
  return variable;
}

//=======================================================================
//function : Append Integer Variable
//purpose  : 
//=======================================================================

Handle(TDataStd_Integer) TDataStd_NoteBook::Append(const Standard_Integer value,
						   const Standard_Boolean ) 
{
  TDF_Label newlabel =  TDF_TagSource::NewChild (Label());
  Handle(TDataStd_Integer) variable = TDataStd_Integer::Set ( newlabel, value); 
  return variable;
}

//=======================================================================
//function : ID
//purpose  : 
//=======================================================================

const Standard_GUID& TDataStd_NoteBook::ID() const
{ return GetID(); }


//=======================================================================
//function : NewEmpty
//purpose  : 
//=======================================================================

Handle(TDF_Attribute) TDataStd_NoteBook::NewEmpty () const
{  
  return new TDataStd_NoteBook(); 
}

//=======================================================================
//function : Restore
//purpose  : 
//=======================================================================

void TDataStd_NoteBook::Restore(const Handle(TDF_Attribute)& ) 
{
}

//=======================================================================
//function : Paste
//purpose  : 
//=======================================================================

void TDataStd_NoteBook::Paste (const Handle(TDF_Attribute)& ,
			       const Handle(TDF_RelocationTable)& ) const
{
}

//=======================================================================
//function : Dump
//purpose  : 
//=======================================================================

Standard_OStream& TDataStd_NoteBook::Dump (Standard_OStream& anOS) const
{  
  anOS << "NoteBook";
  return anOS;
}


