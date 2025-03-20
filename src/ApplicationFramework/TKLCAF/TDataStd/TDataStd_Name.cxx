// Created on: 1997-07-31
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

#include <TDataStd_Name.hxx>
#include <TDF_Label.hxx>

IMPLEMENT_DERIVED_ATTRIBUTE(TDataStd_Name, TDataStd_GenericExtString)

//=================================================================================================

const Standard_GUID& TDataStd_Name::GetID()
{
  static Standard_GUID TDataStd_NameID("2a96b608-ec8b-11d0-bee7-080009dc3333");
  return TDataStd_NameID;
}

//=======================================================================
// function : SetAttr
// purpose  : Implements Set functionality
//=======================================================================
static Handle(TDataStd_Name) SetAttr(const TDF_Label&                  label,
                                     const TCollection_ExtendedString& theString,
                                     const Standard_GUID&              theGuid)
{
  Handle(TDataStd_Name) N;
  if (!label.FindAttribute(theGuid, N))
  {
    N = new TDataStd_Name();
    N->SetID(theGuid);
    label.AddAttribute(N);
  }
  N->Set(theString);
  return N;
}

//=================================================================================================

Handle(TDataStd_Name) TDataStd_Name::Set(const TDF_Label&                  label,
                                         const TCollection_ExtendedString& theString)
{
  return SetAttr(label, theString, GetID());
}

//=======================================================================
// function : Set
// purpose  : Set user defined attribute
//=======================================================================

Handle(TDataStd_Name) TDataStd_Name::Set(const TDF_Label&                  label,
                                         const Standard_GUID&              theGuid,
                                         const TCollection_ExtendedString& theString)
{
  return SetAttr(label, theString, theGuid);
}

//=================================================================================================

TDataStd_Name::TDataStd_Name()
{
  myID = GetID();
}

//=================================================================================================

void TDataStd_Name::Set(const TCollection_ExtendedString& S)
{
  if (myString == S)
    return;

  Backup();
  myString = S;
}

//=================================================================================================

void TDataStd_Name::SetID(const Standard_GUID& theGuid)
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

void TDataStd_Name::SetID()
{
  Backup();
  myID = GetID();
}

// TDF_Attribute methods
//=================================================================================================

Standard_OStream& TDataStd_Name::Dump(Standard_OStream& anOS) const
{
  TDF_Attribute::Dump(anOS);
  anOS << " Name=|" << myString << "|";
  Standard_Character sguid[Standard_GUID_SIZE_ALLOC];
  myID.ToCString(sguid);
  anOS << sguid << std::endl;
  return anOS;
}
