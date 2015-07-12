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

Storage_Root::Storage_Root() : myRef(0)
{
}

Storage_Root::Storage_Root(const TCollection_AsciiString& aName,const Handle(Standard_Persistent)& anObject) : myRef(0)
{
  myName   = aName;
  myObject = anObject;
  
  if (!anObject.IsNull()) {
    myType   = anObject->DynamicType()->Name();
  }
}

void Storage_Root::SetName(const TCollection_AsciiString& aName) 
{
  myName   = aName;
}

TCollection_AsciiString Storage_Root::Name() const
{
  return myName;
}

void Storage_Root::SetObject(const Handle(Standard_Persistent)& anObject) 
{
  myObject = anObject;

  if (!anObject.IsNull()) {
    myType   = anObject->DynamicType()->Name();
  }
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
