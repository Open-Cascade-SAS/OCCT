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

#include <Storage_HeaderData.ixx>

Storage_HeaderData::Storage_HeaderData() : myNBObj(0), myErrorStatus(Storage_VSOk)
{
}

TCollection_AsciiString Storage_HeaderData::CreationDate() const
{
  return myDate;
}

void Storage_HeaderData::SetSchemaVersion(const TCollection_AsciiString& aVersion) 
{
  mySchemaVersion = aVersion;
}

TCollection_AsciiString Storage_HeaderData::SchemaVersion() const
{
  return mySchemaVersion;
}

void Storage_HeaderData::SetSchemaName(const TCollection_AsciiString& aSchemaName) 
{
  mySchemaName = aSchemaName;
}

TCollection_AsciiString Storage_HeaderData::SchemaName() const
{
  return mySchemaName;
}

void Storage_HeaderData::SetApplicationVersion(const TCollection_AsciiString& aVersion) 
{
  myApplicationVersion = aVersion;
}

TCollection_AsciiString Storage_HeaderData::ApplicationVersion() const
{
  return myApplicationVersion;
}

void Storage_HeaderData::SetApplicationName(const TCollection_ExtendedString& aName) 
{
  myApplicationName = aName;
}

TCollection_ExtendedString Storage_HeaderData::ApplicationName() const
{
  return myApplicationName;
}

void Storage_HeaderData::SetDataType(const TCollection_ExtendedString& aName) 
{
  myDataType = aName;
}

TCollection_ExtendedString Storage_HeaderData::DataType() const
{
  return myDataType;
}

void Storage_HeaderData::AddToUserInfo(const TCollection_AsciiString& theUserInfo) 
{
  myUserInfo.Append(theUserInfo);
}

const TColStd_SequenceOfAsciiString& Storage_HeaderData::UserInfo() const
{
  return myUserInfo;
}

void Storage_HeaderData::AddToComments(const TCollection_ExtendedString& aComments) 
{
  myComments.Append(aComments);
}

const TColStd_SequenceOfExtendedString& Storage_HeaderData::Comments() const
{
  return myComments;
}

Standard_Integer Storage_HeaderData::NumberOfObjects() const
{
  return myNBObj;
}

void Storage_HeaderData::SetNumberOfObjects(const Standard_Integer anObjectNumber) 
{
  myNBObj = anObjectNumber;
}

void Storage_HeaderData::SetStorageVersion(const TCollection_AsciiString& v)
{
  myStorageVersion = v;
}

void Storage_HeaderData::SetCreationDate(const TCollection_AsciiString& d)
{
  myDate = d;
}

TCollection_AsciiString Storage_HeaderData::StorageVersion() const
{
  return myStorageVersion;
}

Storage_Error  Storage_HeaderData::ErrorStatus() const
{
  return myErrorStatus;
}

void Storage_HeaderData::SetErrorStatus(const Storage_Error anError)
{
  myErrorStatus = anError;
}

TCollection_AsciiString Storage_HeaderData::ErrorStatusExtension() const
{
  return myErrorStatusExt;
}

void Storage_HeaderData::SetErrorStatusExtension(const TCollection_AsciiString& anErrorExt)
{
  myErrorStatusExt = anErrorExt;
}

void Storage_HeaderData::ClearErrorStatus()
{
  myErrorStatus = Storage_VSOk;
  myErrorStatusExt.Clear();
}

