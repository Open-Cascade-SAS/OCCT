// Copyright (c) 2017-2018 OPEN CASCADE SAS
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

#include <XCAFDoc_NoteBalloon.hxx>

#include <Standard_GUID.hxx>
#include <TDF_Label.hxx>

IMPLEMENT_DERIVED_ATTRIBUTE(XCAFDoc_NoteBalloon, XCAFDoc_NoteComment)

// =======================================================================
// function : GetID
// purpose  :
// =======================================================================
const Standard_GUID&
XCAFDoc_NoteBalloon::GetID()
{
  static Standard_GUID s_ID("1127951D-87D5-4ecc-89D5-D1406576C43F");
  return s_ID;
}

// =======================================================================
// function : Get
// purpose  :
// =======================================================================
Handle(XCAFDoc_NoteBalloon)
XCAFDoc_NoteBalloon::Get(const TDF_Label& theLabel)
{
  Handle(XCAFDoc_NoteBalloon) aThis;
  theLabel.FindAttribute(XCAFDoc_NoteBalloon::GetID(), aThis);
  return aThis;
}

// =======================================================================
// function : Set
// purpose  :
// =======================================================================
Handle(XCAFDoc_NoteBalloon)
XCAFDoc_NoteBalloon::Set(const TDF_Label&                  theLabel,
                         const TCollection_ExtendedString& theUserName,
                         const TCollection_ExtendedString& theTimeStamp,
                         const TCollection_ExtendedString& theComment)
{
  Handle(XCAFDoc_NoteBalloon) aNoteBalloon;
  if (!theLabel.IsNull() && !theLabel.FindAttribute(XCAFDoc_NoteBalloon::GetID(), aNoteBalloon))
  {
    aNoteBalloon = new XCAFDoc_NoteBalloon();
    aNoteBalloon->XCAFDoc_Note::Set(theUserName, theTimeStamp);
    aNoteBalloon->XCAFDoc_NoteComment::Set(theComment);
    theLabel.AddAttribute(aNoteBalloon);
  }
  return aNoteBalloon;
}

// =======================================================================
// function : XCAFDoc_NoteBalloon
// purpose  :
// =======================================================================
XCAFDoc_NoteBalloon::XCAFDoc_NoteBalloon()
{
}

// =======================================================================
// function : ID
// purpose  :
// =======================================================================
const Standard_GUID&
XCAFDoc_NoteBalloon::ID() const
{
  return GetID();
}
