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

#include <Storage_Data.ixx>
#include <Storage_Root.hxx>

Storage_Data::Storage_Data()
{
  myRootData = new Storage_RootData;
  myTypeData = new Storage_TypeData;
  myInternal = new Storage_InternalData;
  myHeaderData = new Storage_HeaderData;
}

void Storage_Data::AddRoot(const Handle(Standard_Persistent)& anObject) const
{
  Handle(Storage_Root) aRoot = new Storage_Root(TCollection_AsciiString(myRootData->NumberOfRoots()+1),anObject);
  myRootData->AddRoot(aRoot);
}

void Storage_Data::AddRoot(const TCollection_AsciiString& aName, const Handle(Standard_Persistent)& anObject) const
{
  Handle(Storage_Root) aRoot = new Storage_Root(aName,anObject);
  myRootData->AddRoot(aRoot);
}

void Storage_Data::RemoveRoot(const TCollection_AsciiString& anObject)
{
  myRootData->RemoveRoot(anObject);
}

Handle(Storage_Root) Storage_Data::Find(const TCollection_AsciiString& aName) const
{
  return myRootData->Find(aName);
}

Standard_Integer Storage_Data::NumberOfRoots() const
{
  return myRootData->NumberOfRoots();
}

Standard_Boolean Storage_Data::IsRoot(const TCollection_AsciiString& aName) const
{
  return myRootData->IsRoot(aName);
}

Handle(Storage_HSeqOfRoot) Storage_Data::Roots() const
{
  return myRootData->Roots();
}

Standard_Integer Storage_Data::NumberOfTypes() const
{
  return myTypeData->NumberOfTypes();
}

Standard_Boolean Storage_Data::IsType(const TCollection_AsciiString& aName) const
{
  return myTypeData->IsType(aName);
}

Handle(TColStd_HSequenceOfAsciiString) Storage_Data::Types() const
{
  return myTypeData->Types();
}

Handle(Storage_RootData) Storage_Data::RootData() const
{
  return myRootData;
}

Handle(Storage_TypeData) Storage_Data::TypeData() const
{
  return myTypeData;
}

Handle(Storage_InternalData) Storage_Data::InternalData() const
{
  return myInternal;
}

Handle(Storage_HeaderData) Storage_Data::HeaderData() const
{
  return myHeaderData;
}

void Storage_Data::Clear() const 
{
  myInternal->Clear();
  myTypeData->Clear();
}

// HEADER

TCollection_AsciiString Storage_Data::CreationDate() const
{
  return myHeaderData->CreationDate();
}

TCollection_AsciiString Storage_Data::SchemaVersion() const
{
  return myHeaderData->SchemaVersion();
}

TCollection_AsciiString Storage_Data::SchemaName() const
{
  return myHeaderData->SchemaName();
}

void Storage_Data::SetApplicationVersion(const TCollection_AsciiString& aVersion) 
{
  myHeaderData->SetApplicationVersion(aVersion);
}

TCollection_AsciiString Storage_Data::ApplicationVersion() const
{
  return myHeaderData->ApplicationVersion();
}

void Storage_Data::SetApplicationName(const TCollection_ExtendedString& aName) 
{
  myHeaderData->SetApplicationName(aName);
}

TCollection_ExtendedString Storage_Data::ApplicationName() const
{
  return myHeaderData->ApplicationName();
}

void Storage_Data::AddToUserInfo(const TCollection_AsciiString& theUserInfo) 
{
  myHeaderData->AddToUserInfo(theUserInfo);
}

const TColStd_SequenceOfAsciiString& Storage_Data::UserInfo() const
{
  return myHeaderData->UserInfo();
}

void Storage_Data::AddToComments(const TCollection_ExtendedString& theUserInfo) 
{
  myHeaderData->AddToComments(theUserInfo);
}

const TColStd_SequenceOfExtendedString& Storage_Data::Comments() const
{
  return myHeaderData->Comments();
}

Standard_Integer Storage_Data::NumberOfObjects() const
{
  return myHeaderData->NumberOfObjects();
}

TCollection_AsciiString Storage_Data::StorageVersion() const
{
  return myHeaderData->StorageVersion();
}

Storage_Error  Storage_Data::ErrorStatus() const
{
  return myErrorStatus;
}

void Storage_Data::SetErrorStatus(const Storage_Error anError)
{
  myErrorStatus = anError;
}

void Storage_Data::ClearErrorStatus()
{
  myErrorStatus = Storage_VSOk;
  myErrorStatusExt.Clear();
  myHeaderData->ClearErrorStatus();
  myRootData->ClearErrorStatus();
  myTypeData->ClearErrorStatus();
}

void Storage_Data::SetDataType(const TCollection_ExtendedString& aName) 
{
  myHeaderData->SetDataType(aName);
}

TCollection_ExtendedString Storage_Data::DataType() const
{
  return myHeaderData->DataType();
}

TCollection_AsciiString Storage_Data::ErrorStatusExtension() const
{
  return myErrorStatusExt;
}

void Storage_Data::SetErrorStatusExtension(const TCollection_AsciiString& anErrorExt)
{
  myErrorStatusExt = anErrorExt;
}
