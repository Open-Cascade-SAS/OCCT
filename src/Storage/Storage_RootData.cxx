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


#include <Standard_NoSuchObject.hxx>
#include <Standard_Persistent.hxx>
#include <Standard_Type.hxx>
#include <Storage_DataMapIteratorOfMapOfPers.hxx>
#include <Storage_Root.hxx>
#include <Storage_RootData.hxx>
#include <Storage_Schema.hxx>
#include <TCollection_AsciiString.hxx>

Storage_RootData::Storage_RootData() : myErrorStatus(Storage_VSOk)
{
}

Standard_Integer Storage_RootData::NumberOfRoots() const
{
  return myObjects.Extent();
}

void Storage_RootData::AddRoot(const Handle(Storage_Root)& aRoot) 
{
  myObjects.Bind(aRoot->Name(),aRoot);
}

Handle(Storage_HSeqOfRoot) Storage_RootData::Roots() const
{
  Handle(Storage_HSeqOfRoot)   anObjectsSeq = new Storage_HSeqOfRoot;
  Storage_DataMapIteratorOfMapOfPers it(myObjects);
  
  for(;it.More(); it.Next()) {
    anObjectsSeq->Append(it.Value());
  }

  return anObjectsSeq;
}

Handle(Storage_Root) Storage_RootData::Find(const TCollection_AsciiString& aName) const
{
  Handle(Storage_Root) p;

  if (myObjects.IsBound(aName)) {
    p = myObjects.Find(aName);
  }

  return p;
}

Standard_Boolean Storage_RootData::IsRoot(const TCollection_AsciiString& aName) const
{
  return myObjects.IsBound(aName);
}

void Storage_RootData::RemoveRoot(const TCollection_AsciiString& aName) 
{
  if (myObjects.IsBound(aName)) {
    myObjects.UnBind(aName);    
  }
}

void Storage_RootData::UpdateRoot(const TCollection_AsciiString& aName,const Handle(Standard_Persistent)& aPers) 
{
  if (myObjects.IsBound(aName)) {
    myObjects.ChangeFind(aName)->SetObject(aPers);
  }
  else {
    Standard_NoSuchObject::Raise();
  }
}

Storage_Error  Storage_RootData::ErrorStatus() const
{
  return myErrorStatus;
}

void Storage_RootData::SetErrorStatus(const Storage_Error anError)
{
  myErrorStatus = anError;
}

void Storage_RootData::ClearErrorStatus()
{
  myErrorStatus = Storage_VSOk;
  myErrorStatusExt.Clear();
}

TCollection_AsciiString Storage_RootData::ErrorStatusExtension() const
{
  return myErrorStatusExt;
}

void Storage_RootData::SetErrorStatusExtension(const TCollection_AsciiString& anErrorExt)
{
  myErrorStatusExt = anErrorExt;
}
