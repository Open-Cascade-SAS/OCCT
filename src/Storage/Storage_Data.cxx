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
#include <Storage_Data.hxx>
#include <Storage_HeaderData.hxx>
#include <Storage_InternalData.hxx>
#include <Storage_Root.hxx>
#include <Storage_RootData.hxx>
#include <Storage_Schema.hxx>
#include <Storage_TypeData.hxx>
#include <TCollection_AsciiString.hxx>
#include <TCollection_ExtendedString.hxx>

IMPLEMENT_STANDARD_RTTIEXT(Storage_Data,Standard_Transient)

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
