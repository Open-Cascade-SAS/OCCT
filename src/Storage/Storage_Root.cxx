// Copyright (c) 1998-1999 Matra Datavision
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

#include <Storage_Root.ixx>

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
