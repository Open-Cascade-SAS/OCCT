// Created on: 1998-01-15
// Created by: Denis PASCAL
// Copyright (c) 1998-1999 Matra Datavision
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
#include <TDataStd_Comment.hxx>
#include <TDataStd_Integer.hxx>
#include <TDataStd_Real.hxx>
#include <TDF_Attribute.hxx>
#include <TDF_ChildIDIterator.hxx>
#include <TDF_ChildIterator.hxx>
#include <TDF_Label.hxx>
#include <TDF_RelocationTable.hxx>
#include <TDF_Tool.hxx>

IMPLEMENT_STANDARD_RTTIEXT(TDataStd_Comment,TDF_Attribute)

#define lid1 45
#define lid2 36

//=======================================================================
//function : GetID
//purpose  : 
//=======================================================================

const Standard_GUID& TDataStd_Comment::GetID () 
{
  static Standard_GUID TDataStd_CommentID ("2a96b616-ec8b-11d0-bee7-080009dc3333");
  return TDataStd_CommentID;
}



//=======================================================================
//function : Set
//purpose  : 
//=======================================================================

Handle(TDataStd_Comment) TDataStd_Comment::Set (const TDF_Label& L,
                                                const TCollection_ExtendedString& S) 
{
  Handle(TDataStd_Comment) A;
  if (!L.FindAttribute(TDataStd_Comment::GetID(),A)) {
    A = new TDataStd_Comment (); 
    L.AddAttribute(A);
  }
  A->Set (S); 
  return A;
}


//=======================================================================
//function : Set
//purpose  : 
//=======================================================================

Handle(TDataStd_Comment) TDataStd_Comment::Set (const TDF_Label& L)
{
  Handle(TDataStd_Comment) A;
  if (!L.FindAttribute(TDataStd_Comment::GetID(),A)) {
    A = new TDataStd_Comment (); 
    L.AddAttribute(A);
  }
  return A;
}


//=======================================================================
//function : TDataStd_Comment
//purpose  : 
//=======================================================================
TDataStd_Comment::TDataStd_Comment () { }



//=======================================================================
//function : Set
//purpose  : 
//=======================================================================
void TDataStd_Comment::Set (const TCollection_ExtendedString& S) 
{
  // OCC2932 correction
  if(myString == S) return;

  Backup();

  myString = S;
}



//=======================================================================
//function : Get
//purpose  : 
//=======================================================================
const TCollection_ExtendedString& TDataStd_Comment::Get () const
{
  return myString;
}

//=======================================================================
//function : ID
//purpose  : 
//=======================================================================

const Standard_GUID& TDataStd_Comment::ID () const { return GetID(); }


//=======================================================================
//function : NewEmpty
//purpose  : 
//=======================================================================

Handle(TDF_Attribute) TDataStd_Comment::NewEmpty () const
{  
  return new TDataStd_Comment(); 
}

//=======================================================================
//function : Restore
//purpose  : 
//=======================================================================

void TDataStd_Comment::Restore(const Handle(TDF_Attribute)& with) 
{
  myString = Handle(TDataStd_Comment)::DownCast (with)->Get ();
  return;
}

//=======================================================================
//function : Paste
//purpose  : 
//=======================================================================

void TDataStd_Comment::Paste (const Handle(TDF_Attribute)& into,
                              const Handle(TDF_RelocationTable)& /*RT*/) const
{
  Handle(TDataStd_Comment)::DownCast (into)->Set (myString);
}

//=======================================================================
//function : Dump
//purpose  : 
//=======================================================================

Standard_OStream& TDataStd_Comment::Dump (Standard_OStream& anOS) const
{ 
  TDF_Attribute::Dump(anOS);
  anOS << "Comment=|"<<myString<<"|";
  return anOS;
}
Standard_Boolean TDataStd_Comment::
AfterRetrieval(const Standard_Boolean)
{
  return Standard_True;
}
