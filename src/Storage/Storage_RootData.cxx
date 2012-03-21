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

#include <Storage_RootData.ixx>
#include <Storage_DataMapIteratorOfMapOfPers.hxx>

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
