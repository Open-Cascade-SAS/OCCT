// Created on: 2017-02-16
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

#include <XCAFDoc_AssemblyItemId.hxx>

XCAFDoc_AssemblyItemId::XCAFDoc_AssemblyItemId()
{

}

XCAFDoc_AssemblyItemId::XCAFDoc_AssemblyItemId(const TColStd_ListOfAsciiString& thePath)
{
  Init(thePath);
}

XCAFDoc_AssemblyItemId::XCAFDoc_AssemblyItemId(const TCollection_AsciiString& theString)
{
  Init(theString);
}

void 
XCAFDoc_AssemblyItemId::Init(const TColStd_ListOfAsciiString& thePath)
{
  myPath = thePath;
}

void 
XCAFDoc_AssemblyItemId::Init(const TCollection_AsciiString& theString)
{
  myPath.Clear();

  for (Standard_Integer iEntry = 1; ; ++iEntry)
  {
    TCollection_AsciiString anEntry = theString.Token("/", iEntry);
    if (anEntry.IsEmpty())
      break;

    myPath.Append(anEntry);
  }
}

Standard_Boolean 
XCAFDoc_AssemblyItemId::IsNull() const
{
  return myPath.IsEmpty();
}

void 
XCAFDoc_AssemblyItemId::Nullify()
{
  myPath.Clear();
}

Standard_Boolean 
XCAFDoc_AssemblyItemId::IsChild(const XCAFDoc_AssemblyItemId& theOther) const
{
  if (myPath.Size() <= theOther.myPath.Size())
    return Standard_False;

  TColStd_ListOfAsciiString::Iterator anIt(myPath), anItOther(theOther.myPath);
  for (; anItOther.More(); anIt.Next(), anItOther.Next())
  {
    if (anIt.Value() != anItOther.Value())
      return Standard_False;
  }

  return Standard_True;
}

Standard_Boolean 
XCAFDoc_AssemblyItemId::IsDirectChild(const XCAFDoc_AssemblyItemId& theOther) const
{
  return ((myPath.Size() == theOther.myPath.Size() - 1) && IsChild(theOther));
}

Standard_Boolean 
XCAFDoc_AssemblyItemId::IsEqual(const XCAFDoc_AssemblyItemId& theOther) const
{
  if (this == &theOther)
    return Standard_True;

  if (myPath.Size() != theOther.myPath.Size())
    return Standard_False;

  TColStd_ListOfAsciiString::Iterator anIt(myPath), anItOther(theOther.myPath);
  for (; anIt.More() && anItOther.More(); anIt.Next(), anItOther.Next())
  {
    if (anIt.Value() != anItOther.Value())
      return Standard_False;
  }

  return Standard_True;
}

const 
TColStd_ListOfAsciiString& XCAFDoc_AssemblyItemId::GetPath() const
{
  return myPath;
}

TCollection_AsciiString 
XCAFDoc_AssemblyItemId::ToString() const
{
  TCollection_AsciiString aStr;
  for (TColStd_ListOfAsciiString::Iterator anIt(myPath); anIt.More(); anIt.Next())
  {
    aStr += '/';
    aStr += anIt.Value();
  }
  return aStr;
}
