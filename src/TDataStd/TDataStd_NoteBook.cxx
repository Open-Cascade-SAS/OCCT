// Created on: 1997-07-29
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


#include <Standard_DomainError.hxx>
#include <Standard_GUID.hxx>
#include <Standard_Type.hxx>
#include <TDataStd_Integer.hxx>
#include <TDataStd_NoteBook.hxx>
#include <TDataStd_Real.hxx>
#include <TDF_Attribute.hxx>
#include <TDF_Label.hxx>
#include <TDF_RelocationTable.hxx>
#include <TDF_TagSource.hxx>

IMPLEMENT_STANDARD_RTTIEXT(TDataStd_NoteBook,TDF_Attribute)

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

  for(;;) {
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
    throw Standard_DomainError("TDataStd_NoteBook::New : not an empty label");
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


