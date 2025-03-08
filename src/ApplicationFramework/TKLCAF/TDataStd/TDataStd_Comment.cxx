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

#include <TDataStd_Comment.hxx>

#include <Standard_Dump.hxx>
#include <TDF_Label.hxx>

IMPLEMENT_DERIVED_ATTRIBUTE(TDataStd_Comment, TDataStd_GenericExtString)

//=================================================================================================

const Standard_GUID& TDataStd_Comment::GetID()
{
  static Standard_GUID TDataStd_CommentID("2a96b616-ec8b-11d0-bee7-080009dc3333");
  return TDataStd_CommentID;
}

//=================================================================================================

Handle(TDataStd_Comment) TDataStd_Comment::Set(const TDF_Label&                  L,
                                               const TCollection_ExtendedString& S)
{
  Handle(TDataStd_Comment) A;
  if (!L.FindAttribute(TDataStd_Comment::GetID(), A))
  {
    A = new TDataStd_Comment();
    L.AddAttribute(A);
  }
  A->Set(S);
  return A;
}

//=================================================================================================

Handle(TDataStd_Comment) TDataStd_Comment::Set(const TDF_Label& L)
{
  Handle(TDataStd_Comment) A;
  if (!L.FindAttribute(TDataStd_Comment::GetID(), A))
  {
    A = new TDataStd_Comment();
    L.AddAttribute(A);
  }
  return A;
}

//=================================================================================================

TDataStd_Comment::TDataStd_Comment()
{
  myID = GetID();
}

//=================================================================================================

void TDataStd_Comment::Set(const TCollection_ExtendedString& S)
{
  // OCC2932 correction
  if (myString == S)
    return;

  Backup();
  myString = S;
}

//=================================================================================================

void TDataStd_Comment::SetID(const Standard_GUID& theGuid)
{
  if (myID == theGuid)
    return;

  Backup();
  myID = theGuid;
}

//=======================================================================
// function : SetID
// purpose  : sets default ID
//=======================================================================

void TDataStd_Comment::SetID()
{
  Backup();
  myID = GetID();
}

//=================================================================================================

Standard_OStream& TDataStd_Comment::Dump(Standard_OStream& anOS) const
{
  TDF_Attribute::Dump(anOS);
  anOS << "Comment=|" << Get() << "|";
  return anOS;
}
