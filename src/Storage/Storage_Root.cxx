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


#include <Standard_Persistent.hxx>
#include <Standard_Type.hxx>
#include <Storage_Root.hxx>
#include <Storage_Schema.hxx>
#include <TCollection_AsciiString.hxx>

IMPLEMENT_STANDARD_RTTIEXT(Storage_Root,Standard_Transient)

Storage_Root::Storage_Root()
  : myRef (0) {}

Storage_Root::Storage_Root (const TCollection_AsciiString&    theName,
                            const Handle(Standard_Persistent)& theObject)
  : myName   (theName)
  , myObject (theObject)
  , myRef    (0)
{}

Storage_Root::Storage_Root (const TCollection_AsciiString& theName,
                            const Standard_Integer         theRef,
                            const TCollection_AsciiString& theType)
  : myName (theName)
  , myType (theType)
  , myRef  (theRef)
{}

void Storage_Root::SetName (const TCollection_AsciiString& theName) 
{
  myName = theName;
}

TCollection_AsciiString Storage_Root::Name() const
{
  return myName;
}

void Storage_Root::SetObject(const Handle(Standard_Persistent)& anObject) 
{
  myObject = anObject;
}

Handle(Standard_Persistent) Storage_Root::Object() const
{
  return myObject;
}

TCollection_AsciiString Storage_Root::Type() const
{
  return myType;
}

void Storage_Root::SetReference(const Standard_Integer aRef) 
{
  myRef = aRef;
}

Standard_Integer Storage_Root::Reference() const
{
  return myRef;
}

void Storage_Root::SetType(const TCollection_AsciiString& aType) 
{
  myType = aType;
}
