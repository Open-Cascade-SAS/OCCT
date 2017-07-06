// Copyright (c) 2017 OPEN CASCADE SAS
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

#include <StdObjMgt_Persistent.hxx>
#include <Standard_Type.hxx>
#include <StdStorage_Root.hxx>
#include <TCollection_AsciiString.hxx>

IMPLEMENT_STANDARD_RTTIEXT(StdStorage_Root, Standard_Transient)

StdStorage_Root::StdStorage_Root()
  : myRef(0) 
{
}

StdStorage_Root::StdStorage_Root(const TCollection_AsciiString&      theName,
                                 const Handle(StdObjMgt_Persistent)& theObject)
  : myName(theName)
  , myType(theObject->PName())
  , myObject(theObject)
  , myRef(0)
{
}

StdStorage_Root::StdStorage_Root(const TCollection_AsciiString& theName,
                                 const Standard_Integer         theRef,
                                 const TCollection_AsciiString& theType)
  : myName(theName)
  , myType(theType)
  , myRef(theRef)
{
}

void StdStorage_Root::SetName(const TCollection_AsciiString& theName)
{
  myName = theName;
}

TCollection_AsciiString StdStorage_Root::Name() const
{
  return myName;
}

void StdStorage_Root::SetObject(const Handle(StdObjMgt_Persistent)& anObject)
{
  myObject = anObject;
}

Handle(StdObjMgt_Persistent) StdStorage_Root::Object() const
{
  return myObject;
}

TCollection_AsciiString StdStorage_Root::Type() const
{
  return myType;
}

void StdStorage_Root::SetReference(const Standard_Integer aRef)
{
  myRef = aRef;
}

Standard_Integer StdStorage_Root::Reference() const
{
  return myRef;
}

void StdStorage_Root::SetType(const TCollection_AsciiString& aType)
{
  myType = aType;
}
