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


#include <Standard_Type.hxx>
#include <Storage_CallBack.hxx>
#include <Storage_TypedCallBack.hxx>
#include <TCollection_AsciiString.hxx>

IMPLEMENT_STANDARD_RTTIEXT(Storage_TypedCallBack,Standard_Transient)

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
