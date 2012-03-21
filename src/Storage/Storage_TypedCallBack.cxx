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

#include <Storage_TypedCallBack.ixx>

Storage_TypedCallBack::Storage_TypedCallBack() : myIndex(0)
{
}

Storage_TypedCallBack::Storage_TypedCallBack(const TCollection_AsciiString& aTypeName,const Handle(Storage_CallBack)& aCallBack) : myIndex(0)
{
  myType = aTypeName;
  myCallBack = aCallBack;
}

void Storage_TypedCallBack::SetType(const TCollection_AsciiString& aType) 
{
  myType = aType;
}

TCollection_AsciiString Storage_TypedCallBack::Type() const
{
  return myType;
}

void Storage_TypedCallBack::SetCallBack(const Handle(Storage_CallBack)& aCallBack) 
{
  myCallBack = aCallBack;
}

Handle(Storage_CallBack) Storage_TypedCallBack::CallBack() const
{
  return myCallBack;
}

void Storage_TypedCallBack::SetIndex(const Standard_Integer anIndex)
{
  myIndex  = anIndex;
}

Standard_Integer Storage_TypedCallBack::Index() const
{
  return myIndex;
}
