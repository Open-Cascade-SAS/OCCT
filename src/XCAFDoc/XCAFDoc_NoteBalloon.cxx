// Created on: 2017-08-10
// Created by: Sergey NIKONOV
// Copyright (c) 2000-2017 OPEN CASCADE SAS
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
#include <TDF_Label.hxx>
#include <XCAFDoc_NoteBalloon.hxx>

IMPLEMENT_STANDARD_RTTIEXT(XCAFDoc_NoteBalloon, XCAFDoc_NoteComment)

const Standard_GUID& 
XCAFDoc_NoteBalloon::GetID()
{
  static Standard_GUID s_ID("1127951D-87D5-4ecc-89D5-D1406576C43F");
  return s_ID;
}

Handle(XCAFDoc_NoteBalloon)
XCAFDoc_NoteBalloon::Get(const TDF_Label& theLabel)
{
  Handle(XCAFDoc_NoteBalloon) aThis;
  theLabel.FindAttribute(XCAFDoc_NoteBalloon::GetID(), aThis);
  return aThis;
}

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

XCAFDoc_NoteBalloon::XCAFDoc_NoteBalloon()
{
}

const Standard_GUID& 
XCAFDoc_NoteBalloon::ID() const
{
  return GetID();
}

Handle(TDF_Attribute) 
XCAFDoc_NoteBalloon::NewEmpty() const
{
  return new XCAFDoc_NoteBalloon();
}
