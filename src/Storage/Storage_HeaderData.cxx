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
#include <Storage_HeaderData.hxx>
#include <Storage_Schema.hxx>
#include <TCollection_AsciiString.hxx>
#include <TCollection_ExtendedString.hxx>

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

